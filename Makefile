Client: main.o MessageHandler.o User.o Poller.o 
	g++ -std=c++11 -o Client main.o Poller.o MessageHandler.o User.o 
main.o: main.cpp
	g++ -std=c++11 -c main.cpp
User.o: User.hpp User.cpp
	g++ -std=c++11 -c User.cpp
MessageHandler.o: MessageHandler.hpp MessageHandler.cpp
	g++ -std=c++11 -c MessageHandler.cpp
Poller.o: Poller.hpp Poller.cpp
	g++ -std=c++11 -c Poller.cpp
clean:
	rm *.o *.txt Client 
