#include "User.hpp"
#include "ClientMessageHandler.hpp"
extern std::ofstream _log;
CUser::CUser(std::string username, std::string password, 
  std::string portNum, std::string serverIPAddr):_username(username),
  _password(password), _portNum(portNum), _serverIPAddr(serverIPAddr),
  _status(-1){

  }
bool CUser::establishConnection(){
  struct addrinfo hints;
  struct addrinfo *result, *rp;
  int s;
  struct sockaddr_storage peer_addr;
  socklen_t peer_addr_len;
  ssize_t nread;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC; /* Allow IPv4 or IPv6 */
  hints.ai_socktype = SOCK_STREAM;/* Datagram socket */
  hints.ai_flags = AI_PASSIVE; /*For wildcard IP address */
  hints.ai_protocol = 0; /* Any protocol */
  hints.ai_canonname = NULL;
  hints.ai_addr = NULL;
  hints.ai_next = NULL;

  s = getaddrinfo(_serverIPAddr.c_str(), _portNum.c_str(), &hints, &result);
  if(s != 0){
    _log << "Error getaddrinfo\n";
    return 0;
  }    

  /* getaddrinfo() returns a list of address structures.
    Try each address until we successfully bind(2).
    If socket(2) (or bind(2)) fails, we (close the socket
    and) try the next address. */

  for(rp = result; rp != NULL; rp = rp->ai_next){
    _sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if(_sockfd == -1){
      continue;
    }

    /* Success */

    if(connect(_sockfd, rp->ai_addr, rp->ai_addrlen) == 0){
      _log << "Successfully connected to server.\n";
      struct pollfd fileDes;
      fileDes.fd = _sockfd;
      _userFD.push_back(_sockfd);
      _userPollfd.push_back(fileDes);
      break;
    }

    close(_sockfd); 
  }

  if(rp == NULL){
    _log << "Error connection fail due to rp is NULL\n";
    exit(0);
    return 0;
  }

  //disable Nagle's algorithm
  int optval = 1;
  int status = setsockopt(_sockfd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
  if(status == -1){
   perror("Error on disabling Naggle's algorithm.");
   _log << "Error on disabling Naggle's algorithm.\n";
   return 0;
  }

  freeaddrinfo(result);
}

void CUser::selectOption(int option){
  //std::cout << "Inside CUser\n";
  if(option == 8){
    return;
  }
  writeMessage(option);
}

bool CUser::readPoll(int temp){
  _log << "Attempt to read from sockfd " << _sockfd << "\n";
  int msgLength = removeInt(_sockfd);
  /* Close socket if its EOF which is msgLength = -1*/
  if(msgLength == -1){
    closeSocketfd(_sockfd);
    return false;
  }

  int action = removeShort(_sockfd);

  if(action == -1){
    closeSocketfd(_sockfd);
  }
  if(action < 0){
    _log << "Error action number is negative\n";
    return false;
  }

  /* Subtract action's two bytes */
  msgLength -= 2;
  if(msgLength <= 0){
    _log << "Error message is 0 or smaller in size\n";
    return false;
  }

  std::string msg = removeBytes(_sockfd, msgLength);
  if(msg.empty()){
    _log << "Error message string is empty\n";
    closeSocketfd(_sockfd);
    return false;
  }

  /* Update user information */

  readMessage(action, msg);
  return true;


}

void CUser::closeSocketfd(int temp){
 int closeResult;
    if(closeResult = close(_sockfd) == 0){
      // Successfully closed pfd
      _log << "Successfully closed pfd " << _sockfd <<"\n";
    }

    else{//Unsuccessful at closing pfd
      if(closeResult == EBADF){
        _log << "Error on close, EBADF\n";
      }

      else if(closeResult == EINTR){
        _log << "Error on close, EINTR\n";
      }

      else if(closeResult == EIO){
        _log << "Error on close, EIO\n";
      }
      _log << "Error cannot close pfd " << _sockfd << closeResult<< "\n";
    }
}

void CUser::start(){
  int option = -999;
  printOptions();
  std::cout << "Option 0 to 9: " ;
  _log << "Option 0 to 9: \n";
  std::cin >> option;
  std::cout << "You selected: " << option << std::endl;
  _log << "You selected: " << option << "\n\n";
  _log.flush();
  selectOption(option);

    if(option == 9){//exit
      exit(0);
    }


  /* Read sockets */
  int ret = 0;
  if( ret = setReadPoll(10) > 0){
    _log << "Poll detected " << ret << " reply \n";
    for(struct pollfd& pfd: _userPollfd){

    /*There is a message to be processed*/
      if(pfd.revents & POLLIN){
        if(readPoll(_sockfd) == false){
          /* TODO */
        }
      }
      else if(pfd.revents & POLLHUP){
        _log << "Poll detected that " << pfd.fd << " hanged up POLLHUP\n";
        exit(0);
      }
    }
  }
}

void CUser::printOptions(){
  std::cout << "Please select an option.\n" << std::endl;
  std::cout << "9. Exit " << std::endl;
  std::cout << "0. Authenticate" << std::endl;
  std::cout << "1. Get available rooms " << std::endl;
  std::cout << "2. Get room status "<< std::endl;
  std::cout << "3. Create room " << std::endl;
  std::cout << "4. Join room " << std::endl;
  std::cout << "5. Leave room " << std::endl;
  std::cout << "6. Send message " << std::endl;
  std::cout << "7. Disconnect " << std::endl;
  std::cout << "8. Refresh " << std::endl << std::endl;
}

void CUser::readMessage(int action, std::string &msg){

  switch(action){
  case CLIENT_AUTH:
    Read_ClientAuth(msg);
    break;
  case GET_AVAILABLE_ROOMS:
    Read_GetAvailableRooms(msg);
    break;
  case GET_ROOM_STATUS:
    Read_GetRoomStatus(msg);
    break;
  case CREATE_ROOM:
    Read_CreateRoom(msg);
    break;
  case JOIN_ROOM:
    Read_JoinRoom(msg);
    break;
  case LEAVE_ROOM:
    Read_LeaveRoom(msg);
    break;
  case DELIVER_MESSAGE_PACKET:
    Read_DeliverMessagePacket(msg);
    break;
  case DISCONNECT:
    Read_Disconnect(msg);
    break;
  default:
    std::cout << "Incorrect option received." << std::endl;
    _log << "Incorrect option received.\n";
  }
}

void CUser::Read_ClientAuth(std::string &msg){
  /* msg should be a string on length 1 */
  std::cout << "Updating ClientAuth " << std::endl;
  _log << "Updating ClientAuth\n ";
  if(msg.length() != 2){
    std::cout << "ClientAuth reply length is wrong" << msg.length() << std::endl;
    _log << "ClientAuth reply length is wrong"<< msg.length() << "\n";
  }
  if(msg == "1,"){
    std::cout << "Successfully authenticated" << std::endl;
    _log << "Successfully authenticated\n";
  }
  else if(msg == "0,"){
    std::cout << "Failed to authenticate" << std::endl;
    _log << "Failed to authenticate\n";
  }

}

void CUser::Read_GetAvailableRooms(std::string &msg){
  std::vector<std::string> words;
  boost::split(words, msg, boost::is_any_of(","), boost::token_compress_on);
  int numRooms;
  try {
    numRooms = boost::lexical_cast<int>( words[0] );
  } catch( boost::bad_lexical_cast const& ) {
    std::cout << "Error: input string was not valid" << std::endl;
  }
  std::cout << "Number of rooms: " << numRooms << std::endl;
  std::cout << "Rooms are:" << std::endl;
  for(int i = 1 ; i < words.size(); ++i){
    if(i != words.size() - 1)
      std::cout << words[i] << ", ";
  }
  std::cout << std::endl;
  _log << "Available rooms are : " << msg << "\n";
}

void CUser::Read_GetRoomStatus(std::string &msg){

}

void CUser::Read_CreateRoom(std::string &msg){

}

void CUser::Read_JoinRoom(std::string &msg){

}

void CUser::Read_LeaveRoom(std::string &msg){

}

void CUser::Read_DeliverMessagePacket(std::string &msg){

}

void CUser::Read_Disconnect(std::string &msg){

}

/*******************************
********** write message *******
*******************************/

bool CUser::writeMessage(int action){
  switch(action){
    case CLIENT_AUTH:
      Write_ClientAuth(sockfd);
      break;
    case GET_AVAILABLE_ROOMS:
      Write_GetAvailableRooms(sockfd);
      break;
    case GET_ROOM_STATUS:
      Write_GetRoomStatus(sockfd);
      break;
    case CREATE_ROOM:
      Write_CreateRoom(sockfd);
      break;
    case JOIN_ROOM:
      Write_JoinRoom(sockfd);
      break;
    case LEAVE_ROOM:
      Write_LeaveRoom(sockfd);
      break;
    case DELIVER_MESSAGE_PACKET:
      Write_DeliverMessagePacket(sockfd);
      break;
    case DISCONNECT:
      Write_Disconnect(sockfd);
      break;
  }
  return true;
}

void CUser::Write_ClientAuth(int sockfd){
  _log << "-------ClientAuth--------\n";
  /*
    Size(4)|Action(2)|Username(?)|,(1)|Password(?)|,(1)
  */
  std::string msg;
  appendInt(msg, ACTION_SIZE + _username.size() + 1 
   + _password.size() + 1);
  appendShort(msg, CLIENT_AUTH);
  appendString(msg, _username);
  appendString(msg, _password);
  std::cout  << msg << std::endl;
  writeMsg(_sockfd, msg);
  _log.flush();
}

void CUser::Write_GetAvailableRooms(int sockfd){
  _log << "-------GetAvailableRooms--------\n";
  /*
    Size(4)|Action(2)|Username(?)|,(1)
  */
  std::string msg;
  appendInt(msg, ACTION_SIZE + _username.size() + 1);
  appendShort(msg, GET_AVAILABLE_ROOMS);
  appendString(msg, _username);
  std::cout << msg << std::endl;
  writeMsg(_sockfd, msg);
  _log.flush();
}

void CUser::Write_GetRoomStatus(int sockfd){
 _log << "-------GetRoomStatus--------\n";
  /*
    Size(4)|Action(2)|Username(?)|,(1)|Room name(?)|,(1)
  */
  std::string room;
  std::string msg;

  std::cout << "Enter room name: " ;
  getline(std::cin, room);
  appendInt(msg, ACTION_SIZE + _username.length() + 1
    + room.length() + 1);
  appendShort(msg, GET_ROOM_STATUS);
  appendString(msg, _username);
  appendString(msg, room);
  std::cout << msg << std::endl;
  writeMsg(_sockfd, msg);
}

void CUser::Write_CreateRoom(int sockfd){
  _log << "-------CreateRoom--------\n";
  /*
    Size(4)|Action(2)|Capacity(?)|Username(?)|,(1)|Room name(?)|,(1)
  */
  std::string msg;
  std::string room;
  std::string capacity;
  /* Clear buffer */
  std::cin.ignore();
  std::cout << "Enter room name: ";
  std::getline(std::cin, room);
  std::cout << "Enter room capacity: ";
  std::getline(std::cin, capacity);
  appendInt(msg, ACTION_SIZE + ACTION_SIZE + _username.length() + 1
    + room.length() + 1);
  appendShort(msg, CREATE_ROOM);
  appendString(msg, capacity);
  appendString(msg, _username);
  appendString(msg, room);
  std::cout << msg << std::endl;
  writeMsg(_sockfd, msg);
}

void CUser::Write_JoinRoom(int sockfd){
  _log << "-------JoinRoom--------\n";
  /*
    Size(4)|Action(2)|Username(?)|,(1)|Room name(?)|,(1)
  */
  std::string msg;
  std::string room;
  std::cout << "Enter room name: ";
  getline(std::cin, room);
  appendInt(msg, ACTION_SIZE + _username.length() + 1
    + room.length() + 1);
  appendShort(msg, JOIN_ROOM);
  appendString(msg, _username);
  appendString(msg, room);
  std::cout << msg << std::endl;
  writeMsg(_sockfd, msg);
}

void CUser::Write_LeaveRoom(int sockfd){
  _log << "-------LeaveRoom--------\n";
  /*
    Size(4)|Action(2)|Username(?)|,(1)|Room name(?)|,(1)
  */
  std::string msg;
  appendInt(msg, ACTION_SIZE + _username.length() + 1
    + _room.length() + 1);
  appendShort(msg, LEAVE_ROOM);
  appendString(msg, _username);
  appendString(msg, _room);
  std::cout << msg << std::endl;
  writeMsg(_sockfd, msg);
}

void CUser::Write_DeliverMessagePacket(int sockfd){
  _log << "-------DeliverMessagePacket--------\n";
  /*
    Size(4)|Action(2)|Username(?)|,(1)|Message(?)|,(1)
  */
  std::string msg;
  std::string message;
  std::cout << _username <<": ";
  getline(std::cin, message);
  appendInt(msg, ACTION_SIZE + _username.length() + 1
    + message.length() + 1);
  appendShort(msg, DELIVER_MESSAGE_PACKET);
  appendString(msg, _username);
  appendString(msg, message);
  std::cout << msg << std::endl;
  writeMsg(_sockfd, msg);
}

void CUser::Write_Disconnect(int sockfd){
  _log << "-------handleDisconnect--------\n";
  /*
    Size(4)|Action(2)|Username(?)|,(1)
  */
  std::string msg;
  appendInt(msg, ACTION_SIZE + _username.length() + 1);
  appendShort(msg, DISCONNECT);
  appendString(msg, _username);
  std::cout << msg << std::endl;
  writeMsg(_sockfd, msg);

}