#include <iostream>
#include <string>
#include <iomanip> // for cout columns
#include <bitset>  // for pretty printing odd-sized bit fields
#include <map>

#include <cstring>
#include <cstdio>

#include <arpa/inet.h>  // for inet_pton
#include <sys/socket.h>
#include <netinet/in.h>

#include "Messages.h"

// #define tab "\t"
#define fcol setw(15)
#define tab setw(10)

using namespace std;


map<int, Header *(*)(char *)> mapMaker() {
	map<int, Header *(*)(char *)> typeToCon;
	typeToCon[2] = (Header *(*)(char *)) GetServiceFac;
	typeToCon[3] = (Header *(*)(char *)) StateServiceFac;
	typeToCon[2] = (Header *(*)(char *)) GetHostInfoFac;
	typeToCon[3] = (Header *(*)(char *)) StateHostInfoFac;
	typeToCon[4] = (Header *(*)(char *)) GetHostFirmwareFac;
	typeToCon[5] = (Header *(*)(char *)) StateHostFirmwareFac;
	typeToCon[16] = (Header *(*)(char *)) GetWifiInfoFac;
	typeToCon[17] = (Header *(*)(char *)) StateWifiInfoFac;
	typeToCon[18] = (Header *(*)(char *)) GetWifiFirmwareFac;
	typeToCon[19] = (Header *(*)(char *)) StateWifiFirmwareFac;
	typeToCon[20] = (Header *(*)(char *)) GetPowerFac;
	typeToCon[21] = (Header *(*)(char *)) SetPowerFac;
	typeToCon[22] = (Header *(*)(char *)) StatePowerFac;
	typeToCon[23] = (Header *(*)(char *)) GetLabelFac;
	typeToCon[24] = (Header *(*)(char *)) SetLabelFac;
	typeToCon[25] = (Header *(*)(char *)) StateLabelFac;
	typeToCon[32] = (Header *(*)(char *)) GetVersionFac;
	typeToCon[33] = (Header *(*)(char *)) StateVersionFac;
	typeToCon[34] = (Header *(*)(char *)) GetInfoFac;
	typeToCon[35] = (Header *(*)(char *)) StateInfoFac;
	typeToCon[45] = (Header *(*)(char *)) AcknowledgementFac;
	typeToCon[48] = (Header *(*)(char *)) GetLocationFac;
	typeToCon[50] = (Header *(*)(char *)) StateLocationFac;
	typeToCon[51] = (Header *(*)(char *)) GetGroupFac;
	typeToCon[53] = (Header *(*)(char *)) StateGroupFac;
	typeToCon[58] = (Header *(*)(char *)) EchoRequestFac;
	typeToCon[59] = (Header *(*)(char *)) EchoResponseFac;
	typeToCon[101] = (Header *(*)(char *)) GetFac;
	typeToCon[102] = (Header *(*)(char *)) SetColorFac;
	typeToCon[107] = (Header *(*)(char *)) StateFac;
	typeToCon[116] = (Header *(*)(char *)) GetPower_LightFac;
	typeToCon[117] = (Header *(*)(char *)) SetPower_LightFac;
	typeToCon[118] = (Header *(*)(char *)) StatePower_LightFac;
	return typeToCon;
}

map<int, Header *(*)(char *)> typeToCon = mapMaker();


MessageBuffer::MessageBuffer(char * _buf, int _size) {
	buf = (char *) malloc(_size);
	size = _size;
	memcpy(buf, _buf, size);
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

}

/**
* Assumes buffer has enough good data for sizeof(Header.head)
*/
Header::Header(char * buffer) {
	memcpy(&head, buffer, sizeof(head) );
}

Header::~Header() {
}

int Header::getType() {
	return head.type;
}

void Header::setTarget(string tar) {
	head.tagged = 0;
	memcpy(head.target, tar.c_str(), 6);
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
		if (val == EAGAIN || val == EWOULDBLOCK) {
			// printf("No data to read.\n");
			Header  * h = new Header();
			h->head.type = 0;
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
	if (ret > sizeof(buffer)) {
		printf("Read too many bytes for buffer, truncating message.\n");
		ret = sizeof(buffer);
	}
	if (ret < sizeof(Header::head)) {
		printf("Not enough bytes to fill a Header object.\n");
	}

	// make and return new object
	Header * h = new Header(buffer);
	if (h->head.protocol != 1024) {
		printf("Incorrect protocol number, probably bad packet.\n");
	}
	// printf("Size: %d\nProtocol: %d\nType: %d\n", h.size, h.protocol, h.type);
	// h.printEverything();


	map<int, Header *(*)(char *)>::iterator it = typeToCon.find(h->head.type);
	if (it == typeToCon.end()) {
		printf("No constructor found for message type: %d\n", h->head.type);
		return h;
	}

	h = (it->second)(buffer);

	return h;
}

MessageBuffer * Header::makeBuffer() {
	char buf[sizeof(Header::head)];
	int size = sizeof(head);
	memcpy(buf, &head, sizeof(head));
	MessageBuffer * mb = new MessageBuffer(buf, size);
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

