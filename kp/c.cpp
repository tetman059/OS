#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>

#include "zmq.h"

using namespace std;

string ReceiveMessage(void* respond) {
        string result = "";
        zmq_msg_t request;
        zmq_msg_init(&request);
        zmq_msg_recv(&request, respond, 0);
        char* ptr = reinterpret_cast<char*>(zmq_msg_data(&request));
        result = string(ptr, zmq_msg_size(&request));
        zmq_msg_close(&request);
        return result;
}

void SendMessage(void* respond, const string& message) {
        zmq_msg_t reply;
        zmq_msg_init_size(&reply, message.size());
        memcpy(zmq_msg_data(&reply), message.c_str(), message.size());
        zmq_msg_send(&reply, respond, 0);
        zmq_msg_close(&reply);
}

int main() {
        void* context_a = zmq_ctx_new();
        void* respond_a = zmq_socket(context_a, ZMQ_REP);
        zmq_bind(respond_a, "tcp://*:9090");

        void* context_b = zmq_ctx_new();
        void* respond_b = zmq_socket(context_b, ZMQ_REQ);
        zmq_connect(respond_b, "tcp://localhost:7070");

        string message = "Received message from A";
        string empty_message = "";
        for (;;) {
                string received_message = ReceiveMessage(respond_a);
                if (received_message == "END") {
                        SendMessage(respond_b, "-1");
                        ReceiveMessage(respond_b);
                        break;
                } else if (received_message == "") {
                        cout << "Empty" << endl;
                        SendMessage(respond_a, empty_message);
                } else {
                        cout << received_message << endl;
                        SendMessage(respond_a, message);
                }
                SendMessage(respond_b, to_string(received_message.size()));
                ReceiveMessage(respond_b);
        }

        zmq_close(respond_a);
        zmq_ctx_destroy(context_a);
        zmq_close(respond_b);
        zmq_ctx_destroy(context_b);

        return 0;
}