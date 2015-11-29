#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <vector>
#include <string>
#include <iostream>
#include <Windows.h>

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
#define SYNC0 (char)0x0F
#define SYNC1 (char)0xF0
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
	private:
		//Private vars
		HANDLE timeoutThread;
		DWORD timeoutThreadId;
		HANDLE protocolThread;
		DWORD protocolThreadId;

		//Private methods : shared
		void wait();
		void write(std::string message);
		void write(char message);

		//Private methods: receiver
		void acknowledgeLine();
		void waitForPacket();
		void packetCheck(std::string packet);
		void acknowledgePacket(std::string packet);
		void checkPriorityStateReceiver();

		//Private methods: sender
		void confirmLine();
		void waitForAck(char signal);
		void sendData(char signal);
		void packetizeData(std::string message);
		void waitForACK(char signal);
		void checkPriorityStateSender(char signal);

	public:
		//Public vars
		bool priority;
		bool isAvailable;
		int syncBit = SYNC0;
		int counter = 0;
		std::string msg;
		std::vector<std::string> messagesToSend;
		std::string packet;
		//Must be public, accessed by Timeout.h
		HANDLE handle;

		//Public methods
		void idle();
		Protocol();
		~Protocol();
		void connect();
		void disconnect();
		void sendMessage(std::string message, bool priority);
		void sendMessage(std::iostream filestream, bool priority);
		//The function that creates a thread MUST be public because it
		//has to be called statically
		VOID initialize_Send(void);
		DWORD WINAPI SendThreadFunc(void);

		TimeoutData timeoutStatus;

	//Private methods : shared
	bool validatePacket(std::string packet);
	std::string packetizePacket(std::string packet);
	template<class UnaryPredicate>
	bool readNextChar(int timeout, char *c, UnaryPredicate predicate);
	std::string readNextPacket(int timeout);
	void checkmessage();
	void send();

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

//temporal method to check message to send
inline void Protocol::checkmessage() {
	char buf[20];
	int j;
	if (messagesToSend.size() > 0) {
		for (int i = 0; i < messagesToSend.size(); i++){
			//for(auto a : messagesToSend) {
			OutputDebugString("\n\n[packet]\n\n");
			
			j = sprintf_s(buf, "[ %d ] size:\t%zd\n",i, messagesToSend[i].length());
			
			if (messagesToSend[i].length() > 0) {
				OutputDebugString(buf);
				OutputDebugString("START overLength\n");
				OutputDebugString(messagesToSend[i].c_str());
				
	//			OutputDebugString("\TLOL\n\n");
			}
			else
				OutputDebugString("EMPTY\n");
		}
	}
	else
		OutputDebugString("no message");


	
}
//temporal method to check send
inline void Protocol::send() {
	while(messagesToSend.size() != 0)
		sendData(ACK);
}
#endif //PROTOCOL_H