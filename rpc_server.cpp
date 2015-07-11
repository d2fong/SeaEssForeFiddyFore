#include <iostream>
#include <map>
#include <sys/socket.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>
#include <netdb.h>
#include <sstream>



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


    vector <Args> a;
    for (int i =0; i < arg_length; i++) {
        a.push_back(Args(argTypes[i]));
        cout << "REGTYPE: " << a[i].get_type() << endl;
    }

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
    vector<string> marshall = split(arg_s, '|');
    vector<Args> arg_info;
    int res=0;

    string f_name = key_s[0];
    int arg_length = stoi(key_s[1]);
    cout << "" << endl;
    cout << "" << endl;
    cout << "creating args " << endl;
    int index=2;
    for (int i=0; i < arg_length; i++) {
        arg_info.push_back(Args(stoi(key_s[index]),stoi(key_s[index+1]),stoi(key_s[index+2]), stoi(key_s[index+3])));
        cout << arg_info[i].get_type() << endl;
        index+=4;
    }
    cout << "unmarshall args " << endl;

    if (server_db.functions.find(key) != server_db.functions.end()) {
        f = server_db.functions[key];
        cout << "Got f" << endl;
        if (server_db.lookup.find(key) != server_db.lookup.end()) {
            int arg_size= calculate_arg_size(arg_info);
//            void **args;

            void** args = (void**)malloc(sizeof(void*) * arg_info.size());
            cout << "MARSHALL BEFORE UNMARSHALL : " <<  arg_s << endl;


            Args curr_arg;
            int m =0;
            int data,arr_len,offset;
            for (int i =0; i < arg_info.size(); i++) {
                curr_arg = arg_info[i];
                arr_len = curr_arg.get_arr_length();
                cout << "Marshal[i] " << marshall[m] << endl;
                switch (curr_arg.get_type()) {
                    case ARG_CHAR: {
                        if (arr_len==0) {
                            char *c = new char[sizeof(char)];
                            memcpy (c,marshall[m].c_str(), sizeof(char));
                            args[i] = (void *)c;
                            m++;
                        }
                        else {
                            if (curr_arg.get_output()==1 && curr_arg.get_input()==0) {
                                char *arr = new char[sizeof(char)*arr_len+1];
                                args[i] = (void *)arr;
                                m++;
                            }
                            else {
                                char *arr = new char[sizeof(char)*arr_len];
                                int j;
                                char *a;
                                for(j =0; j < arr_len; j++) {
                                    a = new char[sizeof(char)];
                                    memcpy (a, marshall[m+j].c_str(), sizeof(char));
                                    arr[j] = *a;
                                }
                                args[i] = (void *)arr;
                                m +=arr_len;
                            }
                        }
                        break;
                    }
                    case ARG_SHORT: {
                        if (arr_len==0) {
                            short *s = new short[sizeof(short)];
                            int a = stoi(marshall[m].c_str());
                            memcpy (s,&a, sizeof(short));
                            args[i] = (void *)s;
                            m++;
                        }
                        else {
                            if (curr_arg.get_output()==1 && curr_arg.get_input()==0) {
                                short *arr = new short[sizeof(short)*arr_len];
                                args[i] = (void *)arr;
                                m++;
                            }
                            else {
                                short *arr = new short[sizeof(short) * arr_len];
                                int j;
                                short *a;
                                for (j = 0; j < arr_len; j++) {
                                    a = new short[sizeof(short)];
                                    int aa = stoi(marshall[m + j].c_str());
                                    memcpy(a, &aa, sizeof(short));
                                    arr[j] = *a;
                                    cout << arr[j] << endl;
                                }
                                args[i] = (void *) arr;
                                m += arr_len;
                            }
                        }
                        break;
                    }
                    case ARG_INT: {
                        if (arr_len==0) {
                            int *in = new int[sizeof(int)];
                            int a = stoi(marshall[m].c_str());
                            memcpy (in,&a,sizeof(int));
                            args[i] = (void *)in;
                            m++;
                        }
                        else {
                            if (curr_arg.get_output()==1 && curr_arg.get_input()==0) {
                                int *arr = new int[sizeof(int)*arr_len];
                                args[i] = (void *)arr;
                                m++;
                            }
                            else {
                                int *arr = new int[sizeof(int) * arr_len];
                                int j;
                                int *a;
                                for (j = 0; j < arr_len; j++) {
                                    a = new int[sizeof(int)];
                                    int aa = stoi(marshall[m + j].c_str());
                                    memcpy(a, &aa, sizeof(int));
                                    arr[j] = *a;
                                    cout << arr[j] << endl;
                                }
                                args[i] = (void *) arr;
                                m += arr_len;
                            }
                        }
                        break;
                    }
                    case ARG_LONG: {
                        if (arr_len==0) {
                            long *l = new long[sizeof(long)];
                            long a = stol(marshall[m].c_str());
                            memcpy (l,&a, sizeof(long));
                            args[i] = (void *)l;
                            m++;
                        }
                        else {
                            if (curr_arg.get_output()==1 && curr_arg.get_input()==0) {
                                cout << "Long array here" << endl;
                                long *arr = new long[sizeof(long)*arr_len];
                                args[i] = (void *)arr;
                                m++;
                            }
                            else {
                                long *arr = new long[sizeof(long) * arr_len];
                                cout << "LONG ARR LEN" << arr_len << endl;
                                int j;
                                long *l;
                                for (j = 0; j < arr_len; j++) {
                                    l = new long[sizeof(long)];
                                    long a = stol(marshall[m + j].c_str());
                                    memcpy(l, &a, sizeof(long));
                                    arr[j] = *l;
                                }
                                args[i] = (void *) arr;
                                m += arr_len;
                            }
                        }
                        break;
                    }
                    case ARG_DOUBLE: {
                        if (arr_len==0) {
                            double *d = new double[sizeof(double)];
                            double a = stod(marshall[m].c_str());
                            memcpy (d,&a, sizeof(double));
                            args[i] = (void *)d;
                            m++;
                        }
                        else {
                            if (curr_arg.get_output()==1 && curr_arg.get_input()==0) {
                                double *arr = new double[sizeof(double)*arr_len];
                                args[i] = (void *)arr;
                                m++;
                            }
                            else {
                                double *arr = new double[sizeof(double) * arr_len];
                                int j;
                                double *d;
                                for (j = 0; j < arr_len; j++) {
                                    d = new double[sizeof(double)];
                                    double dd = stod(marshall[m + j].c_str());
                                    memcpy(d, &dd, sizeof(double));
                                    arr[j] = *d;
                                    cout << arr[j] << endl;
                                }
                                args[i] = (void *) arr;
                                m = +arr_len;
                            }
                        }
                        break;
                    }
                    case ARG_FLOAT: {
                        if (arr_len==0) {
                            float *f = new float[sizeof(float)];
                            float a = stof(marshall[m].c_str());
                            memcpy (f,&a, sizeof(float));
                            args[i] = (void *)f;
                            m++;
                        }
                        else {
                            if (curr_arg.get_output()==1 && curr_arg.get_input()==0) {
                                float *arr = new float[sizeof(float)*arr_len];
                                args[i] = (void *)arr;
                                m++;
                            }
                            else {
                                float *arr = new float[sizeof(float) * arr_len];
                                int j;
                                float *f;
                                for (j = 0; j < arr_len; j++) {
                                    f = new float[sizeof(float)];
                                    float ff = stof(marshall[m + j].c_str());
                                    memcpy(f, &ff, sizeof(float));
                                    arr[j] = *f;
                                    cout << arr[j] << endl;
                                }
                                args[i] = (void *) arr;
                                m = +arr_len;
                            }
                        }
                        break;
                    }
                    default: {
                        cout << "Unmarshall: Shouldn't be here." << endl;
                        return -1;
                    }
                }
            }

            skeleton q = server_db.lookup[key];
            cout << "Got skeleton" << endl;
            res = (*q)(f.get_argtypes(), args);
            if (res != 0) {
                res = ERR_INVALID_ARGS;
            }
            cout << "Skeleton: result : " << res << endl;
            cout << "ARgs[0] After " << ((long *) args[0]) << endl;
//            cout << "ARgs[1] After " << *((int *) args[1]) << endl;

            string marshall = server_marshall_args(f.get_argtypes(), args, arg_length);
            cout << "MARSHALL: " << marshall << endl;


            for (int i =0; i < arg_length; i++) {
                delete args[i];
            }
            delete [] args;
            return s.send_execute_response(socket, f,key, marshall, res);
        }
        else {
            cout << "Did not find skeleton" << endl;
        }

    }
    else {
        cout << "error: server doenst contain func " << endl;
        return ERR_SERVER_DOESNT_CONTAIN_FUNC;
    }
}


