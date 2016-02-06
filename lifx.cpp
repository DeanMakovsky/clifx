#include <iostream>
#include <thread>
#include <map>
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
* check that the Get/Set/State power Light messages are the same content as the Device messages

Considerations:
* Make MySocket non-blocking optionally, my passing in a bool.
* Improve gen_code.py:
	* Take in a file that conforms to a "template" language
	* Add support for custom methods for each class--would solve the need to structs vs classes
*/

map<string, time_t> all_bulbs;
map<string, time_t> ignore_list;

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

string string_to_hex(const string& input) { // from SO
    static const char* const lut = "0123456789ABCDEF";
    size_t len = input.length();

    string output;
    output.reserve(2 * len);
    for (size_t i = 0; i < len; ++i)
    {
        const unsigned char c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
    }
    return output;
}
void printMap(map<string, time_t> * m) {
	for( map<string, time_t>::iterator it = m->begin(); 
		it != m->end();
		++it) {
		cout << "0x" << string_to_hex(it->first) << "\t-> " << it->second << endl;
	}
}

void pruneMap(map<string, time_t> * m) {
	time_t now = time(0);
	for( map<string, time_t>::iterator it = m->begin(); 
		it != m->end();
		++it) {

		if (it->second < now - 50) {
			// TODO remove, might have to use it.prev
			m->erase(it);
		}
	}
}
bool checkIgnore(map<string, time_t> * m, string s ) {
	for( map<string, time_t>::iterator it = m->begin(); it != m->end(); ++it) {
		if (it->first == s) {
			return true;
		}
	}
	return false;
}
void handleMessage(Header * h) {
	
	int type = h->getType();

	if (type == 3) {
		StateService * actual = (StateService *) h;
		// h->printEverything();
		// unsigned int service = actual->payload.service;
		int port = actual->payload.port;
		string target((char *) &(actual->head.target), 6);

		// cout << "length: " << target.size() << "  0x" << string_to_hex(target) << endl;

		// printf("Service: %hhu\n", service);
		// printf("Port: %u\n", port);

		if (port != 56700) {
			printf("Port was not an expected value (%d), not adding it.\n", port);
			return;
		}


		// ensure MAC is filled
		int isZero = 1;
		for (int i = 0; i < 6; i++) {
			if((actual->head.target)[i] != '\0') {
				isZero = 0;
				break;
			}
		}
		if( isZero ) {
			printf("Bulb did not give MAC address\n");
			return;
		}

		// update known bulbs
		all_bulbs[target] = time(0);
		printMap(&all_bulbs);
		// actual->printEverything();
	} else if (type == 25) {
		// printf("Other message: %d\n", type);
		h->printEverything();
	}
}



int main(int argc, char ** argv) {
	ignore_list[string("\xd0\x73\xd5\x11\xdf\xe7")] = 0;  // d0:73:d5 is a common prefix / 0 is a placeholder value
	ignore_list[string("\xd0\x73\xd5\x10\x72\x58")] = 0;  // d0:73:d5 is a common prefix / 0 is a placeholder value
	ignore_list[string("\xd0\x73\xd5\x10\x73\x82")] = 0;  // d0:73:d5 is a common prefix / 0 is a placeholder value

	// test string equality
	// char temp[7];
	// char temp2[7];

	// memset(temp, 0, 7);
	// memset(temp2, 0, 7);
	// string test(temp, 6);
	// string test2("\x00\x00\x00\x00\x00");

	// cout << (test == test2) << endl;



	MySocket sock;
	int messagesRead = 0;
	int iterations = 0;
	// for ( int nums = 0; nums < 3; nums++ ) {
	// 	SetColor c(randy(0,65535),65535,4*4096,randy(2500,9000),10);
	// 	c.setTarget("\xd0\x73\xd5\x11\xdf\xe7");
	// 	MessageBuffer * b = c.makeBuffer();
	// 	sock.send(b->buf, b->size);
	// 	delete b;
	// 	this_thread::sleep_for (chrono::seconds(1));
	// }

	// return 0;

	while (true) {

		// read any outstanding messages
		while (true) { 
			Header * thing = Header::deserialize(sock.getSocket());
			if (thing->getType() != 0) {
				messagesRead += 1;
				printf("Messages Read: %d\n", messagesRead);
				// printf("Type: %d\n", thing->getType());
				handleMessage(thing);
				// thing->printEverything();
			} else {
				delete thing;
				break;
			}
			delete thing;
		}

		// discover stuff every 10s
		if (iterations % 10 == 0) {
			GetService g;
			MessageBuffer * b = g.makeBuffer();
			sock.send(b->buf, b->size);

			GetLabel l;
			b = l.makeBuffer();
			sock.send(b->buf, b->size);

		}

		// prune known light bulbs
		if (iterations % 30 == 2) {
			pruneMap(&all_bulbs);
		}

		if (iterations % 5 == 3) {
			// individual random colors
			for (map<string, time_t>::iterator it = all_bulbs.begin(); it != all_bulbs.end(); it++) {
				if (checkIgnore(&ignore_list, it->first)) {
					continue;
				}
				SetColor c(randy(0,65535),65535,5*4096,randy(2500,9000),3000);
				c.setTarget(it->first);
				MessageBuffer * b = c.makeBuffer();
				sock.send(b->buf, b->size);
				printf("Sent color.\n");
				delete b;
			}
		}
		// printf("->Start to sleep.\n");
		this_thread::sleep_for (chrono::seconds(1));
		iterations += 1;
		if (iterations >= 60) { // a highly divisible number
			iterations = 0;
		}
		// printf("<-End sleep.\n");
	}
	return 0;


	// make random colors!
	// for ( ;; ) {
	// 	SetColor c(randy(0,65535),65535,4*4096,randy(2500,9000),10);
	// 	MessageBuffer * b = c.makeBuffer();
	// 	sock.send(b->buf, b->size);
	// 	delete b;
	// 	this_thread::sleep_for (chrono::seconds(1));
	// }

	// return 0;
}