//
// Created by Dylan Fong on 2015-07-06.
//

#ifndef CS4544REEAL_BINDER_H
#define CS4544REEAL_BINDER_H

<<<<<<< HEAD
#include "Message.h"
=======
#include <string>
using namespace std;
>>>>>>> 393342b0a6d87769ed1d1a10f6d3803238d2e9db


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
    int receive_register_request(int socket, int length);
    int send_register_response(int socket, int flag);

    //Protocol

    //Location Request
    int send_location_success_response(LocationSuccessMessage m);
    int send_location_failure_response(LocationFailureMessage m);

    int receive_location_request(int socket);

    LocationSuccessMessage create_location_success_message(char* serverAddr, char* serverPort);
    LocationFailureMessage create_location_failure_message(int reasonCode);



    int receive_register_request(int socket, int length);

};


#endif //CS4544REEAL_BINDER_H
