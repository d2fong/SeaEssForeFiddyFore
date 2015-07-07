
#include <iostream>
#include "Server.h"
#include "helpers.h"
#include <netinet/in.h>
#include <string.h>



using namespace std;

int Server::get_binder_socket() {
    return binderSocket;
}

int Server::get_client_socket() {
    return clientSocket;
}

string Server::get_host() {
    return host;
}

int Server::get_port() {
    return port;
}

RegisterMessage Server::create_register_message(Function f) {

    int key_size = f.get_key().length() + 1;
    RegisterMessage ret = RegisterMessage();
    ret.setType(REGISTER);

    //create a length to form the message
    int cbf_length = host.length() + 1 + f.get_name().length() + 1 + key_size;
    int ibf_length = 4 + 4 + 4;

    int m_length =  cbf_length+ibf_length-8;
    int b_length = htonl(m_length);
    int b_type = htonl(ret.getType());
    int b_port = htonl(port);

    char *buffer= new char[cbf_length+ibf_length];

    memcpy(buffer,&b_length, 4); // Copy Length
    memcpy(buffer+4,&b_type, 4); // Copy Type
    memcpy(buffer+8, &host, host.length()+1); //Server name
    memcpy(buffer+8+host.length()+1, &b_port, 4);
    memcpy(buffer+12+host.length()+1, f.get_name().c_str(),f.get_name().length()+1);
    memcpy(buffer+12+host.length()+1+f.get_name().length()+1, f.get_key().c_str(), key_size);

    ret.set_s_name(host);
    ret.set_port(port);
    ret.set_f_name(f.get_name());
    ret.set_arg_key(f.get_key());
    ret.set_buff(buffer);
    ret.set_buff_length(cbf_length+ibf_length);

    return ret;
}

int Server::send_register_request(RegisterMessage m, int binderSocket) {
    return 0;
}