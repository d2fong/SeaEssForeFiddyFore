//
// Created by Dylan Fong on 2015-07-06.
//


//TODO put these in its own file
//extern int LOCATION_FAILURE = -1;
//extern int EXECUTE_FAILURE = -2;


#include "rpc.h"
#include "client.h"
#include <stdlib.h>

Client c;

int rpcCall(char* name, int* argTypes, void** args) {
    char* binderAddr = getenv("BINDER_ADDR");
    char* binderPort = getenv("BINDER_PORT");

    c = Client();

    int connectionResult = c.connect_to_something(binderAddr, binderPort);
    if (connectionResult < 0) {
        return connectionResult;
    }
//
//    Message locMsg = c.create_location_request(name, argTypes);
//    int messageResult = c.send_location_request(locMsg);
//    if (messageResult < 0) {
//        return messageResult;
//    }
//    Message locRsp = c.receive_location_response();
//
//    if (locRsp.messageType == LOCATION_FAILURE) {
//        return -1;
//    } else if(locRsp.messageType == LOCATION_SUCCESS) {
//        Message execMsg = c.create_execute_request(name, argTypes, args);
//        c.send_execute_request(execMsg);
//        Message execRsp = c.receive_execute_response();
//
//        if (execRsp.messageType == EXECUTE_FAILURE) {
//            return EXECUTE_FAILURE;
//        } else {
//            return 0;
//        }
//    }
    return 0;
}