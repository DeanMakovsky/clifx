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

#include <iomanip> // for cout columns
#include <bitset>  // for pretty printing odd-sized bit fields
using namespace std;

// #define tab "\t"
#define fcol setw(15)
#define tab setw(10)

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
	static Header * deserialize(int);
	void printEverything();
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
* If the message type is unknown (or payload is empty), returns a regular Header.
* TODO socket will be non-blocking, so if there is no data, then "type" will be 0.
*/
Header * Header::deserialize(int sockfd) {
	
	// set up data structures
	char buffer[100];
	memset(buffer, 0, sizeof(buffer));

	struct sockaddr_in sender;
	socklen_t senderSize = sizeof(sender);
	socklen_t backup = senderSize;


	// get data from socket
	int ret = recvfrom(sockfd, buffer, sizeof(buffer), 0 ,
		(sockaddr *) &sender, &senderSize);
	if (ret == -1) {
		int val = errno;
		printf("Error reading from socket: %d, %s\n", val, strerror(val) );
	}


	// sender diagnostics, mainly for fun
	if (backup != senderSize) {
		printf("The sender address of this message changed (%d -> %d)\n", backup, senderSize);
	}
	// printf("sin_family: %hd\t", sender.sin_family);
	// printf("AF_INET := %d\n", AF_INET);
	char ip_buffer[100];
	memset(ip_buffer, 0, sizeof(ip_buffer));
	inet_ntop(AF_INET, &sender.sin_addr.s_addr, ip_buffer, sizeof(ip_buffer));
	printf("Sending address: %s:%hu\n", ip_buffer, ntohs(sender.sin_port));


	// validate the input some
	printf("Read -%d- bytes.\n", ret);
	if (ret > sizeof(buffer)) {
		printf("Read too many bytes for buffer, truncating message.\n");
		ret = sizeof(buffer);
	}
	Header h;
	memcpy(&h, buffer, ret);
	if (h.protocol != 1024) {
		printf("Incorrect protocol number, probably bad packet.\n");
	}
	// printf("Size: %d\nProtocol: %d\nType: %d\n", h.size, h.protocol, h.type);
	h.printEverything();


	Color * obj = new Color(1,2,3,4,5);
	printf("Returning size: %lu\n", sizeof(obj));  // TODO this is inconsistent, relearn the virtual keyword
	return obj;
}


void Header::printEverything() {
	// /* frame */ 
	// uint16_t size;
	// uint16_t protocol:12;
	// uint8_t  addressable:1;
	// uint8_t  tagged:1;
	// uint8_t  origin:2;
	// uint32_t source;
	// /* frame address */
	// uint8_t  target[8];
	// uint8_t  reserved[6];
	// uint8_t  res_required:1;
	// uint8_t  ack_required:1;
	// uint8_t  :6;
	// uint8_t  sequence;
	// /* protocol header */
	// uint64_t :64;
	// uint16_t type;
	// uint16_t :16;

	cout << "~~~~~ Frame ~~~~~" << endl;
	cout << fcol << "size" << tab << size << endl;
	cout << fcol << "protocol" << tab << protocol << endl;
	cout << fcol << "addressable" << tab << bitset<1>(addressable) << endl;
	cout << fcol << "tagged" << tab << bitset<1>(tagged) << endl;
	cout << fcol << "origin" << tab << bitset<2>(origin) << endl;
	cout << fcol << "source" << tab << source << endl;
	cout << "~~~~~ Frame Address ~~~~~" << endl;
	cout << fcol << "target" << tab << "0x" << flush;
	char buf[8];
	memcpy(buf, &target, 8);
	for (int i = 0; i < 8; i++) {
		printf("%2hhx", buf[i]);
	}
	printf("\n");
	cout << fcol << "reserved" << tab << "6 bytes" << endl;
	cout << fcol << "res_required" << tab << bitset<1>(res_required) << endl;
	cout << fcol << "ack_required" << tab << bitset<1>(ack_required) << endl;
	cout << fcol << "reserved" << tab << "6 bits" << endl;
	cout << fcol << "sequence" << tab << bitset<8>(sequence).to_ulong() << endl;
	cout << "~~~~~ Protocol Header ~~~~~" << endl;
	cout << fcol << "reserved" << tab << "8 bytes" << endl;
	cout << fcol << "type" << tab << type << endl;
	cout << fcol << "reserved" << tab << "2 bytes" << endl;
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

	Header * thing = Header::deserialize(sock.getSocket());
	printf("Returned object size: %lu\n", sizeof(*thing));

	return 0;



	for ( ;; ) {
		Color c(randy(0,65535),65535,4*4096,randy(2500,9000),10);
		sock.send(&c, sizeof(c));
		this_thread::sleep_for (chrono::seconds(1));
	}

	return 0;
}