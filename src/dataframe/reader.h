#pragma once

#include "./row.h"

class Reader {
public:
    virtual bool visit(Row& r) {assert(false);}
};