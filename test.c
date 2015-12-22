#include <stdio.h>
#include <stdlib.h>
// #include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

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
	inet_pton(AF_INET, "localhost", &serverAddress.sin_addr.s_addr);
	// serverAddress.sin_addr.s_addr = INADDR_ANY;

	printf("Inaddr any: %d\n", INADDR_ANY); // = 0

	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	printf("File descriptor: %d\n", fd);
	
	if (fd < 0) {
		printf("%s\n", "Error creating socket!");
	}

	int ret = bind(fd, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

	printf("Bind return value: %d\n", ret);

	printf("%hhx\n", ((char *)&serverAddress.sin_addr.s_addr)[0]);
	printf("%hhx\n", ((char *)&serverAddress.sin_addr.s_addr)[1]);
	printf("%hhx\n", ((char *)&serverAddress.sin_addr.s_addr)[2]);
	printf("%hhx\n", ((char *)&serverAddress.sin_addr.s_addr)[3]);

	// printf("%p\n", &(((char *)&serverAddress.sin_addr.s_addr)[0]));
	// printf("%p\n", &(((char *)&serverAddress.sin_addr.s_addr)[1]));
	// printf("%p\n", &(((char *)&serverAddress.sin_addr.s_addr)[2]));
	// printf("%p\n", &(((char *)&serverAddress.sin_addr.s_addr)[3]));

	printf("%lu\n", sizeof(serverAddress.sin_addr.s_addr));

	ret = sendto(fd, "hi", 2, 0, 0, 0);
	printf("Send return val: %d\n", ret);

}