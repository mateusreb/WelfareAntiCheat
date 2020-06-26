#pragma once
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <fstream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <tchar.h>
#include <wincrypt.h>
#include <wintrust.h>
#include <Softpub.h>
#include <string>
#include <array>
#include <cstdarg>
#include <TlHelp32.h>
#include <iostream>


#pragma comment(lib, "rpcrt4.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "wintrust.lib")
#pragma comment(lib,"ws2_32.lib")

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

using namespace std;

#include "..\ThirdParty\XorStr\XorStr.h"
#include "..\ThirdParty\AES\AES.h"

#include "Variables.h"
#include "DebugTools.h"
#include "ClientNetwork.h"
#include "Service.h"
#include "PipeClient.h"
#include "DigitalSignatureScan.h"
#include "Utils.h"
#include "MainForm.h"