#include "Main.h"

void Svc()
{
	printf("| [F1] Install Service | [F2] Uninstall Service | [F3] Start Service | [F4] Stop Service |\n");
	ServiceManager Service;
	while (true)
	{
		if (GetKeyState(VK_F1) < 0)
		{
			Service.ServiceInstall();
			Sleep(600);
		}
		if (GetKeyState(VK_F2) < 0)
		{
			Service.ServiceDelete();
			Sleep(600);
		}
		if (GetKeyState(VK_F3) < 0)
		{
			Service.ServiceStart();
			Sleep(600);
		}
		if (GetKeyState(VK_F4) < 0)
		{
			Service.ServiceStop();
			Sleep(600);
		}
		Sleep(100);
	}
}


int _tmain(int argc, TCHAR *argv[])
{
	ClientNetwork *Client = new ClientNetwork(27015);
	Client->Write();
	Client->Read();
	getchar();
	return 0;
}