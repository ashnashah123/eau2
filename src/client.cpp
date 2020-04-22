#include <assert.h>
#include "./dataframe/dataframe.h"
#include <stdio.h>
#include <stdarg.h>
#include "./sorer/sorer.h"
#include "./application_layer/wordCount.h"
#include <stdlib.h>
#include <string.h>


int main(int argc, char* argv[]) {
    // command line arguments
    // ./client -nodes 3 -index 1 -port 8081 -masterip 127.0.0.1 -masterport 8080 -ip 127.0.0.2
    size_t num_nodes = atoi(argv[2]);
    size_t index = atoi(argv[4]);
    size_t port = atoi(argv[6]);
    char* masterip = argv[8];
    size_t masterport = atoi(argv[10]);
    char* client_ip = argv[12];
    NUM_NODES = num_nodes;

    NetworkIp* network = new NetworkIp();
    network->client_init(index, port, masterip, masterport, client_ip);
    // network->client_init(1, 8081, (char*)"127.0.0.1", 8080, (char*)"127.0.0.3");

    KVStore* kv = new KVStore(*network, index);
    // KVStore* kv = new KVStore(*network, 1);
    WordCount* wc = new WordCount(index, *kv);
    // WordCount* wc = new WordCount(1, *kv);
    
    wc->start_kv();
    wc->run_();
    
    return 0;
}