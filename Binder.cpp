#include "Binder.h"
#include "constants.h"
#include "helpers.h"
#include "Message.h"
#include "error.h"
#include "DB.h"

#include <iostream>
#include <map>
#include <sys/select.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


using namespace std;

BinderDB binder_db = BinderDB();

vector <ServerInfo> servers;

ServerInfo server_to_service(string key);

int Binder::send_register_response(int socket, int flag) {
    int n_flag = htonl(flag);
    int f_send = send(socket,&n_flag,4,0);
    if (f_send < 0) {
        return ERR_BINDER_REG_FAIL;
    }
    return 0;
}

int Binder::receive_register_request (int socket, int length) {
    int port;
    int n_port;
    char s_name [MAXHOSTNAME +1];
    char f_name [MAXFUNCNAME + 1];
    int key_size = length - (MAXFUNCNAME + 1 + MAXHOSTNAME + 1 + 4);
    char key [key_size+1];

    char *buff = new char[length];
    int recv_buff = recv(socket, buff, length, 0);
    if (recv_buff <0) {
        return ERR_BINDER_REG_FAIL;
    }

    memcpy (s_name, buff, MAXHOSTNAME+1);
    memcpy (&port, buff+MAXHOSTNAME+1, 4);
    memcpy (f_name, buff+MAXHOSTNAME+1+4, MAXFUNCNAME+1);
    memcpy (key, buff+MAXHOSTNAME + 1 + MAXFUNCNAME + 1 + 4, key_size);
    key[key_size] ='\0';
    s_name[MAXHOSTNAME]= '\0';
    f_name[MAXFUNCNAME]= '\0';


    n_port = ntohl(port);

    ServerInfo s = ServerInfo(s_name, n_port);

    int found = 0;
    for (vector<ServerInfo>::size_type i = 0; i != servers.size(); i++) {
        if (servers[i].host == s_name && servers[i].port == n_port) {
            found = 1;
            break;
        }
    }
    if (found ==0) {
        servers.push_back(s);
    }


    int ret_update = binder_db.update_db(f_name, s_name, n_port, key, socket);
    return send_register_response(socket,ret_update);
}

int Binder::handle_request(int socket, int type, fd_set *master) {
    int m_len = 0;
    int recv_length = 0;

    int retVal = 0;
    switch(type){
        case REGISTER: {
            m_len = 0;
            recv_length = recv(socket, &(m_len), 4, 0);
            if (recv_length < 0) {
                return ERR_BINDER_REG_FAIL;
            }
            int rec_reg_req = receive_register_request(socket, ntohl(m_len));
            if (rec_reg_req != 0) {
                return ERR_BINDER_REG_FAIL;
            }
            break;
        }
       case LOCATION_REQUEST: {
           m_len = 0;
           recv_length = recv(socket, &(m_len), 4, 0);
           if (recv_length < 0) {
               return ERR_LOC_REQ_FAIL;
           }
           retVal = receive_location_request(socket,ntohl(m_len));
           if (retVal < 0) {
               return ERR_LOC_RESP_FAIL;
           }
           break;
       }
        case TERMINATE: {
            int a= shutdown_everything(master);
            if (a == 0) {
                exit(0);
            }
            return a;
        }
    }
    return retVal;
}

int Binder::init() {
    char addr[MAXHOSTNAME+1];
    unsigned short port;
    int socket = create_connection_socket(BINDERPORT, &port, addr);

    if (socket < 0) {
        return ERR_SOCKET_CONNECT_FAIL;
    }


    binderPort = port;
    binderAddr = addr;
    print_status();


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


    listener = socket;
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
            return ERR_SELECT_FAIL;
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
                        return ERR_BINDER_ACCEPT_FAIL;
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
                            if (binder_db.socket_map.find(i) != binder_db.socket_map.end()) {
                                int q =update_binder_dbs(i);
                                if (q < 0) {
                                    return ERR_BINDER_DB_UPDATE_FAIL -30;
                                }
                            }


                        } else {
                            return ERR_BINDER_RECV_FAIL;

                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
                    }
                    else {
                        int r = handle_request(i,ntohl(messageType), &master);
                        if (r < 0) {
                            return r;
                        }
                    }
                }
            }
        }
    }
}


int Binder::update_binder_dbs(int socket) {
    ServerInfo s = binder_db.socket_map[socket];

    string key = "";
    vector<ServerInfo> v;
    vector <string> keys;
    typedef map<string, vector<ServerInfo> >::iterator it_type;
    int lookup_index = 0;
    for (it_type it = binder_db.lookup.begin(); it!= binder_db.lookup.end(); it++) {

        key = it->first;
        v = it->second;
        for(vector<ServerInfo>::size_type i = 0; i != v.size(); i++) {
            if (s.host == v[i].host && s.port == v[i].port) {
                keys.push_back(key);
            }
        }
    }

    map<int, ServerInfo>::iterator it;
    for(vector<string>::size_type i = 0; i != keys.size(); i++) {
        v = binder_db.lookup[keys[i]];
        int index = -1;
        for (vector<ServerInfo>::size_type l = 0; l!= v.size(); l++) {
            if (v[l].port == s.port && v[l].host == s.host) {
                index = l;
                break;
            }
        }
        if (index != -1) {
            v.erase(v.begin()+index);
        }
        if (v.size() == 0) {
            binder_db.lookup.erase(keys[i]);
        }
    }

    it = binder_db.socket_map.find(socket);
    binder_db.socket_map.erase(it);

    return 0;
}

void Binder::print_status() {
    cout << "BINDER_ADDRESS " <<  binderAddr << endl;
    cout << "BINDER_PORT " << binderPort << endl;

}

ServerInfo get_server_to_service(string key) {

    vector<ServerInfo> key_servs;
    ServerInfo curr_serv;

    int index_of_server = 0;
    ServerInfo return_server;
    // Find index of next server to service the request
    for (vector<ServerInfo>::size_type i = 0; i != servers.size(); i++) {
        curr_serv = servers[i];
        int s_found = 0;
        if (binder_db.lookup.find(key) != binder_db.lookup.end()) {
            key_servs = binder_db.lookup[key];
            for (vector<ServerInfo>::size_type m = 0; m != key_servs.size(); m++) {
                if (curr_serv.port == key_servs[m].port && curr_serv.host == key_servs[m].host) {
                    return_server = curr_serv;
                    index_of_server = i;
                    s_found = 1;
                    break;
                }
            }
        }
        if (s_found == 1) {
            break;
        }

    }

    // Pop the server at the index found
    servers.erase(servers.begin() + index_of_server);

    // Insert it back into the 'queue'
    servers.push_back(return_server);

    return return_server;
}

int Binder::send_location_response(int socket, string key) {

    int msg_type;
    int reason_code;
    int send_bytes = 4;
    int res;
    if (binder_db.lookup.find(key) == binder_db.lookup.end()) {
        msg_type = htonl(LOCAITON_FAILURE);
        reason_code =htonl(ERR_SERVER_NOT_FOUND);
        res =send_all(socket,(char *)&msg_type,&send_bytes);
        if (res == -1) {
            return ERR_LOC_RESP_FAIL;
        }
        res = send_all(socket, (char *)&reason_code, &send_bytes);
        if (res == -1) {
            return  ERR_LOC_RESP_FAIL;
        }
        return res;
    }
    else {
        ServerInfo s = get_server_to_service(key);

        msg_type = htonl(LOCATION_SUCCESS);
        res =send_all(socket,(char *)&msg_type,&send_bytes);
        if (res == -1) {
            return ERR_LOC_RESP_FAIL;
        }
        char *buff = new char [MAXHOSTNAME+1 + 4];
        int buff_len = MAXHOSTNAME+1+4;
        int port = htonl(s.port);
        memcpy (buff, s.host.c_str(),MAXHOSTNAME+1);
        memcpy (buff+MAXHOSTNAME+1, &port, 4);
        int ret = send_all(socket, buff,&buff_len);
        if (ret == -1) {
            return ERR_LOC_RESP_FAIL;
        }
        delete [] buff;
        return 0;
    }
}

int Binder::receive_location_request(int socket, int length) {

    int key_size = length;
    char key [key_size+1];

    char *buff = new char[length];
    int recv_buff = recv(socket, buff, length, 0);
    if (recv_buff <0) {
        return ERR_LOC_REQ_FAIL;
    }

    memcpy (key, buff, key_size);
    key[key_size] ='\0';
    delete [] buff;
    return send_location_response(socket, key);
}

int Binder::shutdown_everything(fd_set* master) {

    typedef map<int, ServerInfo>::iterator it_type;
    int key =0;
    for (it_type it = binder_db.socket_map.begin(); it!= binder_db.socket_map.end(); it++) {
        int m_type = htonl(TERMINATE);
        int expect = 4;
        key = it->first;

        int r = send_all(key, (char*) &m_type, &expect);
        if (r < 0) {
            return ERR_TERMINATING;
        }
        close(key);
        FD_CLR(key,master);
    }

    return 0;
}
