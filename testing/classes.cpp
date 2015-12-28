#include <iostream>
using namespace std;

class Junk {
	int c,d;
public:
	Junk(int a, int b) : c(a), d(b) {
		cout << "created Junk" << endl;
	}
	void print_me() {
		cout << c << " " << d << endl;
	}
};



class Rectangle {
	int width, height;
public:
	Junk iceJJ;
	Rectangle();
	void print_me();
};

Rectangle::Rectangle() : iceJJ(10, 100) {
	cout << "created Rectangle" << endl;
	Junk iceJJ(100,10);
	width = 10;
	height = 10;
}
void Rectangle::print_me() {
	cout << "hello" << width << height<<endl;
}
#include <bitset>  // for pretty printing odd-sized bit fields



class Test {
public:
	int val;
	Test();
};
Test::Test() {
	val = 0;
}

class LargerTest : public Test {
	char junkspace[100];
public:
	LargerTest();
};
LargerTest::LargerTest() {
	val = 1;
}


#include <cstdio>
int main() {
	// Rectangle r;
	// r.iceJJ.print_me();
	// cout << "helo|" << endl;
	// return 0;
	Test * t = new Test();
	printf("%lu\n", sizeof(*t));

	t = new LargerTest();
	printf("%lu\n", sizeof(*t));

	if (t->val == 1) {
		LargerTest * s = (LargerTest * ) t;
		printf("%lu\n", sizeof(*s));
	}

	// cout << hex << "0x" << bitset<64>().to_ulong() << endl;  // TODO convert this to something usuable
	return 0;
}