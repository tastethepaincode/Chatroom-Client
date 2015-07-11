#ifndef USER_HPP
#define USER_HPP

#include "Poller.hpp"
#include "MessageHandler.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
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


class CUser: public CPoller, public CMessageHandler{
private:
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

public:
    CUser(std::string, std::string, std::string, std::string);
    bool establishConnection();
    void selectOption(int);
    bool readPoll(int);
    void closeSocketfd(int);
    void printOptions();
    void start();


    /*******************************
    ************ Write *************
    *******************************/

    bool writeMessage(int);
    void Write_ClientAuth(int);
    void Write_GetAvailableRooms(int);
    void Write_GetRoomStatus(int);
    void Write_CreateRoom(int);
    void Write_JoinRoom(int);
    void Write_LeaveRoom(int);
    void Write_DeliverMessagePacket(int);
    void Write_Disconnect(int);

    /*******************************
    ************* Read *************
    *******************************/ 
    void readMessage(int, std::string&);
    void Read_ClientAuth(std::string &);
    void Read_GetAvailableRooms(std::string &);
    void Read_GetRoomStatus(std::string &);
    void Read_CreateRoom(std::string &);
    void Read_JoinRoom(std::string &);
    void Read_LeaveRoom(std::string &);
    void Read_DeliverMessagePacket(std::string &);
    void Read_Disconnect(std::string &);

    
};

#endif