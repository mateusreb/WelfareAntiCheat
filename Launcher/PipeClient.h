#pragma once

#include "stdafx.h"

class PipeClient {
public:
	PipeClient(LPCSTR lpName);
	BOOL Connect();
	BOOL Read(LPVOID lpBuffer, SIZE_T  nSize);
	BOOL Write(LPVOID lpBuffer, SIZE_T  nSize);
private:
	LPCSTR lpName;
	HANDLE hPipe;
	BOOL   bSuccess = FALSE;
	DWORD  dwMode;
	DWORD dwRead;
	DWORD dwWritten;
};