main: Messages.h Messages.cpp lifx.cpp MySocket.h MySocket.cpp
	clang++ -Wall lifx.cpp Messages.cpp MySocket.cpp -std=c++11

clean:
	rm a.out