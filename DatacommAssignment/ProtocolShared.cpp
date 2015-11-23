#include <iostream>
#include <string>
#include <windows.h>
#include <stdio.h>
#include "Protocol.h"

using namespace std;

//Carson
Protocol::Protocol() {
	handle = CreateFile(NULL, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
}

Protocol::~Protocol() {

}

//Carson
void Protocol::sendMessage(string message, bool priority = false) {
	//Figure out why it wont append EOT
	message.append("" + SOH);
	size_t i = 0;
	priority = priority;
	cout << message << endl;
	while (i < message.length()) {
		if (message.length() - i <= 512)
			messagesToSend.push_back(message.substr(i, 512));
		else
			messagesToSend.push_back(message.substr(i, message.length() - i));
		i += 512;
	}
}


void Protocol::sendMessage(iostream filestream, bool priority = false) {

}

//Carson
void Protocol::write(string message) {
	unsigned char buffer[516];
	size_t i;
	for (i = 0; i < message.length() || i < 516; i++) {
		buffer[i] = message[i];
	}
	WriteFile(handle, buffer, i, 0, &OVERLAPPED());
}

//Carson
void Protocol::write(char message) {
	unsigned char buffer[1] = { message };
	WriteFile(handle, buffer, 1, 0, &OVERLAPPED());
}

void Protocol::idle() {

}

void Protocol::wait() {

}