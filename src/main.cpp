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

    delete s;
    delete d;
}

void test_serialize_sizet() {
    Serialize* s = new Serialize();

    size_t i = 10;
    s->write(i);
    char* result = s->getChars();
        
    Deserialize* d = new Deserialize(result);

    size_t deserialized_i = d->readSizeT();
    assert(deserialized_i == i);

    delete s;
    delete d;
}

void test_serialize_string() {
    Serialize* s = new Serialize();

    String* str = new String("Ashna is my favorite");
    s->write(str);
    char* result = s->getChars();
	
    Deserialize* d = new Deserialize(result);

    String* st = d->readString();
    assert(st->equals(str));

    delete s;
    delete d;
    delete str;
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

    delete s;
    delete d;
    delete str;
}

void test_serialize_double() {
    Serialize* s = new Serialize();

    double f = 10.0;
    s->write(f);
    char* result = s->getChars();

    Deserialize* d = new Deserialize(result);

    double deserialized_f = d->readDouble();
    assert(deserialized_f == f);

    delete s;
    delete d;
}

void test_serialize_bool() {
    Serialize* s = new Serialize();

    bool b = false;
    s->write(b);
    char* result = s->getChars();

    Deserialize* d = new Deserialize(result);

    bool deserialized_b = d->readBool();
    assert(deserialized_b == b);

    delete s;
    delete d;
}

void test_serialize_char() {
    Serialize* s = new Serialize();

    char ch = 'S';
    s->write(ch);
    char* result = s->getChars();

    Deserialize* d = new Deserialize(result);

    char deserialized_ch = d->readChar();
    assert(deserialized_ch == ch);

    delete s;
    delete d;
}

void test_serialize_chars() {
    Serialize* s = new Serialize();

    char* chs = (char*)"String";
    s->write(chs);
    char* result = s->getChars();

    Deserialize* d = new Deserialize(result);

    char* deserialized_chs = d->readChars(6 * sizeof(char));
    assert(strcmp(deserialized_chs, chs) == 0);
    
    delete s;
    delete d;
}

void test_serialize_register_message() {
    Serialize s;
    sockaddr_in sender;
    Register* register_message = new Register(1, 8081, (char*)"127.0.0.1");

    register_message->serialize(s);
    char* result = s.getChars();
	
    Deserialize d(result);

    Message* deserialized_register_message = Message::deserialize(d, sender);

    assert(register_message->equals(deserialized_register_message));
    std::cout << "success!!\n";

    delete register_message;
}

void test_serialize_stringarray() {
    Serialize* s = new Serialize();
    StringArray* sa = new StringArray();
    sa->push_back(new String("S"));

    sa->serialize(s);

    char* result = s->getChars();
	
    Deserialize* d = new Deserialize(result);
    BaseArray* resultSa = BaseArray::deserialize(d);
    
    assert(sa->equals(resultSa));
    std::cout << "serialize string array success!\n";
    
    delete s;
    delete sa;
    delete d;
}

void test_serialize_schema() {
    Serialize* s = new Serialize();
    Schema* schema = new Schema("S");

    schema->serialize(s);

    char* result = s->getChars();
	
    Deserialize* d = new Deserialize(result);
    Schema* resultSchema = Schema::deserialize(d);
    
    assert(schema->equals(resultSchema));
    std::cout << "serialize schema success!\n";
    delete s;
    delete schema;
    delete d;
}

void test_serialize_stringcolumn() {
    KVStore* kv = new KVStore();
    Serialize* s = new Serialize();
    StringColumn* sc = new StringColumn();

    Key* key1 = new Key(new String("data-0-1"), 0);
    Key* key2 = new Key(new String("data"), 1);

    sc->add_key(key1);
    sc->add_key(key2);

    sc->serialize(s);

    char* result = s->getChars();

    Deserialize* d = new Deserialize(result);
    StringColumn* resultColumn = StringColumn::deserialize(d, kv);

    assert(sc->equals(resultColumn));
    std::cout << "serialize stringcolumn success!\n";
    
    delete kv;
    delete s;
    delete sc;
    delete key1;
    delete key2;
    delete d;
}

void test_serialize_dataframe() {
    KVStore* kv = new KVStore();
    Serialize* s = new Serialize();
    Schema* schema = new Schema("");
    StringColumn* sc = new StringColumn();

    Key* key1 = new Key(new String("data-0-1"), 0);
    Key* key2 = new Key(new String("data"), 1);

    sc->add_key(key1);
    sc->add_key(key2);
    DataFrame* df = new DataFrame(*schema);
    df->add_column(sc);
    
    char* result = df->serialize(s);
    Deserialize* d = new Deserialize(result);
    DataFrame* new_df = DataFrame::deserialize(d, kv);
    
    assert(df->equals(new_df));
    
    std::cout << "serialize dataframe success!\n";

    // delete s;
    // delete schema;
    // delete sc;
    // delete key1;
    // delete key2;
    // delete df;
}

void test_serialize_data_message() {
    Serialize* s = new Serialize();
    Schema* schema = new Schema("");
    StringColumn* sc = new StringColumn();

    Key* key1 = new Key(new String("data-0-1"), 0);
    Key* key2 = new Key(new String("data"), 1);

    sc->add_key(key1);
    sc->add_key(key2);
    DataFrame* df = new DataFrame(*schema);
    df->add_column(sc);
    
    char* serialized_df = df->serialize(s);
    Value* val = new Value(serialized_df, s->get_size());

    Serialize* msg_serializer = new Serialize();
    sockaddr_in sender;
    Key* key = new Key(new String("data-0-1"), 0);
    DataMessage* data_msg = new DataMessage(key, val, 1, 0);

    data_msg->serialize(*msg_serializer);
    char* result = msg_serializer->getChars();
	
    Deserialize d(result);

    Message* deserialized_data_message = Message::deserialize(d, sender);

    assert(data_msg->equals(deserialized_data_message));
    std::cout << "success for serialize data message!!\n";

    delete key1;
    delete key2;
    delete val;
    delete key;
}

class Visitor : public Writer {
public: 
    size_t i = 0;

    bool done() {
        return i++ == 6;
    }

    void visit(Row& row) {
        row.set(0, new String("hello"));
    }
};

void test_local_insert() {
    NUM_NODES = 1;
    Key key{new String("data"), 0};
    NetworkIp net;
    KVStore kv{net, 0};
    Visitor v;
    DataFrame* df = DataFrame::fromVisitor(&key, &kv, "S", v);
    Serialize s;
    char* serialized_df = df->serialize(&s);
    Value* val = new Value(serialized_df, s.get_size());

    kv.put_(&key, val);

    Value* kv_get_result = kv.get_(&key);
    Deserialize d(kv_get_result->get_data());
    DataFrame* new_df = DataFrame::deserialize(&d, &kv);
    
    assert(df->equals(new_df));
    std::cout << "success for local insert!!\n";
}

void test_local_insert_with_messages() {
    NUM_NODES = 1;
    Key key{new String("data"), 0};
    NetworkIp net;
    KVStore kv{net, 0};
    Visitor v;
    DataFrame* df = DataFrame::fromVisitor(&key, &kv, "S", v);
    Serialize s;
    std::cout << "\n";
    char* serialized_df = df->serialize(&s);
    Value* val = new Value(serialized_df, s.get_size());
    std::cout << "\n";

    Deserialize d{val->get_data()};
    std::cout << "deserialize starting ..\n";
    DataFrame* dataframe2 = DataFrame::deserialize(&d, &kv);
    std::cout << "printing dfs\n";
    df->print();
    dataframe2->print();

    assert(df->equals(dataframe2));

    DataMessage* data_msg = new DataMessage(&key, val, 1, 0);
    Serialize dm_s;
    
    std::cout << "about to serialize msg..\n";
    data_msg->serialize(dm_s);
    std::cout << "done.\n";

    sockaddr_in sender;
    char* dm_chars = dm_s.getChars();
    std::cout << "about to deserialize msg..\n";
    Deserialize dm_d{dm_chars};
    Message* deserialized_data_message = Message::deserialize(dm_d, sender);
    DataMessage* result_dm = dynamic_cast<DataMessage*>(deserialized_data_message);
    std::cout << "done.\n";

    Value* dm_data = result_dm->data_;
    Deserialize val_d{dm_data->get_data()};
    DataFrame* dataframe = DataFrame::deserialize(&val_d, &kv);

    df->print();
    dataframe->print();
    assert(df->equals(dataframe));
    std::cout << "success for local insert with messages!!\n";
    delete val;
    delete data_msg;
}

int main(int argc, char* argv[]) {

    test_serialize_int();
    test_serialize_sizet();
    test_serialize_string();
    test_serialize_string_and_int();
    test_serialize_double();
    test_serialize_bool();
    test_serialize_char();
    test_serialize_chars();
    test_serialize_stringarray();
    test_serialize_schema();
    test_serialize_stringcolumn();
    // test_serialize_dataframe();

    test_serialize_data_message();

    test_serialize_register_message();
    test_local_insert();
    test_local_insert_with_messages();

    return 0;
}