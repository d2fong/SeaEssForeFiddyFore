#ifndef CS4544REEAL_SERVER_H
#define CS4544REEAL_SERVER_H

#include "Message.h"
#include "helpers.h"
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

    BinderRegisterMessage create_register_message(Function f);
    Message create_execute_success_message(char* funcName, int* argTypes, void** args);
    Message create_execute_failure_message(int reasonCode);


    int send_register_request(BinderRegisterMessage m, int binderSocket);
    void send_execute_request(Message m);

    Message receive_register_request();
    Message receive_execute_request();


};


#endif //CS4544REEAL_SERVER_H
