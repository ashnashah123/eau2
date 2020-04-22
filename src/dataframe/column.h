#pragma once
#include <stdio.h>
#include "../utils/object.h"
#include "../utils/string.h"
#include "./list.h"
#include <stdarg.h>
#include "../store/serial.h"
#include "../store/KVStore.h"

class IntColumn;
class BoolColumn;
class DoubleColumn;
class StringColumn;
/**************************************************************************
 * Column ::
 * Represents one column of a data frame which holds values of a single type.
 * This abstract class defines methods overriden in subclasses. There is
 * one subclass per element type. Columns are mutable, equality is pointer
 * equality. */
class Column : public Object {
public:
    /** Type converters: Return same column under its actual type, or
    *  nullptr if of the wrong type.  */
    virtual IntColumn* as_int() {return nullptr;}
    virtual BoolColumn*  as_bool() {return nullptr;}
    virtual DoubleColumn* as_double() {return nullptr;}
    virtual StringColumn* as_string() {return nullptr;}
    
    /** Type appropriate push_back methods. Calling the wrong method is
        * undefined behavior. **/
    virtual void push_back(int val) {assert(false);};
    virtual void push_back(bool val) {assert(false);};
    virtual void push_back(double val) {assert(false);}; 
    virtual void push_back(String* val) {assert(false);};
    
    /** Returns the number of elements in the column. */
    virtual size_t num_keys() {return 0;}

    virtual size_t num_elements_per_chunk(Key* k) {return 0;}

    virtual size_t total_num_elements() {return 0;}

    // TODO: this is literally only around to make the tests compile - delete and clean this up
    virtual size_t size() {return 0;}

    virtual void add_key(Key* k) {assert(false);};
    virtual Key* get_key(size_t idx) {assert(false);}
    
    // serializes this Column and writes it to the buffer
    virtual char* serialize(Serialize* s) {return 0;};

    /** Return the type of this column as a char: 'S', 'B', 'I' and 'D'.*/
    char get_type(){
        if (as_int() != nullptr) {
            return 'I';
        }
        else if (as_double() != nullptr) {
            return 'D';
        }
        else if (as_bool() != nullptr) {
            return 'B';
        }
        else if (as_string() != nullptr) {
            return 'S';
        }
        else {
            assert(false);
        }
    }
};

/*************************************************************************
 * IntColumn::
 * Holds int values.
 */
class IntColumn : public Column {
public:
    Array keys_;
    KVStore* kv_;

    IntColumn() : keys_(10) {}

    IntColumn(KVStore* kv) : keys_(10) { kv_ = kv; }

    char* serialize(Serialize* s) {
        s->write(keys_.length());
        for(size_t i = 0; i < keys_.length(); i++) {
            Key* key = dynamic_cast<Key*>(keys_.get(i));
            key->serialize(*s);
        }
        return s->getChars();
    }

    static IntColumn* deserialize(Deserialize* d, KVStore* kv) {
        IntColumn* result = new IntColumn(kv);
        size_t size = d->readSizeT();
        Array* keys = new Array(size);
        for(size_t i = 0; i < size; i++) {
            keys->push(Key::deserialize(*d));
        }
        result->keys_ = *keys;
        return result;
    };

    int get(size_t idx) {
        size_t key_idx = idx / MAX_SIZE;
        size_t offset = idx % MAX_SIZE;
        Key* key = dynamic_cast<Key*>(keys_.get(key_idx));
        size_t node_num = kv_->get_node_num();

        Value* val = nullptr;
        if(node_num == key->home()) {
            val = kv_->get_(key);
        }
        else {
            val = kv_->waitAndGet_(key);
        }
        IntArray* arr_ = dynamic_cast<IntArray*>(BaseArray::deserialize(new Deserialize(val->get_data())));
        return arr_->get(offset);
    }

    IntColumn* as_int() {
        return this;
    }
        
    size_t num_keys() {
        return keys_.length();
    }

    // add the element and resize if necessary
    void push_back(int val) {
        assert(false);
    }

    bool equals(Object* other) {
        if (other == nullptr) return false;
        if (other == this) return true;
        IntColumn* x = dynamic_cast<IntColumn*>(other);
        if (x == nullptr) return false;

        return keys_.equals(&x->keys_);
    }

    size_t hash() {
        assert(false);
    }

    void add_key(Key* k) {
        keys_.push(k);
    }

    Key* get_key(size_t idx) {
        return dynamic_cast<Key*>(keys_.get(idx));
    }    
    
    size_t num_elements_per_chunk(Key* k) {
        Value* val = kv_->waitAndGet_(k);
        IntArray* arr_ = dynamic_cast<IntArray*>(BaseArray::deserialize(new Deserialize(val->get_data())));
        return arr_->size();
    }

    size_t total_num_elements() {
        size_t result = 0;
        for(size_t i = 0; i < num_keys(); i++) {
            result += num_elements_per_chunk(dynamic_cast<Key*>(keys_.get(i)));
        }
        return result;
    }
};

class DoubleColumn : public Column {
public:
    Array keys_;
    KVStore* kv_;

    DoubleColumn() : keys_(10) {}

    DoubleColumn(KVStore* kv) : keys_(10) { kv_ = kv; }

    char* serialize(Serialize* s) {
        s->write(keys_.length());
        for(size_t i = 0; i < keys_.length(); i++) {
            Key* key = dynamic_cast<Key*>(keys_.get(i));
            key->serialize(*s);
        }
        return s->getChars();
    }

    static DoubleColumn* deserialize(Deserialize* d, KVStore* kv) {
        DoubleColumn* result = new DoubleColumn(kv);
        size_t size = d->readSizeT();
        Array* keys = new Array(size);
        for(size_t i = 0; i < size; i++) {
            keys->push(Key::deserialize(*d));
        }
        result->keys_ = *keys;
        return result;
    };

    double get(size_t idx) {
        size_t key_idx = idx / MAX_SIZE;
        size_t offset = idx % MAX_SIZE;
        Key* key = dynamic_cast<Key*>(keys_.get(key_idx));
        size_t node_num = kv_->get_node_num();

        Value* val = nullptr;
        if(node_num == key->home()) {
            val = kv_->get_(key);
        }
        else {
            val = kv_->waitAndGet_(key);
        }
        DoubleArray* arr_ = dynamic_cast<DoubleArray*>(BaseArray::deserialize(new Deserialize(val->get_data())));
        return arr_->get(offset);
    }

    DoubleColumn* as_double() {
        return this;
    }

    size_t num_keys() {
        return keys_.length();
    }

    // add the element and resize if necessary
    void push_back(double val) {
        assert(false);
    }

    bool equals(Object* other) {
        if (other == nullptr) return false;
        if (other == this) return true;
        DoubleColumn* x = dynamic_cast<DoubleColumn*>(other);
        if (x == nullptr) return false;

        return keys_.equals(&x->keys_);
    }

    size_t hash() {
        assert(false);
    }   

    void add_key(Key* k) {
        keys_.push(k);
    } 

    Key* get_key(size_t idx) {
        return dynamic_cast<Key*>(keys_.get(idx));
    }
    
    size_t num_elements_per_chunk(Key* k) {
        Value* val = kv_->waitAndGet_(k);
        DoubleArray* arr_ = dynamic_cast<DoubleArray*>(BaseArray::deserialize(new Deserialize(val->get_data())));
        return arr_->size();
    }

    size_t total_num_elements() {
        size_t result = 0;
        for(size_t i = 0; i < num_keys(); i++) {
            result += num_elements_per_chunk(dynamic_cast<Key*>(keys_.get(i)));
        }
        return result;
    }
};
 
class BoolColumn : public Column {
public:
    Array keys_;
    KVStore* kv_;

    BoolColumn() : keys_(10) {}

    BoolColumn(KVStore* kv) : keys_(10) { kv_ = kv; }

    char* serialize(Serialize* s) {
        s->write(keys_.length());
        for(size_t i = 0; i < keys_.length(); i++) {
            Key* key = dynamic_cast<Key*>(keys_.get(i));
            key->serialize(*s);
        }
        return s->getChars();
    }

    static BoolColumn* deserialize(Deserialize* d, KVStore* kv) {
        BoolColumn* result = new BoolColumn(kv);
        size_t size = d->readSizeT();
        Array* keys = new Array(size);
        for(size_t i = 0; i < size; i++) {
            keys->push(Key::deserialize(*d));
        }
        result->keys_ = *keys;
        return result;
    };

    bool get(size_t idx) {
        size_t key_idx = idx / MAX_SIZE;
        size_t offset = idx % MAX_SIZE;
        Key* key = dynamic_cast<Key*>(keys_.get(key_idx));
        size_t node_num = kv_->get_node_num();

        Value* val = nullptr;
        if(node_num == key->home()) {
            val = kv_->get_(key);
        }
        else {
            val = kv_->waitAndGet_(key);
        }
        BoolArray* arr_ = dynamic_cast<BoolArray*>(BaseArray::deserialize(new Deserialize(val->get_data())));
        return arr_->get(offset);
    }

    BoolColumn* as_bool() {
        return this;
    }
        
    size_t num_keys() {
        return keys_.length();
    }

    // add the element and resize if necessary
    void push_back(bool val) {
        assert(false);
    }

    bool equals(Object* other) {
        if (other == nullptr) return false;
        if (other == this) return true;
        BoolColumn* x = dynamic_cast<BoolColumn*>(other);
        if (x == nullptr) return false;

        return keys_.equals(&x->keys_);
    }

    size_t hash() {
        assert(false);
    }

    void add_key(Key* k) {
        keys_.push(k);
    }      

    Key* get_key(size_t idx) {
        return dynamic_cast<Key*>(keys_.get(idx));
    } 

    size_t num_elements_per_chunk(Key* k) {
        Value* val = kv_->waitAndGet_(k);
        BoolArray* arr_ = dynamic_cast<BoolArray*>(BaseArray::deserialize(new Deserialize(val->get_data())));
        return arr_->size();
    }

    size_t total_num_elements() {
        size_t result = 0;
        for(size_t i = 0; i < num_keys(); i++) {
            result += num_elements_per_chunk(dynamic_cast<Key*>(keys_.get(i)));
        }
        return result;
    }
};

// /*************************************************************************
//  * StringColumn::
//  * Holds string pointers. The strings are external.  Nullptr is a valid
//  * value.
//  */
class StringColumn : public Column {
public:
    Array keys_;
    KVStore* kv_;

    StringColumn() : keys_(10) {}

    StringColumn(KVStore* kv) : keys_(10) { kv_ = kv; }

    char* serialize(Serialize* s) {
        s->write(keys_.length());
        for(size_t i = 0; i < keys_.length(); i++) {
            Key* key = dynamic_cast<Key*>(keys_.get(i));
            key->serialize(*s);
        }
        return s->getChars();
    }

    static StringColumn* deserialize(Deserialize* d, KVStore* kv) {
        StringColumn* result = new StringColumn(kv);
        size_t size = d->readSizeT();
        Array* keys = new Array(size);
        for(size_t i = 0; i < size; i++) {
            Key* key = Key::deserialize(*d);
            keys->push(key);
        }
        result->keys_ = *keys;
        return result;
    };

    String* get(size_t idx) {
        size_t key_idx = idx / MAX_SIZE;
        size_t offset = idx % MAX_SIZE;
        Key* key = dynamic_cast<Key*>(get_key(key_idx));
        size_t node_num = kv_->get_node_num();

        Value* val = nullptr;
        if(node_num == key->home()) {
            val = kv_->get_(key);
        }
        else {
            val = kv_->waitAndGet_(key);
        }
        StringArray* arr_ = dynamic_cast<StringArray*>(BaseArray::deserialize(new Deserialize(val->get_data())));
        return arr_->get(offset);
    }

    StringColumn* as_string() {
        return this;
    }
        
    size_t num_keys() {
        return keys_.length();
    }

    // add the element and resize if necessary
    void push_back(String* val) {
        assert(false);
    }

    bool equals(Object* other) {
        if (other == nullptr) return false;
        if (other == this) return true;
        StringColumn* x = dynamic_cast<StringColumn*>(other);
        if (x == nullptr) return false;

        return keys_.equals(&x->keys_);
    }

    size_t hash() {
        assert(false);
    }

    void add_key(Key* k) {
        keys_.push(k);
    } 

    Key* get_key(size_t idx) {
        assert(keys_.get(idx));
        Key* key = dynamic_cast<Key*>(keys_.get(idx));
        return key;
    }   

    size_t num_elements_per_chunk(Key* k) {
        Value* val = kv_->waitAndGet_(k);
        StringArray* arr_ = dynamic_cast<StringArray*>(BaseArray::deserialize(new Deserialize(val->get_data())));
        return arr_->size();
    }

    size_t total_num_elements() {
        size_t result = 0;
        for(size_t i = 0; i < num_keys(); i++) {
            result += num_elements_per_chunk(dynamic_cast<Key*>(keys_.get(i)));
        }
        return result;
    }
};