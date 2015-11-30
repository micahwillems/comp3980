#include "Protocol.h"
#include <string.h>
#include "checksum.h"
using namespace std;

//for send thread
HANDLE senderThread;
DWORD dwWriteId;
BOOL fWaitingOnSend = FALSE;
OVERLAPPED osWrite = { 0 };

//MESSAGE HANDLING read
DWORD WINAPI Protocol::SendThreadFunc(void) {
	BYTE buf[516];
	DWORD nBytesRead, dwEvent, dwError, dwRead;
	OVERLAPPED os = { 0 };
	COMSTAT cs;
	TCHAR signalMessage;
	DWORD error;
	char str;
	DWORD		numOfByteSignal;
	DWORD		dwSignalRes;

	if ((os.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)) != NULL) {
		OutputDebugString("CREATE Event Error\n");
		return 0;
	}
	if (!SetCommMask(handle, EV_RXCHAR)) {
		OutputDebugString("Set Comm Mask Error\n");
		return 0;
	}
	//wait until port get sth to read
	if (!WaitCommEvent(handle, &dwEvent, NULL)) {
		error = GetLastError();
		if (error != ERROR_IO_PENDING)
			OutputDebugString("WaitCommEvent ERROR");

	}
	if ((dwEvent && EV_RXCHAR) == EV_RXCHAR) { //new char arrived
											   //dwRead = hComm.ReadComm(buf, sizeof(buf));
		signalMessage = dwEvent;
	}
	return 0;
}

DWORD WINAPI WrappedThreadFunc(LPVOID arg) {
	if (!arg)
		return 0;
	Protocol *protPtr = (Protocol*)arg;
	protPtr->SendThreadFunc();
	return 1;
}

VOID Protocol::initialize_Send(void) {
	senderThread = CreateThread(NULL, 0, WrappedThreadFunc, this, 0, &dwWriteId);

}

//TT1 : Confirm Line
void Protocol::confirmLine() {
	if (counter > 4)
		wait();
	write(priority ? ENQ : ENQP);
}

//TR1 wait for ack1
/*void Protocol::waitForAckT1(char signal) {

	OutputDebugString("WaitForAck");
	if (signal == ACK || signal == ACKP) {
		counter = 0;
		sendData(signal);
	}
	else if (STATUS_TIMEOUT) { //not sure find timeout
		counter++;
		//confirmLine();
	}
}

//TR2 : wait for ack
void Protocol::waitForAckT2(char signal) {
	if (signal == ACK || signal == ACKP) {
		//timeout
		checkPriorityStateSender(signal);
	}
	else if (STATUS_TIMEOUT) { // timeout..?!?!?
		counter++;
		sendData(signal);
	}
}*/

//TT2 Tx Data 
//EUNWON
void Protocol::sendData() {
	OutputDebugString("sendData\n");
	string message;
	
	//if (counter > 4) {
	//	checkPriorityStateSender(signal);
//	}
	if (messagesToSend.size() > 0) {
		message = messagesToSend.front();
		messagesToSend.pop_front();
	}
	
	packetizeData(message);

	OutputDebugString("\nPacketized message : ");
	//OutputDebugString(packet.c_str());
	syncBit = (syncBit == SYNC0 ? SYNC1 : SYNC0);
	write(packet);
}



void Protocol::packetizeData(string message) {
	char temp[512] = { '\0' };
	strcpy_s(temp, message.c_str());
	Checksum *cs = new Checksum();
	for (char a : temp)
		cs->add(a);
	vector<char> checksum = cs->get();
	packet = "";
	packet += SOH;
	packet += syncBit;
	packet += checksum[0];
	packet += checksum[1];
	packet += message;

}



//S2 : check ACK for priority
void Protocol::checkPriorityStateSender(char signal) {
	if (signal == ACK && !priority) {
		idle();
	}
	else
		wait();
}