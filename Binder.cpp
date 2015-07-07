//
// Created by Dylan Fong on 2015-07-06.
//

#include "Binder.h"
#include "constants.h"
#include "helpers.h"

#include <iostream>
#include <map>
#include <sys/select.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

using namespace std;


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
                      // The first thing our clients will do is send us the message type
                    int clientMessageType;
                    int nbytes = recv(i, (char*)&clientMessageType, 4, 0);
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
                        cout << ntohl(clientMessageType) << endl;
//                        int r = handle_request(i,m);

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
