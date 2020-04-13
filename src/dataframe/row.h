#pragma once
// /*************************************************************************
//  * Row::
//  *
//  * This class represents a single row of data constructed according to a
//  * dataframe's schema. The purpose of this class is to make it easier to add
//  * read/write complete rows. Internally a dataframe hold data in columns.
//  * Rows have pointer equality.
//  */
class Row : public Object {
public:

    Object** arrs_;
    Schema* schema_; // schema is external
    size_t idx_;

    /** Build a row following a schema. */
    Row(Schema& scm) {
        arrs_ = new Object*[scm.width()];
        for (int i = 0; i < scm.width(); i++) {
            if (scm.col_type(i) == 'I') {
                arrs_[i] = new IntArray();
            }
            else if (scm.col_type(i) == 'D') {
                arrs_[i] = new DoubleArray();
            }
            else if (scm.col_type(i) == 'B') {
                arrs_[i] = new BoolArray();
            }
            else if (scm.col_type(i) == 'S') {
                arrs_[i] = new StringArray();
                dynamic_cast<StringArray*>(arrs_[i])->push_back(nullptr);
            }
            else {
                // If the type is none of the prescribed types (i.e. set as "")
                // then we set the value to be a nullptr. Since there is no type, 
                // we cannot get or set at this index in the row since it is based on the type.
                arrs_[i] = nullptr;
            }
        }
        schema_ = &scm;
    }

    ~Row() { 
        delete[] arrs_;
        // delete schema_;
    }
    
    /** Setters: set the given column with the given value. Setting a column with
        * a value of the wrong type is undefined. */
    void set(size_t col, int val) {
        if (schema_->col_type(col) == 'I') {
            IntArray* list = dynamic_cast<IntArray*>(arrs_[col]);
            // if there is not an element in the row here, add it by calling push back
            if (list->size() == 0) {
                list->push_back(val);
            }
            // if there is an element in the row here, replace the element with the given one
            else {
                fprintf(stderr, "about to set in the row this val: %d\n", val);
                list->set(0, val);
            }
        }
        else {
            printf("the type of the column is not int. cannot set value.\n");
            return;
        }
        
    }

    void set(size_t col, double val) {
        if (schema_->col_type(col) == 'D') {
            DoubleArray* list = dynamic_cast<DoubleArray*>(arrs_[col]);
            // if there is not an element in the row here, add it by calling push back
            if (list->size() == 0) {
                list->push_back(val);
            }
            // if there is an element in the row here, replace the element with the given one
            else {
                list->set(0, val);
            }
        }
        else {
            printf("the type of the column is not double. cannot set value.\n");
            return;
        }
        
    }

    void set(size_t col, bool val) {
        if (schema_->col_type(col) == 'B') {
            BoolArray* list = dynamic_cast<BoolArray*>(arrs_[col]);
            // if there is not an element in the row here, add it by calling push back
            if (list->size() == 0) {
                fprintf(stderr, "about to push back val onto row\n");
                list->push_back(val);
            }
            // if there is an element in the row here, replace the element with the given one
            else {
                fprintf(stderr, "about to set since list size > 0\n");
                list->set(0, val);
            }
        }
        else {
            printf("the type of the column is not bool. cannot set value.\n");
            return;
        }
        
    }

    void set(size_t col, String* val) {
        // printf("in row's set for string: %s\n", val->cstr_);
        assert(schema_->col_type(col) == 'S');
        StringArray* array = dynamic_cast<StringArray*>(arrs_[col]);
        array->set(0, val);
    }
    
    /** Set/get the index of this row (ie. its position in the dataframe. This is
    *  only used for informational purposes, unused otherwise */
    void set_idx(size_t idx) {
        idx_ = idx;
    }
    size_t get_idx() {
        return idx_;
    }
    
    /** Getters: get the value at the given column. If the column is not
        * of the requested type, the result is undefined. */
    int get_int(size_t col) {
        if (schema_->col_type(col) == 'I') {
            IntArray* list = dynamic_cast<IntArray*>(arrs_[col]);
            return list->get(0);
        }
        else {
            printf("the type of the column is not int. cannot get value.");
            return 0;
        }
    }

    bool get_bool(size_t col) {
        if (schema_->col_type(col) == 'B') {
            BoolArray* list = dynamic_cast<BoolArray*>(arrs_[col]);
            return list->get(0);
        }
        else {
            printf("the type of the column is not bool. cannot get value.");
            return 0;
        }
    }

    double get_double(size_t col) {
        if (schema_->col_type(col) == 'D') {
            DoubleArray* list = dynamic_cast<DoubleArray*>(arrs_[col]);
            return list->get(0);
        }
        else {
            printf("the type of the column is not double. cannot get value.");
            return 0;
        }
    }

    String* get_string(size_t col) {
        assert(schema_->col_type(col) == 'S');
        StringArray* list = dynamic_cast<StringArray*>(arrs_[col]);
        return list->get(0);
    }
    
    /** Number of fields in the row. */
    size_t width() {
        return schema_->width();
    }
    
    /** Type of the field at the given position. An idx >= width is  undefined. */
    char col_type(size_t idx) {
        return schema_->col_type(idx);
    }
    
};