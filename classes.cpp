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
int main() {
	Rectangle r;
	r.iceJJ.print_me();
	cout << "helo|" << endl;
	return 0;
}