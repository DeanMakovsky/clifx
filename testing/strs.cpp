#include <iostream>
#include <string>
#include <string.h>
#include <cstdio>

using namespace std;

#define noop

struct {
	char data[100];
} data;

void work(string ss) {
	// cout << ss.size() << endl;
	memcpy(&data, ss.c_str(), ss.size());

	printf("%s\n", &(data.data));
	// printf("%s\n", ss.c_str());

}

void work(int a) {
	cout << a << endl;

}

void work(char buf[]) {
	cout << "uf" << buf << endl;
}
int main() {
	// work(string("asf"));
	char buf[] = "hel\x00looo";
	cout << string(buf,6) << endl;
	return 0;
}