#include <stdio.h>
#include <stdlib.h>
// #include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

/*
struct sockaddr_in
{
  short   sin_family;  // must be AF_INET 
  u_short sin_port;
  struct  in_addr sin_addr;
  char    sin_zero[8];  // Not used, must be zero 
};
*/
int main(int argc, char ** argv) {
	printf("%lu\n", sizeof(short));
	printf("%lu\n", sizeof(unsigned long));

	struct sockaddr_in serverAddress;
	memset(&serverAddress,0,sizeof(serverAddress));

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(55554);
	inet_aton("localhost", &serverAddress.sin_addr.s_addr);

	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	printf("%d\n", fd);
	
	if (fd < 0) {
		printf("%s\n", "Error creating socket!");
	}

	int ret = bind(fd, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

	printf("%d\n", ret);

}