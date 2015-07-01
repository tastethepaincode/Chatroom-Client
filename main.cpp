#include <fstream>
#include <iostream>

#include "User.hpp"

std::ofstream _log("log.txt", std::ofstream::out);
int main(){
    CUser client("james", "zheng", "49999", "127.0.0.1");
    client.establishConnection();
    client.authenticate();
    return 0;
}