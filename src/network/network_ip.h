#include "../utils/object.h"
#pragma once
//land::CwC
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "./networkifc.h"
#include "../utils/string.h"
#include "../store/serial.h"
#include "./message.h"
#include "../utils/thread.h"

/**********************************************************************
 * NodeInfo: each node is identified by its node id (also called index)
 * and its socket address.
 * ********************************************************************/

class NodeInfo : public Object {
public:
    size_t id;
    sockaddr_in address;
};

class NetworkIp : public NetworkIfc, public Thread {
public:
    NodeInfo* nodes_; // all nodes
    size_t this_node_; // node index
    int sock_; // socket
    sockaddr_in ip_; // ip address

    ~NetworkIp() {}

    size_t index() override { return this_node_; }

    void server_init(size_t idx, size_t port) {
        std::cout << "port: " << port << "\n";
        this_node_ = idx;
        assert(idx==0 && "Server must be 0");
        init_sock_(port);
        nodes_ = new NodeInfo[NUM_NODES];
        for (size_t i = 0; i < NUM_NODES; i++) {
            nodes_[i].id = 0;
        }
        nodes_[0].address = ip_;
        nodes_[0].id = 0;
        // Go over the node that sent the register message and set their information in the nodes array
        // sleep(1000000);
        for (size_t i = 0; i < NUM_NODES; i++) {
            Register* msg = dynamic_cast<Register*>(recv_m());
            nodes_[msg->sender()].id = msg->sender();
            nodes_[msg->sender()].address = msg->client_;
            nodes_[msg->sender()].address.sin_family = AF_INET;
            nodes_[msg->sender()].address.sin_addr = msg->client_.sin_addr;
            nodes_[msg->sender()].address.sin_port = htons(msg->port_);
                        
            // struct sockaddr_in ourIPAddressStruct;
            // ourIPAddressStruct.sin_family = AF_INET;
            // ourIPAddressStruct.sin_addr.s_addr = INADDR_ANY;
            // ourIPAddressStruct.sin_port = htons(msg->port_);
            // inet_pton(AF_INET, msg->client_, &ourIPAddressStruct.sin_addr);
        }
        size_t* ports = new size_t[NUM_NODES - 1];
        String** addresses = new String*[NUM_NODES - 1];
        for(size_t i = 0; i < NUM_NODES - 1; i++) {  /////////////////////// this looped from 2-num nodes
            ports[i] = ntohs(nodes_[i+1].address.sin_port);
            addresses[i] = new String(inet_ntoa(nodes_[i+1].address.sin_addr));
        }
        Directory ipd(ports, addresses);
        ipd.log();
        for(size_t i = 0; i < NUM_NODES; i++) {
            ipd.target_ = i;
            send_m(&ipd);
        }
    }

    void client_init(size_t idx, size_t port, char* server_addr, size_t server_port) {
        this_node_ = idx;
        init_sock_(port);
        nodes_ = new NodeInfo[1];
        nodes_[0].id = 0;
        nodes_[0].address.sin_family = AF_INET;
        nodes_[0].address.sin_port = htons(server_port);
        if (inet_pton(AF_INET, server_addr, &nodes_[0].address.sin_addr) <= 0) {
            assert(false && "Invalid server IP address format");
        }
        Register msg(idx, port);
        if (msg.kind_ == MsgKind::REGISTER) {
            std::cout << "client is going to send a register message\n";
        }
        send_m(&msg);
        std::cout << "just sent message from client\n";
        Directory* ipd = dynamic_cast<Directory*>(recv_m());
        ipd->log();
        NodeInfo* nodes = new NodeInfo[NUM_NODES];
        nodes[0] = nodes_[0];
        // for (size_t i = 0; i < ipd->clients; i++) {
        for (size_t i = 0; i < NUM_NODES - 1; i++) {
            nodes[i + 1].id = i + 1;
            nodes[i + 1].address.sin_family = AF_INET;
            nodes[i + 1].address.sin_port = htons(ipd->ports_[i]);
            if (inet_pton(AF_INET, ipd->addresses_[i]->c_str(),
                            &nodes[i + 1].address.sin_addr) <= 0) {
                std::cout << "Invalid IP directory-addr. for node " << i+1 << "\n";
            }
        }
        delete [] nodes_;
        nodes_ = nodes;
        delete ipd;
    }

    /** create a socket and bind it **/
    void init_sock_(size_t port) {
        assert((sock_ = socket(AF_INET, SOCK_STREAM, 0)) >= 0);
        int opt = 1;
        assert(setsockopt(sock_,
                        SOL_SOCKET, SO_REUSEADDR, // | SO_REUSEPORT
                        &opt, sizeof(opt)) == 0);
        ip_.sin_family = AF_INET;
        ip_.sin_addr.s_addr = INADDR_ANY;
        ip_.sin_port = htons(port);
        assert(bind(sock_, (sockaddr*)&ip_, sizeof(ip_)) >= 0);
        assert(listen(sock_, 100) >= 0); // 100 in connections queue size
    }
    
    void send_m(Message* msg) override {
        NodeInfo & tgt = nodes_[msg->target()];
        int conn = socket(AF_INET, SOCK_STREAM, 0);
        assert(conn >= 0 && "Unable to create client socket");
        if (connect(conn, (sockaddr*)&tgt.address, sizeof(tgt.address)) < 0) {
            std::cout << "Unable to connect to remote node" << "\n";
        }
        Serialize ser;
        if (msg->kind_ == MsgKind::REGISTER) {
            std::cout << "inside send_m: sending a register message!!!\n";
        }
        char* buf = msg->serialize(ser);
        std::cout << "buf after serializing: " << buf << "\n";
        sockaddr_in sender;
        Deserialize d(buf);

        Register* deserialized_msg = dynamic_cast<Register*>(Message::deserialize(d, sender));
        assert(dynamic_cast<Register*>(msg)->equals(deserialized_msg));
        std::cout << "something got deserialized right after serializing\n";
        if (deserialized_msg->kind_ == MsgKind::REGISTER) {
            std::cout << "it is a register message!!!\n";
        }

        size_t size = ser.get_size();
        std::cout << "size after serializing: " << size << "\n";
        send(conn, &size, sizeof(size_t), 0);
        send(conn, buf, size, 0);
    }

    Message* recv_m() override {
        sockaddr_in sender;
        socklen_t addrlen = sizeof(sender);
        int req = accept(sock_, (sockaddr*)&sender, &addrlen);
        size_t size = 0;
        if (read(req, &size, sizeof(size_t)) == 0) {
            std::cout << "failed to read" << "\n";
        }
        
        char* buf = new char[size];
        int rd = 0;
        while (rd != size) {
            rd += read(req, buf + rd, size - rd);
        }
        // We may need to bring back the size, don't think we need it for right now
        // Deserialize d(buf, size);
        Deserialize d(buf);
        std::cout << "about to deserialize a message\n";
        // sender param: we know who sent it to us so we can send it back (ex. a data request)
        Message* msg = Message::deserialize(d, sender);
        std::cout << "something got deserialized\n";
        if (msg->kind_ == MsgKind::REGISTER) {
            std::cout << "received a register message\n";
        }
        return msg;
    }

    // TODO: add a teardown method 
    void teardown() {
        // Close connections that have been created
    }

};