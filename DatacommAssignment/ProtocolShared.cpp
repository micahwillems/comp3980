#include <iostream>
#include <string>
#include <windows.h>
#include <stdio.h>
#include "Protocol.h"
#include "Timeout.h"
#include <thread>

using namespace std;

DWORD WINAPI startProtocol(LPVOID lpParameter) {
	Protocol &p = *((Protocol *)lpParameter);
	p.idle();
	return 0;
}

//Carson
Protocol::Protocol() {}

Protocol::~Protocol() {
	disconnect();
}

void Protocol::connect() {
	if ((handle = CreateFile(TEXT("COM1"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL)) == INVALID_HANDLE_VALUE) {
		DWORD dwError = GetLastError();
		OutputDebugStringA("Error creating serial port handle");
	}




	timeoutThread = CreateThread(NULL, 0, startTimer, this, 0, &timeoutThreadId);
	if (timeoutThread == NULL && DEBUG)
		OutputDebugStringA("Error creating timeout thread");

	protocolThread = CreateThread(NULL, 0, startProtocol, this, 0, &protocolThreadId);
	if (protocolThread == NULL && DEBUG)
		OutputDebugStringA("Error creating protocol thread");

}

void Protocol::disconnect() {
	TerminateThread(timeoutThread, 0);
	TerminateThread(protocolThread, 0);
	if (&handle != INVALID_HANDLE_VALUE)
		CloseHandle(handle);
	OutputDebugStringA("Deconstructed");
}

//Carson
void Protocol::sendMessage(string message, bool priority = false) {
	//Figure out why it wont append EOT
	message = message.append("" + SOH);
	string temp;
	char tmp[30];
	size_t i = 0;
	OutputDebugString("MESSAGE SEND \n");
	priority = priority;
	while (i < message.length()) {
		if (message.length() - i >= 512) {
			messagesToSend.push_back(message.substr(i, 512));

		}
		else {
			messagesToSend.push_back(message.substr(i, message.length() - i));
		}
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
	char received = 'a';
	while (1) {
		if (DEBUG)
			OutputDebugStringA("Idle");

		if (messagesToSend.size() > 0) {
			//enq
			//pop
			//GOTO confirmLine
		}

		//GOTO AcknowledgeLine
		if (readNextChar(1, &received, [](char c) {return (c == ENQ || c == ENQP || c == A); }))
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

/*template<class UnaryPredicate>
bool Protocol::readNextChar(int timeout, char * c, UnaryPredicate predicate) {
	char inbuff[1] { 0 };
	bool loop = true;
	DWORD nBytesRead, dwEvent, dwError;
	OVERLAPPED osStatus = { 0 };
	DWORD timestamp = GetTickCount() + timeout;

	// generate event whenever a byte arives
	if (!SetCommMask(handle, EV_RXCHAR)) {
		dwError = GetLastError();
		OutputDebugStringA("Failed to SetCommMask");
		return false;
	}
	timeoutStatus.setTimeout(timeout);

	ListenForEvent:
	if (!WaitCommEvent(handle, &dwEvent, NULL))
		return false;
	
	if (!(dwEvent & EV_RXCHAR))
		return false;

	if (ReadFile(handle, &inbuff, sizeof(char), &nBytesRead, &osStatus)) {
		if (predicate(inbuff[0])) {
			if (DEBUG)
				OutputDebugStringA("Match");
			*c = inbuff[0];
			return true;
		} else {
			dwError = GetLastError();
			OutputDebugStringA("Failed UnaryPredicate Test");
			goto ListenForEvent;
		}
	}

	timeoutStatus.stop();
	return false;
}*/

string Protocol::readNextPacket(int timeout) {
	string packet = "";
	char inbuff[516]{ 0 };
	bool loop = true;
	DWORD nBytesRead, dwEvent, dwError;
	OVERLAPPED osStatus = { 0 };
	DWORD timestamp = GetTickCount() + timeout;

	/* generate event whenever a byte arives */
	if (!SetCommMask(handle, EV_RXCHAR)) {
		dwError = GetLastError();
		OutputDebugStringA("Failed to SetCommMask");
		return false;
	}

	timeoutStatus.setTimeout(timeout);

	if (!WaitCommEvent(handle, &dwEvent, NULL))
		return false;

	if (!(dwEvent & EV_RXCHAR))
		return false;

	ReadFile(handle, &inbuff, sizeof(char) * 516, &nBytesRead, &osStatus);

	timeoutStatus.stop();

	for (char c : inbuff) {
		packet = packet.append(&c);
	}

	return packet;
}