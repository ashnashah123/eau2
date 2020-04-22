#pragma once

#include "../utils/object.h"
#include "msgKind.h"
#include "../store/serial.h"
#include "../utils/string.h"

#include <arpa/inet.h> //close 

size_t NUM_NODES; 

class Message : public Object {
public:
    MsgKind kind_;  // the message kind
    size_t target_; // the index of the receiver node
    size_t sender_; // the index of the sender node
    // size_t id_;     // an id t unique within the node
    Message() {}


    size_t sender() {
        return sender_;
    }

    size_t target() {
        return target_;
    }

    String* type_to_string() {
        if(kind_ == MsgKind::REGISTER) {
            return new String("REGISTER");
        }
        else if(kind_ == MsgKind::DIRECTORY) {
            return new String("DIRECTORY");
        }
        else if(kind_ == MsgKind::DATA) {
            return new String("DATA");
        }
        else if(kind_ == MsgKind::DATA_REQUEST) {
            return new String("DATA_REQUEST");
        }
        else {
            assert(false);
        }
    }

    virtual void serialize(Serialize& s) { assert(false); }

    static Message* deserialize(Deserialize& d, sockaddr_in sender);
};

// A class representing a message when a node sends its IP address to the master to initiate communication
class Register : public Message {
public:
    size_t port_;
    size_t idx_; 
    sockaddr_in client_; // ip address
    char* ip_address_; 

    Register(size_t idx, size_t port, char* client_ip) {
        // puts my ip_address into a struct so that it can be sent over to the server as bytes
        port_ = port;
        idx_ = idx;
        kind_ = MsgKind::REGISTER;
        sender_ = idx;
        target_ = 0;
        ip_address_ = client_ip;
    }

    Register(Deserialize& d, sockaddr_in sender) {
        kind_ = MsgKind::REGISTER;
        port_ = d.readSizeT();
        idx_ = d.readSizeT();
        target_ = d.readSizeT();
        sender_ = d.readSizeT();
        ip_address_ = d.readString()->cstr_;

        client_ = sender;
        client_.sin_family = AF_INET;
        client_.sin_addr.s_addr = INADDR_ANY;
        client_.sin_port = htons(port_);
        inet_pton(AF_INET, ip_address_, &client_.sin_addr);
    }

    // destructor
    ~Register() {}

    void serialize(Serialize& s) {
        s.write(new String("REGISTER"));
        s.write(port_);
        s.write(idx_);
        s.write(target_);
        s.write(sender_);
        s.write(new String(ip_address_));
    }

    static Register* deserialize(Deserialize &d, sockaddr_in sender) {
        return new Register(d, sender);
    }

    bool equals(Object* other) {
        if (other == nullptr) return false;
        if (other == this) return true;
        Register* x = dynamic_cast<Register*>(other);
        if (x == nullptr) return false;
        if (x->kind_ != MsgKind::REGISTER) {
            return false;
        }
        return port_ == x->port_ && idx_ == x->idx_ && target_ == x->target_ && sender_ == x->sender_;
    }
};

// A class representing a message when a node sends its IP address to the master to initiate communication
class Directory : public Message {
public:
    size_t* ports_;
    String** addresses_;
    size_t clients;

    // constructor
    Directory(size_t* ports, String** addresses) {
        kind_ = MsgKind::DIRECTORY;
        ports_ = ports;
        addresses_ = addresses;
        // clients = sizeof(addresses) / sizeof(String*);
        clients = NUM_NODES - 1;
        sender_ = 0;
    }

    Directory(Deserialize& d) {
        kind_ = MsgKind::DIRECTORY;
        clients = d.readSizeT();
        ports_ = new size_t[clients];
        for(size_t i = 0; i < clients; i++) {
            ports_[i] = d.readSizeT();
        }
        addresses_ = new String*[clients];
        for(size_t i = 0; i < clients; i++) {
            addresses_[i] = d.readString();
        }
        target_ = d.readSizeT();
        sender_ = d.readSizeT();
    }

    // destructor
    ~Directory() {}

    void log() { 
        for(size_t i = 0; i < NUM_NODES - 1; i++) {
            // std::cout << "inside directory class port at i: " << i << ": " << ports_[i] << "\n";
            // std::cout << "inside directory class address at i: " << i << ": " << addresses_[i]->cstr_ << "\n";
        } 
    }

    void serialize(Serialize& s) {
        s.write(new String("DIRECTORY"));
        s.write(clients);
        for(size_t i = 0; i < clients; i++) {
            s.write(ports_[i]);
        }
        for(size_t i = 0; i < clients; i++) {
            s.write(addresses_[i]);
        }
        s.write(target_);
        s.write(sender_);
    }

    static Directory* deserialize(Deserialize &d) {
        return new Directory(d);
    }
 };

class DataMessage : public Message {
public:
    Key* key_;
    Value* data_;
    
    DataMessage(Key* key, Value* data, size_t target, size_t sender) {
        kind_ = MsgKind::DATA;
        key_ = key;
        target_ = target;
        sender_ = sender;
        data_ = data;
    }

    DataMessage(Deserialize& d) {
        kind_ = MsgKind::DATA;
        key_ = Key::deserialize(d);
        target_ = d.readSizeT();
        sender_ = d.readSizeT();
        data_ = Value::deserialize(d);
    }

    void serialize(Serialize& s) {
        // std::cout << "TYPE WRITING IN in DataMessage::serialize: DATA\n";
        s.write(new String("DATA"));
        key_->serialize(s);
        s.write(target_);
        s.write(sender_);
        data_->serialize(s);
    }

    static DataMessage* deserialize(Deserialize& d) {
        return new DataMessage(d);
    }

    bool equals(Object* other) {
        if (other == nullptr) return false;
        if (other == this) return true;
        DataMessage* x = dynamic_cast<DataMessage*>(other);
        if (x == nullptr) return false;
        if (x->kind_ != MsgKind::DATA) {
            return false;
        }
        
        // std::cout << "key_: " << key_->get_name()->cstr_ << "\n";
        // std::cout << "x->key_: " << dynamic_cast<Key*>(x->key_)->get_name()->cstr_ << "\n";
        // std::cout << "strcmp(data_->get_data(), x->data_->get_data()): " << strcmp(data_->get_data(), x->data_->get_data()) << "\n";
        // std::cout << "target_: " << target_ << "\n";
        // std::cout << "x->target_: " << x->target_ << "\n";
        // std::cout << "sender: " << sender_ << "\n";
        // std::cout << "x->sender: " << x->sender_ << "\n";
        return key_->equals(x->key_) && (strcmp(data_->get_data(), x->data_->get_data()) == 0) && target_ == x->target_ && sender_ == x->sender_;
    }
};


class DataRequest : public Message {
public:
    Key* key_;

    DataRequest(Key* key, size_t sender) {
        key_ = key;
        target_ = key->home();
        sender_ = sender;
        kind_ = MsgKind::DATA_REQUEST;
    }

    DataRequest(Deserialize& d) {
        kind_ = MsgKind::DATA_REQUEST;
        key_ = Key::deserialize(d);
        target_ = d.readSizeT();
        sender_ = d.readSizeT();
    }

    void serialize(Serialize& s) {
        s.write(new String("DATA_REQUEST"));
        key_->serialize(s);
        s.write(target_);
        s.write(sender_);
    }

    static DataRequest* deserialize(Deserialize& d) {
        return new DataRequest(d);
    }
};


// TODO: why does this take a sender? What would it be used for?
Message* Message::deserialize(Deserialize& d, sockaddr_in sender) {
    String* type = d.readString();

    assert(type != nullptr);
    Message* result; 
    if (type->equals(new String("REGISTER"))) {
        result = dynamic_cast<Message*>(Register::deserialize(d, sender));
    }
    else if (type->equals(new String("DIRECTORY"))) {
        result = dynamic_cast<Message*>(Directory::deserialize(d));
    }
    else if (type->equals(new String("DATA"))) {
        result = dynamic_cast<Message*>(DataMessage::deserialize(d));
    }
    else if (type->equals(new String("DATA_REQUEST"))) {
        result = dynamic_cast<Message*>(DataRequest::deserialize(d));
    }
    else {
        assert(false);
    }
    return result;
    
}