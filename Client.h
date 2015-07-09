//
// Created by Dylan Fong on 2015-07-05.
//

#ifndef CS454A3_CLIENT_H
#define CS454A3_CLIENT_H


#include "Message.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>

using namespace std;

class Client {
private:
    int binderSocket;
    int serverSocket;

public:
    Client() { }
    Client(int binderSocket) : binderSocket(binderSocket) {}

    Client(int binderSocket, int serverSocket) : binderSocket(binderSocket), serverSocket(serverSocket) {}

    int get_binder_socket();
    int get_server_socket();

    //Connect to a server/binder given the addr and port
    int connect_to_something(char* addr, char* port);

    //Create a location request message
    LocationRequestMessage create_location_request(char* funcName, int* argTypes);

    //Create a execute request message
    Message create_execute_request(char* funcName, int* argTypes, void** args);

    //Send the location request message
    int send_location_request(LocationRequestMessage m, int binderSocket);

    //Send the execute request message
    int send_execute_request(Message m);

    //Receive the location response message
    Message receive_location_response();

    //Receive the execute response message
    Message receive_execute_response();

};

#endif