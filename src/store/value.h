#pragma once
#include "../utils/object.h"
#include <stdio.h>
#include <string.h>

class Value : public Object {
public:
    char* data;
    size_t size;

    Value(char* val, size_t len) {
        data = val;
        size = len;
    }

    Value(Deserialize& d) {
        size = d.readSizeT();
        data = d.readCharsNoNull(size);
    }

    ~Value() {}

    // gets and returns the data
    char* get_data() {
        return data;
    }

    void serialize(Serialize& s) {
        s.write(size);
        s.writeNoNull(data, size);
        return;
    }

    static Value* deserialize(Deserialize& d) {
        return new Value(d);
    }
};