#include <iostream>
#include <map>
#include <sys/socket.h>
#include <string>

// File includes
#include "constants.h"
#include "helpers.h"
#include "error.h"
#include "rpc.h"
#include "Server.h"

using namespace std;

Server s;


/**
 * Creates connection socket to service clients. Also opens a connection with the binder
 */
int rpcInit() {
    unsigned short ret_port;
    char ret_host[MAXHOSTNAME + 1];

    cout << "rpcInit invoked" << endl;

    int client_socket = create_connection_socket(SERVERPORT, &(ret_port), ret_host);
    if (client_socket <= 0) {
        return ERR_CREATE_SOCK_FAIL;
    }

    cout << "Created client socket" << endl;

    //Open connection to binder

    char *binder_addr = getenv("BINDER_ADDRESS");
    char *binder_port = getenv("BINDER_PORT");

    int binder_socket = connect_to(binder_addr, binder_port);
    if (binder_socket < 0) {
        return ERR_BINDER_CONNECT_FAIL;
    }

    s = Server::Server(string(ret_host), ret_port, client_socket,binder_socket);

    printf ("%s \n", "Connected to binder");
    return 0;
}

/**
 * 1) Calls binder and informs it that the server procedure with the indicated name and arg type is available at this server
 * 2) Adds <serverId, function> to a local map
 * */
int rpcRegister(char* name, int* argTypes, skeleton f) {


    int arg_length = 0;
    while (argTypes[arg_length++]);

    Function func = Function(string(name), argTypes, arg_length-1);

    cout << "Got to beginning of rpcRegister" << endl;

    RegisterMessage reg_msg = s.create_register_message(func);
    if (reg_msg == NULL) {
        return ERR_REG_FAIL;
    }

    cout << "Binder Register Message generated" << endl;

//    int m_send = s.send_register_request(reg_msg, s.get_binder_socket());
//    if (m_send == -1) {
//        return ERR_REG_FAIL;
//    }
//
//    int m_recv = recv_message(BINDER_FD, &response);
//    if (response.type == REGISTER_FAILURE) {
//        return ERR_REG_FAIL;
//    }
//    else if(response.type == REGISTER_WARNING) {
//        return REGISTER_WARNING;
//    }
//    else {
//        return 0;
////        return update_local_db(func, f);
//    }
    return 0;
}
//
//int update_local_db(func_info *func, skeleton f) {
//
//    cout << func->key << endl;
//    if (func_db.find(func->key) == func_db.end()) {
//        func_db.insert(map<string, skeleton>::value_type(func->key, f));
//    }
//    else {
//        func_db[func->key]= f;
//    }
//
//    free(func->args);
//    free(func);
//    return 0;
//}

int rpcExecute() {
    //TODO
    return 0;
}


