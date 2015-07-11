#include "ClientMessageHandler.hpp"
#include "User.hpp"
extern std::ofstream _log;

CClientMessageHandler::CClientMessageHandler(CUser *myUser):_myUser(myUser){

}

void CClientMessageHandler::A_ClientAuth(int sockfd, std::string &msg){
  _log << "-------ClientAuth--------\n";
  //std::cout << "Inside Client Auth\n" << std::endl;
  /*
    Size(4)|Action(2)|Username(?)|,(1)|Password(?)|,(1)
  */
  appendInt(msg, ACTION_SIZE + _myUser->_username.length() + 1 
   + _myUser->_password.length() + 1);
  appendShort(msg, CLIENT_AUTH);
  appendString(msg, _myUser->_username);
  appendString(msg, _myUser->_password);
  std::cout  << msg << std::endl;
  writeMsg(_myUser->_sockfd, msg);
  _log.flush();
}

void CClientMessageHandler::A_GetAvailableRooms(int sockfd){
  _log << "-------GetAvailableRooms--------\n";
  /*
    Size(4)|Action(2)|Username(?)|,(1)
  */
  std::string msg;
  appendInt(msg, ACTION_SIZE + _myUser->_username.length() + 1);
  appendShort(msg, GET_AVAILABLE_ROOMS);
  appendString(msg, _myUser->_username);
  std::cout << msg << std::endl;
  writeMsg(_myUser->_sockfd, msg);
}

void CClientMessageHandler::A_GetRoomStatus(int sockfd, std::string &room){
 _log << "-------GetRoomStatus--------\n";
  /*
    Size(4)|Action(2)|Username(?)|,(1)|Room name(?)|,(1)
  */
  std::string roomname;
  std::cout << "Enter room name: " ;
  getline(std::cin, roomname);
  std::string msg;
  appendInt(msg, ACTION_SIZE + _myUser->_username.length() + 1
    + room.length() + 1);
  appendShort(msg, GET_ROOM_STATUS);
  appendString(msg, _myUser->_username);
  appendString(msg, room);
  std::cout << msg << std::endl;
  writeMsg(_myUser->_sockfd, msg);
}

void CClientMessageHandler::A_CreateRoom(int sockfd, std::string &room){
  _log << "-------CreateRoom--------\n";
  /*
    Size(4)|Action(2)|Capacity(2)|Username(?)|,(1)|Room name(?)|,(1)
  */
  std::string msg;
  uint16_t capacity = 10;
  appendInt(msg, ACTION_SIZE + ACTION_SIZE + _myUser->_username.length() + 1
    + room.length() + 1);
  appendShort(msg, CREATE_ROOM);
  appendShort(msg, capacity);
  appendString(msg, _myUser->_username);
  appendString(msg, room);
  std::cout << msg << std::endl;
  writeMsg(_myUser->_sockfd, msg);
}

void CClientMessageHandler::A_JoinRoom(int sockfd, std::string &room){
  _log << "-------JoinRoom--------\n";
  /*
    Size(4)|Action(2)|Username(?)|,(1)|Room name(?)|,(1)
  */
  std::string msg;
  appendInt(msg, ACTION_SIZE + _myUser->_username.length() + 1
    + room.length() + 1);
  appendShort(msg, JOIN_ROOM);
  appendString(msg, _myUser->_username);
  appendString(msg, room);
  std::cout << msg << std::endl;
  writeMsg(_myUser->_sockfd, msg);
}

void CClientMessageHandler::A_LeaveRoom(int sockfd, std::string &room){
  _log << "-------LeaveRoom--------\n";
  /*
    Size(4)|Action(2)|Username(?)|,(1)|Room name(?)|,(1)
  */
  std::string msg;
  appendInt(msg, ACTION_SIZE + _myUser->_username.length() + 1
    + _myUser->_room.length() + 1);
  appendShort(msg, LEAVE_ROOM);
  appendString(msg, _myUser->_username);
  appendString(msg, _myUser->_room);
  std::cout << msg << std::endl;
  writeMsg(_myUser->_sockfd, msg);
}

void CClientMessageHandler::A_DeliverMessagePacket(int sockfd, std::string &message){
  _log << "-------DeliverMessagePacket--------\n";
  /*
    Size(4)|Action(2)|Username(?)|,(1)|Message(?)|,(1)
  */
  std::string msg;
  appendInt(msg, ACTION_SIZE + _myUser->_username.length() + 1
    + message.length() + 1);
  appendShort(msg, DELIVER_MESSAGE_PACKET);
  appendString(msg, _myUser->_username);
  appendString(msg, message);
  std::cout << msg << std::endl;
  writeMsg(_myUser->_sockfd, msg);
}

void CClientMessageHandler::A_Disconnect(int sockfd){
  _log << "-------handleDisconnect--------\n";
  /*
    Size(4)|Action(2)|Username(?)|,(1)
  */
  std::string msg;
  appendInt(msg, ACTION_SIZE + _myUser->_username.length() + 1);
  appendShort(msg, DISCONNECT);
  appendString(msg, _myUser->_username);
  std::cout << msg << std::endl;
  writeMsg(_myUser->_sockfd, msg);

}

bool CClientMessageHandler::processMessage(int action, std::string &msg){

  switch(action){
    case CLIENT_AUTH:
      A_ClientAuth(_myUser->_sockfd, msg);
      break;
    case GET_AVAILABLE_ROOMS:
      A_GetAvailableRooms(_myUser->_sockfd);
      break;
    case GET_ROOM_STATUS:
      A_GetRoomStatus(_myUser->_sockfd, msg);
      break;
    case CREATE_ROOM:
      A_CreateRoom(_myUser->_sockfd, msg);
      break;
    case JOIN_ROOM:
      A_JoinRoom(_myUser->_sockfd, msg);
      break;
    case LEAVE_ROOM:
      A_LeaveRoom(_myUser->_sockfd, msg);
      break;
    case DELIVER_MESSAGE_PACKET:
      A_DeliverMessagePacket(_myUser->_sockfd, msg);
      break;
    case DISCONNECT:
      A_Disconnect(action);
      break;
  }
  return true;
}