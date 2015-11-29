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
			//Someone set the timer to start
			ended = true;
			timeoutTime = p.timeoutStatus.timeoutDuration;
			p.timeoutStatus.timeoutDuration = -1;

			while (p.timeoutStatus.timeSet + timeoutTime > curTime) {
				if (p.timeoutStatus.loop) {
					curTime = GetTickCount();
				} else {
					p.timeoutStatus.timeout = false;
					ended = false;
					break;
				}
			}
					
			if (ended) {
				p.timeoutStatus.timeout = true;
				p.timeoutStatus.timeoutDuration = -1;
				SetCommMask(p.handle, 0);
			}
		}
	}
}

#endif