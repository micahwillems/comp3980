#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <string.h>
#include "Main.h"
#include "Protocol.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static TCHAR Name[] = TEXT("Assign4");
HWND hwndMain, hwndButtonSend, hwndButtonConnect, hwndDisplayMain,
	hwndDisplayStats, hwndButtonHelp, hwndChkBoxPriority;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hprevInstance,
	LPSTR lspszCmdParam, int nCmdShow)
{
	MSG Msg;
	WNDCLASSEX Wcl;

	Wcl.cbSize = sizeof(WNDCLASSEX);
	Wcl.style = 0; 
	Wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	Wcl.hIconSm = NULL; 
	Wcl.hCursor = LoadCursor(NULL, IDC_ARROW);  

	Wcl.lpfnWndProc = WndProc;
	Wcl.hInstance = hInst;  
	Wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); 
	Wcl.lpszClassName = Name;

	Wcl.lpszMenuName = NULL; 
	Wcl.cbClsExtra = 0;      
	Wcl.cbWndExtra = 0;

	if (!RegisterClassEx(&Wcl))
		return 0;

	// The main application window
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

	ShowWindow(hwndMain, nCmdShow);
	UpdateWindow(hwndMain);
	
	// The main textbox area for showing received data (content)
	hwndDisplayMain = CreateWindowEx(0,
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

	// Help button for displaying help dialog
	hwndButtonHelp = CreateWindow(
		TEXT("BUTTON"),  
		TEXT("?"),      
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  
		950,      
		10,       
		25,       
		25,       
		hwndMain, 
		NULL,     
		hInst,
		NULL);    
	
	// Send file button which will load a standard open file dialog
	hwndButtonSend = CreateWindow(
		TEXT("BUTTON"),  
		TEXT("SEND FILE"),  
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_DISABLED,  
		550,
		60,        
		90,        
		25,        
		hwndMain,  
		NULL,      
		hInst,
		NULL);

	// Connect/disconnect button
	hwndButtonConnect = CreateWindow(
		TEXT("BUTTON"),  
		TEXT("CONNECT"),     
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 
		550,         
		100,         
		100,        
		25,        
		hwndMain,     
		NULL,       
		hInst,
		NULL);      

	// Checkbox for priority mode
	hwndChkBoxPriority = CreateWindow(
		TEXT("BUTTON"),  
		TEXT("PRIORITY"),      
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX ,
		550,         
		140,         
		100,        
		25,       
		hwndMain,     
		NULL,       
		hInst,
		NULL);      

	// Text edit box for displaying statistics about the packet
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

	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg); 
	}

	return Msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message,
	WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT paintstruct;
	TEXTMETRIC tm;
	char str[255];	

	switch (Message)
	{
	case WM_CTLCOLORSTATIC:
		SetBkColor((HDC)wParam, RGB(255, 255, 255));
		return (LRESULT)GetStockObject(WHITE_BRUSH);
		break;
	case WM_COMMAND:
		if ((HWND)lParam == hwndButtonHelp) {
			// ****Help Menu/Dialog Button Clicked*****************************************************
		}
		else if ((HWND)lParam == hwndButtonSend) {
			// ***********Send File Button Clicked*****************************************************
		}
		else if ((HWND)lParam == hwndButtonConnect) {
			// ***********Connect/Disconnect Button Clicked********************************************
			char btnTxt[11];
			SendMessage((HWND)lParam, WM_GETTEXT, 11, (LPARAM)btnTxt);
			if (strcmp(btnTxt, "CONNECT") == 0) {
				SendMessage((HWND)lParam, WM_SETTEXT, 0, (LPARAM)TEXT("DISCONNECT"));
				EnableWindow(hwndButtonSend, TRUE);
				Protocol protocol;
			}
			else {
				SendMessage((HWND)lParam, WM_SETTEXT, 0, (LPARAM)TEXT("CONNECT"));
				EnableWindow(hwndButtonSend, FALSE);
			}
		}
		else if ((HWND)lParam == hwndChkBoxPriority) {
			if (SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == BST_CHECKED) {
				// ***************Priority Mode Box Checked********************************************
			}
			else if (SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == BST_UNCHECKED) {
				// ***************Priority Mode Box Unchecked******************************************
			}
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &paintstruct);
		GetTextMetrics(hdc, &tm);		
		sprintf_s(str, "Packet Statistics:");
		TextOut(hdc, 530, 200, str, strlen(str));
		break;
	case WM_DESTROY:		
		PostQuitMessage(0);
		break;
	default: 
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}