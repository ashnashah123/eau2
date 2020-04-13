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
    StringArray* types_;
    size_t row_count_;
    
    /** Copying constructor */
    Schema(Schema& from) {
        types_ = from.types_;
        row_count_ = from.row_count_;
    }
    
    /** Create an empty schema **/
    Schema() {
        types_ = new StringArray();
        row_count_ = 0;
    }
    
    /** Create a schema from a string of types. A string that contains
        * characters other than those identifying the four type results in
        * undefined behavior. The argument is external, a nullptr argument is
        * undefined. **/
    Schema(const char* types) {
        // if types is nullptr, then just construct an empty schema 
        if (types == nullptr) {
            Schema();
        }
        types_ = new StringArray();
        row_count_ = 0;
        for(size_t i = 0; i < strlen(types); i++) {
            if (types[i] == 'S' || types[i] == 'D' || types[i] == 'B' || types[i] == 'I') {
                const char* c = &types[i];
                String* s = new String(c);
                types_->push_back(s);
            }
            else {
                const char* c = "";
                types_->push_back(new String(c));
            }
        }
    }

    // default constructor for all fields of a schema
    Schema(StringArray* types, size_t row_count) {
        types_ = types;
        row_count_ = row_count;
    }

    ~Schema() {
        delete types_;
    }
    
    // serializes this schema
    char* serialize(Serialize* s) {
        types_->serialize(s);

        s->write(row_count_);

        return s->getChars();
    }

    // deserializes this schema
    static Schema* deserialize(Deserialize* d) {
        StringArray* types = dynamic_cast<StringArray*>(BaseArray::deserialize(d));

        size_t row_count = d->readSizeT();

        return new Schema(types, row_count);
    }

    /** Add a column of the given type and name (can be nullptr), name
        * is external. Names are expectd to be unique, duplicates result
        * in undefined behavior. */
    void add_column(char typ) {
        if (typ == 'S' || typ == 'D' || typ == 'B' || typ == 'I') {
            StrBuff buff;
            buff.c(typ);
            String* s = buff.get();
            types_->push_back(s);
        }
        else {
            types_->push_back(nullptr);
        }   
    }
    
    /** Updates the number of rows. No row names at the moment. */
    void add_row() {
        row_count_++;
    }
    
    /** Return type of column at idx. An idx >= width is undefined. */
    char col_type(size_t idx) {
        if (idx >= types_->size()) {
            printf("index out of bounds. cannot return col type.");
        }
        String* type_string = types_->get(idx);
        if (type_string == nullptr) {
            return '\0';
        }
        // need to extract the value since the char is wrapped in a string
        return *type_string->cstr_;
    }

    /** The number of columns */
    size_t width() {
        return types_->size();
    }
    
    /** The number of rows */
    size_t length() {
        return row_count_;
    }

    bool equals(Object* other) {
        if (other == nullptr) return false;
        if (other == this) return true;
        Schema* x = dynamic_cast<Schema*>(other);
        if (x == nullptr) return false;

        return types_->equals(x->types_);
    }
};
