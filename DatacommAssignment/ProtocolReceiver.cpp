#include "Protocol.h"
#include <string>
#include <iostream>

using namespace std;

void Protocol::acknowledgeLine() {
	if (DEBUG)
		OutputDebugStringA("\nAcknowledge Line");

	//Acknowledge to the sender that you received their ENQ/ENQP
	//and are willing to receive their packet.
	write(priority ? ACKP : ACK);

	//Wait for the packet to arrive.
	waitForPacket();
}

void Protocol::waitForPacket() {
	if (DEBUG)
		OutputDebugStringA("\nWait For Packet");
	string packet(516, '\0');
	if (readNextPacket(S1_TIMEOUT, packet)) {
		//If it did not timeout and succesfully read a packet

		if (packet.size() >= 1 && (packet[0] == ENQ || packet[0] == ENQP)) {
			//If we received a ENQ or ENQP again, the sender must not have received
			//our ACK/ACKP, so go back to that state and resend the acknowledgement.
			acknowledgeLine();
		}
		else {
			//We received a packet, check if it is valid.
			packetCheck(packet);
		}
	} else {
		//If it timed out
		checkPriorityStateReceiver();
	}
}

void Protocol::packetCheck(string packet) {
	if (DEBUG)
		OutputDebugStringA("\nPacket Check");

	if (validatePacket(packet)) {
		OutputDebugStringA("\nValid Packet");
		//Check if the packet is valid. If so, hand it to acknowledgePacket for handling.
		acknowledgePacket(packet);
	} else {
		//Otherwise, we need to let the sender timeout and resend us the packet.
		OutputDebugStringA("\nInvalid Packet");
		waitForPacket();
	}
}

void Protocol::acknowledgePacket(string packet) {
	//Hand to network layer
	if (DEBUG)
		OutputDebugStringA("\nAcknowledge Packet");

	//Sends to sender that we did receive the packet successfully and it was valid.
	write(priority ? ACKP : ACK);

	//Wait for sender in case they resend the packet. If they did, they never received our 
	//acknowledgement.
	waitForPacket();
}

void Protocol::checkPriorityStateReceiver() {
	if (DEBUG)
		OutputDebugStringA("\nCheck Priority State (Receiver)");
	//Receiver wins all ties to insure fairness. If we have priority or the other side doesent, we skip wait.
	if (priority || !otherPriority)
		idle();

	//If receiver does not want priority and the sender does, we receiver goes to wait to allow sender to bid first.
	if (!priority && otherPriority)
		wait();
}
