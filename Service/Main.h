#pragma once

#include "stdafx.h"

SERVICE_STATUS          g_ServiceStatus;
SERVICE_STATUS_HANDLE   g_ServiceStatusHandle;
HANDLE                  g_ServiceStopEvent = NULL;
HANDLE					hMainThread;