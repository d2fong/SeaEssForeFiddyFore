#include <iostream>
#include <map>
#include <sys/socket.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>
#include <netdb.h>



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


int handle_request (int socket, int messageType);
int exec_args(int socket, string key, string args_s);
int send_client_msg (int * argTypes, void **args, int arg_length);

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
    if (m_send < 0) {
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


    listener = s.get_client_socket();
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
                        if (r < 0) {
                            cout << "We errd in server" << endl;
                        }
                    }
                }
            }
        }
    }
    return 0;
}


int handle_request (int socket, int messageType) {
    int rec;
    switch (messageType) {
        case EXECUTE: {
            int m_length =0;
            int key_len = 0;
            int args_len = 0;
            int n_key_len=0;
            int n_args_len=0;

            rec = recv(socket, &(m_length), 4, 0);
            if (rec < 0) {
                return ERR_SERVER_CLIENT_RECV;
            }
            int n_len = ntohl(m_length);
            cout << "N_Length " << n_len << endl;

            char * buff = new char [n_len];
            rec = recv(socket, buff, n_len,0);

            memcpy (&key_len, buff,4);
            n_key_len = ntohl(key_len);

            cout << "Key Length " << n_key_len << endl;

            char key[n_key_len+1];
            memcpy (key, buff+4, n_key_len);
            key[n_key_len]= '\0';

            cout << "Key " << key << endl;

            memcpy(&args_len, buff+4+n_key_len,4);
            n_args_len = ntohl(args_len);
            cout << "Args Length " << n_args_len << endl;

            char args[n_args_len+1];
            memcpy(args, buff+8+n_key_len, n_args_len);
            args[n_args_len] ='\0';


            cout << "KEY: " << key << endl;
            cout << "ARGS: " << args << endl;
            cout << "ARG LENGTH" << n_args_len << endl;
            delete [] buff;
            return exec_args(socket,key, args);
        }
        default: {
            return 0;
        }
    }
}

int exec_args(int socket, string key, string arg_s) {
    Function f;
    vector<string> key_s = split(key, '|');
    vector<string> args_s = split(arg_s, '|');
    vector<Args> arg_info;
    int res=0;

    string f_name = key_s[0];
    int arg_length = stoi(key_s[1]);
    cout << "creating args " << endl;
    int index=2;
    for (int i=0; i < arg_length; i++) {
        arg_info.push_back(Args(stoi(key_s[index]),stoi(key_s[index+1]),stoi(key_s[index+2]), stoi(key_s[index+3])));
        index+=4;
    }

    int arg_size= calculate_arg_size(arg_info);
    void ** args = (void **) malloc(arg_size);
    cout << "malloc args size " << arg_size << endl;

    int unmarshall = unmarshall_args(args, arg_info, args_s);
    if (unmarshall < 0) {
        return ERR_UNMARSHALLING_SERVER;
    }

    cout << "unmarshall args " << endl;


    if (server_db.functions.find(key) != server_db.functions.end()) {
        f = server_db.functions[key];
        cout << "Got f" << endl;
        if (server_db.lookup.find(key) != server_db.lookup.end()) {
            skeleton q = server_db.lookup[key];
            cout << "Got skeleton" << endl;
            res = (*q)(f.get_argtypes(), args);
            if (res != 0) {
                res = ERR_INVALID_ARGS;
            }
        }
        else {
            cout << "Did not find skeleton" << endl;
        }

        cout << "About to Marshall args" << endl;
        string marshall  = marshall_args(f.get_argtypes(),args, arg_length);
        delete [] args;
        cout << "Marshall in exec: " << marshall << endl;
        return s.send_execute_response(socket, f,key,marshall, res);
    }
    else {
        cout << "error: server doenst contain func " << endl;
        return ERR_SERVER_DOESNT_CONTAIN_FUNC;
    }
}


