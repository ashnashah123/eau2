#include <assert.h>
#include "./dataframe/dataframe.h"
#include <stdio.h>
#include <stdarg.h>
#include "./sorer/sorer.h"
#include "./application_layer/wordCount.h"
#include <stdlib.h>
#include <string.h>

/***************************** SERIALIZATION TESTS *******************************/
void test_serialize_int() {
    Serialize* s = new Serialize();

    int i = 10;
    s->write(i);
    char* result = s->getChars();

    Deserialize* d = new Deserialize(result);

    int deserialized_i = d->readInt();
    assert(deserialized_i == i);
}

void test_serialize_sizet() {
    Serialize* s = new Serialize();

    size_t i = 10;
    s->write(i);
    char* result = s->getChars();
        
    Deserialize* d = new Deserialize(result);

    size_t deserialized_i = d->readSizeT();
    assert(deserialized_i == i);
}

void test_serialize_string() {
    Serialize* s = new Serialize();

    String* str = new String("Ashna is my favorite");
    s->write(str);
    char* result = s->getChars();
	
    Deserialize* d = new Deserialize(result);

    String* st = d->readString();
    assert(st->equals(str));
}

void test_serialize_string_and_int() {
    Serialize* s = new Serialize();

    String* str = new String("Yo let's make friends with an integer");
    int i = 234;

    s->write(str);
    s->write(i);
    char* result = s->getChars();

    Deserialize* d = new Deserialize(result);

    String* deserialized_str = d->readString();
    int deserialized_i = d->readInt();

    assert(deserialized_str->equals(str));
    assert(deserialized_i == i);
}

void test_serialize_double() {
    Serialize* s = new Serialize();

    double f = 10.0;
    s->write(f);
    char* result = s->getChars();

    Deserialize* d = new Deserialize(result);

    double deserialized_f = d->readDouble();
    assert(deserialized_f == f);
}

void test_serialize_bool() {
    Serialize* s = new Serialize();

    bool b = false;
    s->write(b);
    char* result = s->getChars();

    Deserialize* d = new Deserialize(result);

    bool deserialized_b = d->readBool();
    assert(deserialized_b == b);
}

void test_serialize_register_message() {
    Serialize s;
    sockaddr_in sender;
    Register* register_message = new Register(1, 8081);

    register_message->serialize(s);
    char* result = s.getChars();
	
    Deserialize d(result);

    Message* deserialized_register_message = Message::deserialize(d, sender);

    assert(register_message->equals(deserialized_register_message));
    std::cout << "success!!\n";
}




/***************************** Constructing DF from Sorer text file test *******************************/

// void test_read_data_and_build_df() {
//     Schema* s = new Schema("BDDSDIBDSD");

//     DataFrame* df = new DataFrame(*s);
//     //CITATION: https://www.programiz.com/c-programming/examples/read-file
//     FILE *fptr = nullptr;
//     char c[10000];
//     if ((fptr = fopen("datafile.txt", "r")) == NULL) {
//         printf("Error! opening file");
//         // Program exits if file pointer returns NULL.
//         exit(1);
//     }
//     while (fgets(c, 10000, fptr) !=  NULL) {
//         char* split_str = strtok(c, ",");

//         Row* r = new Row(*s);
//         int i = 0;
//         while(split_str != NULL) {
//             if (i == 0) {
//                 bool b = 1;
//                 if (*split_str == '1') {
//                     b = 1;
//                 }
//                 else {
//                     b = 0;
//                 }
//                 r->set(0, b);
//             }
//             else if (i == 1) {
//                 const char* s = split_str;
//                 double f = atof(s);
//                 r->set(1, f);
//             }
//             else if (i == 2) {
//                 const char* s = split_str;
//                 double f = atof(s);
//                 r->set(2, f);
//             }
//             else if (i == 3) {
//                 const char* s = split_str;
//                 String* new_string = new String(s);
//                 r->set(3, new_string);
//             }
//             else if (i == 4) {
//                 const char* s = split_str;
//                 double f = atof(s);
//                 r->set(4, f);
//             }
//             else if (i == 5) {
//                 const char* s = split_str;
//                 int int_val = atoi(s);
//                 r->set(5, int_val);
//             }
//             else if (i == 6) {
//                 bool b = 1;
//                 if (*split_str == '1') {
//                     b = 1;
//                 }
//                 else {
//                     b = 0;
//                 }
//                 r->set(6, b);
//             }
//             else if (i == 7) {
//                 const char* s = split_str;
//                 double f = atof(s);
//                 r->set(7, f);
//             }
//             else if (i == 8) {
//                 const char* s = split_str;
//                 String* new_string = new String(s);
//                 r->set(8, new_string);
//             }
//             else {
//                 const char* s = split_str;
//                 double f = atof(s);
//                 r->set(9, f);
//             }
//             split_str = strtok(NULL, ",\n ");
            
//             i++;
//         }
//         df->add_row(*r);
//         fprintf(stderr, "successfully added row\n");
//     }

    // int k = 0;
    // while(k < 5) {
    // /***MAP TIMING FOR SUM****/
    //     // std::chrono::high_resolution_clock::time_point h_start, h_end;
    //     auto h_start = std::chrono::high_resolution_clock::now();

        
    //     SumRower* sumrower = new SumRower();
    //     df->map(*sumrower);
        
    //     auto h_end = std::chrono::high_resolution_clock::now();
        
    //     auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(h_end - h_start);
    //     int count_map = duration.count();
    //     std::cout << "highres time of sumrower with map= " << count_map << " milliseconds \n";

    // /***MAP TIMING FOR FIB****/
    //     auto h_start_fib_map = std::chrono::high_resolution_clock::now();
        
    //     FibRower* fibrower = new FibRower();
    //     df->map(*fibrower);
        
    //     auto h_end_fib_map = std::chrono::high_resolution_clock::now();
        
    //     auto duration_fib_map = std::chrono::duration_cast<std::chrono::milliseconds>(h_end_fib_map - h_start_fib_map);
    //     int count_map_fib = duration_fib_map.count();
    //     std::cout << "highres time of fibrower with map= " << count_map_fib << " milliseconds \n";

    //     k++;
    // }

//     delete df;
// }


// DataFrame* createDataFrame(int argc, char* argv[]) {
//     fprintf(stderr, "returning the column set\n");

//     // Parse arguments
//     char* filename = nullptr;
//     // -1 represents argument not provided
//     ssize_t start = -1;
//     ssize_t len = -1;
//     ssize_t col_type = -1;
//     ssize_t col_idx_col = -1;
//     ssize_t col_idx_off = -1;
//     ssize_t missing_idx_col = -1;
//     ssize_t missing_idx_off = -1;

//     parse_args(argc, argv, &filename, &start, &len, &col_type, &col_idx_col, &col_idx_off,
//                &missing_idx_col, &missing_idx_off);

//     // Check arguments
//     if (filename == nullptr) {
//         printf("No file provided\n");
//         return 0;
//     }

//     // Open requested file
//     FILE* file = fopen(filename, "r");
//     if (file == NULL) {
//         printf("Failed to open file\n");
//         return 0;
//     }
//     fseek(file, 0, SEEK_END);
//     size_t file_size = ftell(file);
//     fseek(file, 0, SEEK_SET);

//     // Set argument defaults
//     if (start == -1) {
//         start = 0;
//     }

//     if (len == -1) {
//         len = file_size - start;
//     }

//     {
//         // Run parsing
//         SorParser parser{file, (size_t)start, (size_t)start + len, file_size};
//         parser.guessSchema();
//         parser.parseFile();
//         ColumnSet* columnSet = parser.getColumnSet();

//         // create a string of the schema to pass to our dataframe for creation
//         ColumnType* types = parser._typeGuesses;
//         StrBuff* str = new StrBuff();

//         for (int i = 0; i < parser._num_columns; i++) {
//             const char* val = columnTypeToString(types[i]);
//             char first_letter = val[0];
//             str->c(first_letter);
//         }
//         char* scm = str->get()->cstr_;

//         Schema* schema = new Schema(scm);
//         DataFrame* df = new DataFrame(*schema);

//         // fclose(file);
//         for (int i = 0; i < parser._num_columns; i++) {
//             BaseColumn* bc = columnSet->getColumn(i);
//             if (strcmp(columnTypeToString(types[i]), "STRING") == 0) {
//                 StringColumn* strcol = dynamic_cast<StringColumn*>(df->columns_->get(i));
//                 StringColumnSorer* sr = dynamic_cast<StringColumnSorer*>(bc);
//                 for (int j = 0; j < sr->getLength(); j++) {
//                     String* newString = new String(sr->getEntry(j));
//                     // fprintf(stderr, "newString: %s\n", newString->cstr_);
//                     strcol->push_back(newString);

//                     String* val = dynamic_cast<StringColumn*>(df->columns_->get(i))->get(j);
//                     // printf("value in df itself: %s \n", df->get_string(i,j)->cstr_);
//                     fprintf(stderr, "val after put in our df: %s\n", val->cstr_);
//                 }
//             }
//             else if (strcmp(columnTypeToString(types[i]), "INTEGER") == 0) {
//                 IntColumn* intcol = dynamic_cast<IntColumn*>(df->columns_->get(i));
//                 IntegerColumn* ic = dynamic_cast<IntegerColumn*>(bc);
//                 for (int j = 0; j < ic->getLength(); j++) {
//                     // fprintf(stderr, "int: %d\n", ic->getEntry(j));

//                     intcol->push_back(ic->getEntry(j));
//                 }
//             }
//             else if (strcmp(columnTypeToString(types[i]), "BOOL") == 0) {
//                 BoolColumn* boolcol = dynamic_cast<BoolColumn*>(df->columns_->get(i));
//                 BoolColumnSorer* bcr = dynamic_cast<BoolColumnSorer*>(bc);
//                 for (int j = 0; j < bcr->getLength(); j++) {
//                     // fprintf(stderr, "bool: %d\n", bcr->getEntry(j));

//                     boolcol->push_back(bcr->getEntry(j));
//                 }
//             }
//             else if (strcmp(columnTypeToString(types[i]), "FLOAT") == 0) {
//                 DoubleColumn* doublecol = dynamic_cast<DoubleColumn*>(df->columns_->get(i));
//                 DoubleColumnSorer* fc = dynamic_cast<DoubleColumnSorer*>(bc);
//                 for (int j = 0; j < fc->getLength(); j++) {
//                     // fprintf(stderr, "double: %d\n", fc->getEntry(j));

//                     doublecol->push_back(fc->getEntry(j));
//                 }
//             }
//             else {
//                 printf("type unknown");
//             }
//         }

//         return df;
//     }

//     fclose(file);
// }



int main(int argc, char* argv[]) {
    // size_t num_nodes = atoi(argv[2]);
    // size_t index = atoi(argv[4]);
    // std::cout << "INDEX: " << index << "\n";
    // size_t port = atoi(argv[6]);
    // std::cout << "atoi(argv[6]): " << port << "\n";

    // size_t masterport = 0;
    // char* masterip = nullptr;
    
    // if (index > 0) {
    //     size_t masterport = atoi(argv[8]);
    //     char* masterip = argv[10];        
    // }
    // for (size_t i = 0; i < index; i++) {
    //     NetworkIp* network = new NetworkIp(index, port, masterport, masterip);
    //     network->start();

    //     KVStore* kv = new KVStore(*network, index);
    //     WordCount* wc = new WordCount(index, *kv, *network);

    //     wc->start();
    //     wc->join();
    //     network->join();
    // }

    
    // NetworkIp* network_1 = new NetworkIp();
    // KVStore* kv1 = new KVStore(*network_1, 1);
    // WordCount* wc1 = new WordCount(1, *kv1, *network_1);
    // wc1->start();
    // WordCount* wc2 = new WordCount(2, *kv, *network_);
    // wc2->start();

    
    // wc1->join();
    // wc2->join();

    // DataFrame* df = createDataFrame(argc, argv);

    test_serialize_int();
    test_serialize_sizet();
    test_serialize_string();
    test_serialize_string_and_int();
    test_serialize_double();
    test_serialize_bool();

    test_serialize_register_message();

    return 0;
}