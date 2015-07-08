#include <fstream>
#include <iostream>

#include "User.hpp"

std::ofstream _log("log.txt", std::ofstream::out);
void printOptions();
int main(){
    CUser client("james", "zheng", "49999", "127.0.0.1");
    client.establishConnection();
    while(1){
        int option = -999;
        printOptions();
        std::cout << "Option 0 to 6: " ;
        _log << "Option 0 to 6: \n";
        std::cin >> option;
        client.selectOption(option);
        if(option == 9){//exit
            exit(0);
        }
        std::cout << "You selected: " << option << std::endl;
        _log << "You selected: " << option << "\n";
        _log.flush();
    }
    return 0;
}

void printOptions(){
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
}