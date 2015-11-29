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
	if (DEBUG)
		OutputDebugStringA("Acknowledge Packet");
}

void Protocol::checkPriorityStateReceiver() {
	if (DEBUG)
		OutputDebugStringA("Check Priority State (Receiver)");
}
