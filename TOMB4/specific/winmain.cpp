#include "../tomb4/pch.h"
#include "winmain.h"
#include "function_stubs.h"

bool WinRunCheck(LPSTR WindowName, LPSTR ClassName, HANDLE* mutex)
{
	HWND window;

	Log(2, "WinRunCheck");
	*mutex = CreateMutex(0, 1, WindowName);

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		window = FindWindow(ClassName, WindowName);

		if (window)
			SendMessage(window, WM_ACTIVATE, 0, 0);

		return 1;
	}

	return 0;
}

void inject_winmain(bool replace)
{
	INJECT(0x0048F6A0, WinRunCheck, replace);
}
