//
// Created by Dylan Fong on 2015-07-06.
//


//TODO put these in its own file
//extern int LOCATION_FAILURE = -1;
//extern int EXECUTE_FAILURE = -2;


#include "rpc.h"
#include "Client.h"
#include "error.h"
#include "constants.h"
#include <sys/socket.h>
#include "helpers.h"
#include <iostream>
#include <stdlib.h>

using namespace std;
Client c = Client();

int rpcCall(char* name, int* argTypes, void** args) {
    char *binderAddr = getenv("BINDER_ADDRESS");
    char *binderPort = getenv("BINDER_PORT");
    int i = 0;
    while (argTypes[i++]);

    cout << "Init rpc call" << endl;

    //Connect to the binder
    int connectionResult = c.connect_to_something(binderAddr, binderPort);
    if (connectionResult < 0) {
        cout << "error: rpc_client couldn't connect to binder" << endl;
        return connectionResult;
    }


    cout << "Connected to binder" << endl;
    //Create a message and serialize name and argTypes into a buffer
    LocationRequestMessage locMsg = c.create_location_request(name, argTypes);
    int messageResult = c.send_location_request(locMsg, c.get_binder_socket());
    if (messageResult < 0) {
        cout << "error: rpc_client couldn't send the message correctly" << endl;
        return messageResult;
    }

    int msg_type;
    int reason_code = 0;
    int res = recv(c.get_binder_socket(), &msg_type, 4, 0);
    if (res == -1) {
        return ERR_BINDER_RECV_FAIL;
    }
    msg_type = ntohl(msg_type);
    if (msg_type == LOCAITON_FAILURE) {
        cout << "Location failure" << endl;
        res = recv(c.get_binder_socket(), &reason_code, 4, 0);
        if (res == -1) {
            return ntohl(reason_code);
        }
    }
    else if (msg_type == LOCATION_SUCCESS) {
        cout << "Location success" << endl;
        char buff[MAXHOSTNAME + 1 + 4];
        char s_name[MAXHOSTNAME + 1];
        int port = 0;
        res = recv(c.get_binder_socket(), buff, MAXHOSTNAME + 1 + 4, 0);
        memcpy(s_name, buff, MAXHOSTNAME + 1);
        memcpy(&port, buff + MAXHOSTNAME + 1, 4);
        s_name[MAXHOSTNAME] = '\0';
        port = ntohl(port);
        cout << "server" << s_name << endl;
        cout << "port" << port << endl;

        //connect to the server
        int s_server = connect_to(s_name, (char *) to_stri(port).c_str());
        if (s_server < 0) {
            cout << "error: could not connect to specified server" << endl;
            return -1;
        }
        c.set_server_socket(s_server);
        int r = c.send_execute_request(c.get_server_socket(), name, argTypes, args);
//        if (r < 0) {
//            cout << "error:  Couldnt send
//        }
//    } else {
//        cout << "received neither location failure or location success" << endl;
//    }
//
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
    }
    return 0;
}
