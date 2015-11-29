#include <iostream>
#include <string>
#include <windows.h>
#include <stdio.h>
#include "Protocol.h"
#include "Timeout.h"
#include <thread>

using namespace std;

//Carson
Protocol::Protocol() {
	if ((handle = CreateFile(TEXT("COM1"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL)) == INVALID_HANDLE_VALUE) {
		DWORD dwError = GetLastError();
	}
	OutputDebugStringA("Created File");
	timeoutThread = CreateThread(NULL, 0, startTimer, this, 0, &timeoutThreadId);
	OutputDebugStringA("Created Timeout Thread");
	//thread timeoutThread(&startTimer, this);
	//timeoutThread.detach();
	if (timeoutThread == NULL && DEBUG)
		OutputDebugStringA("Error creating timeout thread");
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
	DWORD i;
	for (i = 0; i < message.length() || i < 516; i++) {
		buffer[i] = message[i];
	}
	WriteFile(handle, buffer, i, 0, &OVERLAPPED());
}

//Carson
void Protocol::write(char message) {
	char buffer[1] = { message };
	WriteFile(handle, buffer, 1, 0, &OVERLAPPED());
}

void Protocol::idle() {
	char received;
	while (1) {
		if (messagesToSend.size() > 0) {
			//GOTO confirmLine
		}

		//GOTO AcknowledgeLine
		if (readNextChar(3, &received, [](char c) {return c == ENQ || c == ENQP; }))
			acknowledgeLine();
	}
}

void Protocol::wait() {

}

bool Protocol::validatePacket(string packet) {
	return true;
}

string Protocol::packetizePacket(string packet) {
	return packet;
}