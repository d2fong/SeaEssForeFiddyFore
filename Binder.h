//
// Created by Dylan Fong on 2015-07-06.
//

#ifndef CS4544REEAL_BINDER_H
#define CS4544REEAL_BINDER_H


class Binder {
private:
    int sockForClients;
    int initialized;
    FuncDb fdb;

    void print_status();


public:

    Binder(int sockForClients, const ::Binder::FuncDb &fdb, int initialized) : sockForClients(sockForClients), fdb(fdb),
                                                                               initialized(initialized) { }

    int is_initialized();
    void init();


    Message create_register_response_message(MessageType, int status);
    //TODO finish the function signatures for the message
//    Message create_location_success_message();
    Message create_location_failure_message(int status);

    void send_register_response(Message m);
    void send_location_response(Message m);


    Message receive_terminate_request();
    Message receive_register_request();
    Message receive_location_request();


};


#endif //CS4544REEAL_BINDER_H
