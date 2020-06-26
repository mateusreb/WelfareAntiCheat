#include "DriverManager.h"

BOOL DriverManager::DriverInstall()
{
	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (hSCManager == NULL) {
		WelfareLog("[WAC-SVC] OpenSCManager ERROR(%i).", GetLastError());
		return FALSE;
	}

	hService = CreateService(hSCManager, DRIVER_NAME, DRIVER_NAME, SC_MANAGER_ALL_ACCESS,
		SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, DRIVER_PATH, NULL, NULL, NULL, NULL, NULL);

	if (hService == NULL)
	{
		hService = OpenService(hSCManager, DRIVER_NAME, SERVICE_ALL_ACCESS);

		if (hService == NULL)
		{
			CloseServiceHandle(hSCManager);
			WelfareLog("[WAC-SVC] CreateService Failed ERROR(%i).", GetLastError());
			return FALSE;
		}
	} 
	CloseServiceHandle(hSCManager);
	return TRUE;
}

BOOL DriverManager::DriverDelete()
{
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

	if (hSCManager == NULL)
		WelfareLog("[WAC-SVC] OpenSCManager ERROR(%i).", GetLastError());

	hService = OpenService(hSCManager, DRIVER_NAME, SERVICE_ALL_ACCESS);
	if (hService == NULL)
	{
		CloseServiceHandle(hSCManager);
		WelfareLog("[WAC-SVC] OpenService ERROR(%i).", GetLastError());
	}
	DeleteService(hService);
	CloseServiceHandle(hSCManager);
	return TRUE;
}

BOOL DriverManager::DriverStart()
{
	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (!hSCManager)
	{
		WelfareLog("[WAC-SVC] OpenSCManager ERROR(%i).", GetLastError());
		return FALSE;
	}
	hService = OpenService(hSCManager, DRIVER_NAME, SERVICE_ALL_ACCESS);
	if (!hService)
	{
		WelfareLog("[WAC-SVC] OpenService ERROR(%i).", GetLastError());
		CloseServiceHandle(hSCManager);
		return FALSE;
	}
	if (!StartService(hService, 0, NULL))
	{
		WelfareLog("[WAC-SVC] StartService ERROR(%i).", GetLastError());
		CloseServiceHandle(hSCManager);
		CloseServiceHandle(hService);
		return FALSE;
	}
	CloseServiceHandle(hSCManager);
	CloseServiceHandle(hService);
	return TRUE;
}

BOOL DriverManager::DriverStop()
{
	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (!hSCManager) {
		WelfareLog("[WAC-SVC] OpenSCManager ERROR(%i).", GetLastError());
		return FALSE;
	}
	hService = OpenService(hSCManager, DRIVER_NAME, SERVICE_ALL_ACCESS);

	if (!hService)
	{
		CloseServiceHandle(hSCManager);
		WelfareLog("[WAC-SVC] OpenService ERROR(%i).", GetLastError());
		return FALSE;
	}

	if (!ControlService(hService, SERVICE_CONTROL_STOP, &ss))
	{
		WelfareLog("[WAC-SVC] ControlService ERROR(%i).", GetLastError());
		CloseServiceHandle(hSCManager);
		CloseServiceHandle(hService);
		return FALSE;
	}
	CloseServiceHandle(hSCManager);
	CloseServiceHandle(hService);
	return TRUE;
}

DriverManager::DriverManager()
{

}

DriverManager::~DriverManager()
{

}