#include <string>
#include <windows.h>
#include <algorithm>
#include "Protocol.h"
#include "Timeout.h"
#include "checksum.h"

using namespace std;

DWORD WINAPI startProtocol(LPVOID lpParameter) {
	Protocol &p = *((Protocol *)lpParameter);
	p.idle();
	return 0;
}

//Carson
Protocol::Protocol() {}

Protocol::~Protocol() {
	disconnect();
}

void Protocol::connect() {
	if ((handle = CreateFile(TEXT("COM1"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL)) == INVALID_HANDLE_VALUE) {
		DWORD dwError = GetLastError();
		OutputDebugStringA("Error creating serial port handle");
	}

	timeoutThread = CreateThread(NULL, 0, startTimer, this, 0, &timeoutThreadId);
	if (timeoutThread == NULL && DEBUG)
		OutputDebugStringA("Error creating timeout thread");

	protocolThread = CreateThread(NULL, 0, startProtocol, this, 0, &protocolThreadId);
	if (protocolThread == NULL && DEBUG)
		OutputDebugStringA("Error creating protocol thread");

	sync = SYNC0;
}

void Protocol::disconnect() {
	TerminateThread(timeoutThread, 0);
	TerminateThread(protocolThread, 0);
	if (&handle != INVALID_HANDLE_VALUE && &handle != nullptr)
		CloseHandle(handle);
	
	OutputDebugStringA("Deconstructed");
}

//Carson
void Protocol::sendMessage(string message, bool priority = false) {
	//Figure out why it wont append EOT
	message = message.append("" + SOH);
	size_t i = 0;
	priority = priority;
	cout << message << endl;
	while (i < message.length()) {
		if (message.length() - i > 512)
			messagesToSend.push_back(message.substr(i, 512));
		else
			messagesToSend.push_back(message.substr(i, message.length() - i));
		i += 512;
	}
}


void Protocol::sendMessage(iostream filestream, bool priority = false) {

}

//Carson
void Protocol::write(string message) {
	unsigned char buffer[516];
	DWORD i;
	for (i = 0; i < message.length() || i < 516; i++) {
		buffer[i] = message[i];
	}
	WriteFile(handle, buffer, i, 0, &OVERLAPPED());
}

//Carson
void Protocol::write(char message) {
	char buffer[1] = { message };
	WriteFile(handle, buffer, 1, 0, &OVERLAPPED());
}

void Protocol::idle() {
	PurgeComm(handle, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
	char received;
	otherPriority = false;
	if (DEBUG)
		OutputDebugStringA("\nIdle ");
	while (1) {

		if (messagesToSend.size() > 0) {
			//GOTO confirmLine
		}
		
		if (readNextChar(1, &received, [this](char c) {
			if (c == ENQP)
				otherPriority = true;
			return (c == ENQ || c == ENQP);
			})) {
			timeoutStatus.stop();
			acknowledgeLine();
		}
	}
}

void Protocol::wait() {
	otherPriority = false;
	DWORD curTime = GetTickCount();
	char received;
	if (DEBUG)
		OutputDebugStringA("\nWait");
	while (1) {
		if (readNextChar(2, &received, [this](char c) {
			if (c == ENQP)
				otherPriority = true;
			return (c == ENQ || c == ENQP);
		}))
			acknowledgeLine();
		if (curTime + GLOBAL_TIMEOUT > GetTickCount()) {
			idle();
		}
	}
}

bool Protocol::validatePacket(string packet) {
	vector<char> chars;
	Checksum checksum;

	//Valid length
	if (packet.length() < 5 || packet.length() > 516)
		return false;

	for (int i = 4; i < packet.length(); i++)
		checksum.add(packet[i]);

	chars = checksum.get();

	//Validate header, sync bit and  checksum
	return (packet[0] == SOH &&
		packet[1] == sync == SYNC0 ? SYNC0 : SYNC1 &&
		packet[2] == '0' &&//chars[0] &&
		packet[3] == '0');//chars[1]);
}

string Protocol::packetizePacket(string packet) {
	return packet;
}

template<class UnaryPredicate>
bool Protocol::readNextChar(int timeout, char * c, UnaryPredicate predicate) {
	char inbuff[1] { 0 };
	bool loop = true;
	DWORD nBytesRead, dwEvent, dwError;
	OVERLAPPED osStatus = { 0 };

	/* generate event whenever a byte arives */
	if (!SetCommMask(handle, EV_RXCHAR)) {
		dwError = GetLastError();
		OutputDebugStringA("(Char)Failed to SetCommMask ");
		return false;
	}
	timeoutStatus.setTimeout(timeout);

	ListenForEvent:
	if (!WaitCommEvent(handle, &dwEvent, NULL)) {
		if (!(timeoutStatus.timeout)) {
			OutputDebugStringA("\n(Char)Failed to WaitCommEvent");
			timeoutStatus.stop();
		}
		return false;
	}
	
	if (!(dwEvent & EV_RXCHAR)) {
		if (!(timeoutStatus.timeout)) {
			OutputDebugStringA("\n(Char)Failed to read character");
			timeoutStatus.stop();
		}
		return false;
	}

	if (ReadFile(handle, &inbuff, sizeof(char), &nBytesRead, &osStatus)) {
		if (predicate(inbuff[0])) {
			if (DEBUG)
				OutputDebugStringA("\nMatch");
			*c = inbuff[0];
			return true;
		} else {
			dwError = GetLastError();
			OutputDebugStringA("Failed UnaryPredicate Test");
			goto ListenForEvent;
		}
	}

	timeoutStatus.stop();
	return false;
}

bool Protocol::readNextPacket(int timeout, string& s) {
	char inbuff[516]{ '\0' };
	DWORD nBytesRead = 0, dwEvent, dwError;
	OVERLAPPED osStatus = { 0 };

	/* generate event whenever a byte arives */
	if (!SetCommMask(handle, EV_RXCHAR)) {
		dwError = GetLastError();
		OutputDebugStringA("(Packet)Failed to SetCommMask");
		return false;
	}

	timeoutStatus.setTimeout(timeout);

	ListenForEvent:
	if (WaitCommEvent(handle, &dwEvent, NULL) == 0) {
		if (!(timeoutStatus.timeout)) {
			OutputDebugStringA("\n(Packet)Failed to WaitCommEvent");
			timeoutStatus.stop();
		}
		return false;
	}

	if (!(dwEvent & EV_RXCHAR)) {
		if (!(timeoutStatus.timeout)) {
			OutputDebugStringA("\n(Packet)Failed to read character");
			timeoutStatus.stop();
		}
		return false;
	}

	ReadFile(handle, &inbuff, sizeof(char) * 516, &nBytesRead, &osStatus);
	GetOverlappedResult(handle, &osStatus, &nBytesRead, true);

	timeoutStatus.stop();

	for (int i = 0; i < nBytesRead; i++)
		s[i] = inbuff[i];
	return true;
}