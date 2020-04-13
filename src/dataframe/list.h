#pragma once
#include "../store/serial.h"
#include "../utils/array.h"

class StringArray;
class DoubleArray;
class IntArray;
class BoolArray;

#define MAX_SIZE 5

union Payload {
public:
    String* s_;
    int i_; 
    double d_;
    bool b_; 
};

class DFData : public Object {
public:
    bool missing_;
    Payload payload_;

    DFData() {}

    bool equals(Object* other) { 
        DFData* l = dynamic_cast<DFData*>(other);
        if (l == nullptr) return false;

        return missing_ == l->missing_
        // && payload_.d_ == l->payload_.d_ ;
        // && payload_.i_ == l->payload_.i_;
        // && payload_.b_ == l->payload_.b_ ;
        && payload_.s_->equals(l->payload_.s_);
    }
};

//*********** ARRAY (CHUNK) CLASSES ****************
class BaseArray : public Object{
public:
    DFData* array_;
    size_t num_elements_ = 0;
    
    char type_;
    size_t capacity_;

    BaseArray(char type) : type_(type) {
        capacity_ = MAX_SIZE;
        array_ = new DFData[MAX_SIZE];   
    }

    ~BaseArray() {}

    size_t size() { return num_elements_; }

    bool equals(Object* other) {
        BaseArray* l = dynamic_cast<BaseArray*>(other);
        if (l == nullptr) return false;
        bool equal_arrays = true;
        for(size_t i = 0; i < num_elements_; i++) {
            DFData* l_array = l->array_;
            equal_arrays = equal_arrays && array_[i].equals(&l_array[i]);
        }
        return equal_arrays && num_elements_ == l->num_elements_ && type_ == l->type_;
    }

    size_t hash() { assert(false); }

    DFData get_(size_t idx) {
        assert(idx < num_elements_);
        return array_[idx];
    }

    void push_back_(DFData data) {
        array_[num_elements_] = data;
        // fprintf(stderr, "ITEM IN BASE ARRAY array_ %s\n", array_[num_elements_].payload_.s_->cstr_);
        num_elements_++;
    }

    void set_(size_t idx, DFData data) {
        assert(idx < num_elements_);
        array_[idx] = data;
    }

    char* serialize(Serialize* s) {
        std::cout << "BASE ARRAY SERIALIZE TYPE: " << type_ << "\n";
        s->write(capacity_);
        s->write(type_);
        s->write(num_elements_);

        for (size_t i = 0; i < num_elements_; i++) {
            DFData element = array_[i];
            s->write(element.missing_);
            if (type_ == 'S') {
                s->write(element.payload_.s_);
            }
            else if (type_ == 'I') {
                s->write(element.payload_.i_);
            }
            else if (type_ == 'B') {
                s->write(element.payload_.b_);
            }
            else {
                s->write(element.payload_.d_);
            }
        }
        return s->getChars();
    }

    static BaseArray* deserialize(Deserialize* d);
    
};

class StringArray : public BaseArray  {
public:

    StringArray() : BaseArray('S') {}
    
    void push_back(String* val) {
        DFData dfd;
        dfd.payload_.s_ = val;
        dfd.missing_ = false;
        push_back_(dfd);

    }

    String* get(size_t idx) {
        DFData dfd = get_(idx);
        if (dfd.missing_) {
            return nullptr;
        }
        return dfd.payload_.s_;
    }

    void set(size_t idx, String* val) {
        DFData dfd;
        dfd.payload_.s_ = val;
        dfd.missing_ = false;
        set_(idx, dfd);
    }
};

class DoubleArray : public BaseArray  {
public:

    DoubleArray() : BaseArray('D') {}
    
    void push_back(double val) {
        DFData dfd;
        dfd.payload_.d_ = val;
        dfd.missing_ = false;
        push_back_(dfd);
    }

    double get(size_t idx) {
        DFData dfd = get_(idx);
        assert(!dfd.missing_);
        return dfd.payload_.d_;
    }

    void set(size_t idx, double val) {
        DFData dfd;
        dfd.payload_.d_ = val;
        dfd.missing_ = false;
        set_(idx, dfd);
    }
};

class IntArray : public BaseArray  {
public:

    IntArray() : BaseArray('I') {}
    
    void push_back(int val) {
        DFData dfd;
        dfd.payload_.i_ = val;
        dfd.missing_ = false;
        push_back_(dfd);
    }

    int get(size_t idx) {
        DFData dfd = get_(idx);
        assert(!dfd.missing_);
        return dfd.payload_.i_;
    }

    void set(size_t idx, int val) {
        DFData dfd;
        dfd.payload_.i_ = val;
        dfd.missing_ = false;
        set_(idx, dfd);
    }
};

class BoolArray : public BaseArray  {
public:

    BoolArray() : BaseArray('B') {}
    
    void push_back(bool val) {
        DFData dfd;
        dfd.payload_.b_ = val;
        dfd.missing_ = false;
        push_back_(dfd);
    }

    bool get(size_t idx) {
        DFData dfd = get_(idx);
        assert(!dfd.missing_);
        return dfd.payload_.b_;
    }

    void set(size_t idx, bool val) {
        DFData dfd;
        dfd.payload_.b_ = val;
        dfd.missing_ = false;
        set_(idx, dfd);
    }
};

BaseArray* BaseArray::deserialize(Deserialize* d) {
    std::cout << "about to deserialize a basearray \n";
    size_t cap = d->readSizeT(); // read in size_t capacity NECESSARY
    std::cout << "BASE ARRAY DESERIALIZE capacity BEING READ IN: " << cap << "\n";

    char type = d->readChar();
    std::cout << "BASE ARRAY DESERIALIZE TYPE BEING READ IN: " << type << "\n";
    size_t num_elements = d->readSizeT();
    std::cout << "BASE ARRAY DESERIALIZE num elements BEING READ IN: " << num_elements << "\n";

    BaseArray* result;
    switch(type) {
    case 'S' : result = new StringArray(); break;
    case 'I' : result = new IntArray(); break;
    case 'B' : result = new BoolArray(); break;
    case 'D' : result = new DoubleArray(); break;
    }

    for (size_t i = 0; i < num_elements; i++) {
        DFData element;
        element.missing_ = d->readBool();
        if (type == 'S') {
            element.payload_.s_ = d->readString();
        }
        else if (type == 'I') {
            element.payload_.i_ = d->readInt();
        }
        else if (type == 'B') {
            element.payload_.b_ = d->readBool();
        }
        else {
            element.payload_.d_ = d->readDouble();
        }

        if (type == 'S') {
            dynamic_cast<StringArray*>(result)->push_back(element.payload_.s_);
        }
        else if (type == 'I') {
            dynamic_cast<IntArray*>(result)->push_back(element.payload_.i_);
        }
        else if (type == 'B') {
            dynamic_cast<BoolArray*>(result)->push_back(element.payload_.b_);
        }
        else {
            dynamic_cast<DoubleArray*>(result)->push_back(element.payload_.d_);
        }                        
    }
    
    return result;
}