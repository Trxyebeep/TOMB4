#include "../tomb4/pch.h"
#include "winmain.h"
#include "function_stubs.h"
#include "cmdline.h"
#include "registry.h"
#include "dxshell.h"
#include <time.h>
#include "../game/text.h"

COMMAND commands[] =
{
	{ "SETUP", 0, &CLSetup },
	{ "NOFMV", 0, &CLNoFMV }
};

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

void WinProcessCommandLine(LPSTR cmd)
{
	COMMAND* command;
	char* pCommand;
	char* p;
	char* last;
	ulong l;
	long num;
	char parameter[20];

	Log(2, "WinProcessCommandLine");

	num = sizeof(commands) / sizeof(commands[0]);
	
	for (int i = 0; i < num; i++)
	{
		command = &commands[i];
		command->code((char*)"_INIT");
	}

	for (int i = 0; (ulong)i < strlen(cmd); i++)
	{
		if (toupper(cmd[i]))
			cmd[i] = toupper(cmd[i]);
	}

	for (int i = 0; i < num; i++)
	{
		command = &commands[i];
		memset(parameter, 0, sizeof(parameter));
		pCommand = strstr(cmd, command->command);

		if (pCommand)
		{
			if (command->needs_parameter)
			{
				p = 0;
				l = strlen(pCommand);

				for (int j = 0; (ulong)j < l; j++, pCommand++)
				{
					if (*pCommand != '=')
						continue;

					p = pCommand + 1;
					l = strlen(p);

					for (j = 0; (ulong)j < l; j++, p++)
					{
						if (*p != ' ')
							break;
					}

					last = p;
					l = strlen(last);

					for (j = 0; (ulong)j < l; j++, last++)
					{
						if (*last == ' ')
							break;
					}

					strncpy(parameter, p, j);
					break;
				}

				command->code(parameter);
			}
			else
				command->code(0);
		}
	}
}

void WinClose()
{
	Log(2, "WinClose");
	SaveSettings();
	CloseHandle(App.mutex);
	DXFreeInfo(&App.DXInfo);
	DestroyAcceleratorTable(App.hAccel);
	DXClose();

	if (!G_dxptr)
		return;

	DXAttempt(G_dxptr->Keyboard->Unacquire());

	if (G_dxptr->Keyboard)
	{
		Log(4, "Released %s @ %x - RefCnt = %d", "Keyboard", G_dxptr->Keyboard, G_dxptr->Keyboard->Release());
		G_dxptr->Keyboard = 0;
	}
	else
		Log(1, "%s Attempt To Release NULL Ptr", "Keyboard");

	if (G_dxptr->lpDirectInput)
	{
		Log(4, "Released %s @ %x - RefCnt = %d", "DirectInput", G_dxptr->lpDirectInput, G_dxptr->lpDirectInput->Release());
		G_dxptr->lpDirectInput = 0;
	}
	else
		Log(1, "%s Attempt To Release NULL Ptr", "DirectInput");
}

float WinFrameRate()
{
	double t, time_now;
	static float fps;
	static long time, counter;
	static char first_time;

	if (!(first_time & 1))
	{
		first_time |= 1;
		time = clock();
	}

	counter++;

	if (counter == 10)
	{
		time_now = clock();
		t = (time_now - time) / (double)CLOCKS_PER_SEC;
		time = (long)time_now;
		fps = float(counter / t);
		counter = 0;
	}

	App.fps = fps;
	return fps;
}

void WinDisplayString(long x, long y, char* string, ...)
{
	va_list list;
	char buf[4096];

	va_start(list, string);
	vsprintf(buf, string, list);
	PrintString((ushort)x, (ushort)y, 6, buf, 0);
}

long CheckMMXTechnology()
{
#ifdef GENERAL_FIXES	//original works but is dodgy as fuck
	return 0;
#else
	ulong _edx;
	long mmx;

	mmx = 1;

	__try
	{
		__asm
		{
			pusha
			mov eax, 1
			cpuid
			mov _edx, edx
			popa
		}

	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		mmx = 0;
	}

	if (!mmx)
		mmx = 0;

	if (_edx & 0x800000)
	{
		__try
		{
			__asm
			{
				emms
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			mmx = 0;
		}
	}
	else
		mmx = 0;

	return mmx;
#endif
}

void WinProcMsg()
{
	MSG msg;

	Log(2, "WinProcMsg");

	do
	{
		GetMessage(&msg, 0, 0, 0);

		if (!TranslateAccelerator(App.hWnd, App.hAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	} while (!MainThread.ended && msg.message != WM_QUIT);
}

void inject_winmain(bool replace)
{
	INJECT(0x0048F6A0, WinRunCheck, replace);
	INJECT(0x0048F700, WinProcessCommandLine, replace);
	INJECT(0x0048EF20, WinClose, replace);
	INJECT(0x0048F840, WinFrameRate, replace);
	INJECT(0x0048F8C0, WinDisplayString, replace);
	INJECT(0x0048EE50, CheckMMXTechnology, replace);
	INJECT(0x0048EF70, WinProcMsg, replace);
}
