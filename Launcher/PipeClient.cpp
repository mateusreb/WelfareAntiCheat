#include "PipeClient.h"

PipeClient::PipeClient(LPCSTR pName)
{	
	lpName = pName;
}

BOOL PipeClient::Connect()
{
	hPipe = CreateFile(lpName, GENERIC_READ | GENERIC_WRITE, 0,	NULL, OPEN_EXISTING, 0,	NULL);
	if (hPipe != INVALID_HANDLE_VALUE)
		return TRUE;
	else
	{
		WelfareLog("Could not open pipe. ERROR(%i).", GetLastError());
		return FALSE;
	}
}

BOOL PipeClient::Read(LPVOID lpBuffer, SIZE_T  nSize)
{
	if (hPipe != INVALID_HANDLE_VALUE)
	{
		if (ReadFile(hPipe, lpBuffer, nSize, &dwRead, NULL))
			return TRUE;
		else
			return FALSE;
	}
	else
	{
		WelfareLog("hPipe INVALID_HANDLE_VALUE.\n");
		return FALSE;
	}
	return FALSE;
}

BOOL PipeClient::Write(LPVOID lpBuffer, SIZE_T  nSize)
{
	if (hPipe != INVALID_HANDLE_VALUE)
	{
		if (WriteFile(hPipe, lpBuffer, nSize, &dwWritten, NULL))
			return TRUE;
	}
	else
	{
		WelfareLog("hPipe INVALID_HANDLE_VALUE.\n");
		return FALSE;
	}
	return FALSE;
}