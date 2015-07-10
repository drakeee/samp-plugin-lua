#include "CUtility.h"

void CUtility::printf(const char* szFormat, ...)
{
	va_list vaArgs;
	
	char szBuffer[2048];
	va_start(vaArgs, szFormat);
	vsnprintf(szBuffer, sizeof(szBuffer), szFormat, vaArgs);
	va_end(vaArgs);

	sampgdk::logprintf("[Lua] %s", szBuffer);
}