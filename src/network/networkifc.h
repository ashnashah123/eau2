#pragma once

#include "../utils/object.h"
#include "./message.h"

// An interface to represent the network layer
class NetworkIfc : public Object{
public:
    virtual void register_node(size_t idx, String* tid) {}

    virtual size_t index() { assert(false); }

    virtual void send_m(Message* m) = 0;

    virtual Message* recv_m() = 0;
};