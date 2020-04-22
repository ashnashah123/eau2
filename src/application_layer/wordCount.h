#pragma once
#include "./application.h"
#include "../store/key.h"
#include "../utils/helper.h"
#include "../utils/string.h"
#include "../dataframe/fileReader.h"
#include "../dataframe/adder.h"
#include "../dataframe/summer.h"
#include "../dataframe/dataframe.h"
#include "../store/KDStore.h"
#include "../utils/KeyBuff.h"
#include "../utils/si_map.h"
#include "../network/networkifc.h"

/****************************************************************************
 * Calculate a word count for given file:
 *   1) read the data (single node)
 *   2) produce word counts per homed chunks, in parallel
 *   3) combine the results
 **********************************************************author: pmaj ****/
class WordCount: public Application {
public:
  static const size_t BUFSIZE = 1024;
  Key in;
  KeyBuff kbuf;
  SIMap all;
  KVStore & kv;
 
  WordCount(size_t idx, KVStore& kvstore):
    Application(idx), in{new String("data"), 0}, kbuf(new Key(new String("wc-map-"),0)), kv{kvstore} {
    }
 
    size_t this_node() {
        return idx;
    }
  /** The master nodes reads the input, then all of the nodes count. */
    void run_() override {
        if (this_node() == 0) {
            fprintf(stderr, "inside the run method on node 0\n");
            FileReader fr;
            DataFrame::fromVisitor(&in, &kv, "S", fr);
        }
        local_count();
        std::cout << "finished local count!!\n";
        reduce();
    }
 
    void start_kv() {
        kv.start();
        // kv.join();
    }
  /** Returns a key for given node.  These keys are homed on master node
   *  which then joins them one by one. */
    Key* mk_key(size_t idx) {
        KeyBuff* kbuf = new KeyBuff(new Key(new String("wc-map-"),0));
        Key * k = kbuf->c(idx).get();
        fprintf(stderr, "Created key %s\n", k->get_name()->cstr_);
        return k;
    }
    
    /** Compute word counts on the local node and build a data frame. */
    void local_count() {
        std::cout << "NODE: " << this_node() << ": starting local count ...\n";
        Deserialize* d;
        if (this_node() == 0) {
            d = new Deserialize(kv.waitAndGet_(&in)->get_data());
        }
        else {
            d = new Deserialize(kv.waitAndGet_(&in)->get_data());
        }
        DataFrame* words = DataFrame::deserialize(d, &kv);
        SIMap map;
        Adder add(map);
        words->local_map(add, this_node());
        delete words;
        Summer cnt(map);
        delete DataFrame::fromVisitor(mk_key(this_node()), &kv, "SI", cnt);
    }
    
    /** Merge the data frames of all nodes */
    void reduce() {
        if (this_node() != 0) return;
        std::cout << "Node 0: reducing counts...\n";
        SIMap map;

        Key* own = mk_key(0);
        merge(DataFrame::deserialize(new Deserialize(kv.get_(own)->get_data()), &kv), map);

        for (size_t i = 1; i < NUM_NODES; ++i) { // merge other nodes
            Key* ok = mk_key(i);
            merge(DataFrame::deserialize(new Deserialize(kv.waitAndGet_(ok)->get_data()), &kv), map);
            delete ok;
        }
        std::cout << "**************************************\n";
        std::cout << "Different words: " << map.size() << "\n";
        delete own;
    }
    
    void merge(DataFrame* df, SIMap& m) {
        Adder add(m);
        df->map(add);
        delete df;
    }
};
