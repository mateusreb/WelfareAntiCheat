#include "PipeServer.h"

PipeServer::PipeServer(LPCSTR lpName)
{
	if (!CreatePipeSecurity(&pSa))
	{
		WelfareLog("CreatePipeSecurity ERROR(%i)\n", GetLastError());
		return;
	}

	hPipe = CreateNamedPipe(
		PIPE_NAME,
		PIPE_ACCESS_INBOUND | PIPE_ACCESS_DUPLEX | WRITE_DAC, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_NOWAIT,
		3, 
		PIPE_BUFSIZE,
		PIPE_BUFSIZE,
		0,
		pSa);

	if (hPipe == INVALID_HANDLE_VALUE)
	{
		WelfareLog("CreateNamedPipe ERROR(%i)\n", GetLastError());
		return;
	}
	WelfareLog("Pipe Created!\n");
}

PipeServer::~PipeServer()
{
	CloseHandle(hPipe);
	LocalFree(pSd);
	LocalFree(pSa);
}

BOOL PipeServer::WaitForClient()
{
	WelfareLog("Pipe Wait Connection!\n");
	while (!ConnectNamedPipe(hPipe, 0) && GetLastError() != ERROR_PIPE_CONNECTED)
	{
		Sleep(125);		
	}
	WelfareLog("Pipe Connected!\n");
	return TRUE;
}

BOOL PipeServer::Read(LPVOID lpBuffer, SIZE_T  nSize)
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

BOOL PipeServer::Write(LPVOID lpBuffer, SIZE_T  nSize)
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

BOOL PipeServer::CreatePipeSecurity(PSECURITY_ATTRIBUTES *ppSa)
{
	if (!ConvertStringSecurityDescriptorToSecurityDescriptor(PIPE_SDDL,
		SDDL_REVISION_1, &pSd, NULL))
	{
		LocalFree(pSd);
		WelfareLog("ConvertStringSecurityDescriptorToSecurityDescriptor ERROR(%i)\n", GetLastError());
		return FALSE;
	}

	pSa = (PSECURITY_ATTRIBUTES)LocalAlloc(LPTR, sizeof(*pSa));
	if (pSa == NULL)
	{ 
		WelfareLog("LocalAlloc ERROR(%i)\n", GetLastError());
		LocalFree(pSa);
		return FALSE;
	}

	pSa->nLength = sizeof(*pSa);
	pSa->lpSecurityDescriptor = pSd;
	pSa->bInheritHandle = FALSE;
	*ppSa = pSa;
	return TRUE;
}
