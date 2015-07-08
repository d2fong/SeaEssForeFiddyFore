//
// Created by Dylan Fong on 2015-07-06.
//

#include "Binder.h"
#include "constants.h"
#include "helpers.h"
#include "Message.h"
#include "error.h"
#include "DB.h"

#include <iostream>
#include <map>
#include <sys/select.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


using namespace std;

int Binder::receive_register_request (int socket, int length) {
    int port;
    char s_name [MAXHOSTNAME +1];
    char f_name [MAXFUNCNAME + 1];
    int key_size = length - (MAXFUNCNAME + 1 + MAXHOSTNAME + 1 + 4);
    char key [key_size+1];

    cout << "BUFF LENGTH" << length << endl;
    char *buff = new char[length];
    int recv_buff = recv(socket, buff, length, 0);
    if (recv_buff <0) {
        return ERR_RECV_FAIL;
    }


    memcpy (s_name, buff, MAXHOSTNAME+1);
    memcpy (&port, buff+MAXHOSTNAME+1, 4);
    memcpy (f_name, buff+MAXHOSTNAME+1+4, MAXFUNCNAME+1);
    memcpy (key, buff+MAXHOSTNAME + 1 + MAXFUNCNAME + 1 + 4, key_size);
    key[key_size] ='\0';

//    cout << "Name: " << f_name << endl;
//    cout << "S_name : " << s_name << endl;
//    cout << "Port: " << ntohl(port) << endl;
//    cout << "Key: " << key << endl;


    return 0;
}

int Binder::handle_request(int socket, int type) {

    int m_len = 0;

    int retVal = 0;
    switch(type){
//        case REGISTER:
//            int m_len=0;
//            int recv_length = recv(socket,&(m_len),4,0);
//            if (recv_length <0) {
//                return ERR_RECV_FAIL;
//            }
//            int rec_reg_req = receive_register_request(socket, ntohl(m_len));
//            if (rec_reg_req != 0) {
//                return ERR_RECV_FAIL;
//            }
//            return 0;
        case LOCATION_REQUEST:
            retVal = receive_location_request(socket);
            break;
        default:
            retVal = -1;
            break;
    }
    return retVal;
}

int Binder::init() {
    char addr[MAXHOSTNAME+1];
    unsigned short port;
    int socket = create_connection_socket(BINDERPORT, &port, addr);

    if (socket < 0) {
        perror("socket connect fail");
    }

    binderPort = port;
    binderAddr = addr;
    print_status();

    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number

    int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

    char buf[256];    // buffer for client data
    int nbytes;


    int i;

    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);


    listener = socket;
    if(listener == -1)
        exit(-1);

    // add the listener to the master set
    FD_SET(listener, &master);

    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one

    // main loop
    for(;;)
    {
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1)
        {
            perror("select");
            exit(4);
        }
        // run through the existing connections looking for data to read
        for(i = 0; i <= fdmax; i++)
        {
            if (FD_ISSET(i, &read_fds))
            {
                if (i == listener)
                {
                    addrlen = sizeof(remoteaddr);
                    newfd = accept(listener,(struct sockaddr* ) &remoteaddr , &addrlen);

                    if (newfd == -1)
                    {
                        perror("accept");
                    }
                    else
                    {
                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax)
                        {    // keep track of the max
                            fdmax = newfd;
                        }
                    }
                }
                else {

                    int messageType;
                    //Get Length
                    int nbytes = recv(i, (char*)&messageType, 4, 0);
                    if (nbytes <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0) {
                            // connection closed
                            printf("selectserver: socket %d hung up\n", i);
                        } else {
                            perror("recv");
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
                    }
                    else {
                        int r = handle_request(i,ntohl(messageType));

                    }
                }
            }
        }
    }
}

void Binder::print_status() {
    cout << "BINDER_ADDRESS " <<  binderAddr << endl;
    cout << "BINDER_PORT " << binderPort << endl;

}


int Binder::receive_location_request(int socket) {
    int result = 0;
    int funcNameLength = 0;
    int actual = 4;
    result += recv_all(socket, (char*)&funcNameLength, &actual);
    funcNameLength = ntohl(funcNameLength);
    cout << funcNameLength << endl;

    char funcNameBuffer[funcNameLength];
    actual = funcNameLength;
    result += recv_all(socket, funcNameBuffer, &actual);
    cout << funcNameBuffer << endl;

    int argsLength = 0;
    actual = 4;
    result += recv_all(socket, (char*)&argsLength, &actual);
    argsLength = ntohl(argsLength);
    cout << argsLength << endl;

    char argsTypeBuffer[argsLength];
    actual = argsLength;
    result += recv_all(socket, (char*)&argsTypeBuffer, &actual);
    cout << argsTypeBuffer << endl;

    return 0;
}
