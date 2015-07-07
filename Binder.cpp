//
// Created by Dylan Fong on 2015-07-06.
//

#include "Binder.h"
#include "constants.h"

#include <iostream>
#include <map>
#include <sys/select.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

using namespace std;


int create_connection_socket(unsigned short portnum, unsigned short* ret_port, char* ret_host)
{
    int    s;
    struct sockaddr_in sa;
    struct hostent *hp;
    char myname[MAXHOSTNAME+1];

    memset(&sa, 0, sizeof(struct sockaddr_in)); /* clear our address */
    int a = gethostname(myname, MAXHOSTNAME);           /* who are we? */
    hp = gethostbyname(myname);                  /* get our address info */

    if (hp == NULL)                           /* we don't exist !? */
        return(-1);

    sa.sin_family= AF_INET;              /* this is our host address */
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_port= htons(portnum);                /* this is our port number */

    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return(-1);

    }

    if (bind(s,(struct sockaddr *) &sa,sizeof(struct sockaddr_in)) < 0) {
        close(s);
        return(-1);                               /* bind address to socket */
    }

    socklen_t len = sizeof(sa);
    if(getsockname(s, (struct sockaddr *)&sa, &len) == -1) {
        return(-1);
    }

    //copy the host name and port numbers to stack arguments
    strncpy(ret_host, hp->h_name, strlen(hp->h_name) + 1);
    *ret_port = (unsigned short) ntohs(sa.sin_port);

    //listen to at most 5 clients
    listen(s, 5);
    return(s);
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
                    // handle data from a client
//                    msg m;
//                    nbytes = recv_message(i, &m);
//                    if (nbytes <= 0) {
//                        // got error or connection closed by client
//                        if (nbytes == 0) {
//                            // connection closed
//                            printf("selectserver: socket %d hung up\n", i);
//                        } else {
//                            perror("recv");
//                        }
//                        close(i); // bye!
//                        FD_CLR(i, &master); // remove from master set
//                    }
//                    else {
//                        cout << buf << endl;
//                        int r = handle_request(i,m);
//
//                    }
                }
            }
        }
    }
}

void Binder::print_status() {
    cout << "BINDER_ADDRESS " <<  binderAddr << endl;
    cout << "BINDER_PORT " << binderPort << endl;

}
