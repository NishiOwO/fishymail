/* $Id$ */
#include <fishymail.h>

#ifdef DEBUG
#ifdef _WIN32
#include <windows.h>
#endif

void DebugInit(void) {
#if defined(_WIN32) && 1
	FreeConsole();
	AllocConsole();
	SetConsoleTitle("FishyMail Win32 Debug Window");

	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	freopen("CONIN$", "r", stdin);
#endif
}

void DebugLog(const char* fmt, ...) {
	va_list va;

	va_start(va, fmt);

	printf("[FishyMail] ");
	vprintf(fmt, va);
	printf("\n");

	va_end(va);
}
#endif
