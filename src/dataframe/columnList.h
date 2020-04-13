#pragma once
#include <stdio.h>
#include "../utils/object.h"
#include "../utils/array.h"
#include "./column.h"
#include "./schema.h"

class ColumnList : public Object {
public:
    Array arr_;

    ColumnList() : arr_(2) {}

    // ColumnList(Array & arr) : arr_(arr) {}

    ColumnList(Deserialize* d, Schema* s, KVStore* kv) : arr_(0) {
        size_t size = d->readSizeT();
        Array* arr = new Array(size);
        for (size_t i = 0; i < size; i++) {
            Column* col = nullptr;
            char type = s->col_type(i);
            switch(type) {
                case 'S': col = StringColumn::deserialize(d, kv); break;
                case 'I': col = IntColumn::deserialize(d, kv); break;
                case 'D': col = DoubleColumn::deserialize(d, kv); break;
                case 'B': col = BoolColumn::deserialize(d, kv); break;
                default: assert(false);
            }
            arr->push(col);
        }
        arr_ = *arr;
    }

    char* serialize(Serialize* s) {
        s->write(arr_.length());
        for (size_t i = 0; i < arr_.length(); i++) {
            dynamic_cast<Column*>(arr_.get(i))->serialize(s);
        }
        return s->getChars();
    }

    static ColumnList* deserialize(Deserialize* d, Schema* s,  KVStore* kv) {
        return new ColumnList(d, s, kv);
    }

    void push_back(Column* val) { arr_.push(val); }

    Column* get(size_t idx) {
        Column* result = dynamic_cast<Column*>(arr_.get(idx));
        return result;
    }

    void set(size_t idx, Column* val) { arr_.set(val, idx); }

    size_t size() { return arr_.length(); }

    size_t index_of(Column* s) { assert(false); }

    bool equals(Object* other) {
        ColumnList* l = dynamic_cast<ColumnList*>(other);
        if (l == nullptr) return false;
        return arr_.equals(&l->arr_);
    }

    size_t hash() { assert(false); }  
};