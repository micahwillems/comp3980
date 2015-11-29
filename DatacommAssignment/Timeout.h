#ifndef TIMEOUT_H
#define TIMEOUT_H

#include <Windows.h>
#include <iostream>
#include "Protocol.h"

using namespace std;

DWORD WINAPI startTimer(LPVOID lpParameter) {
	Protocol &p = *((Protocol *)lpParameter);
	DWORD curTime = GetTickCount();
	int timeoutTime = 0;
	bool ended = false;
	while (1) {
		if (p.timeoutStatus.timeoutDuration >= 0) {
			OutputDebugStringA("Entered timeout loop");
			bool ended = false;
			timeoutTime = p.timeoutStatus.timeoutDuration;
			p.timeoutStatus.timeoutDuration = -1;

			while (p.timeoutStatus.timeSet + timeoutTime > curTime) {
				OutputDebugStringA("Entered second timeout loop");
				if (p.timeoutStatus.loop) {
					curTime = GetTickCount();
				}
				else {
					p.timeoutStatus.timeout = false;
					bool ended = true;
				}
			}
					
			if (!ended) {
				OutputDebugStringA("Entered third timeout loop");
				SetCommMask(p.handle, EV_RXCHAR);
				p.timeoutStatus.timeout = true;
				ended = false;
				p.timeoutStatus.timeoutDuration = -1;
			}
		}
	}
}

#endif