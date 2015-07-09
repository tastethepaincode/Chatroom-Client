#ifndef USER_HPP
#define USER_HPP

#include "Poller.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <string.h>
#include <vector>

#include <stdint.h>
#include <poll.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <fstream>

class CMessageHandler;
class CClientMessageHandler;

class CUser: public CPoller{
private:
    friend class CClientMessageHandler;
    std::string _username;
    std::string _password;
    int _status;
    std::string _room;
    std::string _portNum;
    std::string _serverIPAddr;
    int _sockfd;
    std::vector<std::string> _otherUser;
    struct sockaddr_in _serv_addr;
    struct hosten *_server;
    CClientMessageHandler *_msgHandler;

public:
    CUser(std::string, std::string, std::string, std::string);
    bool establishConnection();
    void selectOption(int);
    bool readPoll(int);
    void closeSocketfd(int);
    void printOptions();
    void start();
    void update(int, std::string&);

    /* Updates user's chatroom */
    void U_ClientAuth(std::string &);
    void U_GetAvailableRooms(std::string &);
    void U_GetRoomStatus(std::string &);
    void U_CreateRoom(std::string &);
    void U_JoinRoom(std::string &);
    void U_LeaveRoom(std::string &);
    void U_DeliverMessagePacket(std::string &);
    void U_Disconnect(std::string &);


    
};

#endif