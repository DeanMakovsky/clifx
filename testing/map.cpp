#include <iostream>
#include <map>
#include <string>

using namespace std;

class Super {
public:
	int c;

};
class Test : public Super {
public:
	Test() {};
	int a;
	int b;
};

class Quiz : public Super{
public:
	Quiz() {};
	int a;
	int b;
};
// consider using factories
Quiz * qfac() {
	return new Quiz();
}
Test * tfac() {
	return new Test();
}
int main() {
	cout << "hello" << endl;

	map<string, Super *(*)()> mymap;
	mymap["one"] = (Super *(*)()) qfac;
	mymap["two"] = (Super *(*)())tfac;
	Super * t = mymap["two"]();
	cout << t->c << endl;
	// cout << mymap["one"] << endl;
	// cout << mymap["two"] << endl;

	return 0;
}