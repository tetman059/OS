#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>

#include "zmq.h"

using namespace std;

vector<string> ReadData(size_t size, istream& is = cin) {
        vector<string> result;
        result.reserve(size + 1);
        for (size_t i = 0; i < size; ++i) {
                string value = "";
                is >> value;
                result.push_back(move(value));
        }
        result.push_back("END");
        return result;
}

void SendMessage(void* request, const string& message) {
        zmq_msg_t req;
        zmq_msg_init_size(&req, message.size());
        memcpy(zmq_msg_data(&req), message.c_str(), message.size());
        zmq_msg_send(&req, request, 0);
        zmq_msg_close(&req);
}

string ReceiveMessage(void* respond) {
        string result = "";
        zmq_msg_t reply;
        zmq_msg_init(&reply);
        zmq_msg_recv(&reply, respond, 0);
        char* ptr = reinterpret_cast<char*>(zmq_msg_data(&reply));
        result = string(ptr, zmq_msg_size(&reply));
        zmq_msg_close(&reply);
        return result;
}

int main() {
        void* context_c = zmq_ctx_new();
        void* request_c = zmq_socket(context_c, ZMQ_REQ);
        zmq_connect(request_c, "tcp://localhost:9090");

        void* context_b = zmq_ctx_new();
        void* request_b = zmq_socket(context_b, ZMQ_REQ);
        zmq_connect(request_b, "tcp://localhost:8080");

        cout << "Enter the number of strings: ";
        size_t size = 0;
        cin >> size;

        vector<string> strings = ReadData(size, cin);
        size_t pos = 0;
        bool send = false, received = true;
        for(;;) {
                if (pos == strings.size() - 1) {
                        SendMessage(request_c, strings[pos]);
                        SendMessage(request_b, "-1");
                        ReceiveMessage(request_b);
                        break;
                }
                if (received) {
                        SendMessage(request_c, strings[pos]);
                        SendMessage(request_b, to_string(strings[pos].size()));
                        ReceiveMessage(request_b);
                        pos++;
                        send = true;
                        received = false;
                }
                if (send) {
                        string received_message = ReceiveMessage(request_c);
                        cout << received_message << endl;
                        received = true;
                        send = false;
                }
        }

        zmq_close(request_c);
        zmq_ctx_destroy(context_c);
        zmq_close(request_b);
        zmq_ctx_destroy(context_b);

        return 0;
}