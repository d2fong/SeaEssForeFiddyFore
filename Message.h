//
// Created by Dylan Fong on 2015-07-06.
//

#ifndef CS4544REEAL_MESSAGE_H
#define CS4544REEAL_MESSAGE_H

enum MessageType {
    REGISTER,
    REGISTER_SUCCESS,
    REGISTER_FAILURE,

    LOCATION_REQUEST,
    LOCATION_SUCCESS,
    LOCAITON_FAILURE,

    EXECUTE,
    EXECUTE_SUCCESS,
    EXECUTE_FAILURE,

    TERMINATE
};


class Message {
private:
    MessageType t;
    int payloadlength;
//    MessageContent payload;

public:
    Message() { }
//    Message(const MessageType &t, int payloadlength, const ::Message::MessageContent &payload) : t(t), payloadlength(
//            payloadlength), payload(payload) { }

    int send(int socket);


    const MessageType &getT() const {
        return t;
    }

    int getPayloadlength() const {
        return payloadlength;
    }


    void setT(const MessageType &t) {
        Message::t = t;
    }

    void setPayloadlength(int payloadlength) {
        Message::payloadlength = payloadlength;
    }
};


//class MessageContent {
//
//};


#endif //CS4544REEAL_MESSAGE_H
