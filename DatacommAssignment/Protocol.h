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
//#define	ENQ   (char)0x05
//#define	ENQP  (char)0x12
//#define ACK   (char)0x06
//#define ACKP  (char)0x11
//#define SOH   (char)0x01
//#define EOT   (char)0x04
//#define SYNC0 (char)0xF1
//#define SYNC1 (char)0xF0
// For testing:
#define	ENQ   (char)101
#define	ENQP  (char)69
#define ACK   (char)97
#define ACKP  (char)65
#define SOH   (char)83
#define EOT   (char)122
#define SYNC0 (char)48
#define SYNC1 (char)49

struct TimeoutData {
	bool loop;
	bool timeout;
	int timeoutDuration;
	DWORD timeSet;

	TimeoutData() {
		stop();
		timeSet = GetTickCount();
	}

	void stop() {
		timeoutDuration = -1;
	}

	void setTimeout(int time) {
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
	char sync;

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
	bool otherPriority;
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
	bool readNextPacket(int timeout, std::string& s);
	void checkmessage();
	void send();

};

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