//
// Created by Dylan Fong on 2015-07-05.
//
//
//
//#include "Client.h"
//
//#include <iostream>
//#include <stdio.h>
//#include <string.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <netdb.h>
//#include <protocol.h>
//
//
//// From http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#sendall
//int send_all(int s, char *buf, int *len)
//{
//    int total = 0;        // how many bytes we've sent
//    int bytesleft = *len; // how many we have left to send
//    int n;
//
//
//    while(total < *len) {
//        n = send(s, buf+total, bytesleft, 0);
//        if (n == -1) { break; }
//        total += n;
//        bytesleft -= n;
//    }
//
//    *len = total; // return number actually sent here
//
//    return n==-1?-1:0; // return -1 on failure, 0 on success
//}
//
//
//int send_message(msg *m, int socket) {
//    return send_message(m,socket, 1);
//}
//
//int send_message(msg *m, int socket, int expect_response) {
//    int status= send_all(socket, m->buf, &(m->length));
//    cout << "Send: msg len " << m-> length << endl;
//    cout << "Send: msg type " << m->type << endl;
//    if (status == -1) {
//        perror("Error sending messagse");
//    }
//    return 0;
//}
//
//
//int recv_message(int socket, msg* m) {
//    int m_len = 0;
//    int m_type = 0;
//
//    int get_m_len = recv(socket, &(m_len), 4, 0);
//    if (get_m_len < 0) {
//        return ERR_REG_RESPONSE_FAIL;
//    }
//
//    int b_len = ntohl(m_len);
//    cout << "Recv: msg len " << b_len << endl;
//
//    int get_m_type = recv(socket, &(m_type), 4, 0);
//    if (get_m_type < 0) {
//        return ERR_REG_RESPONSE_FAIL;
//    }
//
//    int b_type = ntohl(m_type);
//    cout << "Recv: msg type " << b_type << endl;
//
//    char* buf = (char*) malloc(sizeof(char) * (b_len));
//    int msg_content = recv(socket, buf, b_len, 0);
//    if (msg_content < 0) {
//        return ERR_REG_RESPONSE_FAIL;
//    }
//
//    m->buf = buf;
//    m->length = b_len;
//    m->type = b_type;
//
//    return msg_content;
//}
//
//
//
//
//
//
//
//
//
//// Return 0 on success, non zero value on error
//int Client::connect_to_binder(char* binder_addr, char* binder_port) {
//
//
//    struct addrinfo hints, *res;
//    int sockfd;
//    int connect_result;
//
//    // first, load up address structs with getaddrinfo():
//    memset(&hints, 0, sizeof hints);
//    hints.ai_family = AF_UNSPEC;
//    hints.ai_socktype = SOCK_STREAM;
//
//    getaddrinfo(binder_addr, binder_port, &hints, &res);
//
//    // make a socket:
//    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
//
//    if (sockfd < 0) {
//	return sockfd;
//    }
//
//    // connect!
//    connect_result = connect(sockfd, res->ai_addr, res->ai_addrlen);
//
//    if (connect_result != 0) {
//        return connect_result;
//    }
//
//    // set the binder socket to be the sockfd
//    binder_socket = sockfd;
//
//    return 0;
//}
//
//int Client::get_binder_socket() {
//    return binder_socket;
//}
//
//
//
//
//int Client::create_location_request(char* func_name, int* argTypes, msg* m) {
//
//    int glm = gen_loc_msg(func_name, argTypes, m);
//
//    return 0;
//}
//
//int Client::create_execute_request(char* func_name, int* argTypes, void** args, msg* m) {
//
//    int gem = gen_exec_msg(func_name, argTypes, args, m);
//
//    return 0;
//}
//
////Message create_execute_request(string func_name, int* argTypes, void** args);
////void send_location_request(Message m);
////void send_execute_message(Message m);
////Message receive_location_response();
////Message receive_execute_response();