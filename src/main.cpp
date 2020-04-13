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

int main(int argc, char* argv[]) {

    test_serialize_int();
    test_serialize_sizet();
    test_serialize_string();
    test_serialize_string_and_int();
    test_serialize_double();
    test_serialize_bool();

    test_serialize_register_message();

    return 0;
}