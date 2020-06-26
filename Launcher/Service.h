#pragma once
#include "stdafx.h"

class ServiceManager {
public:
	BOOL ServiceInstall();
	BOOL ServiceDelete();
	BOOL ServiceStart();
	BOOL ServiceStop();
private:
	SC_HANDLE hSCManager;
	SC_HANDLE hService;
	SERVICE_STATUS_PROCESS ssStatus;
	DWORD dwOldCheckPoint;
	DWORD dwStartTickCount;
	DWORD dwWaitTime;
	DWORD dwBytesNeeded;
	DWORD dwStartTime;
	DWORD dwTimeout;
};