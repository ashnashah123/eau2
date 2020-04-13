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

    void server_init(size_t idx, size_t port, char* server_ip) {
        this_node_ = idx;
        assert(idx==0 && "Server must be 0");
        init_sock_(port, server_ip);
        nodes_ = new NodeInfo[NUM_NODES];
        for (size_t i = 0; i < NUM_NODES; i++) {
            nodes_[i].id = 0;
        }
        nodes_[0].address = ip_;
        nodes_[0].id = 0;
        // Go over the node that sent the register message and set their information in the nodes array
        for (size_t i = 1; i < NUM_NODES; i++) { /////////////////////// this looped from 2-num nodes to <= NUM NODES
            Register* msg = dynamic_cast<Register*>(recv_m());
            nodes_[msg->sender()].id = msg->sender();
            nodes_[msg->sender()].address = msg->client_;
            nodes_[msg->sender()].address.sin_family = AF_INET;
            nodes_[msg->sender()].address.sin_addr = msg->client_.sin_addr;
            nodes_[msg->sender()].address.sin_port = htons(msg->port_);
        }
        size_t* ports = new size_t[NUM_NODES - 1];
        String** addresses = new String*[NUM_NODES - 1];
        for(size_t i = 0; i < NUM_NODES - 1; i++) {  /////////////////////// this looped to NUM_NODES - 1
            ports[i] = ntohs(nodes_[i+1].address.sin_port);
            String* addr = new String(inet_ntoa(nodes_[i+1].address.sin_addr));
            std::cout << "the address: " << addr->cstr_ << "\n";
            addresses[i] = addr;
        }
        Directory ipd(ports, addresses);
        ipd.log();
        for(size_t i = 1; i < NUM_NODES; i++) { ///////////////////////////// this was i = 1 loop to NUM_NODES
            ipd.target_ = i;
            send_m(&ipd);
        }
    }

    void client_init(size_t idx, size_t port, char* server_addr, size_t server_port, char* client_ip) {
        this_node_ = idx;
        init_sock_(port, client_ip);
        nodes_ = new NodeInfo[1];
        nodes_[0].id = 0;
        nodes_[0].address.sin_family = AF_INET;
        nodes_[0].address.sin_port = htons(server_port);
        if (inet_pton(AF_INET, server_addr, &nodes_[0].address.sin_addr) <= 0) {
            assert(false && "Invalid server IP address format");
        }
        Register msg(idx, port, client_ip);
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
    void init_sock_(size_t port, char* ip_address) {
        std::cout << "CALLING INIT_SOCK_ WITH IP ADDRESS: " << ip_address << "\n";
        assert((sock_ = socket(AF_INET, SOCK_STREAM, 0)) >= 0);
        int opt = 1;
        assert(setsockopt(sock_,
                        SOL_SOCKET, SO_REUSEADDR, // | SO_REUSEPORT
                        &opt, sizeof(opt)) == 0);
        ip_.sin_family = AF_INET;
        ip_.sin_addr.s_addr = INADDR_ANY;
        ip_.sin_port = htons(port);
        inet_pton(AF_INET, ip_address, &ip_.sin_addr);
        assert(bind(sock_, (struct sockaddr*)&ip_, sizeof(ip_)) >= 0);
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
        msg->serialize(ser);
        char* buf = ser.getChars();
        
        size_t size = ser.get_size();
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
        // sender param: we know who sent it to us so we can send it back (ex. a data request)
        Message* msg = Message::deserialize(d, sender);
        return msg;
    }

    // TODO: add a teardown method 
    void teardown() {
        // Close connections that have been created
    }

};