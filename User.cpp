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

  /*Set up msgHandler*/
  _msgHandler = new CClientMessageHandler(this);

}

void CUser::selectOption(int option){
  //std::cout << "Inside CUser\n";
  std::string myStr;
  if(option == 8){
    return;
  }
  _msgHandler->processMessage(option, myStr);
}

bool CUser::readPoll(int temp){
  _log << "Attempt to read from sockfd " << _sockfd << "\n";
  int msgLength = _msgHandler->removeInt(_sockfd);
  /* Close socket if its EOF which is msgLength = -1*/
  if(msgLength == -1){
    closeSocketfd(_sockfd);
    return false;
  }

  int action = _msgHandler->removeShort(_sockfd);

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

  std::string msg = _msgHandler->removeBytes(_sockfd, msgLength);
  if(msg.empty()){
    _log << "Error message string is empty\n";
    closeSocketfd(_sockfd);
    return false;
  }

  /* Update user information */

  update(action, msg);
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
  _log << "You selected: " << option << "\n";
  _log.flush();
  selectOption(option);

    if(option == 9){//exit
      exit(0);
    }


  /* Read sockets */
  int ret = 0;
  if( ret = setReadPoll(1000) > 0){
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
  std::cout << "8. Refresh " << std::endl;
}

void CUser::update(int action, std::string &msg){

  switch(action){
  case CLIENT_AUTH:
    U_ClientAuth(msg);
    break;
  case GET_AVAILABLE_ROOMS:
    U_GetAvailableRooms(msg);
    break;
  case GET_ROOM_STATUS:
    U_GetRoomStatus(msg);
    break;
  case CREATE_ROOM:
    U_CreateRoom(msg);
    break;
  case JOIN_ROOM:
    U_JoinRoom(msg);
    break;
  case LEAVE_ROOM:
    U_LeaveRoom(msg);
    break;
  case DELIVER_MESSAGE_PACKET:
    U_DeliverMessagePacket(msg);
    break;
  case DISCONNECT:
    U_Disconnect(msg);
    break;
  default:
    std::cout << "Incorrect option received." << std::endl;
    _log << "Incorrect option received.\n";
  }
}

void CUser::U_ClientAuth(std::string &msg){
  /* msg should be a string on length 1 */
  std::cout << "Updating ClientAuth " << std::endl;
  _log << "Updating ClientAuth\n ";
  if(msg.length() != 1){
    std::cout << "ClientAuth reply length is wrong" << std::endl;
    _log << "ClientAuth reply length is wrong\n";
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

void CUser::U_GetAvailableRooms(std::string &msg){

}

void CUser::U_GetRoomStatus(std::string &msg){

}

void CUser::U_CreateRoom(std::string &msg){

}

void CUser::U_JoinRoom(std::string &msg){

}

void CUser::U_LeaveRoom(std::string &msg){

}

void CUser::U_DeliverMessagePacket(std::string &msg){

}

void CUser::U_Disconnect(std::string &msg){

}