#include <iostream>
#include <map>
#include <sys/socket.h>
#include <string>
#include <string.h>
#include <stdlib.h>

// File includes
#include "constants.h"
#include "helpers.h"
#include "error.h"
#include "rpc.h"
#include "Server.h"
#include "DB.h"

using namespace std;

Server s;
ServerDB server_db;

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
        cout << "Could not connect to binder" << endl;
        return ERR_BINDER_CONNECT_FAIL;
    }

    s = Server(string(ret_host), ret_port, client_socket,binder_socket);
    server_db = ServerDB();
    cout << "Connected to binder" << endl;
    return 0;
}

/**
 * 1) Calls binder and informs it that the server procedure with the indicated name and arg type is available at this server
 * 2) Adds <serverId, function> to a local map
 * */
int rpcRegister(char* name, int* argTypes, skeleton f) {

    string flag;
    int arg_length = 0;
    while (argTypes[arg_length++]);

    Function func = Function(string(name), argTypes, arg_length-1);

    cout << "Got to beginning of rpcRegister.." << endl;
    RegisterMessage reg_msg = s.create_register_message(func);
    cout << "Binder Register Message generated.." << endl;

    int m_send = s.send_register_request(reg_msg, s.get_binder_socket());
    if (m_send == -1) {
        return REGISTER_FAILURE;
    }
    cout << "Message to binder is sent" << endl;

    int m_recv = recv(s.get_binder_socket(),&flag,4,0);
    if (atoi(flag.c_str()) == REGISTER_FAILURE) {
        return ERR_REG_FAIL;
    }
    else {
        return server_db.update_db(atoi(flag.c_str()),func,f);
    }
}



int rpcExecute() {
    //TODO
    return 0;
}


