//
// Created by Dylan Fong on 2015-07-06.
//

#ifndef CS4544REEAL_BINDER_H
#define CS4544REEAL_BINDER_H

#include "Message.h"
#include <string>
using namespace std;


class Binder {
private:
    unsigned short binderPort;
    char* binderAddr;
    void print_status();
    int handle_request(int socket, int type, fd_set *master);
public:

    Binder() {
    }


    int init();
    int receive_register_request(int socket, int length);
    int send_register_response(int socket, int flag);
    int send_location_response(int socket, string key);

    int receive_location_request(int socket, int length);

    LocationSuccessMessage create_location_success_message(char* serverAddr, char* serverPort);
    LocationFailureMessage create_location_failure_message(int reasonCode);

    int shutdown_everything(fd_set *master);
    int update_binder_dbs(int socket);


};


#endif //CS4544REEAL_BINDER_H
