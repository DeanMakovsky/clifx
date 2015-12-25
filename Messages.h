#ifndef MESS_AGES
#define MESS_AGES


class MessageBuffer {
public:
	char * buf;
	size_t size;
	MessageBuffer(int);
	~MessageBuffer();
};


#pragma pack(push, 1)

class Header {
protected:
	struct {
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
	} head;
public:
	Header();
	Header(char *);
	int getType();
	static Header deserialize(int);
	virtual MessageBuffer * makeBuffer();
	virtual void printEverything();
};



class Color: public Header {
protected:
	struct {
		uint8_t  :8;
		uint16_t hue:16;
		uint16_t saturation:16;
		uint16_t brightness:16;
		uint16_t kelvin:16;
		uint32_t duration:32;
	} payload;
public:
	Color(unsigned int,unsigned int,unsigned int,unsigned int,unsigned int);
	void print();
	MessageBuffer * makeBuffer();
	void printEverything();
};




#pragma pack(pop)

#endif