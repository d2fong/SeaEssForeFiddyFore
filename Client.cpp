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

void Client::set_server_socket(int socket) {
    serverSocket = socket;
}

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


//
//Create a execute request message
int Client::send_execute_request(int serverSocket, char *name, int *argTypes, void **args){
    int arg_length = 0;
    while(argTypes[arg_length++]);
    arg_length -= 1;

    Function func = Function(string(name), argTypes, arg_length);
    string func_key = func.get_key();
    int func_size = func_key.length() + 1;

    string dataMarshallingKey = marshall_args(argTypes, args, arg_length);
    cout << "MARSHALL KEY" << dataMarshallingKey << endl;

    int marshallSize = dataMarshallingKey.length() + 1;
    cout << "MARSHALL SIZE" << marshallSize << endl;

    int cbf_length = func_size + marshallSize;
    int ibf_length = 16;

    int m_length = func_size;
    int b_length = htonl(m_length);

    int mars_length = marshallSize;
    int b_mars_length = htonl(mars_length);

    int b_type = htonl(EXECUTE);

    char *buffer = new char[cbf_length + ibf_length];

    int mlen= htonl(cbf_length+ ibf_length-8);

    memcpy(buffer, &b_type, 4);
    memcpy(buffer+4,&mlen , 4);
    memcpy(buffer+8, &b_length, 4);
    memcpy(buffer+12, func_key.c_str(), func_size);
    memcpy(buffer+12+func_size, &b_mars_length, 4);
    memcpy(buffer+16+func_size, dataMarshallingKey.c_str(), mars_length);

    cout << "Sending execute message" << endl;

    int byte_length = cbf_length + ibf_length;
    return send_all(serverSocket, buffer, &byte_length);
}
