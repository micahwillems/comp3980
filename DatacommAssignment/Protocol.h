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

class Protocol {
	private:
		//Private vars
		HANDLE handle;

		//Private methods : shared
		void idle();
		void wait();
		void write(std::string message);
		void write(char message);

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
};

#endif //PROTOCOL_H