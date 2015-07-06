//
// Created by Dylan Fong on 2015-07-05.
//

#ifndef CS454A3_CLIENT_H
#define CS454A3_CLIENT_H

#include <string>

using namespace std;

class Client {
private:
    int binderSocket;
    int serverSocket;

public:
    Client() { }
    Client(int binderSocket) : binderSocket(binderSocket) { }

    Client(int binderSocket, int serverSocket) : binderSocket(binderSocket), serverSocket(serverSocket) {}

    int get_binder_socket();
    int get_server_socket();
    int connect_to_something(char* addr, char* port);

    Message create_location_request(char* func_name, int* argTypes);
    Message create_execute_request(char* func_name, int* argTypes, void** args);

    void send_location_request(Message m);
    void send_execute_request(Message m);

    Message receive_location_response();
    Message receive_execute_response();

};

#endif