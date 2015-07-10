#ifndef CS4544REEAL_DB_H
#define CS4544REEAL_DB_H

#include <string>
#include <vector>
#include <iostream>
#include <map>
#include "rpc.h"

using namespace std;


class Args {
private:
    int input; // 1 if input
    int output; // 1 if output
    int type;
    int arr_length;
    void set_arg_types(int arg_type);

public:
    Args() {}
    Args (int arg_type) {
        set_arg_types(arg_type);
    }
    Args (int input, int output, int type, int arr_length) : input(input), output(output), type(type),
                                                                         arr_length(arr_length) {}
    int get_input();
    int get_output();
    int get_type();
    int get_type_from_arg_type();
    int get_arr_length();
    string get_args_key ();
};


class Function {
private:
    string name;
    int *arg_types;
    string key;
    int arg_length;
    vector <Args> args;
    void gen_key();
    void gen_args();
public:
    Function() {}
    Function(string name, int *arg_types, int arg_length) : name(name), arg_types(arg_types),arg_length(arg_length) {
        gen_args();
        gen_key();
    }
    string get_name();
    int get_arg_length();
    string get_key();
    int * get_argtypes();
    vector <Args> get_args();
};


class ServerInfo {
public:
    string host;
    int port;
    ServerInfo(){}
    ServerInfo(string host, int port) : host(host), port(port){}
};

class BinderDB {
public:
    map<string, vector<ServerInfo> > lookup;
    BinderDB() {}
    int update_db(string f_name, string s_name, int port, string key);
};


class ServerDB {
public:
    map<string, skeleton> lookup;
    map<string, Function> functions;
    int update_db(int flag,Function func,skeleton f);
};

#endif //CS4544REEAL_DB_H
