//
// Created by Dylan Fong on 2015-07-06.
//

#ifndef CS4544REEAL_BINDER_H
#define CS4544REEAL_BINDER_H


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


//    Message create_register_response_message(MessageType, int status);
//    //TODO finish the function signatures for the message
////    Message create_location_success_message();
//    Message create_location_failure_message(int status);
//
//    void send_register_response(Message m);
//    void send_location_response(Message m);
//
//
//    Message receive_terminate_request();
      int receive_register_request(int socket, int length);
//    Message receive_location_request();


};


#endif //CS4544REEAL_BINDER_H
