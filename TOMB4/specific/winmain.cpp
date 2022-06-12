#include "../tomb4/pch.h"
#include "winmain.h"
#include "function_stubs.h"
#include "cmdline.h"

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

void inject_winmain(bool replace)
{
	INJECT(0x0048F6A0, WinRunCheck, replace);
	INJECT(0x0048F700, WinProcessCommandLine, replace);
}
