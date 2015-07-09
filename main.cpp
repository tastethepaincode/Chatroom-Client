#include <fstream>
#include <iostream>

#include "User.hpp"

std::ofstream _log("log.txt", std::ofstream::out);
void printOptions();
int main(){
  CUser client("james", "zheng", "49999", "127.0.0.1");
  client.establishConnection();
  while(1){
    /* Send Message */
    client.start();
  }
  return 0;
}
