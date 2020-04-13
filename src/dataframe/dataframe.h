#pragma once
#include <stdio.h>
#include <thread>
#include "../utils/object.h"
#include "../utils/string.h"
#include "./column.h"
#include "./columnList.h"
#include "./row.h"
#include "../utils/helper.h"
#include "../store/serial.h"
#include "../store/key.h"
#include "./writer.h"
#include "./adder.h"
#include "../network/network_pseudo.h"

class KDStore;

/****************************************************************************
 * DataFrame::
 *
 * A DataFrame is table composed of columns of equal length. Each column
 * holds values of the same type (I, S, B, F). A dataframe has a schema that
 * describes it.
 */
class DataFrame : public Object {
public:
    ColumnList* columns_;
    Schema* schema_;
    size_t num_rows_with_data_;

    /** Create a data frame with the same columns as the given df but with no rows or rownames */
    DataFrame(DataFrame& df) {
        schema_ = df.schema_;
        columns_ = df.columns_;
        num_rows_with_data_ = df.num_rows_with_data_;
    }
    
    /** Create a data frame from a schema and columns. All columns are created
        * empty. */
    DataFrame(Schema& schema) {
        schema_ = &schema;
        columns_ = new ColumnList();
        num_rows_with_data_ = 0;

        for (int i = 0; i < schema.width(); i++) {
            if (schema.col_type(i) == 'I') {
                columns_->push_back(new IntColumn());
            }
            else if (schema.col_type(i) == 'D') {
                columns_->push_back(new DoubleColumn());
            }
            else if (schema.col_type(i) == 'B') {
                columns_->push_back(new BoolColumn());
            }
            else if (schema.col_type(i) == 'S') {
                columns_->push_back(new StringColumn());
            }
            else {
                // If the type is none of the prescribed types (i.e. set as "")
                // then we set the value to be a nullptr. Since there is no type, 
                // we cannot get or set at this index in the row since it is based on the type.
                columns_->push_back(nullptr);
            }
        }
    }

    DataFrame(Schema& schema, KVStore* kv) {
        schema_ = &schema;
        columns_ = new ColumnList();
        num_rows_with_data_ = 0;

        for (int i = 0; i < schema.width(); i++) {
            if (schema.col_type(i) == 'I') {
                columns_->push_back(new IntColumn(kv));
            }
            else if (schema.col_type(i) == 'D') {
                columns_->push_back(new DoubleColumn(kv));
            }
            else if (schema.col_type(i) == 'B') {
                columns_->push_back(new BoolColumn(kv));
            }
            else if (schema.col_type(i) == 'S') {
                columns_->push_back(new StringColumn(kv));
            }
            else {
                // If the type is none of the prescribed types (i.e. set as "")
                // then we set the value to be a nullptr. Since there is no type, 
                // we cannot get or set at this index in the row since it is based on the type.
                columns_->push_back(nullptr);
            }
        }
    }

    DataFrame() {
        schema_ = new Schema();
        columns_ = new ColumnList();
        num_rows_with_data_ = 0;
    }

    DataFrame(ColumnList* columns, Schema* schema, size_t num_rows_with_data) {
        columns_ = columns;
        schema_ = schema;
        num_rows_with_data_ = num_rows_with_data;
    }

    ~DataFrame() {
        delete schema_;
        delete columns_;
    }

    char* serialize(Serialize* s) {
        schema_->serialize(s);

        columns_->serialize(s);

        s->write(num_rows_with_data_);

        return s->getChars();
    }

    static DataFrame* deserialize(Deserialize* d, KVStore* kv) {
        Schema* schema = Schema::deserialize(d);

        ColumnList* columns = ColumnList::deserialize(d, schema, kv);

        size_t num_rows_with_data = d->readSizeT();

        DataFrame* df = new DataFrame(columns, schema, num_rows_with_data);

        return df;
    }

    static DataFrame* fromVisitor(Key* in, KVStore* kv, const char* schema, Writer &writer) {
        // Schema sch{schema};
        Schema* sch = new Schema(schema);
        DataFrame* result = new DataFrame(*sch, kv);
        size_t chunk_idx = 0;
        Array base_arrays(sch->width());
        for (size_t i = 0; i < sch->width(); i++) {
            if (sch->col_type(i) == 'S') {
                base_arrays.push(new StringArray());
            }
            else if (sch->col_type(i) == 'I') {
                base_arrays.push(new IntArray());
            }
            else if (sch->col_type(i) == 'D') {
                base_arrays.push(new DoubleArray());
            }
            else {
                base_arrays.push(new BoolArray());
            }
        }
        size_t count = 0;  
        while (!writer.done()) {
            Row r{*sch};
            writer.visit(r);
            for (size_t i = 0; i < r.width(); i++) {
                if (sch->col_type(i) == 'S') {
                    StringArray* arr = dynamic_cast<StringArray*>(base_arrays.get(i));
                    std::cout << "PUSHING back a STRING to the df: " << r.get_string(i)->cstr_ << "\n"; 
                    arr->push_back(r.get_string(i));
                }
                else if (sch->col_type(i) == 'I') {
                    IntArray* arr = dynamic_cast<IntArray*>(base_arrays.get(i));
                    arr->push_back(r.get_int(i));
                    std::cout << "pushing back an INT to the df: " << r.get_int(i) << "\n"; 
                }
                else if (sch->col_type(i) == 'D') {
                    DoubleArray* arr = dynamic_cast<DoubleArray*>(base_arrays.get(i));
                    arr->push_back(r.get_double(i));
                }
                else {
                    BoolArray* arr = dynamic_cast<BoolArray*>(base_arrays.get(i));
                    arr->push_back(r.get_bool(i));
                }
            }
            count++;
            if (count >= MAX_SIZE) {
                count = 0;
                // generate key and distribute all the chunks
                for (size_t i = 0; i < sch->width(); i++) {
                    StrBuff buff;
                    buff.c(*in->get_name()).c("-k-").c(i).c("-").c(chunk_idx);
                    size_t node_num = chunk_idx % NUM_NODES;
                    String* buff_val = buff.get();
                    Key* key = new Key(buff_val, node_num);

                    BaseArray* arr = dynamic_cast<BaseArray*>(base_arrays.get(i));
                    Value* val = new Value(arr->serialize(new Serialize()));
                    kv->put_(key, val);
                    result->columns_->get(i)->add_key(key);
                }
                chunk_idx++;
                // reset all the chunks
                for (size_t i = 0; i < sch->width(); i++) {
                    if (sch->col_type(i) == 'S') {
                        base_arrays.set(new StringArray(), i);
                    }
                    else if (sch->col_type(i) == 'I') {
                        base_arrays.set(new IntArray(), i);
                    }
                    else if (sch->col_type(i) == 'D') {
                        base_arrays.set(new DoubleArray(), i);
                    }
                    else {
                        base_arrays.set(new BoolArray(), i);
                    }
                }
            }
        }
        // grab the last chunk if it is < MAX_SIZE
        if (count % MAX_SIZE != 0) {
            for (size_t i = 0; i < sch->width(); i++) {
                StrBuff buff;
                buff.c(*in->get_name()).c("-k-").c(i).c("-").c(chunk_idx);
                size_t node_num = chunk_idx % NUM_NODES;
                Key* key = new Key(new String(buff.val_), node_num);

                BaseArray* arr = dynamic_cast<BaseArray*>(base_arrays.get(i));
                Value* val = new Value(arr->serialize(new Serialize()));
                kv->put_(key, val);            
                result->columns_->get(i)->add_key(key);
            }
        }

        // add the df to the kv store at the in key
        Value* val = new Value(result->serialize(new Serialize()));
        kv->distribute_df_to_all(in, val);

        return result;
    }

    bool equals(Object* other) {
        if (other == nullptr) return false;
        if (other == this) return true;
        DataFrame* x = dynamic_cast<DataFrame*>(other);
        if (x == nullptr) return false;

        return columns_->equals(x->columns_) && schema_->equals(x->schema_)
                && num_rows_with_data_ == x->num_rows_with_data_;
    }

    // The implementation of these methods can be found in the KDStore class
    static DataFrame* fromArray(Key* key, KDStore* kd, size_t length, double* vals);
    static DataFrame* fromArray(Key* key, KDStore* kd, size_t length, int* vals);
    static DataFrame* fromArray(Key* key, KDStore* kd, size_t length, bool* vals);
    static DataFrame* fromArray(Key* key, KDStore* kd, size_t length, String** vals);
    static DataFrame* fromScalar(Key* key, KDStore* kd, double val);
    static DataFrame* fromScalar(Key* key, KDStore* kd, int val);
    static DataFrame* fromScalar(Key* key, KDStore* kd, String* val);
    static DataFrame* fromScalar(Key* key, KDStore* kd, bool val);

    /** Returns the dataframe's schema. Modifying the schema after a dataframe
        * has been created is invalid, and the dataframe will be broken. */
    Schema& get_schema() {
        return *schema_;
    }
    
    /** Adds a column to this dataframe, updates the schema, the new column
        * is external, and appears as the last column of the dataframe, the
        * name is optional and external. A nullptr column is not allowed. */
    void add_column(Column* col) {
        if (col == nullptr) {
            printf("cannot add nullpointer columns.");
            return;
        }
        char type = col->get_type();
        schema_->add_column(type);
        columns_->push_back(col);
        if (num_rows_with_data_ < col->num_keys()) {
            num_rows_with_data_ = col->num_keys();
        }
    }
    
    int get_int(size_t col, size_t row) {
        if (col >= columns_->size()) {
            printf("column number is out of bounds.\n");
            return 0;
        }

        Column* column = columns_->get(col);
        if (row >= column->total_num_elements()) {
            printf("row number is out of bounds in get_int() in df.\n");
            return 0;
        }
        if (column->get_type() != 'I') {
            printf("not an int column.\n");
            return 0;
        }

        IntColumn* intcol = column->as_int();
        
        int res = intcol->get(row);
        return res;
    }

    bool get_bool(size_t col, size_t row) {
        if (col >= columns_->size()) {
            printf("column number is out of bounds.\n");
            return 0;
        }
        Column* column = columns_->get(col);
        if (row >= column->total_num_elements()) {
            printf("row number is out of bounds in get_bool() in df.\n");
            return 0;
        }
        if (column->get_type() != 'B') {
            printf("not an bool column.");
            return 0;
        }
        BoolColumn* boolcol = column->as_bool();
        bool res = boolcol->get(row);
        return res;
    }

    double get_double(size_t col, size_t row) {
        if (col >= columns_->size()) {
            printf("column number is out of bounds.\n");
            return 0;
        }
        Column* column = columns_->get(col);
        if (row >= column->total_num_elements()) {
            printf("row number is out of bounds in get_double() in df.\n");
            return 0;
        }
        if (column->get_type() != 'D') {
            printf("not an double column.\n");
            return 0;
        }
        DoubleColumn* doublecol = column->as_double();
        double res = doublecol->get(row);
        return res;
    }

    String* get_string(size_t col, size_t row) {
        if (col >= columns_->size()) {
            printf("column number is out of bounds.\n");
            return 0;
        }
        Column* column = columns_->get(col);
        if (row >= column->total_num_elements()) {
            printf("row number is out of bounds in get_string() in df.\n");
            return 0;
        }
        if (column->get_type() != 'S') {
            printf("not an string column.\n");
            return 0;
        }
        StringColumn* stringcol = column->as_string();
        String* res = stringcol->get(row);
        return res;
    }
    
    /** Set the fields of the given row object with values from the columns at
        * the given offset.  If the row is not from the same schema as the
        * dataframe, results are undefined.
        */
    void fill_row(size_t idx, Row& row) {
        for (size_t i = 0; i < columns_->size(); i++) {
            Column* column = columns_->get(i);
            char type = column->get_type();
            char row_elt_type = row.col_type(i);
            if (type == 'I') {
                int val = get_int(i, idx);
                if (type == row_elt_type) {
                    row.set(i, val);
                }
                else {
                    printf("not the right type");
                    return;
                }
            }
            else if (type == 'D') {
                double f = get_double(i, idx);
                if (type == row_elt_type) {
                    row.set(i, f);
                }
                else {
                    printf("not the right type");
                    return;
                }
            }
            else if (type == 'B') {
                bool b = get_bool(i, idx);
                if (type == row_elt_type) {
                    row.set(i, b);
                }
                else {
                    printf("not the right type");
                    return;
                }
            }
            else {
                String* s = get_string(i, idx);
                if (type == row_elt_type) {
                    row.set(i, s);
                }
                else {
                    printf("not the right type");
                    return;
                }
            }            
        }
    }
    
    /** The number of rows in the dataframe. */
    size_t nrows() {
        return schema_->length();
    }
    
    /** The number of columns in the dataframe.*/
    size_t ncols() {
        return schema_->width();
    }

    // Gets the number of keys in each column of the dataframe.
    // The dataframe must have the same number of keys per column.
    // Assumes that all the columns are of the same length
    size_t get_num_keys_per_column_() {
        return columns_->get(0)->num_keys();
    }
    
    void local_map(Adder& add, size_t node_num) {
        std::cout << "INSIDE LOCAL MAP\n";
        for (size_t i = 0; i < columns_->size(); i++) {
            for (size_t j = 0; j < get_num_keys_per_column_(); j++) {
                Key* key = columns_->get(i)->get_key(j);
                if (key->home() == node_num) {
                    for (size_t k = 0; k < columns_->get(i)->num_elements_per_chunk(key); k++) {
                        Row r{*schema_};
                        fill_row((MAX_SIZE * j) + k, r);
                        add.visit(r);
                    }
                }                
            }
        }
    }

    // Grab the first column since every column has to have the same number of elements
    void map(Adder& add) {
        Column* col = columns_->get(0);
        for(size_t j = 0; j < col->total_num_elements(); j++) {
            Row r{*schema_};
            fill_row(j, r);
            add.visit(r);  
        }
    }
};