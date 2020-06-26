#include <Windows.h>
#include <stdio.h>
#include <iostream>

using namespace std;

VOID WINAPI SetConsoleColor() 
{
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFOEX cbi;
	cbi.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
	GetConsoleScreenBufferInfoEx(hOutput, &cbi);
	//cbi.wAttributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
	cbi.ColorTable[0] = RGB(255, 255, 255);
	cbi.ColorTable[7] = RGB(0, 0, 255);
	SetConsoleScreenBufferInfoEx(hOutput, &cbi);
	system("CLS");
}

int main()
{  
	//RemovePeHeader(GetModuleHandle("Game.exe"));
	SetConsoleColor();
	cout << "Game Started...";
	getchar();
	return 0;
}

/*
HANDLE hNamedEvent = OpenEvent(EVENT_ALL_ACCESS, TRUE, "WelfareEventGame");
SetEvent(hNamedEvent);
*/