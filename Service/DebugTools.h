#pragma once

#ifdef DEBUG_ENABLED
static void WriteLog(const char *fmt, ...)
{
	va_list va_alist;
	char logbuf[256] = { 0 };
	va_start(va_alist, fmt);
	vsnprintf_s(logbuf + strlen(logbuf), sizeof(logbuf) - strlen(logbuf), _TRUNCATE, fmt, va_alist);
	va_end(va_alist);
	OutputDebugStringA(logbuf);
}
#define WelfareLog(...) WriteLog(__VA_ARGS__);
#else
#define WelfareLog(...)	{  }		
#endif