#include "Main.h"

/*void CreateUserProcess()
{

	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	HANDLE hToken, hTokenThis;
	DWORD dwUIAccess = 1;


	si.lpDesktop = (LPSTR)"Winsta0\\Default";

	WTSQueryUserToken(WTSGetActiveConsoleSessionId(), &hTokenThis);

	DuplicateTokenEx(hTokenThis, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &hToken);

	::SetTokenInformation(hToken, TokenUIAccess, &dwUIAccess, sizeof(dwUIAccess));

	if (CreateProcessAsUser(hToken, LAUNCH_PATH, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		WelfareLog("[WAC-SVC] CreateProcessAsUser OK.");
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
	else
	{
		WelfareLog("[WAC-SVC] CreateProcessAsUser ERROR(%i).", GetLastError());
	}
}*/

void PacketDecryptData(BYTE* lpMsg, int size, BYTE key)
{
	for (int n = 0; n < size; n++)
	{
		lpMsg[n] = (lpMsg[n] + key) ^ 0xA0;
	}
}

void PacketEncryptData(BYTE* lpMsg, int size, BYTE key)
{
	for (int n = 0; n < size; n++)
	{
		lpMsg[n] = (lpMsg[n] ^ 0xA0) - key;
	}
}

DWORD WINAPI LauncherThread(PipeServer* pPipeServer)
{	
	if (pPipeServer->WaitForClient())
	{
		HEARTBEAT HeartBeat;	

		while (true)
		{
			if (pPipeServer->Read(&HeartBeat, sizeof(HeartBeat)))
			{
				//PacketDecryptData((BYTE*)&HeartBeat, sizeof(HeartBeat), 0x68);
				WelfareLog("[WAC-SVC] ID: %X Game: %s Size: %x", HeartBeat.ID, HeartBeat.Game, sizeof(HeartBeat));				
				//pPipeServer->Write(&HeartBeat, sizeof(HeartBeat));
			}
			Sleep(100);
		}
	}
	return 0;
}

DWORD WINAPI GameClientThread(PipeServer* pPipeServer)
{
	if (pPipeServer->WaitForClient())
	{
		HEARTBEAT HeartBeat;

		while (true)
		{
			if (pPipeServer->Read(&HeartBeat, sizeof(HeartBeat)))
			{
				pPipeServer->Write(&HeartBeat, sizeof(HeartBeat));
				printf("Buffer: %X\n", HeartBeat.ID);
			}
			Sleep(200);
		}
	}
	return 0;
}

VOID ReportServiceStatus(DWORD dwCurrentState,	DWORD dwWin32ExitCode,	DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;

	// Fill in the SERVICE_STATUS structure.
	g_ServiceStatus.dwCurrentState = dwCurrentState;
	g_ServiceStatus.dwWin32ExitCode = dwWin32ExitCode;
	g_ServiceStatus.dwWaitHint = dwWaitHint;

	if (dwCurrentState == SERVICE_START_PENDING)
		g_ServiceStatus.dwControlsAccepted = 0;
	else 
		g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

	if ((dwCurrentState == SERVICE_RUNNING) || (dwCurrentState == SERVICE_STOPPED))
		g_ServiceStatus.dwCheckPoint = 0;
	else 
		g_ServiceStatus.dwCheckPoint = dwCheckPoint++;

	WelfareLog("[WAC-SVC] ReportSvcStatus(0x%X, %i, %i)", dwCurrentState, dwWin32ExitCode, dwWaitHint)
	// Report the status of the service to the SCM.
	SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus);
}

VOID WINAPI ServiceCtrlHandler(DWORD dwCtrl)
{
	// Handle the requested control code. 
	WelfareLog("[WAC-SVC] ServiceCtrlHandler.");
	switch (dwCtrl)
	{
	case SERVICE_CONTROL_STOP:
		ReportServiceStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
		// Signal the service to stop.
		SetEvent(g_ServiceStopEvent);
		ReportServiceStatus(g_ServiceStatus.dwCurrentState, NO_ERROR, 0);
		return;
	case SERVICE_CONTROL_INTERROGATE:
		break;
	default:
		break;
	}
}

DWORD WINAPI ServiceGameThread(LPVOID lpParam)
{
	DWORD ProcessID = (DWORD)(lpParam);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, ProcessID);
	WaitForSingleObject(hProcess, INFINITE);
	ReportServiceStatus(SERVICE_STOPPED, NO_ERROR, 0);

	if (!SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus))
	{
		WelfareLog("[WAC-SVC] SetServiceStatus ERROR(%i).", GetLastError());
	}
	SetEvent(g_ServiceStopEvent);
	CloseHandle(hProcess);
	return TRUE;
}

DWORD WINAPI ServiceMainThread(LPVOID lpParam)
{
	while (true)
	{
		Sleep(125);
	} 
	SetEvent(g_ServiceStopEvent);
}

VOID ServiceInit(DWORD dwArgc, LPTSTR *lpszArgv)
{
	HANDLE hLauncherThread;
	LPSTR lpCommandLine;
	WelfareLog("[WAC-SVC] SvcInit.");

	g_ServiceStopEvent = CreateEvent(NULL, TRUE, FALSE,	NULL);

	if (g_ServiceStopEvent == NULL)
	{
		ReportServiceStatus(SERVICE_STOPPED, NO_ERROR, 0);
		WelfareLog("[WAC-SVC] CreateEvent ERROR(%i).", GetLastError());
		return;
	}

	/*HANDLE hNamedEvent = CreateEvent(NULL, TRUE, FALSE, "WelfareServiceBin");

	if (hNamedEvent == NULL)
	{
		ReportServiceStatus(SERVICE_STOPPED, NO_ERROR, 0);
		WelfareLog("[WAC] CreateEvent ERROR(%i).", GetLastError());
		return;
	}*/

	ReportServiceStatus(SERVICE_RUNNING, NO_ERROR, 0);

	//CreateUserProcess();

	PipeServer* pPipeLauncher = new PipeServer(PIPE_NAME);
	hLauncherThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)LauncherThread, (LPVOID)pPipeLauncher, 0, NULL);
	//DriverManager *pDriverManager = new DriverManager();
	//pDriverManager->DriverInstall();
	//pDriverManager->DriverStart();
	WaitForSingleObject(g_ServiceStopEvent, INFINITE);	
	ReportServiceStatus(SERVICE_STOPPED, NO_ERROR, 0);
	WelfareLog("[WAC-SVC] WelfareService STOPPED.");
	//pDriverManager->DriverStop();
	//pDriverManager->DriverDelete();
	return;
}

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
	WelfareLog("[WAC-SVC] ServiceMain.");
	// Register the handler function for the service
	g_ServiceStatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceCtrlHandler);

	if (!g_ServiceStatusHandle)
	{
		WelfareLog("[WAC-SVC] RegisterServiceCtrlHandler error (%i).", GetLastError());		
		return;
	}

	g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	g_ServiceStatus.dwServiceSpecificExitCode = 0;

	// Report initial status to the SCM
	ReportServiceStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

	// Perform service-specific initialization and work.
	ServiceInit(dwArgc, lpszArgv);
}

int main(int argc, char *argv[])
{
	SERVICE_TABLE_ENTRY ServiceTable[] =
	{
		{ SERVICE_NAME, (LPSERVICE_MAIN_FUNCTIONA)ServiceMain },
		{ NULL, NULL }
	};
	 
	if (!StartServiceCtrlDispatcherA(ServiceTable))
	{
		return GetLastError();
	}
	return 0;
}



	/*PipeServer* pPipeLauncher = new PipeServer(PIPE_NAME);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)LauncherThread,	(LPVOID)pPipeLauncher,0,NULL);
	PipeServer* pPipeGameClient = new PipeServer(PIPE_NAME);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GameClientThread, (LPVOID)pPipeGameClient, 0, NULL);*/