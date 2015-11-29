#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <vector>
#include <string>
#include <iostream>
#include <Windows.h>
#include <atomic>

//Debug
#define DEBUG true

//Timeouts
#define GLOBAL_TIMEOUT 400
#define MINI_TIMEOUT 250
#define S1_TIMEOUT 300

//Special Characters
#define	ENQ  (char)5
#define	ENQP (char)18
#define ACK  (char)6
#define ACKP (char)17
#define SOH  (char)1
#define EOT  (char)4
#define A    (char)65

struct TimeoutData {
	bool loop;
	bool timeout;
	int timeoutDuration;
	DWORD timeSet;

	TimeoutData() {
		timeout = false;
		timeoutDuration = -1;
		timeSet = GetTickCount();
		loop = false;
	}

	void stop() {
		loop = false;
		timeoutDuration = -1;
		timeout = false;
	}

	void setTimeout(int time) {
		if (DEBUG)
			std::cout << "Setting Timeout: " << time << "ms" << std::endl;
		loop = true;
		timeout = false;
		timeoutDuration = time;
		timeSet = GetTickCount();
	}
};

class Protocol {
public:
	//Public vars
	bool priority;
	bool isAvailable;
	std::vector<std::string> messagesToSend;
	HANDLE handle;
	TimeoutData timeoutStatus;

	//Public methods
	Protocol();
	~Protocol();
	void sendMessage(std::string message, bool priority);
	void sendMessage(std::iostream filestream, bool priority);

//private:
	//Private vars
	HANDLE timeoutThread;
	DWORD timeoutThreadId;

	//Private methods : shared
	void idle();
	void wait();
	void write(std::string message);
	void write(char message);
	bool validatePacket(std::string packet);
	std::string packetizePacket(std::string packet);
	template<class UnaryPredicate>
	bool readNextChar(int timeout, char *c, UnaryPredicate predicate);
	std::string readNextPacket(int timeout);

	//Private methods: receiver
	void acknowledgeLine();
	void waitForPacket();
	void packetCheck(std::string packet);
	void acknowledgePacket(std::string packet);
	void checkPriorityStateReceiver();

	//Private methods: sender
	void confirmLine();
	void waitForAck();
	void packetizeData();
	void waitForACK();
	void checkPriorityStateSender();

	char test();
};

template<class UnaryPredicate>
inline bool Protocol::readNextChar(int timeout, char * c, UnaryPredicate predicate)
{
	char inbuff[1]{ 0 };
	bool loop = true;
	DWORD nBytesRead, dwEvent, dwError;
	OVERLAPPED osStatus = { 0 };	
	DWORD timestamp = GetTickCount() + timeout;

	/* generate event whenever a byte arives */
	if (!SetCommMask(handle, EV_RXCHAR)) {
		dwError = GetLastError();
		OutputDebugStringA("Failed to SetCommMask");
		return 0;
	}
	timeoutStatus.setTimeout(timeout);

	ListenForEvent:
	if (WaitCommEvent(handle, &dwEvent, NULL)) {
		if (dwEvent & EV_RXCHAR) {
			if (ReadFile(handle, &inbuff, sizeof(char), &nBytesRead, &osStatus)) {
				if (predicate(inbuff[0])) {
					*c = inbuff[0];
					return true;
				}
				else {
					dwError = GetLastError();
					OutputDebugStringA("Failed UnaryPredicate Test");
					goto ListenForEvent;
				}
			}
			else {
				dwError = GetLastError();
				OutputDebugStringA("Failed to ReadFile");
			}
		}
		else {
			dwError = GetLastError();
			OutputDebugStringA("Failed to dwEvent & RX_CHAR");
		}
	}
	else {
		dwError = GetLastError();
		OutputDebugStringA("Failed to WaitCommEvent");
	}

	if (DEBUG)
		if (timeoutStatus.timeout) {
			OutputDebugStringA("Timeout");
		} else
			OutputDebugStringA("Error in Protocol reading character");
	timeoutStatus.stop();
	return false;
}

#endif //PROTOCOL_H