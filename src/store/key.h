#pragma once
#include "../utils/string.h"
#include "../utils/object.h"
#include "./serial.h"

class Key : public Object {
public:
    String* name_;
    size_t home_;

    Key(String* name, size_t home) {
        name_ = name;
        home_ = home;
    }

    Key(Deserialize& d) {
        String* name = d.readString();
        size_t home = d.readSizeT();
        name_ = name;
        home_ = home;
    }

    ~Key() {
    }

    bool equals(Object* other) {
        // std::cout << "INSIDE EQUALS FOR KEY" << "\n";
        if (other == nullptr) return false;
        Key* key = dynamic_cast<Key*>(other);
        if (key == nullptr) return false;
        return name_->equals(key->name_) && home_ == key->home_;
    }

    char* serialize(Serialize& s) {
        s.write(name_);
        s.write(home_);
        return s.getChars();
    }

    static Key* deserialize(Deserialize& d) {
        return new Key(d);
    }

    size_t home() {
        return home_;
    }

    String* get_name() {
        return name_;
    }
};