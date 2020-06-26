#pragma once
#include "stdafx.h"

class PipeServer {
public:
	PipeServer(LPCSTR lpName);
	~PipeServer();
	BOOL CreatePipeSecurity(PSECURITY_ATTRIBUTES *ppSa);
	BOOL WaitForClient();
	BOOL Read(LPVOID lpBuffer, SIZE_T  nSize);
	BOOL Write(LPVOID lpBuffer, SIZE_T  nSize);
private:
	HANDLE hPipe;
	DWORD dwRead;
	DWORD dwWritten;
	PSECURITY_DESCRIPTOR pSd = NULL;
	PSECURITY_ATTRIBUTES pSa = NULL;
};