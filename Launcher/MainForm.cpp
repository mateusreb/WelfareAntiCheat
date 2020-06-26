#include "MainForm.h"

int FormWidth = 470;
int FormHeight = 280;

int FormX = (GetSystemMetrics(SM_CXSCREEN) - FormWidth) / 2;
int FormY = (GetSystemMetrics(SM_CYSCREEN) - FormHeight) / 2;

HWND hTextBox;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CREATE:
		{
		HFONT hFont = CreateFont(15, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, "Arial");
		HWND ButtonExit = CreateWindow("BUTTON", "Exit", WS_VISIBLE | WS_CHILD, 255, 210, 90, 25, hWnd, (HMENU)0, GetModuleHandle(0), NULL);
		HWND ButtonHelp = CreateWindow("BUTTON", "Help", WS_VISIBLE | WS_CHILD, 350, 210, 90, 25, hWnd, (HMENU)0, GetModuleHandle(0), NULL);
		
		hTextBox = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "Mateus é top\r\nerror ihdsjdnsdns",
				WS_CHILD | WS_VISIBLE | WS_VSCROLL |
				ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
				10, 10, FormWidth - 40, FormHeight - 170 , hWnd, 0, GetModuleHandle(0), NULL);
			HWND GroupBox = CreateWindow("BUTTON", "Details", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 10, 125, FormWidth - 40, 80, hWnd, 0, GetModuleHandle(0), NULL);
						
			SendMessage(ButtonExit, WM_SETFONT, (WPARAM)hFont, TRUE);	
			SendMessage(ButtonHelp, WM_SETFONT, (WPARAM)hFont, TRUE);
			SendMessage(GroupBox, WM_SETFONT, (WPARAM)hFont, TRUE);
			SendMessage(hTextBox, WM_SETFONT, (WPARAM)hFont, TRUE);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

void WINAPI TForm()
{
	HWND hWnd;
	HINSTANCE hInst = GetModuleHandle(0);
	WNDCLASS wc;
	MSG msg;

	memset(&wc, 0, sizeof(wc));

	wc.hInstance = hInst;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = "WFA";
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);;
	wc.lpszMenuName = NULL;
	RegisterClass(&wc);

	hWnd = CreateWindowEx(
		0,
		"WFA",
		"WelfareAntiCheat",
		WS_SYSMENU | WS_CAPTION | WS_EX_TOPMOST,
		FormX,
		FormY,
		FormWidth,
		FormHeight,
		NULL,
		NULL,
		hInst,
		NULL);

	ShowWindow(hWnd, SW_SHOW);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnregisterClass("WFA", hInst);
	ExitThread(0);
}