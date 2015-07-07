#ifndef CS4544REEAL_MESSAGE_H
#define CS4544REEAL_MESSAGE_H


#include "DB.h"
using namespace std;



const int    REGISTER = 1;
const int    REGISTER_SUCCESS = 2;
const int    REGISTER_FAILURE = 3;

const int    LOCATION_REQUEST = 4;
const int    LOCATION_SUCCESS = 5;
const int    LOCAITON_FAILURE = 6;

const int    EXECUTE = 7;
const int    EXECUTE_SUCCESS = 8;
const int    EXECUTE_FAILURE = 9;

const int    TERMINATE = 10;

//Reason Codes
//TODO



class Message {
private:
    int type;

public:
    Message() { }

    int getType() {
        return type;
    }

    void setType(int t) {
        type = t;
    }

};

class LocationRequestMessage: public Message {
private:
    int funcNameLength;
    int argTypesLength;
    char* funcNameBuffer;
    char* argTypesBuffer;

public:

    LocationRequestMessage(int funcNameLength, int argTypesLength, char *funcNameBuffer, char *argTypesBuffer)
            : funcNameLength(funcNameLength), argTypesLength(argTypesLength), funcNameBuffer(funcNameBuffer),
              argTypesBuffer(argTypesBuffer) {}


    int getFuncNameLength() const {
        return funcNameLength;
    }

    void setFuncNameLength(int funcNameLength) {
        LocationRequestMessage::funcNameLength = funcNameLength;
    }

    int getArgTypesLength() const {
        return argTypesLength;
    }

    void setArgTypesLength(int argTypesLength) {
        LocationRequestMessage::argTypesLength = argTypesLength;
    }

    char *getFuncNameBuffer() const {
        return funcNameBuffer;
    }

    void setFuncNameBuffer(char *funcNameBuffer) {
        LocationRequestMessage::funcNameBuffer = funcNameBuffer;
    }

    char *getArgTypesBuffer() const {
        return argTypesBuffer;
    }

    void setArgTypesBuffer(char *argTypesBuffer) {
        LocationRequestMessage::argTypesBuffer = argTypesBuffer;
    }
};

class LocationSuccessMessage: public Message {
private:
    char* serverAddr;
    char* serverPort;
};

class RegisterMessage : public Message {
private:
    string s_name;
    int s_port;
    string f_name;
    string arg_key;
    char *buff;
    int buff_length;

public:
    RegisterMessage(){}
    RegisterMessage(string s_name, int s_port, Function f, char* buff, int buff_length): s_name(s_name), s_port(s_port), f_name(f.get_name()),
                                                                                               arg_key(f.get_key()), buff(buff), buff_length(buff_length) { }
    char * getBuff() {
        return  buff;
    }

    int getBuffLength() {
        return buff_length;
    }

    void set_s_name (string s) {
        s_name = s;
    }

    void set_port (int p) {
        s_port = p;
    }

    void set_f_name (string f) {
        f_name = f;
    }

    void set_arg_key(string k) {
        arg_key =k;
    }

    void set_buff (char *b) {
        buff = b;
    }

    void set_buff_length (int len) {
        buff_length = len;
    }

    void set_register_message(string host, int port, Function f, char* buf, int length) {
        s_name = host;
        s_port = port;
        f_name = f.get_name();
        arg_key =f.get_key();
        buff= buf;
        buff_length= length;
    }
};


class LocationFailureMessage: public Message {
private:
    int reasonCode;
};

class ExecuteMessage: public Message {
private:
    int funcNameLength;
    int argsLength;
    char* funcNameBuffer;
    char* argTypesBuffer;
    char* argsBuffer;
};

class ExecuteSuccessMessage: public Message {
private:
    int funcNameLength;
    int argsLength;
    char* funcNameBuffer;
    char* argTypesBuffer;
    char* argsBuffer;
};

class ExecuteFailureMessage: public Message {
private:
    int reasonCode;
};

class TerminateMessage: public Message {
private:
    int reasonCode;
};

#endif //CS4544REEAL_MESSAGE_H
