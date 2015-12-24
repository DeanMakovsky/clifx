#include <iostream>
#include <thread>
#include <cstring>
#include <cstdio>

#include <arpa/inet.h>  // for inet_pton
#include <sys/socket.h>
#include <netinet/in.h>

#include "Messages.h"
#include "MySocket.h"

using namespace std;




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

	return 0;


	// make random colors!
	for ( ;; ) {
		Color c(randy(0,65535),65535,4*4096,randy(2500,9000),10);
		MessageBuffer * b = c.makeBuffer();
		sock.send(b->buf, b->size);
		delete b;
		this_thread::sleep_for (chrono::seconds(1));
	}

	return 0;
}