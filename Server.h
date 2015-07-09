#ifndef CS4544REEAL_SERVER_H
#define CS4544REEAL_SERVER_H

#include "Message.h"
#include "helpers.h"
#include "DB.h"
#include <string>

using namespace std;

class Server {
private:
    int clientSocket;
    int binderSocket;
    string host;
    int port;

public:
    Server() { }
    Server(string host, int port, int sockForClient, int sockForBinder) : host(host), port(port), clientSocket(sockForClient), binderSocket(sockForBinder) { }

    int get_binder_socket();
    int get_client_socket();

    string get_host();
    int get_port();

    RegisterMessage create_register_message(Function f);

    int send_register_request(RegisterMessage m, int binderSocket);
    int send_execute_response( int* argTypes, void** args, int arg_length, int reason_code);


};


#endif //CS4544REEAL_SERVER_H
