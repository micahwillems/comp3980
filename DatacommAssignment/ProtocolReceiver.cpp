#include "Protocol.h"
#include <string>
#include <iostream>

using namespace std;

void Protocol::acknowledgeLine() {
	write(priority ? ACKP : ACK);

	//GOTO: WaitForPacket
	waitForPacket();
}

void Protocol::waitForPacket() {
	string packet = readNextPacket(MINI_TIMEOUT);
	//GOTO ValidatePacket
	packetCheck(packet);
}

void Protocol::packetCheck(string packet) {
	if (validatePacket(packet))
		acknowledgePacket(packet);
	else
		waitForPacket();
}

void Protocol::acknowledgePacket(string packet) {

}

void Protocol::checkPriorityStateReceiver() {

}

char Protocol::test() {
	char temp = 'N';
	if (readNextChar(10000, &temp, [this](char buffer) {
		return (buffer == ENQ || buffer == ENQP || buffer == A);
	}))
		return temp;
	return temp;
}

string Protocol::readNextPacket(int timeout) {
	char newchar = '\0';
	string packet = "";
	while (newchar != EOT && packet.length() < 516) {
		if (readNextChar(10, &newchar, [](char c) {return true; }))
			packet += newchar;
	}
	OutputDebugStringA("Received Message");
	return packet;
}