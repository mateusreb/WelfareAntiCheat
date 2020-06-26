#pragma once
#include "stdafx.h"

class DriverManager {
public:
	DriverManager();
	~DriverManager();
	BOOL DriverInstall();
	BOOL DriverDelete();
	BOOL DriverStart();
	BOOL DriverStop();
private:
	SC_HANDLE hSCManager;
	SC_HANDLE hService;
	SERVICE_STATUS ss;
};
