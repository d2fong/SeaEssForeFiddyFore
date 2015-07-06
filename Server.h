//
// Created by Dylan Fong on 2015-07-06.
//

#ifndef CS4544REEAL_SERVER_H
#define CS4544REEAL_SERVER_H


class Server {
private:
    int sockForClient;
    int sockForBinder;


public:
    Server() { }
    Server(int sockForClient, int sockForBinder) : sockForClient(sockForClient), sockForBinder(sockForBinder) { }

    int connect_to_binder(char* binderAddr, char* binderPort);
    int get_binder_socket();
    int get_client_socket();

    Message create_register_message(char* funcName, int* argTypes);
    Message create_execute_success_message(char* funcName, int* argTypes, void** args);
    Message create_execute_failure_message(int reasonCode);


    void send_register_request(Message m);
    void send_execute_request(Message m);

    Message receive_register_request();
    Message receive_execute_request();


};


#endif //CS4544REEAL_SERVER_H
