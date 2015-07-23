Client: main.o MessageHandler.o User.o Poller.o 
	g++ -std=c++11 -pthread -o Client main.o Poller.o MessageHandler.o User.o 
main.o: main.cpp Macroz.hpp
	g++ -std=c++11 -pthread -c main.cpp
User.o: User.hpp User.cpp
	g++ -std=c++11 -pthread -c User.cpp
MessageHandler.o: MessageHandler.hpp MessageHandler.cpp Macroz.hpp
	g++ -std=c++11 -pthread -c MessageHandler.cpp
Poller.o: Poller.hpp Poller.cpp Macroz.hpp
	g++ -std=c++11 -pthread -c Poller.cpp
clean:
	rm *.o *.txt Client 
