#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

#include <iostream>
#include <exception>
#include <unordered_map>
#include <vector>
#include <string>
#include <cstring>

#include "zmq.h"

#include "zmq_requests.h"

using namespace std;

void PrintMenu(ostream& os = cout) {
        os << "List of commands: " << endl
           << "1) create id [parent] - create new server node" << endl
           << "2) remove id - remove existing server node" << endl
           << "3) exec id n k1...kn - evaluate sum of n numbers" << endl
           << "4) ping id - check if server still available" << endl
           << "5) exit - exit program" << endl
           << "6) print - print menu" << endl;
}

string CreateNewServerNode(const unordered_map<int, void*>& socket, const int id, const int parent,
                unordered_map<int, pid_t>& server_nodes_pids) {
        string result = "";
        if (parent == -1) {
                if (server_nodes_pids.find(id) != server_nodes_pids.end()) {
                        result = string("Error: Already exists");
                } else {
                        cout << "Enter the port to connect: ";
                        int port = 0;
                        cin >> port;
                        string address = "tcp://localhost:" + to_string(port);
                        cout << "Enter the executable file (\"./smth\"): ";
                        string path = "";
                        cin >> path;
                        pid_t pid = fork();
                        if (pid == -1) {
                                result = "Error: Unable to create new server node";
                        } else if (pid == 0) { // ребенок
                                CreateServerNode(path, id, port);
                        } else { // родитель
                                if (static_cast<int>(zmq_connect(socket.at(id), address.c_str())) == 0) {
                                        server_nodes_pids[id] = pid;
                                        SendMessage(socket.at(id), "Ping");
                                        result = ReceiveMessage(socket.at(id));
                                        result += ": " + to_string(pid);
                                } else {
                                        result = "Can't connect to the server with id " + to_string(id);
                                        result += " and port " + to_string(port) + ". Please try again";
                                }
                        }
                }
        } else {
                result = string("Error: Parent not found");
        }
        return result;
}

string RemoveServerNode(const pid_t pid) {
        string result = "";
        int is_killed = kill(pid, SIGKILL); // второй аргумент для немедленного завершения процесса
        is_killed == 0 ? result = "Ok" : result = "Can't kill the process";
        return result;
}

string Exec(const unordered_map<int, void*>& socket, const int id, vector<int> v) {
        string result = "";
        if (socket.find(id) != socket.end()) {
                string to_calc = "Exec ";
                for (const auto& item : v) {
                        to_calc += to_string(item);
                        to_calc += " ";
                }
                SendMessage(socket.at(id), to_calc);
                result = ReceiveMessage(socket.at(id));
        } else {
                result = "Error: Not found";
        }
        return result;
}

string PingServerNode(const unordered_map<int, void*>& socket, const int id,
                unordered_map<int, pid_t>& server_nodes_pids) {
        string result = "";
        if (server_nodes_pids.find(id) != server_nodes_pids.end()) {
                try {
                        SendMessage(socket.at(id), "Ping");
                        result = ReceiveMessage(socket.at(id));
                        result += ": 1";
                } catch(...) {
                        result += ": 0";
                }
        } else {
                result = "Error: Not found";
        }
        return result;
}

int main () {
        unordered_map<int, pid_t> server_nodes_pids;

        void* context = zmq_ctx_new();
        cout << "Client starting..." << endl;
        PrintMenu();

        unordered_map<int, void*> request;

        while (true) {
                string command;
                cin >> command;
                if (command == "create") {
                        int id = 0, parent = 0;
                        cin >> id >> parent;
                        if (server_nodes_pids.find(id) == server_nodes_pids.end()) {
                                request[id] = zmq_socket(context, ZMQ_REQ);
                        }
                        cout << CreateNewServerNode(request, id, parent, server_nodes_pids) << endl;
                } else if (command == "remove") {
                        int id = 0;
                        cin >> id;
                        auto it = server_nodes_pids.find(id);
                        if (it != server_nodes_pids.end()) {
                                cout << RemoveServerNode(it->second) << endl;
                                server_nodes_pids.erase(it);
                                request.erase(id);
                        } else {
                                cout << "Error: Not found" << endl;
                        }
                } else if (command == "exec") {
                        int id = 0, k = 0;
                        cin >> id >> k;
                        string ping = PingServerNode(request, id, server_nodes_pids);
                        if (ping == "Ok: 1") {
                                vector<int> numbers;
                                numbers.reserve(k);
                                for (int i = 0; i < k; ++i) {
                                        int n = 0;
                                        cin >> n;
                                        numbers.push_back(n);
                                }
                                cout << Exec(request, id, move(numbers)) << endl;
                        } else {
                                cout << "Error: Server is unavailiable" << endl;
                        }
                } else if (command == "ping") {
                        int id = 0;
                        cin >> id;
                        cout << PingServerNode(request, id, server_nodes_pids) << endl;
                } else if (command == "exit") {
                        break;
                } else if (command == "print") {
                        PrintMenu();
                } else {
                        cout << "Wrong input" << endl;
                }
        }

        for (const auto socket : request) {
                zmq_close(socket.second);
        }
        zmq_ctx_destroy(context);

        return 0;
}