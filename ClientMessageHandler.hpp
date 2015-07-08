#ifndef CLIENTMESSAGEHANDLER_HPP
#define CLIENTMESSAGEHANDLER_HPP

#include "MessageHandler.hpp"
#include <fstream>
#include <iostream>

class CUser;
class CClientMessageHandler: public CMessageHandler{
private:
    friend class CUser;
    CUser *_myUser;
public:
    CClientMessageHandler(CUser *);
    bool processMessage(int,std::string&);
    void A_ClientAuth();
    void A_GetAvailableRooms();
    void A_GetRoomStatus(std::string);
    void A_CreateRoom(std::string, int);
    void A_JoinRoom(std::string);
    void A_LeaveRoom();
    void A_DeliverMessagePacket(std::string);
    void A_Disconnect();

};

#endif