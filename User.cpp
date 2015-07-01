#include "User.hpp"
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
            break;
        }

        close(_sockfd); 
    }

    if(rp == NULL){
        _log << "Error connection fail due to rp is NULL\n";
    }

    //disable Nagle's algorithm
    int optval = 1;
    int status = setsockopt(_sockfd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
    if(status == -1){
      perror("Error on disabling Naggle's algorithm.");
      _log << "Error on disabling Naggle's algorithm.\n";
    }

    freeaddrinfo(result);
}

bool CUser::authenticate(){
    std::string authMsg;
    msgHandler.appendInt(authMsg, ACTION + _username.length() + 1 
      + _password.length() + 1);
    msgHandler.appendShort(authMsg, CLIENT_AUTH);
    msgHandler.appendString(authMsg, _username);
    msgHandler.appendString(authMsg, _password);
    std::cout  << authMsg << std::endl;
    msgHandler.writeMsg(_sockfd, authMsg);
    return 0;
}
bool CUser::getAvailableRooms(){
    return 0;
}
bool CUser::getRoomStatus(){
    return 0;
}
bool CUser::createRoom(){
    return 0;
}
bool CUser::joinRoom(){
    return 0;
}
bool CUser::leaveRoom(){
    return 0;
}
