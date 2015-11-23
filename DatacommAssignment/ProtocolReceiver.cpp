#include "Protocol.h"
#include <string>

using namespace std;

void Protocol::acknowledgeLine() {
	write(priority ? ACKP : ACK);
}

//Event drivet
void Protocol::waitForPacket(string message) {
	BYTE inbuff[100];
	DWORD nBytesRead, dwEvent, dwError;
	COMSTAT cs;

	/* generate event whenever a byte arives */
	SetCommMask(handle, EV_RXCHAR);

	if (WaitCommEvent(handle, &dwEvent, NULL)) {
		ClearCommError(handle, &dwError, &cs);
		if ((dwEvent & EV_RXCHAR) && cs.cbInQue)
			if (ReadFile(handle, inbuff, cs.cbInQue, &nBytesRead, NULL))
				if (nBytesRead == 1 && (inbuff[0] == ENQ || inbuff[0] == ENQP))
					write(priority ? ACKP : ACK);
	}

	PurgeComm(handle, PURGE_RXCLEAR);
}

void Protocol::packetCheck() {

}

void Protocol::acknowledgePacket() {

}

void Protocol::checkPriorityStateReceiver() {

}