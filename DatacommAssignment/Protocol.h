#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <vector>
#include <string>
#include <iostream>
#include <Windows.h>

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
#define A (char)65

class Protocol {
public:
	//Public vars
	bool priority;
	bool isAvailable;
	std::vector<std::string> messagesToSend;

	//Public methods
	Protocol();
	~Protocol();
	void sendMessage(std::string message, bool priority);
	void sendMessage(std::iostream filestream, bool priority);

//private:
	//Private vars
	HANDLE handle;

	//Private methods : shared
	void idle();
	void wait();
	void write(std::string message);
	void write(char message);
	template<class UnaryPredicate>
	bool readNextChar(int timeout, char *c, UnaryPredicate predicate);
	std::string readNextPacket(int timeout);

	//Private methods: receiver
	void acknowledgeLine();
	void waitForPacket(std::string message);
	void packetCheck();
	void acknowledgePacket();
	void checkPriorityStateReceiver();

	//Private methods: sender
	void confirmLine();
	void waitForAck();
	void packetizeData();
	void waitForACK();
	void checkPriorityStateSender();

	char test();
};

inline std::string Protocol::readNextPacket(int timeout) {
	char temp;
	std::string packet = "";
	while (temp != EOT && packet.length() < 516) {
		if (readNextChar(10000, &temp, [this](char buffer) {
			return (buffer == ENQ || buffer == ENQP);
		}))
			packet += temp;
	}

	return packet;
}

inline
char Protocol::test(){
	char temp = 'N';
	if (readNextChar(10000, &temp, [this](char buffer) {
		return (buffer == ENQ || buffer == ENQP);
	}))
		return temp;
	return temp;
}


template<class UnaryPredicate>
inline bool Protocol::readNextChar(int timeout, char * c, UnaryPredicate predicate)
{
	char inbuff[1]{ 0 };
	bool loop = true;
	DWORD nBytesRead, dwEvent, dwError;
	COMSTAT cs;
	OVERLAPPED osStatus = { 0 };	
	DWORD timestamp = GetTickCount() + timeout;

	/* generate event whenever a byte arives */
	if (!SetCommMask(handle, EV_RXCHAR)) {
		dwError = GetLastError();
		OutputDebugStringA("Failed to SetCommMask");
		return 0;
	}

	if (WaitCommEvent(handle, &dwEvent, NULL)) {
		if (dwEvent & EV_RXCHAR) {
			if (ReadFile(handle, &inbuff, sizeof(char), &nBytesRead, &osStatus)) {
				if (predicate(inbuff[0])) {
					*c = inbuff[0];
					return true;
				}
				else {
					dwError = GetLastError();
					OutputDebugStringA("Failed BinaryPredicate Test");
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
	return false;
}

#endif //PROTOCOL_H