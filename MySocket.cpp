#include <arpa/inet.h>  // for inet_pton
#include <sys/socket.h>
#include <netinet/in.h>

#include <errno.h>
#include <cstring>
#include <cstdio>

#include "MySocket.h"

using namespace std;


struct sockaddr_in makeBCastDest() {
	struct sockaddr_in dest;
	memset(&dest,0,sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_port = htons(56700);
	inet_pton(AF_INET, "255.255.255.255", &dest.sin_addr.s_addr);
	return dest;
}

MySocket::MySocket() {
	struct sockaddr_in serverAddress;
	memset(&serverAddress,0,sizeof(serverAddress));

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(56700);
	// inet_pton(AF_INET, "localhost", &serverAddress.sin_addr.s_addr);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	// check for correctness
	// printf("File descriptor: %d\n", fd);
	if (fd == -1) {
		int val = errno;
		printf("Error creating socket: %d, %s\n", val, strerror(val) );
	}

	// allow UDP broadcast
	int broadcastEnable=1;
	int ret = setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
	// printf("Socket options return value: %d\n", ret);
	if (ret == -1) {
		int val = errno;
		printf("Error setting socket options: %d, %s\n", val, strerror(val) );
	}


	ret = bind(fd, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
	// printf("Bind return value: %d\n", ret);
	if (ret == -1) {
		int val = errno;
		printf("Error binding socket: %d, %s\n", val, strerror(val) );
	}
	
}
void MySocket::send(void * start, int len) {
	struct sockaddr_in destination = makeBCastDest();

	int ret = sendto(fd, start, len, 0, (struct sockaddr *) &destination, sizeof(destination));
	// printf("Send return val: %d\n", ret);
	if (ret == -1) {
		int val = errno;
		printf("Errno: %d, %s\n", val, strerror(val) );
	}
}