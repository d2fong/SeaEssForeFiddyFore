#include "DB.h"
#include "constants.h"
#include "helpers.h"
#include <iostream>
#include <string>
#include <string.h>
#include "Message.h"

using namespace std;

int ServerDB::update_db(int flag, Function func, skeleton f) {
    cout << "Updating db: Flag: " << flag << endl;
    if (flag == REGISTER_WARNING){
        lookup[func.get_key()] =f;
    }
    else {
        // Register Success
        lookup.insert(map<string, skeleton>::value_type(func.get_key(), f));
        functions.insert(map<string, Function>::value_type(func.get_key(),func));
    }
    return 0;
}


int BinderDB::update_db(string f_name, string s_name, int port, string key, int socket) {
    ServerInfo s = ServerInfo(s_name,port);
    vector<ServerInfo> servers;


    cout << "SOCKET MAP SIZE" << socket_map.size () << endl;
    if (socket_map.find(socket) == socket_map.end()) {
        cout << "Didn't find in socket map" << s.host << endl;
        cout << s.port << endl;
        socket_map.insert(map<int, ServerInfo>::value_type(socket, s));
        cout << "after" << socket_map.size() << endl;
        cout << "Sock host" << socket_map[socket].host << endl;
    }
    else {
        socket_map[socket]= s;
    }


    if (lookup.find(key) == lookup.end()) {
        vector <ServerInfo> s_info;
        s_info.push_back(s);
        lookup.insert(map<string, vector<ServerInfo> >::value_type(key, s_info));
        cout << "DB: adding key" << key << endl;
    }
    else {
        int found = 0;
        servers = lookup[key];
        for(vector<ServerInfo>::size_type i = 0; i != servers.size(); i++) {
            if (servers[i].host == s_name && servers[i].port == port) {
                found = 1;
                break;
            }
        }

        if (found == 0) {
            lookup[key].push_back(s);
        }
        else {
            return REGISTER_WARNING;
        }
    }
    return REGISTER_SUCCESS;

}




/**
 * Args
 */
int Args::get_input() {
    return input;
}

int Args::get_output() {
    return output;
}

int Args::get_type() {
    return type;
}

int Args::get_arr_length() {
    return arr_length;
}


void Args::set_arg_types(int arg_type) {
    input = (arg_type >> ARG_INPUT) & 0x1;
    output = (arg_type >> ARG_OUTPUT) & 0x1;
    type = (arg_type >> 16) & 0xf;
    arr_length= (arg_type & 0xFFFF);
}

string Args::get_args_key() {
    string delimiter = "|";
    string s= to_stri(input) + delimiter + to_stri(output) + delimiter + to_stri(type)
           + delimiter + to_stri(arr_length);
    return s;
}



void Function::gen_args() {
    for (int i =0; i< arg_length; i++) {
        Args a = Args(arg_types[i]);
        args.push_back(a);
    }
}

void Function::gen_key() {
    string delimiter = "|";
    string as = "";

    for (int i=0; i < arg_length; i++) {
        as += args[i].get_args_key() + delimiter;
    }
    string arg_str = as.substr(0, as.size()-1);
    key = name + delimiter + to_stri(arg_length) + delimiter + arg_str;
}

string Function::get_name(){
    return name;
}

int Function::get_arg_length() {
    return arg_length;
};


int * Function::get_argtypes() {
    return arg_types;
}

string Function::get_key() {
    return key;
};

vector <Args> Function::get_args() {
    return args;
}



