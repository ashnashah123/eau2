#pragma once
#include "./key.h"
#include "./value.h"
#include "../utils/map.h"
#include "../utils/thread.h"
#include "../network/networkifc.h"
#include "../network/network_ip.h"
#include "../network/msgKind.h"

class KVStore : public Thread {
public:
    Map map; // does not need to be a pointer - KV store owns map
    Lock lock_;
    NetworkIp* net_;
    size_t node_num_; // the current node this KVStore is on

    KVStore() {
    }

    KVStore(NetworkIp& net, size_t node_num) {
        net_ = &net;
        node_num_ = node_num;
    }

    ~KVStore() {
        map.delete_all();
    }
    
    // puts the kv store into an infinite listening state
    void run_() {
        while(1) {
            // constantly receiving messages
            Message* msg = net_->recv_m();
            if (msg->kind_ == MsgKind::DATA) {
                DataMessage* data_msg = dynamic_cast<DataMessage*>(msg);
                // std::cout << "Received a msgkind::DATA. I am node: " << node_num_ << " with key name: " << data_msg->key_->get_name()->cstr_ << " with key_->home(): " << data_msg->key_->home() << "\n";
                if (data_msg->key_->home() == node_num_) {
                    put_(data_msg->key_, data_msg->data_);
                }
                else {
                    put_in_cache_(data_msg->key_, data_msg->data_);
                }
                
            }
            else if (msg->kind_ == MsgKind::DATA_REQUEST) {
                DataRequest* data_request = dynamic_cast<DataRequest*>(msg);
                // std::cout << "Received a msgkind::DATA_REQUEST. I am node: " << node_num_ << " with key name: " << data_request->key_->get_name()->cstr_ << " with key_->home(): " << data_request->key_->home() << "\n";

                Value* val = waitAndGet_(data_request->key_);
                assert(val != nullptr);
                // if the data does not exist on our node (like if we never had data in the first place so the SI DF was never created... then don't send back a null value)
                if (val != nullptr) {
                    DataMessage* data_msg = new DataMessage(data_request->key_, val, data_request->sender_, node_num_);
                    // std::cout << "Sending a message back. I am node: " << node_num_ << " with key name: " << data_msg->key_->get_name()->cstr_ << " with key_->home(): " << data_msg->key_->home() << "\n";
                    net_->send_m(data_msg);
                }
            }
            else {
                assert(false);
            }
        }
    }

    size_t get_node_num() {
        return node_num_;
    }

    Value* get_(Key* key) {
        lock_.lock(); // Acquire the lock
        Value* result = dynamic_cast<Value*>(map.get(key));
        lock_.unlock();
        return result;
    }

    Value* waitAndGet_(Key* key) {
        // std::cout << "wag::Key(" << key->get_name()->cstr_ << ", " << key->home() << ") \n";
        // if the key is living / will live on us:
        if (key->home() != node_num_) {
            // ask the network to get the value for us by sending a data request message with our node num so they know the sender
            DataRequest* data_request = new DataRequest(key, node_num_);
            net_->send_m(data_request);
        }
        
        lock_.lock();
        while (map.get(key) == nullptr) {
            lock_.wait();
        }
        Value* result = (Value*)map.get(key);
        lock_.unlock();

        return result;
    }

    // called when this node receives data from another node (so that waitAndGet can return)
    // data is put at its original key in this kvstore
    void put_in_cache_(Key* key, Value* value) {
        lock_.lock();
        map.put(key, value);
        lock_.notify_all();
        lock_.unlock();
    }

    // called by any node when distributing data
    void put_(Key* key, Value* value) {
        Key k{new String("data-k-0-1"), 1};
        if (k.equals(key)) {
            std::cout << "val size: " << value->size <<  "\n";
        }
        // check that we are on the intended node for the key passed in
        if (key->home() == node_num_) {
            lock_.lock(); // Acquire the lock
            map.put(key, value);
            lock_.notify_all();
            lock_.unlock();
        }
        else {
            DataMessage* msg = new DataMessage(key, value, key->home(), node_num_);
            net_->send_m(msg);
        }
    }

    void distribute_df_to_all(Key* key, Value* val) {
        put_(key, val);
        for(size_t i = 1; i < NUM_NODES; i++) {
            DataMessage* msg = new DataMessage(key, val, i, node_num_);
            net_->send_m(msg);
        }
    }
};