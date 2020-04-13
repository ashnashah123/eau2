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
    // ./server -nodes 3 -index 0 -port 8080 
    size_t num_nodes = atoi(argv[2]);
    size_t index = atoi(argv[4]);
    std::cout << "INDEX: " << index << "\n";
    size_t port = atoi(argv[6]);
    std::cout << "atoi(argv[6]): " << port << "\n";

    NetworkIp* network = new NetworkIp();
    
    while(true) {
        network->server_init(index, port);
    }
    

    KVStore* kv = new KVStore(*network, index);
    WordCount* wc = new WordCount(index, *kv);
    
    wc->start_kv();
    wc->run_();

    while(1) {

    }

    return 0;
}