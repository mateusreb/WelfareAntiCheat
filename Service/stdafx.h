#pragma once

#include <windows.h> 
#include <stdio.h> 
#include <tchar.h>
#include <strsafe.h>
#include <sddl.h> 
#include <strsafe.h>
#include <fstream>
#include <wtsapi32.h>
#include <userenv.h>

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "Wtsapi32.lib")
#pragma comment(lib, "Userenv.lib")
using namespace std;

#include "..\ThirdParty\AES\AES.h"

#include "Variables.h"
#include "DebugTools.h"
#include "PipeServer.h"
#include "DriverManager.h"