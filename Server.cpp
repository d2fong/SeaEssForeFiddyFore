#include "Server.h"
#include "helpers.h"
#include "constants.h"


#include <iostream>
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
    int cbf_length = MAXHOSTNAME + 1 + MAXFUNCNAME + 1 + key_size;
    int ibf_length = 12;

    int m_length =  cbf_length+ibf_length-8;
    int b_length = htonl(m_length);
    int b_type = htonl(ret.getType());
    int b_port = htonl(port);

    char *buffer= new char[cbf_length+ibf_length];

    memcpy(buffer,&b_type, 4); // Copy Type
    memcpy(buffer+4,&b_length, 4); // Copy Length
    memcpy(buffer+8, host.c_str(), MAXHOSTNAME+1); //Server name
    memcpy(buffer+8+MAXHOSTNAME+1, &b_port, 4);
    memcpy(buffer+12+MAXHOSTNAME+1, f.get_name().c_str() ,MAXFUNCNAME +1);
    memcpy(buffer+12+MAXHOSTNAME+1+MAXFUNCNAME+1, f.get_key().c_str(), key_size);

    ret.set_register_message(host,port,f,buffer,cbf_length+ibf_length);

//    cout << "Name: " << f.get_name() << endl;
//    cout << "S_name : " << host << endl;
//    cout << "Port: " << port << endl;
//    cout << "Key: " << f.get_key() << endl;
//    cout << "Key Size: " << key_size << endl;

    return ret;
}

int Server::send_register_request(RegisterMessage m, int binderSocket) {
    int byte_length = m.getBuffLength();
    cout << "Buff Length: " << byte_length << endl;
    return send_all(binderSocket,m.getBuff(),&byte_length);
}