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
    // ./server -nodes 3 -index 0 -port 8080 -ip 127.0.0.1
    size_t num_nodes = atoi(argv[2]);
    size_t index = atoi(argv[4]);
    std::cout << "INDEX: " << index << "\n";
    size_t port = atoi(argv[6]);
    std::cout << "atoi(argv[6]): " << port << "\n";
    char* server_ip = argv[8];
    NUM_NODES = num_nodes;
    NetworkIp* network = new NetworkIp();
    
    network->server_init(index, port, server_ip);
    // network->server_init(0, 8080, (char*)"127.0.0.1");

    KVStore* kv = new KVStore(*network, index);
    // KVStore* kv = new KVStore(*network, 0);
    WordCount* wc = new WordCount(index, *kv);
    // WordCount* wc = new WordCount(0, *kv);
    
    wc->start_kv();
    wc->run_();

    return 0;
}