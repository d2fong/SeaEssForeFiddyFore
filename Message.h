//
// Created by Dylan Fong on 2015-07-06.
//

#ifndef CS4544REEAL_MESSAGE_H
#define CS4544REEAL_MESSAGE_H

static int    REGISTER = 1;
static int    REGISTER_SUCCESS = 2;
static int    REGISTER_FAILURE = 3;

static int    LOCATION_REQUEST = 4;
static int    LOCATION_SUCCESS = 5;
static int    LOCAITON_FAILURE = 6;

static int    EXECUTE = 7;
static int    EXECUTE_SUCCESS = 8;
static int    EXECUTE_FAILURE = 9;

static int    TERMINATE;


class Message {
private:
    int type;

public:
    Message() { }

    int send(int socket);


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



//class MessageContent {
//
//};


#endif //CS4544REEAL_MESSAGE_H
