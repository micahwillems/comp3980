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
//#define	ENQ   (char)0x05
//#define	ENQP  (char)0x12
//#define ACK   (char)0x06
//#define ACKP  (char)0x11
//#define SOH   (char)0x01
//#define EOT   (char)0x04
//#define SYNC0 (char)0xF1
//#define SYNC1 (char)0xF0
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
public:
	//Public vars
	bool priority;
	bool otherPriority;
	bool isAvailable;
	std::vector<std::string> messagesToSend;
	HANDLE handle;
	TimeoutData timeoutStatus;

	//Public methods
	Protocol();
	~Protocol();
	void connect();
	void disconnect();
	void sendMessage(std::string message, bool priority);
	void sendMessage(std::iostream filestream, bool priority);
	void idle();

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
	bool validatePacket(std::string packet);
	std::string packetizePacket(std::string packet);
	template<class UnaryPredicate>
	bool readNextChar(int timeout, char * c, UnaryPredicate predicate);
	bool readNextPacket(int timeout, std::string& s);

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
};

#endif //PROTOCOL_H