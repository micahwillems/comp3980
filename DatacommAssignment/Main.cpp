#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "Main.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static TCHAR Name[] = TEXT("Assign4");
HWND hwndMain, hwndButtonBrowse, hwndButtonConnect, hwndDisplayCont, hwndDisplayStats;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hprevInstance,
	LPSTR lspszCmdParam, int nCmdShow)
{
	MSG Msg;
	WNDCLASSEX Wcl;

	// Define a Window class
	Wcl.cbSize = sizeof(WNDCLASSEX);
	Wcl.style = 0; // default style
	Wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION); // large icon 
	Wcl.hIconSm = NULL; // use small version of large icon
	Wcl.hCursor = LoadCursor(NULL, IDC_ARROW);  // cursor style

	Wcl.lpfnWndProc = WndProc; // window function
	Wcl.hInstance = hInst; // handle to this instance
	Wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //white background
	Wcl.lpszClassName = Name; // window class name

	Wcl.lpszMenuName = NULL; // no class menu 
	Wcl.cbClsExtra = 0;      // no extra memory needed
	Wcl.cbWndExtra = 0;

	// Register the class
	if (!RegisterClassEx(&Wcl))
		return 0;

	// Create the main window
	hwndMain = CreateWindow(
		Name, 
		Name, 
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME,
		CW_USEDEFAULT,	
		CW_USEDEFAULT, 
		1000, 
		700,
		NULL, 
		NULL, 
		hInst, 
		NULL 
		);

	// Display the window
	ShowWindow(hwndMain, nCmdShow);
	UpdateWindow(hwndMain);
	
	// creates an "EDIT" window or edit text box
	hwndDisplayCont = CreateWindowEx(0,
		TEXT("EDIT"),
		NULL,
		WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL | ES_READONLY,
		0,
		0,
		500,
		660,
		hwndMain,
		(HMENU)IDC_TEXT_AREA,
		(HINSTANCE)GetWindowLong(hwndMain, GWL_HINSTANCE),
		NULL);

	hwndButtonBrowse = CreateWindow(
		TEXT("BUTTON"),  // Predefined class; Unicode assumed 
		TEXT("BROWSE"),      // Button text 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
		550,         // x position 
		60,         // y position 
		90,        // Button width
		25,        // Button height
		hwndMain,     // Parent window
		NULL,       // No menu.
		hInst,
		NULL);      // Pointer not needed.

	hwndButtonConnect = CreateWindow(
		TEXT("BUTTON"),  // Predefined class; Unicode assumed 
		TEXT("CONNECT"),      // Button text 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
		550,         // x position 
		110,         // y position 
		100,        // Button width
		25,        // Button height
		hwndMain,     // Parent window
		NULL,       // No menu.
		hInst,
		NULL);      // Pointer not needed.

	// creates an "EDIT" window or edit text box
	hwndDisplayStats = CreateWindowEx(0,
		TEXT("EDIT"),
		NULL,
		WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY | WS_BORDER,
		530,
		230,
		420,
		400,
		hwndMain,
		(HMENU)IDC_TEXT_AREA,
		(HINSTANCE)GetWindowLong(hwndMain, GWL_HINSTANCE),
		NULL);

	// Create the message loop
	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg); // translate keybpard messages
		DispatchMessage(&Msg); // dispatch message and return control to windows
	}

	return Msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message,
	WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT paintstruct;
	TEXTMETRIC tm;
	SIZE size;
	char str[255];	//output buffer

	switch (Message)
	{
	case WM_CTLCOLORSTATIC:
		if ((HWND)lParam == hwndDisplayStats) {
			SetBkColor((HDC)wParam, RGB(255, 255, 255));
			return (LRESULT)GetStockObject(WHITE_BRUSH);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &paintstruct); // Acquire DC
		GetTextMetrics(hdc, &tm);		// get text metrics 
		sprintf_s(str, "Packet Statistics:");
		TextOut(hdc, 530, 200, str, strlen(str));
		break;
	case WM_DESTROY:		// message to terminate the program
		PostQuitMessage(0);
		break;
	default: // Let Win32 process all other messages
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}