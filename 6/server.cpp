#include <unistd.h>
#include <sys/types.h>

#include <iostream>
#include <string>
#include <cstring>

#include "zmq.h"

#include "zmq_requests.h"

using namespace std;

int ParseRequest(const string& s) {
        int result = 0;
        string current_number;
        for (const auto item : s) {
                if (item != ' ' && isdigit(item)) {
                        current_number.push_back(item);
                } else if (item == ' ' && !current_number.empty()) {
                        result += stoi(current_number);
                        current_number.clear();
                }
        }
        return result;
}

int main(int argc, char const *argv[]) {
        int id = stoi(argv[1]);
        int port = stoi(argv[2]);
        string address = "tcp://*:" + to_string(port);

        void* context = zmq_ctx_new();
        void* respond = zmq_socket(context, ZMQ_REP);

        zmq_bind(respond, address.c_str());

        string request = "";
        request = ReceiveMessage(respond);
        while (true) {
                sleep(1);
                if (request == "Ping") {
                        SendMessage(respond, "Ok");
                } else if (request.substr(0, 4) == "Exec") {
                        int sum = ParseRequest(request);
                        string result = "";
                        result = "Ok: " + to_string(id) + ": " + to_string(sum);
                        SendMessage(respond, result);
                }
                request = ReceiveMessage(respond);
        }
        zmq_close(respond);
        zmq_ctx_destroy(context);

        return 0;
}