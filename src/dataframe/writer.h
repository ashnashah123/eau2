#pragma once
#include "./row.h"

class Writer {
public:

    Writer() {}

    ~Writer() {}

    virtual void visit(Row &r) {

    }

    virtual bool done() {
        return 0;
    }
};