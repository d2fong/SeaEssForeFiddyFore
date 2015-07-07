//
// Created by Dylan Fong on 2015-07-06.
//

#include "helpers.h"
#include <sys/socket.h>

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