#pragma once

#include "./networkifc.h"
#include "../utils/si_map.h"
#include "./message_queue.h"

class NetworkPseudo : public NetworkIfc {
public:
    // maps thread id to node id
    AsyncSIMap thread_node_map_; 
    MQArray mqarray_;
    Lock lock_;

    NetworkPseudo() : mqarray_(1) {
        for (size_t i = 0; i < NUM_NODES; i++) {
            mqarray_.push(new MessageQueue());
        }
    }

    void register_node(size_t idx, String* tid) override {
        std::cout << "THREAD TO BE REGISTERED IN REGISTER NODE: " << tid->cstr_ << "\n";
        lock_.lock(); // Acquire the lock
        thread_node_map_.set(*tid, new Num(idx));
        std::cout << "thread id: " << tid->cstr_ << "\n";
        std::cout << "thread node map:" << thread_node_map_.get(*tid)->get_value() << "\n";
        lock_.unlock();
        delete tid;
    }

    void send_m(Message* msg) override {
        // gets the queue for the destination of the message and pushes onto the queue
        mqarray_.get(msg->target_)->push(msg);
    }

    Message* recv_m() override {
        String* tid = Thread::thread_id();
        std::cout << "tid in recv_m: " << tid->cstr_ << "\n";
        Num* num = thread_node_map_.get(*tid);
        size_t node_num = num->get_value();
        delete tid;
        return mqarray_.get(node_num)->pop();
    }

};