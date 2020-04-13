#pragma once
#include <stdio.h>
#include "../utils/thread.h"
#include "../network/networkifc.h"

class Application {
public:
    size_t idx;

    Application(size_t index) {
        idx = index;
    }

    virtual void run_() {};
};