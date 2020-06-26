#include "ClientNetwork.h"

ClientNetwork::ClientNetwork(unsigned short sin_port)
{
	WSAStartup(MAKEWORD(2, 2), &lpWSAData);
	len = sizeof(serverInfo);

	serverInfo.sin_family = AF_INET;
	serverInfo.sin_port = htons(27015);
	serverInfo.sin_addr.s_addr = inet_addr("192.168.0.101");

	ClientSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (ClientSocket == INVALID_SOCKET)
	{
		printf("socket error: %i.\n", WSAGetLastError());
	}
}

int ClientNetwork::Read()
{
	int iResult;
	int len = sizeof(serverInfo);
	int SenderAddrSize;
	char buffer[1024];

	while (true)
	{
		iResult = recvfrom(ClientSocket, buffer, sizeof(buffer), 0, (sockaddr*)&serverInfo, &len);
		if (iResult != SOCKET_ERROR)
		{
			printf("Server: %s\n", buffer);
		}
		Sleep(500);
	}

	/*iResult = recvfrom(ClientSocket, (char*)buffer, sizeof(buffer), 0, (sockaddr*)&serverInfo, &SenderAddrSize);
	if (iResult == SOCKET_ERROR) 
	{
		wprintf(L"recvfrom failed with error %d\n", WSAGetLastError());
	}*/
	return iResult;
}

int ClientNetwork::Write()
{
	int iResult;
	char buffer[100] = "Welcome message.";
	iResult = sendto(ClientSocket, buffer, sizeof(buffer), 0, (sockaddr*)&serverInfo, sizeof(serverInfo));
	if (iResult == SOCKET_ERROR)
	{
		wprintf(L"sendto failed with error: %d\n", WSAGetLastError());
		//closesocket(ClientSocket);
		//WSACleanup();
	}
	return iResult;
}

ClientNetwork::~ClientNetwork()
{

}