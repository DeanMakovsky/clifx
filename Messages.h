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

// ~~~ Subclasses ~~~

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
	MessageBuffer * makeBuffer();
	void printEverything();
};


class GetService: public Header { // no payload
public: 
	GetService(); // TODO make sure that if the GetService(char *) is used, then only the parent class is called
};


class StateService: public Header {
	struct {
		uint8_t  service:8;
		uint32_t port:32;
	} payload;
public:
	StateService();
	StateService(char *);
	void printEverything();
};


class GetHostInfo: public Header { // no payload
public: 
	GetHostInfo(); 
};


class StateHostInfo: public Header {
	struct {
		float    signal;
		uint32_t tx;
		uint32_t rx;
		int16_t  reserved;
	} payload;
public:
	StateHostInfo();
	StateHostInfo(char *);
	void printEverything();
};






class GetHostFirmware: public Header {
public:
	GetHostFirmware();
};


class StateHostFirmware: public Header {
	struct {
		uint64_t build;
		
	} payload;
public:
	StateHostFirmware();
	StateHostFirmware(char *);
	void printEverything();
};


class GetWifiInfo: public Header {
public:
	GetWifiInfo();
};


class StateWifiInfo: public Header {
	struct {
		
	} payload;
public:
	StateWifiInfo();
	StateWifiInfo(char *);
	void printEverything();
};


class GetWifiFirmware: public Header {
public:
	GetWifiFirmware();
};


class StateWifiFirmware: public Header {
	struct {
		
	} payload;
public:
	StateWifiFirmware();
	StateWifiFirmware(char *);
	void printEverything();
};


class GetPower: public Header {
public:
	GetPower();
};


class SetPower: public Header {
	struct {
		
	} payload;
public:
	SetPower();
	SetPower(char *);
	void printEverything();
};


class StatePower: public Header {
	struct {
		
	} payload;
public:
	StatePower();
	StatePower(char *);
	void printEverything();
};


class GetLabel: public Header {
public:
	GetLabel();
};


class SetLabel: public Header {
	struct {
		
	} payload;
public:
	SetLabel();
	SetLabel(char *);
	void printEverything();
};


class StateLabel: public Header {
	struct {
		
	} payload;
public:
	StateLabel();
	StateLabel(char *);
	void printEverything();
};


class GetVersion: public Header {
public:
	GetVersion();
};


class StateVersion: public Header {
	struct {
		
	} payload;
public:
	StateVersion();
	StateVersion(char *);
	void printEverything();
};


class GetInfo: public Header {
public:
	GetInfo();
};


class StateInfo: public Header {
	struct {
		
	} payload;
public:
	StateInfo();
	StateInfo(char *);
	void printEverything();
};


class Acknowledgement: public Header {
public:
	Acknowledgement();
};


class GetLocation: public Header {
public:
	GetLocation();
};


class StateLocation: public Header {
	struct {
		
	} payload;
public:
	StateLocation();
	StateLocation(char *);
	void printEverything();
};


class GetGroup: public Header {
public:
	GetGroup();
};


class StateGroup: public Header {
	struct {
		
	} payload;
public:
	StateGroup();
	StateGroup(char *);
	void printEverything();
};


class EchoRequest: public Header {
	struct {
		
	} payload;
public:
	EchoRequest();
	EchoRequest(char *);
	void printEverything();
};


class EchoResponse: public Header {
	struct {
		
	} payload;
public:
	EchoResponse();
	EchoResponse(char *);
	void printEverything();
};


class Get: public Header {
public:
	Get();
};


class SetColor: public Header {
	struct {
		
	} payload;
public:
	SetColor();
	SetColor(char *);
	void printEverything();
};


class State: public Header {
	struct {
		
	} payload;
public:
	State();
	State(char *);
	void printEverything();
};

/*
class GetPower_Light: public Header {
public:
	GetPower_Light();
};


class SetPower_Light: public Header {
	struct {
		
	} payload;
public:
	SetPower_Light();
	SetPower_Light(char *);
	void printEverything();
};


class StatePower_Light: public Header {
	struct {
		
	} payload;
public:
	StatePower_Light();
	StatePower_Light(char *);
	void printEverything();
};

*/



#pragma pack(pop)

#endif