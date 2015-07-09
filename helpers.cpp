//
// Created by Dylan Fong on 2015-07-06.
//

#include "helpers.h"
#include <sys/socket.h>
#include "constants.h"
#include <netinet/in.h>
#include <iostream>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <vector>
#include <string>
#include "DB.h"

using namespace std;


string marshall_args (int * argTypes, void **args, int arg_length) {


    vector <Args> a;
    for (int i =0; i < arg_length; i++) {
        a.push_back(Args(argTypes[i]));
    }
    stringstream  s;
    string buff="";
    int curr_index=0;
    for (int i =0; i < arg_length; i++) {
        switch (a[i].get_type()) {
            case ARG_CHAR: {
                if (a[i].get_scalar() == 1) {
                    s << *((char*) (args[i])) << "|";
                }
                else {
                    for (int  m=0; m < a[i].get_arr_length();m++) {
                        s << ((char *)(args[i]))[m] << "|";
                    }
                }
                break;
            }
            case ARG_SHORT: {
                if (a[i].get_scalar() == 1) {
                    s << *((short*) (args[i])) << "|";
                }
                else {
                    for (int  m=0; m < a[i].get_arr_length();m++) {
                        s << ((short *)(args[i]))[m] << "|";
                    }
                }
                break;
            }
            case ARG_INT: {
                if (a[i].get_scalar() == 1) {
                    s << *((int*) (args[i])) << "|";
                }
                else {
                    for (int  m=0; m < a[i].get_arr_length();m++) {
                        s << ((int *)(args[i]))[m] << "|";
                    }
                }
                break;
            }
            case ARG_LONG: {
                if (a[i].get_scalar() == 1) {
                    s << *((long*) (args[i])) << "|";
                }
                else {
                    for (int  m=0; m < a[i].get_arr_length();m++) {
                        s << ((long *)(args[i]))[m] << "|";
                    }
                }
                break;
            }
            case ARG_DOUBLE: {
                if (a[i].get_scalar() == 1) {
                    s << *((double*) (args[i])) << "|";
                }
                else {
                    for (int  m=0; m < a[i].get_arr_length();m++) {
                        s << ((double *)(args[i]))[m] << "|";
                    }
                }
                break;
            }
            case ARG_FLOAT: {
                if (a[i].get_scalar() == 1) {
                    s << *((float*) (args[i])) << "|";
                }
                else {
                    for (int  m=0; m < a[i].get_arr_length();m++) {
                        s << ((float *)(args[i]))[m] << "|";
                    }
                }
                break;
            }
            default: {
                return "";
            }
        }
    }
    return s.str();
}

/**
 * Get the size of a argsArray passed by the rpcCall
 */
int get_int_array_length(int* intArray) {
    int i = 0;
    while(intArray[i] != 0) {
        i++;
    }
    //We dont count the zero we find as another argType
    return i - 1;
}

/**
 * Helper to send all of buf to the socket in 4 byte chunks
 */
int send_all(int socket, char* buf, int* bytesToSend) {
    int total = 0;        // how many bytes we've sent
    int bytesLeft = *bytesToSend; // how many we have left to send
    int n;

    while(total < *bytesToSend) {
        n = send(socket, buf+total, bytesLeft, 0);
        if (n == -1) { break; }
        total += n;
        bytesLeft -= n;
    }

     *bytesToSend = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
}

/**
 * Helper to recv a specific amount of bytes into the buffer
 */
 int recv_all(int socket, char* buf, int* bytesToReceive) {
    int total = 0;
    int bytesLeft = *bytesToReceive;
    int n;

    while(total < *bytesToReceive) {
        n = recv(socket, buf+total,bytesLeft, 0);
        if (n == -1) { break; }
        total += n;
        bytesLeft -= n;
    }

    *bytesToReceive = total;

    return n==-1?-1:0;
}

/**
 * Creates a connection socket
 */
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

    if (::bind(s,(struct sockaddr *) &sa,sizeof(struct sockaddr_in)) < 0) {
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

/*
 * Connect to server
 */
int connect_to(char* addr, char* port) {

    struct addrinfo hints, *res;
    int sockfd;
    // first, load up address structs with getaddrinfo():

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    getaddrinfo(addr, port, &hints, &res);

    // make a socket:

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    // connect!

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) != 0) {
        cout << "GOT HERE"<< endl;

        return -1;
    }
    else {
        return sockfd;
    }
}


string to_stri(int i) {
    string s;
    stringstream out;
    out << i;
    return out.str();
}
void split(vector<string> &tokens, const string &text, char sep) {
    int start = 0, end = 0;
    while ((end = text.find(sep, start)) != string::npos) {
        tokens.push_back(text.substr(start, end - start));
        start = end + 1;
    }
    tokens.push_back(text.substr(start));
}
