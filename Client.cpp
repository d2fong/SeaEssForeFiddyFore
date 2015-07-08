//
// Created by Dylan Fong on 2015-07-05.
//


#include "Client.h"
#include "helpers.h"
#include "constants.h"
#include "DB.h"

#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <string>

using namespace std;


int Client::get_binder_socket() {
    return binderSocket;
}

int Client::get_server_socket() {
    return serverSocket;
}

int Client::connect_to_something(char *addr, char *port) {
    struct addrinfo hints, *res;
    int s;
    int connectionResult;

// first, load up address structs with getaddrinfo():
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    getaddrinfo(addr, port, &hints, &res);

    // make a socket:
    s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if (s < 0) {
        cout << "error: socket failure" << endl;
        return s;
    }

    // connect!
    connectionResult = connect(s, res->ai_addr, res->ai_addrlen);

    if (connectionResult != 0) {
        cout << "error: connection failure" << endl;
        return connectionResult;
    }

    // set the binder socket to be the sockfd
    binderSocket = s;


    return 0;

}

LocationRequestMessage Client::create_location_request(char* funcName, int* argTypes) {

    int funcNameLength = (strlen(funcName) + 1) * sizeof(char);
    int arg_length=0;
    while (argTypes[arg_length++]);


//    int argTypesLength = get_int_array_length(argTypes);
    int argTypesLength = arg_length-1;

    //Encode the funcname and argTypes into the buffer
    if (funcNameLength > MAXFUNCNAME) {
        funcNameLength = MAXFUNCNAME;
    }
    char* funcNameBuf = new char[funcNameLength];
    memcpy(funcNameBuf, funcName, funcNameLength);

    LocationRequestMessage ret = LocationRequestMessage(funcNameLength, argTypesLength, funcNameBuf, argTypes);
    ret.setType(LOCATION_REQUEST);
    
    return ret;
}

//Send the location request message
int Client::send_location_request(LocationRequestMessage m, int binderSocket) {

    Function func = Function(string(m.getFuncNameBuffer()), m.getArgTypesBuffer(),m.getArgTypesLength());

    int key_size = func.get_key().length() + 1;
    cout << "KEY SIZE" << key_size << endl;
    int cbf_length = key_size;
    int ibf_length = 8;

    int m_length =  key_size;
    int b_length = htonl(m_length);
    int b_type = htonl(m.getType());

    char *buffer= new char[cbf_length+ibf_length];

    memcpy(buffer,&b_type, 4); // Copy Type
    memcpy(buffer+4,&b_length, 4); // Copy Length
    memcpy(buffer+8, func.get_key().c_str(),key_size); //Key

    cout << "A" << func.get_key().c_str() << endl;

    int byte_length = cbf_length+ ibf_length;
    return send_all(binderSocket, buffer, &byte_length);
}



//Create a execute request message
Message Client::create_execute_request(char* func_name, int* argTypes, void** args){
    return Message();
}
