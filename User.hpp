#ifndef USER_HPP
#define USER_HPP

#include "Poller.hpp"
#include "MessageHandler.hpp"
#include "Macroz.hpp"
#include <thread>
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
    // Controls when to print out and ask for input
    bool _input = true;
    /* Status shows where the user is in the chatroom program */
    // 0 == unauth, 1 = auth, 2 = in room
    int _status;
    std::string _room;
    std::string _portNum;
    std::string _serverIPAddr;
    int _sockfd;
    std::vector<std::string> _otherUser;
    struct sockaddr_in _serv_addr;
    struct hosten *_server;
    //Do auto update off
    int _autoUpdate = 0;

public:
    CUser(std::string, std::string, std::string, std::string);
    /*Connect with server*/
    bool establishConnection();
    /*Allow user to decide what to do such as creating a room*/
    void selectOption(int);
    /*Handles incoming messages from server*/
    bool readPoll(int);
    /*Close file descriptor*/
    void closeSocketfd(int);
    /*Display a list of options user can choose to perform*/
    void printOptions();
    /*Starts listening to the server and sending info to server */
    void start();
    /* This is a thread that concurrently reads 
     * and prints information from server 
     */
    void constantUpdate();


    /*******************************
    ************ Write *************
    *******************************/
    /* After message is packaged, send it through socket file descriptor*/
    bool writeMessage(int);
    /* Use username and password to authenticate with server*/
    void Write_ClientAuth(int);
    /* Get the name of all the rooms available in server */
    void Write_GetAvailableRooms(int);
    /* Get the chatroom's size and the capacity */
    void Write_GetRoomStatus(int);
    /* Create a chatroom */
    void Write_CreateRoom(int);
    /* Join a chatroom*/
    void Write_JoinRoom(int);
    /* Leave a chatroom*/
    void Write_LeaveRoom(int);
    /* You are in a chatroom and you want to send a message 
     * to all other users in your chatroom
     */
    void Write_DeliverMessagePacket(int);
    /* Send server signal that you will disconnect
     * you will disconnect without acknowledging with server
     */
    void Write_Disconnect(int);

    /*******************************
    ************* Read *************
    *******************************/ 
    /* After a string is pulled from the socket, 
     * the first two bytes will be read to understand how to
     * process the message
     */
    void readMessage(int, std::string&);

    /* Processing various types of message */
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