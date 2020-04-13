// #pragma once
// #include "../dataframe/dataframe.h"
// #include "KVStore.h"
// #include "serial.h"

// class KDStore {
// public:
//     KVStore kv;

//     KDStore() {
//     }

//     ~KDStore() {
//     }

//     DataFrame* get(Key* key) {
//         Value* val = kv.get_(key);
//         Deserialize* d = new Deserialize(val->get_data());
//         DataFrame* df = DataFrame::deserialize(d);
//         delete d;
//         return df;
//     }

//     DataFrame* waitAndGet(Key* key) {
//         // p((size_t)kv).pln();
//         Value* val = kv.waitAndGet_(key);
//         Deserialize* d = new Deserialize(val->get_data());
//         DataFrame* df = DataFrame::deserialize(d);
//         delete d;
//         return df;
//     }

//     // serialize the dataframe and put it into the KV Store
//     void put(Key* key, DataFrame* df) {
//         // Serialize s;
//         // serialize the dataframe
//         // char* serialized_df = df->serialize(&s);

//         // Value* val = new Value(serialized_df);

//         // put the serialized df into the KVStore
//         // kv.put_(key, val);
//     }
// };

// // a method that takes an array of doubles and builds a dataframe with one column in it
// // and stores the data in the given kv store at the given key
// DataFrame* DataFrame::fromArray(Key* key, KDStore* kd, size_t length, double* vals) {
//     // make the dataframe to return
//     DataFrame* df = new DataFrame();
//     DoubleColumn* fc = new DoubleColumn();
//     for (int i = 0; i < length; i++) {
//         fc->push_back(vals[i]);
//     }
//     df->add_column(fc);

//     // put the dataframe into the KDStore at the given key
//     kd->put(key, df);
    
//     return df;
// }

// // a method that takes an array of ints and builds a dataframe with one column in it
// DataFrame* DataFrame::fromArray(Key* key, KDStore* kd, size_t length, int* vals) {
//     // make the dataframe to return
//     DataFrame* df = new DataFrame();
//     IntColumn* c = new IntColumn();
//     for (int i = 0; i < length; i++) {
//         c->push_back(vals[i]);
//     }
//     df->add_column(c);

//     // put the dataframe into the KDStore at the given key
//     kd->put(key, df);
    
//     return df;
// }

// // a method that takes an array of bools and builds a dataframe with one column in it
// DataFrame* DataFrame::fromArray(Key* key, KDStore* kd, size_t length, bool* vals) {
//     // make the dataframe to return
//     DataFrame* df = new DataFrame();
//     BoolColumn* c = new BoolColumn();
//     for (int i = 0; i < length; i++) {
//         c->push_back(vals[i]);
//     }
//     df->add_column(c);

//     // put the dataframe into the KDStore at the given key
//     kd->put(key, df);
    
//     return df;
// }

// // a method that takes an array of Strings and builds a dataframe with one column in it
// DataFrame* DataFrame::fromArray(Key* key, KDStore* kd, size_t length, String** vals) {
//     // make the dataframe to return
//     DataFrame* df = new DataFrame();
//     StringColumn* c = new StringColumn();
//     for (int i = 0; i < length; i++) {
//         c->push_back(vals[i]);
//     }
//     df->add_column(c);

//     // put the dataframe into the KDStore at the given key
//     kd->put(key, df);
    
//     return df;
// }

// // a method that takes a value and builds a data frame with one column with the value in it
// DataFrame* DataFrame::fromScalar(Key* key, KDStore* kd, double val) {
//     DataFrame* df = new DataFrame();
//     DoubleColumn* c = new DoubleColumn();
//     c->push_back(val);
//     df->add_column(c);
//     kd->put(key, df);
//     return df;
// }
// DataFrame* DataFrame::fromScalar(Key* key, KDStore* kd, int val) {
//     DataFrame* df = new DataFrame();
//     IntColumn* c = new IntColumn();
//     c->push_back(val);
//     df->add_column(c);
//     fprintf(stderr, "just pushed back an int column\n");
//     kd->put(key, df);
//     return df;
// }
// DataFrame* DataFrame::fromScalar(Key* key, KDStore* kd, String* val) {
//     DataFrame* df = new DataFrame();
//     StringColumn* c = new StringColumn();
//     c->push_back(val);
//     df->add_column(c);
//     kd->put(key, df);
//     return df;
// }
// DataFrame* DataFrame::fromScalar(Key* key, KDStore* kd, bool val) {
//     DataFrame* df = new DataFrame();
//     BoolColumn* c = new BoolColumn();
//     c->push_back(val);
//     df->add_column(c);
//     kd->put(key, df);
//     return df;
// }