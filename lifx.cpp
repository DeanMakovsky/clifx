#include <iostream>
#include <stdint.h>
#include <cstring>
#include <cstdlib>
#include <thread>


#include <arpa/inet.h>  // for inet_pton

#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <errno.h>

using namespace std;


/* 
compile with:
clang++ -Wall lifx.cpp -std=c++11
*/




#pragma pack(push, 1)
class Header {
protected:
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
public:
	Header();
	static Header deserialize(int);
};

class Color: public Header {
protected:
	uint8_t  :8;
	uint16_t hue:16;
	uint16_t saturation:16;
	uint16_t brightness:16;
	uint16_t kelvin:16;
	uint32_t duration:32;
public:
	Color(unsigned int,unsigned int,unsigned int,unsigned int,unsigned int);
	void print();
};

#pragma pack(pop)





Color::Color(unsigned int _hue, unsigned int _sat, unsigned int _bright, unsigned int _kelvin, unsigned int _duration) : Header() {
	// cout << "Color size (this): " << sizeof(*this) << endl;
	// cout << "Color size (Header): " << sizeof(Header) << endl;
	// cout << "Color size (Color): " << sizeof(Color) << endl;
	
	size = sizeof(*this);

	// zero out everything except what the superconstructor did
	memset(((char *)this)+sizeof(Header), 0, sizeof(*this) - sizeof(Header));
	hue = _hue;
	saturation = _sat;
	brightness = _bright;
	kelvin = _kelvin;
	duration = _duration;
}

Header::Header() {
	// cout << "Header size (this): " << sizeof(*this) << endl;
	// cout << "Header size (Header): " << sizeof(Header) << endl;
	// cout << "Header size (Color): " << sizeof(Color) << endl;
	memset(this, 0, sizeof(*this));
	size = sizeof(Color);
	protocol = 1024;
	tagged = 1;
	addressable = 1;
	source = 0;

	// h.target = 0; // for all MACs
	sequence = 0;

	type = 102;
}

/**
* Reads from a socket the appropriate number of bytes and puts it into a new object.
* If the message type is unknown, prints error info and returns a regular Header.
*/
Header Header::deserialize(int sockfd) {
	char buffer[100];
	memset(buffer, 0, sizeof(buffer));
	printf("Buffer size: %lu\n", sizeof(buffer));

	struct sockaddr_in sender;
	socklen_t senderSize = sizeof(sender);
	socklen_t backup = senderSize;

	int ret = recvfrom(sockfd, buffer, sizeof(buffer), 0 ,
		(sockaddr *) &sender, &senderSize);
	if (ret == -1) {
		int val = errno;
		printf("Error reading from socket: %d, %s\n", val, strerror(val) );
	}

	if (backup != senderSize) {
		printf("The sender address of this message changed (%d -> %d)\n", backup, senderSize);
	}

	printf("Read -%d- bytes.\n", ret);
	if (ret > sizeof(buffer)) {
		printf("Read too many bytes, truncating message.\n");
		ret = sizeof(buffer);
	}
	Header h;
	memcpy(&h, buffer, ret);
	printf("Size: %d\nProtocol: %d\nType: %d\n", h.size, h.protocol, h.type);



	Color obj(1,2,3,4,5);
	printf("Returning size: %lu\n", sizeof(obj));  // TODO this is inconsistent, relearn the virtual keyword
	return obj;
}

void Color::print() {
	// TODO look up this stuff for printf
	printf("structure size : %zu bytes\n",sizeof(*this));
	for(int i = 0; i < sizeof(*this) ; i++) {
		// printf("%02x ",this[i]);
	}

}




struct sockaddr_in makeBCastDest() {
	struct sockaddr_in dest;
	memset(&dest,0,sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_port = htons(56700);
	inet_pton(AF_INET, "255.255.255.255", &dest.sin_addr.s_addr);
	return dest;
}


class MySocket {
	int fd;
public:
	MySocket();
	void send(void *, int);
	int getSocket() { return fd; }
};

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
	printf("Send return val: %d\n", ret);
	if (ret == -1) {
		int val = errno;
		printf("Errno: %d, %s\n", val, strerror(val) );
	}
}
int randy(int min, int max) {
	int ret = rand();
	return (ret % (max - min)) + min;
}


int main(int argc, char ** argv) {

	MySocket sock;

	Header thing = Header::deserialize(sock.getSocket());
	printf("Returned object size: %lu\n", sizeof(thing));

	return 0;



	for ( ;; ) {
		Color c(randy(0,65535),65535,4*4096,randy(2500,9000),10);
		sock.send(&c, sizeof(c));
		this_thread::sleep_for (chrono::seconds(1));
	}

	return 0;
}