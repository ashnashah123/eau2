#pragma once
#include "../utils/object.h"
#include <stdio.h>
#include <string.h>

class Value : public Object {
public:
    char* data;
    size_t size;

    Value(char* val) {
        data = val;
        size = strlen(val);
    }

    Value(Deserialize& d) {
        size = d.readSizeT();
        char* data = d.readChars(size);
    }

    ~Value() {}

    // gets and returns the data
    char* get_data() {
        return data;
    }

    char* serialize(Serialize& s) {
        s.write(size);
        s.write(data);
        return s.getChars();
    }

    static Value* deserialize(Deserialize& d) {
        return new Value(d);
    }
};