//
// Created by Dylan Fong on 2015-07-06.
//

#ifndef CS4544REEAL_SOURCE_FILE_H
#define CS4544REEAL_SOURCE_FILE_H


int get_int_array_length(int* intArray);
int send_all(int socket, char* buf, int* bytesToSend);
int recv_all(int socket, char* buf, int* bytesToReceive);


#endif //CS4544REEAL_SOURCE_FILE_H
