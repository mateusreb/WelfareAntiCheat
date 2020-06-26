#pragma once
#define DEBUG_ENABLED

#define SERVICE_NAME	(LPSTR)"WelfareService"
#define SERVICE_ERROR	((DWORD)0xC0020001L)

#define DRIVER_NAME     "WelfareSystem"
#define DRIVER_PATH		"D:\\AC\\WelfareAntiCheat\\Bin\\WelfareSystem.sys"
#define LAUNCH_PATH		"D:\\AC\\WelfareAntiCheat\\Bin\\Game.exe"
#define PIPE_NAME		"\\\\.\\pipe\\Welfare"
#define PIPE_SDDL		"D:(A;OICI;GRGW;;;AU)(A;OICI;GA;;;BA)"
#define PIPE_BUFSIZE	4096

typedef struct _HEARTBEAT {
	DWORD ID;
	CHAR Game[32];
}HEARTBEAT, *PHEARTBEAT;

struct Packet
{
	__int8 PacketType;
	char Data[256];
}; 

