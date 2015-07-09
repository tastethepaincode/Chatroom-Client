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
    void A_ClientAuth(int, std::string&);
    void A_GetAvailableRooms(int);
    void A_GetRoomStatus(int, std::string&);
    void A_CreateRoom(int, std::string&);
    void A_JoinRoom(int, std::string&);
    void A_LeaveRoom(int, std::string&);
    void A_DeliverMessagePacket(int, std::string&);
    void A_Disconnect(int);

};

#endif