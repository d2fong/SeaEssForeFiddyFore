//
// Created by Dylan Fong on 2015-07-06.
//


//TODO put these in its own file
extern int LOCATION_FAILURE = -1;
extern int EXECUTE_FAILURE = -2;


#include "rpc.h"

Client c;

int rpcCall(char* name, int* argTypes, void** args) {
    char* binderAddr = getenv("BINDER_ADDR");
    char* binderPort = getenv("BINDER_PORT");

    c = Client();
    c.connect_to_something(binderAddr, binderPort);

    Message locMsg = c.create_location_request(name, argTypes);
    c.send_location_request(locMsg);

    Message locRsp = c.receive_location_response();

    if (locRsp.messageType == LOCATION_FAILURE) {
        return LOCATION_FAILURE;
    } else {
        Message execMsg = c.create_execute_request(name, argTypes, args);
        c.send_execute_request(execMsg);
        Message execRsp = c.receive_execute_response();

        if (execRsp.messageType == EXECUTE_FAILURE) {
            return EXECUTE_FAILURE;
        } else {
            return 0;
        }
    }
}