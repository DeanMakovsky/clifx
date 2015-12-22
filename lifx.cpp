#include <iostream>
#include <stdint.h>
#include <cstring>
using namespace std;

#pragma pack(push, 1)
typedef struct {
  /* frame */
  uint16_t size;
  uint16_t protocol:12;
  uint8_t  addressable:1;
  uint8_t  tagged:1;
  uint8_t  origin:2;
  uint32_t source;
  /* frame address */
  uint8_t  target[8];
  uint8_t  reserved[6];
  uint8_t  res_required:1;
  uint8_t  ack_required:1;
  uint8_t  :6;
  uint8_t  sequence;
  /* protocol header */
  uint64_t :64;
  uint16_t type;
  uint16_t :16;
  /* variable length payload follows */
} Header;

typedef struct {
	Header h;
	uint8_t  :8;
	uint16_t hue:16;
	uint16_t saturation:16;
	uint16_t brightness:16;
	uint16_t kelvin:16;
	uint32_t duration:32;

} Color;

#pragma pack(pop)

#include <arpa/inet.h>  // for inet_pton

#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <errno.h>

struct sockaddr_in makeBCastDest() {
	struct sockaddr_in dest;
	memset(&dest,0,sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_port = htons(56700);
	inet_pton(AF_INET, "255.255.255.255", &dest.sin_addr.s_addr);
	return dest;
}

void sendPacket(Color c) {

	struct sockaddr_in serverAddress;
	memset(&serverAddress,0,sizeof(serverAddress));

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(56700);
	// inet_pton(AF_INET, "localhost", &serverAddress.sin_addr.s_addr);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	// check for correctness
	printf("File descriptor: %d\n", fd);
	if (fd < 0) {
		printf("%s\n", "Error creating socket!");
	}

	// allow UDP broadcast
	int broadcastEnable=1;
	int ret=setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
	printf("Socket options return value: %d\n", ret);



	ret = bind(fd, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

	printf("Bind return value: %d\n", ret);

	struct sockaddr_in destination = makeBCastDest();

	ret = sendto(fd, &c, sizeof(Color), 0, (struct sockaddr *) &destination, sizeof(destination));
	printf("Send return val: %d\n", ret);
	if (ret < 0) {
		int val = errno;
		printf("Errno: %d, %s\n", val, strerror(val) );
	}
}


int main(int argc, char ** argv) {
	Header h;
	memset(&h, 0, sizeof(Header));
	h.size = sizeof(Color);
	h.protocol = 1024;
	h.origin = 3;
	h.source = 0;

	// h.target = 0; // for all MACs
	h.sequence = 0;

	h.type = 102;



	Color c;
	memset(&c, 0, sizeof(Color));
	c.h = h;

	c.hue = 0;
	c.saturation = 65535;
	c.brightness = 4096;
	c.kelvin = 2500;
	c.duration = 10;

	sendPacket(c);

	// cout << "size " << sizeof(lx_protocol_header_t) << endl;
	return 0;
}