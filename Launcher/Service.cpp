#include "Service.h"

BOOL ServiceManager::ServiceInstall()
{
	hSCManager = OpenSCManager(
		NULL,
		NULL,
		SC_MANAGER_ALL_ACCESS);

	if (hSCManager == NULL)
	{
		WelfareLog("OpenSCManager failed (%d)\n", GetLastError());
		return FALSE;
	}

	hService = CreateService(
		hSCManager, 
		SERVICE_NAME,
		SERVICE_NAME,
		SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL,
		SERVICE_PATH,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL);

	if (hService == NULL)
	{
		WelfareLog("CreateService failed (%d)\n", GetLastError());
		CloseServiceHandle(hSCManager);
		return FALSE;
	}
	WelfareLog("Service installed successfully\n");
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
	return TRUE;	
}

BOOL ServiceManager::ServiceDelete()
{
	hSCManager = OpenSCManager(NULL, NULL,	SC_MANAGER_ALL_ACCESS);

	if (NULL == hSCManager)
	{
		WelfareLog("OpenSCManager failed (%d)\n", GetLastError());
		return FALSE;
	}
	hService = OpenService( hSCManager,	SERVICE_NAME, DELETE);

	if (hService == NULL)
	{
		WelfareLog("OpenService failed (%d)\n", GetLastError());
		CloseServiceHandle(hSCManager);
		return FALSE;
	}

	if (!DeleteService(hService))
	{
		WelfareLog("DeleteService failed (%d)\n", GetLastError());
		CloseServiceHandle(hService);
		return FALSE;
	}
	WelfareLog("Service deleted successfully\n");
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
	return TRUE;
}

BOOL ServiceManager::ServiceStart()
{
	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (NULL == hSCManager)
	{
		WelfareLog("OpenSCManager failed (%d)\n", GetLastError());
		return FALSE;
	}

	hService = OpenService(hSCManager, SERVICE_NAME, SERVICE_ALL_ACCESS);

	if (hService == NULL)
	{
		WelfareLog("OpenService failed (%d)\n", GetLastError());
		CloseServiceHandle(hSCManager);
		return FALSE;
	}

	if (!QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssStatus, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded))
	{
		WelfareLog("QueryServiceStatusEx failed (%d)\n", GetLastError());
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCManager);
		return FALSE;
	}

	// Check if the service is already running. It would be possible 
	// to stop the service here, but for simplicity this example just returns. 

	if (ssStatus.dwCurrentState != SERVICE_STOPPED && ssStatus.dwCurrentState != SERVICE_STOP_PENDING)
	{
		WelfareLog("Cannot start the service because it is already running\n");
		
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCManager);
		return FALSE;
	}

	// Save the tick count and initial checkpoint.
	dwStartTickCount = GetTickCount();
	dwOldCheckPoint = ssStatus.dwCheckPoint;

	// Wait for the service to stop before attempting to start it.
	while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
	{
		// Do not wait longer than the wait hint. A good interval is 
		// one-tenth of the wait hint but not less than 1 second  
		// and not more than 10 seconds.
		dwWaitTime = ssStatus.dwWaitHint / 10;

		if (dwWaitTime < 1000)
			dwWaitTime = 1000;
		else if (dwWaitTime > 10000)
			dwWaitTime = 10000;

		Sleep(dwWaitTime);

		// Check the status until the service is no longer stop pending.
		if (!QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssStatus, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded))
		{
			WelfareLog("QueryServiceStatusEx failed (%d)\n", GetLastError());
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCManager);
			return FALSE;
		}
		if (ssStatus.dwCheckPoint > dwOldCheckPoint)
		{
			// Continue to wait and check.
			dwStartTickCount = GetTickCount();
			dwOldCheckPoint = ssStatus.dwCheckPoint;
		}
		else
		{
			if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
			{
				WelfareLog("Timeout waiting for service to stop\n");
				CloseServiceHandle(hService);
				CloseServiceHandle(hSCManager);
				return FALSE;
			}
		}
	}
	// Attempt to start the service.
	if (!StartService(hService, 0,NULL))
	{
		WelfareLog("StartService failed (%d)\n", GetLastError());
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCManager);
		return FALSE;
	}	

	WelfareLog("Service start pending...\n");

	// Check the status until the service is no longer start pending. 
	if (!QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssStatus, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded))
	{
		WelfareLog("QueryServiceStatusEx failed (%d)\n", GetLastError());
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCManager);
		return FALSE;
	}

	// Save the tick count and initial checkpoint.
	dwStartTickCount = GetTickCount();
	dwOldCheckPoint = ssStatus.dwCheckPoint;

	while (ssStatus.dwCurrentState == SERVICE_START_PENDING)
	{
		// Do not wait longer than the wait hint. A good interval is 
		// one-tenth the wait hint, but no less than 1 second and no 
		// more than 10 seconds.

		dwWaitTime = ssStatus.dwWaitHint / 10;

		if (dwWaitTime < 1000)
			dwWaitTime = 1000;
		else if (dwWaitTime > 10000)
			dwWaitTime = 10000;

		Sleep(dwWaitTime);

		// Check the status again.
		if (!QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssStatus, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded))              // if buffer too small
		{
			WelfareLog("QueryServiceStatusEx failed (%d)\n", GetLastError());
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCManager);
			return FALSE;
		}

		if (ssStatus.dwCheckPoint > dwOldCheckPoint)
		{
			// Continue to wait and check.
			dwStartTickCount = GetTickCount();
			dwOldCheckPoint = ssStatus.dwCheckPoint;
		}
		else
		{
			if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
			{
				WelfareLog("Timeout waiting for service to stop\n");
				CloseServiceHandle(hService);
				CloseServiceHandle(hSCManager);
				return FALSE;
			}
		}
	}

	// Determine whether the service is running.
	if (ssStatus.dwCurrentState == SERVICE_RUNNING)
	{
		WelfareLog("Service started successfully.\n");
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCManager);
		return TRUE;
	}
	else
	{
		WelfareLog("Service not started. \n");
		WelfareLog("  Current State: %d\n", ssStatus.dwCurrentState);
		WelfareLog("  Exit Code: %d\n", ssStatus.dwWin32ExitCode);
		WelfareLog("  Check Point: %d\n", ssStatus.dwCheckPoint);
		WelfareLog("  Wait Hint: %d\n", ssStatus.dwWaitHint);
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCManager);
		return FALSE;
	}
}

BOOL ServiceManager::ServiceStop()
{
	dwStartTime = GetTickCount();
	dwTimeout = 30000;

	// Get a handle to the SCM database. 
	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (!hSCManager)
	{
		WelfareLog("OpenSCManager failed (%d).\n", GetLastError());
		return FALSE;
	}

	// Get a handle to the service.
	hService = OpenService(hSCManager, SERVICE_NAME, SERVICE_STOP |	SERVICE_QUERY_STATUS | SERVICE_ENUMERATE_DEPENDENTS);

	if (!hService)
	{
		WelfareLog("OpenService failed (%d).\n", GetLastError());
		CloseServiceHandle(hSCManager);
		return FALSE;
	}

	// Make sure the service is not already stopped.
	if (!QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssStatus, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded))
	{
		WelfareLog("QueryServiceStatusEx failed (%d).\n", GetLastError());
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCManager);
		return FALSE;
	}

	if (ssStatus.dwCurrentState == SERVICE_STOPPED)
	{
		WelfareLog("Service is already stopped.\n");
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCManager);
		return FALSE;
	}

	// If a stop is pending, wait for it.
	while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
	{
		WelfareLog("Service stop pending.\n");
		// Do not wait longer than the wait hint. A good interval is 
		// one-tenth of the wait hint but not less than 1 second  
		// and not more than 10 seconds. 
		dwWaitTime = ssStatus.dwWaitHint / 10;

		if (dwWaitTime < 1000)
			dwWaitTime = 1000;
		else if (dwWaitTime > 10000)
			dwWaitTime = 10000;

		Sleep(dwWaitTime);

		if (!QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssStatus, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded))
		{
			WelfareLog("QueryServiceStatusEx failed (%d).\n", GetLastError());
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCManager);
			return FALSE;
		}

		if (ssStatus.dwCurrentState == SERVICE_STOPPED)
		{
			WelfareLog("Service stopped successfully.\n");
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCManager);
			return FALSE;
		}

		if (GetTickCount() - dwStartTime > dwTimeout)
		{
			WelfareLog("Service stop timed out.\n");
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCManager);
			return FALSE;
		}
	}
	// If the service is running, dependencies must be stopped first.
	// StopDependentServices();
	// Send a stop code to the service.

	if (!ControlService(hService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&ssStatus))
	{
		WelfareLog("ControlService failed (%d).\n", GetLastError());
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCManager);
		return FALSE;
	}

	// Wait for the service to stop.
	while (ssStatus.dwCurrentState != SERVICE_STOPPED)
	{
		Sleep(ssStatus.dwWaitHint);
		if (!QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssStatus, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded))
		{
			WelfareLog("QueryServiceStatusEx failed (%d).\n", GetLastError());
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCManager);
			return FALSE;
		}
		if (ssStatus.dwCurrentState == SERVICE_STOPPED)
			break;

		if (GetTickCount() - dwStartTime > dwTimeout)
		{
			WelfareLog("Wait timed out.\n");
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCManager);
			return FALSE;
		}
	}
	WelfareLog("Service stopped successfully.\n");
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
	return TRUE;
}