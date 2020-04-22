#pragma once
#include "../store/serial.h"

// /*************************************************************************
//  * Schema::
//  * A schema is a description of the contents of a data frame, the schema
//  * knows the number of columns and number of rows, the type of each column,
//  * optionally columns and rows can be named by strings.
//  * The valid types are represented by the chars 'S', 'B', 'I' and 'D'.
//  */
class Schema : public Object {
public:
    // StringArray* types_;
    char* types_;
    size_t width_;
    
    /** Copying constructor */
    Schema(Schema& from) {
        types_ = from.types_;
        width_ = from.width_;
        
    }
    
    /** Create an empty schema **/
    Schema() {
        types_ = new char[1];
        types_[0] = 0;
        width_ = 0;
    }
    

    Schema(const char* types) {
        types_ = strdup(types);
        width_ = strlen(types_);
    }

    Schema(Deserialize* d) {
        width_ = d->readSizeT();
        types_ = d->readChars(width_);
    }

    const char* to_string() {
        return types_;
    }
    
    // serializes this schema
    void serialize(Serialize* s) {
        s->write(width_);
        s->write(types_);
    }

    // deserializes this schema
    static Schema* deserialize(Deserialize* d) {
        return new Schema(d);
    }

    /** Add a column of the given type and name (can be nullptr), name
        * is external. Names are expectd to be unique, duplicates result
        * in undefined behavior. */
    void add_column(char typ) {
        char* new_types = new char[width_ + 2];
        memcpy(new_types, types_, width_ + 1);
        types_ = new_types;
        width_++;
        types_[width_] = typ;
        types_[width_ + 1] = 0;
    }
    
    /** Updates the number of rows. No row names at the moment. */
    // void add_row() {
    //     row_count_++;
    // }
    
    /** Return type of column at idx. An idx >= width is undefined. */
    char col_type(size_t idx) {
        if (idx >= width()) {
           assert(false);
        }
        // need to extract the value since the char is wrapped in a string
        return types_[idx];
    }

    /** The number of columns */
    size_t width() {
        return width_;
    }
    
    // /** The number of rows */
    // size_t length() {
    //     return row_count_;
    // }

    bool equals(Object* other) {
        if (other == nullptr) return false;
        if (other == this) return true;
        Schema* x = dynamic_cast<Schema*>(other);
        if (x == nullptr) return false;

        return (strcmp(types_, x->types_) == 0) && width_ == x->width_;
    }
};
