#include "Server.h"
#include "helpers.h"
#include "constants.h"
#include "error.h"

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

    return ret;
}

int Server::send_register_request(RegisterMessage m, int binderSocket) {
    int byte_length = m.getBuffLength();
    int ret = send_all(binderSocket,m.getBuff(),&byte_length);
    if (ret == -1) {
        return ERR_REG_RESPONSE_FAIL;
    }
    delete [] m.getBuff();
    return 0;
}


int Server::send_execute_response(int socket, Function f, string key, string marshall, int reason_code) {
    reason_code= htonl(reason_code);
    int flag;
    char *buff= NULL;
    int byte_length;
    int cbf_length;
    int ibf_length;
    if (reason_code != 0) {
        int msg_size = 8;
        flag = htonl(EXECUTE_FAILURE);
        buff = new char[msg_size];
        memcpy (buff, &flag, 4);
        memcpy (buff+4, &reason_code, 4);
        int a = send_all(socket, buff,&msg_size);
        if (a < 0) {
            return ERR_SENDING_EXEC_RESP;
        }
    }
    else {

        flag = htonl(EXECUTE_SUCCESS);

        int cbf_length = marshall.length()+1 + key.length()+1;
        int ibf_length = 16;

        int b_mars_length = htonl(marshall.length()+1);
        int mlen= htonl(cbf_length+ ibf_length-8);
        int b_key_len = htonl(f.get_key().length());

        buff = new char [cbf_length+ibf_length];
        memcpy (buff, &flag, 4);
        memcpy (buff+4, &mlen, 4);
        memcpy (buff+8, &b_mars_length, 4);
        memcpy (buff+12, &b_key_len, 4);
        memcpy (buff+16, marshall.c_str(), marshall.length()+1);
        memcpy (buff+16 + marshall.length()+1, f.get_key().c_str(), f.get_key().length()+1);

        byte_length= cbf_length+ ibf_length;
        return send_all(socket, buff, &byte_length);
    }
    return 0;
}


