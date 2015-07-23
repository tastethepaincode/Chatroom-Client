#include "User.hpp"
extern std::ofstream _log;
extern int globalDisconnect;
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
    ps(Error server is down);
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
      _log << "Error cannot close pfd " << _sockfd << "\n";
    }
}

void CUser::start(){
  //This thread reads packets from Server
  std::thread t1([this] {this->constantUpdate();});
  t1.detach();
  _input = false;
  writeMessage(CLIENT_AUTH);
  while(1){
    //if user in room then always write message
    if(_room.compare("") != 0){
      selectOption(DELIVER_MESSAGE_PACKET);
    }
    //if not in room the display other options
    else{
      if(_input== false){
        continue;
      }
      int option = -999;
      printOptions();
      std::cout << "Option 0 to 9: " ;
      _log << "Option 0 to 9: \n";
      std::cin >> option;
      std::cin.ignore();
      if(option < 0 || option > 9){
        ps(Invalid option try again.);
        continue;
      }
      std::cout << "You selected: " << option << std::endl;
      _log << "You selected: " << option << "\n\n";
      _log.flush();
      selectOption(option);

      if(option == 9){//exit
        exit(0);
      }
      if(option == 8){
        continue;
      }

      _input = false;
    }
  }
}

void CUser::printOptions(){
  std::cout << std::endl;
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
  ps();
  logs(==================================);
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
  //read first then turn input back to on
  _input = true;
  logs(====================================);
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
    _status = 1;
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
  ps();
  _log << "Available rooms are : " << msg << "\n";
}

void CUser::Read_GetRoomStatus(std::string &msg){
  //  Size(4)|Action(2)|Roomname(?)|,(1)|Capacity(?)|,(1)|Size(?)|,(1)|Username(?)|,(1)
  std::vector<std::string> words;
  boost::split(words, msg, boost::is_any_of(","), boost::token_compress_on);
  if(words.size() == 3){
    //No room exist
    psvs(Room, words[0], does not exist);
    logsvs(Room, words[0], does not exist);
    return;
  }
  else{
    std::string roomname = words[0];
    std::string capacity = words[1];
    std::string size = words[2];
    psv(Room, roomname);
    logsv(Room, roomname);
    psv(Size:, size);
    logsv(Size:, size);
    psv(Max size:, capacity);
    logsv(Max size:, capacity);
    // turn words into vector of std::string username
    words.erase(words.begin()+0);
    words.erase(words.begin()+0);
    words.erase(words.begin()+0);
    ps(Players:);
    logs(Players:);
    for(std::string usr: words){
      pv(usr);
      logv(usr);
    }
  }
}

void CUser::Read_CreateRoom(std::string &msg){
  //Size(4)|Action(2)|Result(2)|Roomname(?)
  std::vector<std::string> words;
  boost::split(words, msg, boost::is_any_of(","), boost::token_compress_on);
  if(words[0].compare("0") == 0){
    psv(Failed to create room: , words[1]);
  }
  else{
    psv(Created room: , words[1]);
  }
}

void CUser::Read_JoinRoom(std::string &msg){
  //Size(4)|Action(2)|Result(2)|Roomname
  std::vector<std::string> words;
  boost::split(words, msg, boost::is_any_of(","), boost::token_compress_on);
  if(words[0].compare("1") == 0){
    _room = words[1];
    psv(you have joined room, words[1]);
    logsv(you have joined romm, words[1]);
    _status = 2;
    _autoUpdate = 1;
    return;
  }

  psv(cannot join room, words[1]);
  logsv(cannot join room, words[1]);
}

void CUser::Read_LeaveRoom(std::string &msg){
  // Size(4)|Action(2)|Result(2)|Roomname(?)
  std::vector<std::string> words;
  boost::split(words, msg, boost::is_any_of(","), boost::token_compress_on);
  if(words[0].compare("1") == 0){
    if(_room.compare(words[1]) == 0){
      psv(You left room: , _room);
      logsv(You left room: , _room);
      _room = "";
      _status = 1;
    }
    else{
      psvsv(The room you requested to leave is , words[1], but you are in room , _room);
      logsvsv(The room you requested to leave is , words[1], but you are in room , _room);
    }
  }
  else{
    psv("Error: unable to leave room ", words[1]);
    logsv("Error: unable to leave room ", words[1]);

  }

}

void CUser::Read_DeliverMessagePacket(std::string &msg){
  // Size(4)|Action(2)|Username(?)|Roomname(?)
  logs(Read_DeliverMessagePacket);
  std::vector<std::string> words;
  boost::split(words, msg, boost::is_any_of(","), boost::token_compress_on);
  std::string username = words[0];
  std::string message = words[1];
  //Print out only others' message
  if(username.compare(_username) != 0){
    std::cout << username << ": " << message << std::endl;
    std::cout.flush();
  }
  else{
    _input = true;
  }
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
  writeMsg(_sockfd, msg);
  _log.flush();
}

void CUser::Write_GetAvailableRooms(int sockfd){
  _log << "-------GetAvailableRooms--------\n";
  /*
    Size(4)|Action(2)|Username(?)|,(1)
  */
  if(_status == 0){
    ps(Please login first.);
    return;
  }
  std::string msg;
  appendInt(msg, ACTION_SIZE + _username.size() + 1);
  appendShort(msg, GET_AVAILABLE_ROOMS);
  appendString(msg, _username);
  writeMsg(_sockfd, msg);
  _log.flush();
}

void CUser::Write_GetRoomStatus(int sockfd){
 _log << "-------GetRoomStatus--------\n";
  /*
    Size(4)|Action(2)|Username(?)|,(1)|Room name(?)|,(1)
  */
  if(_status == 0){
    ps(Please login first.);
    return;
  }
  std::string room;
  std::string msg;

  std::cout << "Enter room name: " ;
  getline(std::cin, room);
  appendInt(msg, ACTION_SIZE + _username.length() + 1
    + room.length() + 1);
  appendShort(msg, GET_ROOM_STATUS);
  appendString(msg, _username);
  appendString(msg, room);
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
  std::cout << "Enter room name: ";
  std::getline(std::cin, room);
  std::cout << "Enter room capacity: ";
  std::getline(std::cin, capacity);
  appendInt(msg, ACTION_SIZE + capacity.size() + 1 + _username.length() + 1
    + room.size() + 1);
  appendShort(msg, CREATE_ROOM);
  appendString(msg, capacity);
  appendString(msg, _username);
  appendString(msg, room);
  writeMsg(_sockfd, msg);
}

void CUser::Write_JoinRoom(int sockfd){
  _log << "-------JoinRoom--------\n";
  /*
    Size(4)|Action(2)|Username(?)|,(1)|Room name(?)|,(1)
  */
  if(_status == 0){
    ps(Please login first.)
  }
  if(_room.compare("") != 0){
    ps(Please leave your room before you join another.);
    logs(Please leave your room before you join another.);
    return;
  }

  std::string msg;
  std::string room;
  std::cout << "Enter room name: ";
  getline(std::cin, room);
  appendInt(msg, ACTION_SIZE + _username.length() + 1
    + room.length() + 1);
  appendShort(msg, JOIN_ROOM);
  appendString(msg, _username);
  appendString(msg, room);
  writeMsg(_sockfd, msg);
}

void CUser::Write_LeaveRoom(int sockfd){
  _log << "-------LeaveRoom--------\n";
  /*
    Size(4)|Action(2)|Username(?)|,(1)|Room name(?)|,(1)
  */
  if(_status == 0){
    ps(Please login first.);
  }
  if(_status == 1){
    ps(You are not in a room.);
    logs(You are not in a room.);
    return;
  }
  std::string msg;
  appendInt(msg, ACTION_SIZE + _username.length() + 1
    + _room.length() + 1);
  appendShort(msg, LEAVE_ROOM);
  appendString(msg, _username);
  appendString(msg, _room);
  writeMsg(_sockfd, msg);
}

void CUser::Write_DeliverMessagePacket(int sockfd){
  _log << "-------DeliverMessagePacket--------\n";
  /*
    Size(4)|Action(2)|Username(?)|,(1)|Message(?)|,(1)
  */
  std::string msg;
  std::string message;
  getline(std::cin, message);
  //If user wants to leave
  if(message.compare(LEAVE_INDICATION) == 0){
    Write_LeaveRoom(sockfd);
    _room = "";
    return;
  }
  appendInt(msg, ACTION_SIZE + _username.length() + 1
    + message.length() + 1);
  appendShort(msg, DELIVER_MESSAGE_PACKET);
  appendString(msg, _username);
  appendString(msg, message);
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
  writeMsg(_sockfd, msg);
  globalDisconnect = true;

}

void CUser::constantUpdate(){
    /* Read sockets */
  while(1){
    if(globalDisconnect == true){
      exit(0);
    }
    int ret = 0;
    if( ret = setReadPoll(500) > 0){
      _log << "Poll detected " << ret << " reply \n";
      for(struct pollfd& pfd: _userPollfd){

      /*There is a message to be processed*/
        if(pfd.revents & POLLIN){
          if(readPoll(_sockfd) == false){
            /* TODO */
            psv(Error in readPoll processing socket , _sockfd);
            closeSocketfd(_sockfd);
            exit(0);
          }
        }
        else if(pfd.revents & POLLHUP){
          _log << "Poll detected that " << pfd.fd << " hanged up POLLHUP\n";
          exit(0);
        }
      }
    }
  }
}