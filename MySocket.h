#ifndef MY_SOCKET
#define MY_SOCKET

class MySocket {
	int fd;
public:
	MySocket();
	void send(void *, int);
	int getSocket() { return fd; }
};

#endif