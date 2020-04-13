#pragma once

#include "../utils/array.h"
#include "./message.h"
#include "../utils/thread.h"

class MessageArrayQ : public Array {
public: 
    MessageArrayQ(size_t capacity) : Array(capacity) {}

    Message* pop() {
        Message* result = dynamic_cast<Message*>(get(num_elements_ - 1));
        array_[num_elements_ - 1] = nullptr;
        num_elements_--;
        return result;
    }
};

class MessageQueue : public Object {
public:
    MessageArrayQ q_;
    Lock lock_;

    MessageQueue() : q_(2) {}
    
    void push(Message* m) {
        lock_.lock();
        assert(m);
        q_.push(m);
        lock_.notify_all();
        lock_.unlock();
    }

    Message* pop () {
        lock_.lock();
        while(q_.length() == 0) {
            lock_.wait();
        }
        Message* result = q_.pop();;
        lock_.unlock();
        return result;
    }
};

class MQArray : public Array {
public:
    MQArray(size_t cap) : Array(cap) {}

    MessageQueue* get(size_t idx) {
        return dynamic_cast<MessageQueue*>(Array::get(idx));
    }
};