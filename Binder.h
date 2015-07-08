//
// Created by Dylan Fong on 2015-07-06.
//

#ifndef CS4544REEAL_BINDER_H
#define CS4544REEAL_BINDER_H

#include <string>
using namespace std;


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


//    //TODO finish the function signatures for the message
////    Message create_location_success_message();
//    Message create_location_failure_message(int status);
//

//    void send_location_response(Message m);
//
//
//    Message receive_terminate_request();
//    Message receive_location_request();


};


#endif //CS4544REEAL_BINDER_H
