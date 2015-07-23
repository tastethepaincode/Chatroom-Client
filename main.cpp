#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include "User.hpp"
bool globalDisconnect = false;
std::ofstream _log("log.txt", std::ofstream::out);
std::mutex _mtx;
void printOptions();
int main(){
  std::string username;
  std::cout << "Enter username: ";
  std::getline(std::cin, username);
  CUser client(username, "zheng", "49999", "127.0.0.1");
  client.establishConnection();
    /* Send Message */
  client.start();
  return 0;
}
