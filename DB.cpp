//
// Created by Shayan Masood on 15-07-07.
//

#include "DB.h"
#include "constants.h"
#include "helpers.h"
#include <iostream>
#include <string>
#include <string.h>

using namespace std;

/**
 * Args Helpers
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

int Args::get_scalar() {
    return scalar;
}

void Args::set_arg_types(int arg_type) {
    input = (arg_type >> ARG_INPUT) & 0x1;
    output = (arg_type >> ARG_OUTPUT) & 0x1;
    type = (arg_type >> 16) & 0xf;
    arr_length= (arg_type & 0xFFFF);
    if (arr_length == 0) {
        scalar=1;
        arr_length=1;
    }
    else {
        scalar=0;
    }
}

string Args::get_args_key() {
    string delimiter = "|";
    string s= to_stri(input) + delimiter + to_stri(output) + delimiter + to_stri(type)
           + delimiter + to_stri(arr_length) + delimiter + to_stri(scalar);
    return s;
}



/**
 * Function Helpers
 */

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


string Function::get_key() {
    return key;
};

vector <Args> Function::get_args() {
    return args;
}
