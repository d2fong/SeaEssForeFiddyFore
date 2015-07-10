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
        else {
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

            delete [] buff;

            vector<string> key_s = split(key, '|');
            vector<string> args_s = split(marshalled, '|');
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
           // args = (void **) malloc(arg_size);
            cout << "malloc args size " << endl;

//            int unmarshall = unmarshall_args(args, arg_info, args_s);
//            if (unmarshall < 0) {
//                cout << "Err unmarshalling client" << endl;
//                return ERR_UNMARSHALLING_SERVER;

//            }
        }
        return 0;
    }
    else {
        cout << "Neither success nor failure?" << endl;
        return 0;
    }
}


