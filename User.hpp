#ifndef USER_HPP
#define USER_HPP

#define CLIENT_AUTH 0
#define GET_AVAILABLE_ROOMS 1
#define GET_ROOM_STATUS 2
#define CREATE_ROOM 3
#define LEAVE_ROOM 4
#define MESSAGE_PACKET 5
#define DISCONNECT 6
#define SIZE 4
#define ACTION 2
#include "MessageHandler.hpp"
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

class CUser{
private:

    std::string _username;
    std::string _password;
    int _status;
    std::string _roomName;
    std::string _portNum;
    std::string _serverIPAddr;
    int _sockfd;
    std::vector<std::string> _otherUser;
    struct sockaddr_in _serv_addr;
    struct hosten *_server;
    MessageHandler msgHandler;
public:
    bool establishConnection();
    bool authenticate();
    bool getAvailableRooms();
    bool getRoomStatus();
    bool createRoom();
    bool joinRoom();
    bool leaveRoom();
    CUser(std::string, std::string, std::string, std::string);
    
};

#endif