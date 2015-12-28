#include <iostream>
#include <thread>
#include <cstring>
#include <cstdio>

#include <arpa/inet.h>  // for inet_pton
#include <sys/socket.h>
#include <netinet/in.h>

#include <fcntl.h>

#include "Messages.h"
#include "MySocket.h"

using namespace std;

/*

Things to do:
* implement more messages
	* messages that have only text payloads have identical constructor as the deserialize
	* setting payload content of strings 
* return correct messages from deserialize()
* target individual bulbs
* check that the Get/Set/State power Light messages are the same content as the Device messages

Considerations:
* Make MySocket non-blocking optionally, my passing in a bool.

*/

void checkBlocking(int sockfd) {
	if(fcntl(sockfd, F_GETFL) & O_NONBLOCK) {
    	// socket is non-blocking
    	printf("Socket non-blocking.\n");
	} else {
		printf("Socket is blocking.\n");
	}
}

int randy(int min, int max) {
	int ret = rand();
	return (ret % (max - min)) + min;
}

int main(int argc, char ** argv) {

	MySocket sock;

	// test sizes
	// Header thing;
	// printf("%lu\n", sizeof(thing));
	// Color t2(1,2,3,4,5);
	// printf("%lu\n", sizeof(t2));


	// read some messages
	// Header * thing = Header::deserialize(sock.getSocket());
	// printf("Returned object size: %lu\n", sizeof(*thing));
	// thing->printEverything();


	// get socket options
	int sockfd = sock.getSocket();
	checkBlocking(sockfd);

	// printf("O_RDWR: %d\n", O_RDWR);
	// printf("Socket has: %d\n", fcntl(sockfd, F_GETFL));

	// Put the socket in non-blocking mode:
	if(fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL) | O_NONBLOCK) < 0) {
	    printf("Error putting socket in non-blocking mode.\n");
	}

	printf("~~Socket options changed~~\n");
	checkBlocking(sockfd);


	// test non-blocking sockets
	int messagesRead = 0;
	for ( ;; ) {

		while (true) { // while there are messages left, read them
			Header thing = Header::deserialize(sock.getSocket());
			if (thing.getType() != 0) {
				messagesRead += 1;
				printf("Messages Read: %d\n", messagesRead);
				thing.printEverything();
			} else {
				break;
			}
		}
		// printf("->Start to sleep.\n");
		this_thread::sleep_for (chrono::seconds(1));
		// printf("<-End sleep.\n");
	}
	return 0;


	// make random colors!
	for ( ;; ) {
		SetColor c(randy(0,65535),65535,4*4096,randy(2500,9000),10);
		MessageBuffer * b = c.makeBuffer();
		sock.send(b->buf, b->size);
		delete b;
		this_thread::sleep_for (chrono::seconds(1));
	}

	return 0;
}