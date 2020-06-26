#pragma once
#include "stdafx.h"

class ClientNetwork
{
private:
	WSADATA lpWSAData;
	SOCKET ClientSocket;
	struct sockaddr_in serverInfo;
	int len;

public:
	ClientNetwork(unsigned short sin_port);
	int Write();
	int Read();
	~ClientNetwork();
};

