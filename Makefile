CC=g++
CFLAGS=-std=c++11 -pthread -c
CFLAGSO=-std=c++11 -pthread -o
Client: main.o MessageHandler.o User.o Poller.o 
	$(CC) $(CFLAGSO) Client main.o Poller.o MessageHandler.o User.o 
main.o: main.cpp Macroz.hpp
	$(CC) $(CFLAGS) main.cpp
User.o: User.hpp User.cpp
	$(CC) $(CFLAGS) User.cpp
MessageHandler.o: MessageHandler.hpp MessageHandler.cpp Macroz.hpp
	$(CC) $(CFLAGS) MessageHandler.cpp
Poller.o: Poller.hpp Poller.cpp Macroz.hpp
	$(CC) $(CFLAGS) Poller.cpp
clean:
	rm -f *.o *.txt Client 
