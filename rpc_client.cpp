//
// Created by Dylan Fong on 2015-07-06.
//


//TODO put these in its own file
//extern int LOCATION_FAILURE = -1;
//extern int EXECUTE_FAILURE = -2;


#include "rpc.h"
#include "Client.h"
#include "error.h"
#include "constants.h"
#include <sys/socket.h>
#include "DB.h"
#include "helpers.h"
#include <iostream>
#include <stdlib.h>
#include <map>


using namespace std;
Client c = Client();

int rpcCall(char* name, int* argTypes, void** args) {
    char *binderAddr = getenv("BINDER_ADDRESS");
    char *binderPort = getenv("BINDER_PORT");
    int i = 0;
    while (argTypes[i++]);

    cout << "Init rpc call" << endl;

    //Connect to the binder
    int connectionResult = c.connect_to_something(binderAddr, binderPort);
    if (connectionResult < 0) {
        cout << "error: rpc_client couldn't connect to binder" << endl;
        return connectionResult;
    }


    cout << "Connected to binder" << endl;
    //Create a message and serialize name and argTypes into a buffer
    LocationRequestMessage locMsg = c.create_location_request(name, argTypes);
    int messageResult = c.send_location_request(locMsg, c.get_binder_socket());
    if (messageResult < 0) {
        cout << "error: rpc_client couldn't send the message correctly" << endl;
        return messageResult;
    }
    delete []locMsg.getFuncNameBuffer();

    int msg_type;
    int reason_code = 0;
    int res = recv(c.get_binder_socket(), &msg_type, 4, 0);
    if (res == -1) {
        return ERR_BINDER_RECV_FAIL;
    }
    msg_type = ntohl(msg_type);
    if (msg_type == LOCAITON_FAILURE) {
        cout << "Location failure" << endl;
        res = recv(c.get_binder_socket(), &reason_code, 4, 0);
        if (res == -1) {
            return ntohl(reason_code);
        }
    }
    else if (msg_type == LOCATION_SUCCESS) {

        cout << "Location success" << endl;
        char buff[MAXHOSTNAME + 1 + 4];
        char s_name[MAXHOSTNAME + 1];
        int port = 0;
        res = recv(c.get_binder_socket(), buff, MAXHOSTNAME + 1 + 4, 0);
        memcpy(s_name, buff, MAXHOSTNAME + 1);
        memcpy(&port, buff + MAXHOSTNAME + 1, 4);
        s_name[MAXHOSTNAME] = '\0';
        port = ntohl(port);
        cout << "server" << s_name << endl;
        cout << "port" << port << endl;

        //connect to the server
        int s_server = connect_to(s_name, (char *) to_stri(port).c_str());
        if (s_server < 0) {
            cout << "error: could not connect to specified server" << endl;
            return -1;
        }
        c.set_server_socket(s_server);
        int r = c.send_execute_request(c.get_server_socket(), name, argTypes, args);
        if (r < 0) {
            cout << "error: Sending request failed" << endl;
        }
//        return c.receive_execute_response(&args);
        int ret;
        int flag;
        int reason_code;
        ret = recv(c.get_server_socket(), &flag, 4,0);
        if (ret < 0) {
            return ERR_RECV_CLIENT;
        }
        if (ntohl(flag) == EXECUTE_FAILURE) {
            ret = recv(c.get_server_socket(), &reason_code,4,0);
            return ntohl(reason_code);
        }
        else if (ntohl(flag) == EXECUTE_SUCCESS){
            int m_length =0;
            int mars_len = 0;
            int key_len = 0;
            int n_mars_len=0;
            int n_key_len=0;

            ret = recv(c.get_server_socket(), &(m_length), 4, 0);
            if (ret < 0) {
                return ERR_SERVER_CLIENT_RECV;
            }
            int n_len = ntohl(m_length);
            cout << "RECV EXECUTE RESPONSE: len: " << n_len << endl;

            char * buff = new char [n_len];
            ret = recv(c.get_server_socket(), buff, n_len,0);
            if (ret < 0) {
                return ERR_SERVER_CLIENT_RECV;
            }

            memcpy (&mars_len, buff,4);
            n_mars_len = ntohl(mars_len);

            cout << "marshall Length " << n_mars_len << endl;
            memcpy (&key_len, buff+4 ,4);

            n_key_len= ntohl(key_len);
            cout << "arg length" << n_key_len << endl;

            char marshalled [n_mars_len+1];
            memcpy (marshalled, buff+8, n_mars_len);
            marshalled[n_mars_len]= '\0';

            cout << "Marshalled " << marshalled << endl;

            char key[n_key_len+1];
            memcpy(key, buff+8+n_mars_len,n_key_len+1);
            key[n_key_len]= '\0';

            cout << "Key in client " << key << endl;

            vector<string> key_s = split(key, '|');
            vector<string> marshall = split(marshalled, '|');
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


//            int arg_size= calculate_arg_size(arg_info);
//            cout << "ArgInfo size " << arg_info.size() << endl;
//            for (int i =0; i < arg_length; i++) {
//                delete [] args[i];
//            }
//            delete [] args;


//            void**args = (void**)malloc(sizeof(void*)* arg_info.size());
            Args curr_arg;
            int data,arr_len,offset;
            int m =0;
            for (int i =0; i < arg_info.size(); i++) {
                curr_arg = arg_info[i];
                arr_len = curr_arg.get_arr_length();
                offset = i+arr_len-1;
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
                            if (curr_arg.get_output()==1) {
                                char *arr = new char[sizeof(char)*marshall[m].length()+1];
                                memcpy (arr,marshall[i].c_str(), marshall[m].length()+1);
                                arr[marshall[m].length()+1]='\0';
                                args[i] = (void *)arr;
                                m++;
                                cout << ((char *) args[0]) << endl;
                            }
                            else {
                                string str = append_vector_string(marshall, i, offset);
//                            cout << "STR" << str << endl;
                                char *arr = new char[sizeof(char) * arr_len];
                                memcpy(arr, str.c_str(), sizeof(char) * arr_len);
                                arr[arr_len] = '\0';
                                args[i] = (void *) arr;
                                i = offset;
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
                            string str = append_vector_string(marshall,i, offset);
                            short *arr = new short[sizeof(short)*arr_len];
                            memcpy (arr,str.c_str(),sizeof(short)*arr_len);
                            args[i] = (void *)arr;
                            i = offset;
                        }
                        break;
                    }
                    case ARG_INT: {
                        if (arr_len==0) {
                            int *in = new int[sizeof(int)];
                            int a = stoi(marshall[m].c_str());
                            cout << "string to int a " << a << endl;
                            memcpy(in, &a, sizeof(int));
                            args[i] = (void *) in;
                            m++;
                        }
                        else {
                            string str = append_vector_string(marshall,i, offset);
                            int *arr = new int[sizeof(int)*arr_len];
                            memcpy (arr,str.c_str(),sizeof(int)*arr_len);
                            args[i] = (void *)arr;
                            i = offset;
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
                            long *arr = new long[sizeof(long)*arr_len];
                            int j;
                            long *l;
                            for(j =0; j < arr_len; j++) {
                                l = new long[sizeof(long)];
                                long a = stol(marshall[m+j].c_str());
                                memcpy(l, &a, sizeof(long));
                                arr[j] = *l;
                                cout << arr[j] << endl;
                            }
                            args[i] = (void *)arr;
                            m +=arr_len;
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
                            cout << "Arg " << *(double*)args[i] << endl;
                        }
                        else {
                            string str = append_vector_string(marshall,i, offset);
                            double *arr = new double[sizeof(double)*arr_len];
                            memcpy (arr,str.c_str(),sizeof(double)*arr_len);
                            args[i] = (void *)arr;
                            i = offset;
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
                            cout << "Arg " << *(float*)args[i] << endl;
                        }
                        else {
                            string str = append_vector_string(marshall,i, offset);
                            float *arr = new float[sizeof(float)*arr_len];
                            memcpy (arr,str.c_str(),sizeof(float)*arr_len);
                            args[i] = (void *)arr;
                            i = offset;
                        }
                        break;
                    }
                    default: {
                        cout << "Unmarshall: Shouldn't be here." << endl;
                        return -1;
                    }
                }
            }
            return 0;
        }
    }
    else {
        cout << "Neither success nor failure?" << endl;
        return 0;
    }
    return 0;
}


