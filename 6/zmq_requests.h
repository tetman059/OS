#include <unistd.h>
#include <cstring>

#include <iostream>
#include <exception>
#include <string>

#include "zmq.h"

void CreateServerNode(std::string path, const int id, const int port) {
        char* id_ = strdup(std::to_string(id).c_str());
        char* port_ = strdup(std::to_string(port).c_str());
        char* args[] = {strdup(path.c_str()), id_, port_, NULL};
        execv(strdup(path.c_str()), args);
}

void SendMessage(void* request, const std::string message) {
        zmq_msg_t req;
        zmq_msg_init_size(&req, message.size());
        memcpy(zmq_msg_data(&req), message.c_str(), message.size());
        zmq_msg_send(&req, request, 0);
        zmq_msg_close(&req);
}

std::string ReceiveMessage(void* request) {
        zmq_msg_t reply;
        zmq_msg_init(&reply);
        zmq_msg_recv(&reply, request, 0);
        std::string result(static_cast<char*>(zmq_msg_data(&reply)), zmq_msg_size(&reply));
        zmq_msg_close(&reply);
        return result;
}