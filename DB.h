#ifndef CS4544REEAL_DB_H
#define CS4544REEAL_DB_H

#include <string>
#include <vector>

using namespace std;


class DB {
};

class Args {
private:
    int input; // 1 if input
    int output; // 1 if output
    int type;
    int arr_length;
    int scalar; //1 if scalar
    void set_arg_types(int arg_type);

public:
    Args() {}
    Args (int arg_type) {
        set_arg_types(arg_type);
    }
    Args (int input, int output, int type, int arr_length, int scalar) : input(input), output(output), type(type),
                                                                         arr_length(arr_length), scalar(scalar) {}
    int get_input();
    int get_output();
    int get_type();
    int get_arr_length();
    int get_scalar();
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
        get_args();
        gen_key();
    }
    string get_name();
    int get_arg_length();
    string get_key();
    vector <Args> get_args();
};


#endif //CS4544REEAL_DB_H
