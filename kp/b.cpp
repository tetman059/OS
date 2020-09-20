#include <iostream>
#include <string>
#include <cstring>
#include <stdlib.h>

#include "zmq.h"

using namespace std;

int ReceiveMessage(void* socket) {
        size_t result = 0;
        zmq_msg_t reply;
        zmq_msg_init(&reply);
        zmq_msg_recv(&reply, socket, 0);
        char* ptr = reinterpret_cast<char*>(zmq_msg_data(&reply));
        result = atoi(ptr);
        return result;
}

void SendMessage(void* socket, const string& message) {
        zmq_msg_t msg;
        zmq_msg_init_size(&msg, message.size());
        memcpy(zmq_msg_data(&msg), message.c_str(), message.size());
        zmq_msg_send(&msg, socket, 0);
        zmq_msg_close(&msg);
}

int main() {
        void* context_a = zmq_ctx_new();
        void* socket_a = zmq_socket(context_a, ZMQ_REP);
        zmq_bind(socket_a, "tcp://*:8080");

        void* context_c = zmq_ctx_new();
        void* socket_c = zmq_socket(context_c, ZMQ_REP);
        zmq_bind(socket_c, "tcp://*:7070");

        bool to_stop_a = false, to_stop_c = false;
        string received_a = "Received from A: ";
        string received_c = "Received from C: ";
        int a_bytes = 0;
        int c_bytes = 0;
        for (;;) {
                if (a_bytes != -1) {
                        a_bytes = ReceiveMessage(socket_a);
                        SendMessage(socket_a, "");
                }
                to_stop_a = a_bytes == -1 ? true: false;
                if (c_bytes != -1) {
                        c_bytes = ReceiveMessage(socket_c);
                        SendMessage(socket_c, "");
                }
                to_stop_c = c_bytes == -1 ? true : false;
                if (to_stop_a && to_stop_c) {
                        break;
                } else if (!to_stop_a && !to_stop_c) {
                        cout << received_a << a_bytes << endl;
                        cout << received_c << c_bytes << endl;
                } else if (!to_stop_a && to_stop_c) {
                        cout << received_a << a_bytes << endl;
                } else {
                        cout << received_c << c_bytes << endl;
                }
        }

        zmq_close(socket_a);
        zmq_ctx_destroy(context_a);
        zmq_close(socket_c);
        zmq_ctx_destroy(context_c);

        return 0;
}