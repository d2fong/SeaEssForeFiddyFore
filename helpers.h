//
// Created by Dylan Fong on 2015-07-06.
//

#ifndef CS4544REEAL_SOURCE_FILE_H
#define CS4544REEAL_SOURCE_FILE_H

#include <iostream>
#include <string>
#include <vector>
#include "DB.h"

using namespace std;

int get_int_array_length(int* intArray);
int send_all(int socket, char* buf, int* bytesToSend);
int recv_all(int socket, char* buf, int* bytesToReceive);
int create_connection_socket(unsigned short portnum, unsigned short* ret_port, char* ret_host);
int connect_to(char* addr, char* port);
vector<string> split(const string &text, char sep);
string marshall_args (int * argTypes, void **args, int arg_length) ;
string server_marshall_args (int * argTypes, void **args, int arg_length) ;
string to_stri(int i);
int calculate_arg_size(vector<Args> arg_info);
int unmarshall_args (void ***args, vector<Args> arg_info, vector<string>marshall);
string append_vector_string (vector<string> vec, int to, int from);
#endif //CS4544REEAL_SOURCE_FILE_H
