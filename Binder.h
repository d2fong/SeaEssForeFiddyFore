//
// Created by Dylan Fong on 2015-07-06.
//

#ifndef CS4544REEAL_BINDER_H
#define CS4544REEAL_BINDER_H

#include "Message.h"


class Binder {
private:
    unsigned short binderPort;
    char* binderAddr;
    void print_status();
    int handle_request(int socket, int type);

public:

    Binder() {
    }

    int init();

    //Protocol

    //Location Request
    int send_location_success_response(LocationSuccessMessage m);
    int send_location_failure_response(LocationFailureMessage m);

    int receive_location_request(int socket);

    LocationSuccessMessage create_location_success_message(char* serverAddr, char* serverPort);
    LocationFailureMessage create_location_failure_message(int reasonCode);




//    Message create_register_response_message(MessageType, int status);
//
//    void send_register_response(Message m);
//    void send_location_response(Message m);
//
//
//    Message receive_terminate_request();
      int receive_register_request(int socket, int length);


};


#endif //CS4544REEAL_BINDER_H
