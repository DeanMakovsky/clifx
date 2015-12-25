#include <iostream>
#include <cstdio>
#include <iomanip> // for cout columns
#include <bitset>  // for pretty printing odd-sized bit fields
#include <cstring>

#include <arpa/inet.h>  // for inet_pton
#include <sys/socket.h>
#include <netinet/in.h>

#include "Messages.h"

// #define tab "\t"
#define fcol setw(15)
#define tab setw(10)

using namespace std;


MessageBuffer::MessageBuffer(int _size) {
	buf = (char *) malloc(_size);
	size = _size;
}
MessageBuffer::~MessageBuffer() {
	free(buf);
}


Header::Header() {
	// cout << "Header size (this): " << sizeof(*this) << endl;
	// cout << "Header size (Header): " << sizeof(Header) << endl;
	// cout << "Header size (Color): " << sizeof(Color) << endl;

	// zero out header
	memset(&head, 0, sizeof(head));
	head.size = sizeof(head);
	head.protocol = 1024;
	head.tagged = 1;
	head.addressable = 1;
	head.source = 0;

	// h.target = 0; // for all MACs
	head.sequence = 0;

	head.type = 2; // TODO this is only sometimes correct

}

/**
* Assumes buffer has enough good data for sizeof(Header.head)
*/
Header::Header(char * buffer) {
	memcpy(&head, buffer, sizeof(head) );
}

int Header::getType() {
	return head.type;
}

/**
* Reads from a socket the appropriate number of bytes and puts it into a new object.
* If the message type is unknown (or payload is empty), returns a regular Header.
* TODO socket will be non-blocking, so if there is no data, then "type" will be 0.
*/
Header Header::deserialize(int sockfd) {
	
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
		if (val == EAGAIN || val == EWOULDBLOCK) {
			// printf("No data to read.\n");
			Header h;
			h.head.type = 0;
			return h;
		}
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
	if (ret < sizeof(Header::head)) {
		printf("Not enough bytes to fill a Header object.\n");
	}

	// make and return new object
	Header h(buffer);
	if (h.head.protocol != 1024) {
		printf("Incorrect protocol number, probably bad packet.\n");
	}
	// printf("Size: %d\nProtocol: %d\nType: %d\n", h.size, h.protocol, h.type);
	// h.printEverything();




	// TODO make a map of message types to classes and instantiate the proper class right here.




	return h;
}

MessageBuffer * Header::makeBuffer() {
	int size = sizeof(head);
	MessageBuffer * mb = new MessageBuffer(size);
	memcpy(mb->buf, &head, sizeof(head));
	return mb;
}

void Header::printEverything() {
	cout << "~~~~~~~ Frame ~~~~~~~" << endl;
	cout << fcol << "size" << tab << head.size << endl;
	cout << fcol << "protocol" << tab << head.protocol << endl;
	cout << fcol << "addressable" << tab << bitset<1>(head.addressable) << endl;
	cout << fcol << "tagged" << tab << bitset<1>(head.tagged) << endl;
	cout << fcol << "origin" << tab << bitset<2>(head.origin) << endl;
	cout << fcol << "source" << tab << head.source << endl;
	cout << "~~~~~ Frame Address ~~~~~" << endl;
	cout << fcol << "target" << tab << "0x" << flush;
	char buf[8];
	memcpy(buf, &(head.target), 8);
	for (int i = 0; i < 8; i++) {
		printf("%2hhx", buf[i]);
	}
	printf("\n");
	cout << fcol << "reserved" << tab << "6 bytes" << endl;
	cout << fcol << "res_required" << tab << bitset<1>(head.res_required) << endl;
	cout << fcol << "ack_required" << tab << bitset<1>(head.ack_required) << endl;
	cout << fcol << "reserved" << tab << "6 bits" << endl;
	cout << fcol << "sequence" << tab << bitset<8>(head.sequence).to_ulong() << endl;
	cout << "~~~~~ Protocol Header ~~~~~" << endl;
	cout << fcol << "reserved" << tab << "8 bytes" << endl;
	cout << fcol << "type" << tab << head.type << endl;
	cout << fcol << "reserved" << tab << "2 bytes" << endl;
}



Color::Color(unsigned int _hue, unsigned int _sat, unsigned int _bright, unsigned int _kelvin, unsigned int _duration) : Header() {
	// cout << "Color size (this): " << sizeof(*this) << endl;
	// cout << "Color size (Header): " << sizeof(Header) << endl;
	// cout << "Color size (Color): " << sizeof(Color) << endl;
	head.size = sizeof(head) + sizeof(payload);

	// zero out payload
	memset(&payload, 0, sizeof(payload));
	payload.hue = _hue;
	payload.saturation = _sat;
	payload.brightness = _bright;
	payload.kelvin = _kelvin;
	payload.duration = _duration;

	head.type = 102; // color message

}

MessageBuffer * Color::makeBuffer() {
	int size = sizeof(head) + sizeof(payload);
	MessageBuffer * mb = new MessageBuffer(size);
	memcpy(mb->buf, &head, sizeof(head));
	memcpy(mb->buf + sizeof(head), &payload, sizeof(payload));
	return mb;
}

void Color::printEverything() {
	Header::printEverything();
	cout << "~~~~~ Protocol Header ~~~~~" << endl;
	cout << fcol << "reserved" << tab << "1 byte" << endl;
	cout << fcol << "hue" << tab << payload.hue << endl;
	cout << fcol << "saturation" << tab << payload.saturation << endl;
	cout << fcol << "brightness" << tab << payload.brightness << endl;
	cout << fcol << "kelvin" << tab << payload.kelvin << endl;
	cout << fcol << "duration" << tab << payload.duration << endl;
}