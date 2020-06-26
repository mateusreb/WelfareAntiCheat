#pragma once

#ifdef DEBUG_ENABLED
static void WriteLog(const char *fmt, ...)
{
	ofstream ofile;
	ofile.open("WelfareAntiCheat.txt", ios::app);

	va_list va_alist;
	char logbuf[256] = { 0 };
	va_start(va_alist, fmt);
	vsnprintf_s(logbuf + strlen(logbuf), sizeof(logbuf) - strlen(logbuf), _TRUNCATE, fmt, va_alist);
	va_end(va_alist);
	ofile << logbuf;
	printf(logbuf); 
	ofile.close();
}
#define WelfareLog(...) WriteLog(__VA_ARGS__);
#else
#define WelfareLog(...)	{  }		
#endif

/*typedef struct _WELFARE_REQ
{
	BYTE ID;
	LPVOID Game[32];
} WELFARE_REQ, *PWELFARE_REQ;

typedef struct HEARTBEAT
{
	DWORD ID;
	CHAR Game[500];
} HEARTBEAT, *PHEARTBEAT;


BOOL HardwareBreakpoints()
{
	PCONTEXT ctx = PCONTEXT(VirtualAlloc(NULL, sizeof(ctx), MEM_COMMIT, PAGE_READWRITE));
	SecureZeroMemory(ctx, sizeof(CONTEXT));

	ctx->ContextFlags = CONTEXT_DEBUG_REGISTERS;

	if (GetThreadContext(GetCurrentThread(), ctx) == 0)
		return FALSE;

	if (ctx->Dr0 != 0 || ctx->Dr1 != 0 || ctx->Dr2 != 0 || ctx->Dr3 != 0)
	{
		VirtualFree(ctx, sizeof(ctx), MEM_RELEASE);
		return TRUE;
	}
	else
	{
		VirtualFree(ctx, sizeof(ctx), MEM_RELEASE);
		return FALSE;
	}
}

PipeClient PipeClient(PIPE_NAME);

HEARTBEAT HeartBeat;

HeartBeat.ID = 0x01;
memcpy(HeartBeat.Game, "Game.exe", 9);

//PacketEncryptData((BYTE*)&HeartBeat, sizeof(HeartBeat), 0x68);


if (PipeClient.Connect())
{
	//WelfareLog("Pipe connected.\n");
	if (PipeClient.Write(&HeartBeat, sizeof(HeartBeat)))
	{
		//WelfareLog("Pipe sent data.\n");
	}
}
bool HideThread(HANDLE hThread)
{
	typedef NTSTATUS(NTAPI *pNtSetInformationThread)
		(HANDLE, UINT, PVOID, ULONG);
	NTSTATUS Status;

	// Get NtSetInformationThread
	pNtSetInformationThread NtSIT = (pNtSetInformationThread)
		GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")),
			"NtSetInformationThread");

	// Shouldn't fail
	if (NtSIT == NULL)
		return false;

	// Set the thread info
	if (hThread == NULL)
		Status = NtSIT(GetCurrentThread(), 9, (PVOID)LoadLibraryA, 0x4);
	else
		Status = NtSIT(GetCurrentThread(), 9, (PVOID)LoadLibraryA, 0x4);

	printf("ERROR(%X)", Status);

	if (Status != 0x00000000)
		return false;
	else
		return true;
}

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

VOID WINAPI CallBack(PVOID ptr)
{
	auto Function_Call = (VOID(__thiscall*)())ptr;
	return Function_Call();
}*/