#include "../tomb4/pch.h"
#include "winmain.h"
#include "function_stubs.h"
#include "cmdline.h"
#include "registry.h"
#include "dxshell.h"
#include <time.h>
#include "../game/text.h"
#include "lighting.h"
#include "function_table.h"
#include "d3dmatrix.h"
#include "3dmath.h"
#include "audio.h"
#include "output.h"
#include "file.h"
#include "../game/gameflow.h"
#include "dxsound.h"
#include "gamemain.h"
#include "fmv.h"

static COMMANDLINES commandlines[] =
{
	{ "SETUP", 0, &CLSetup },
	{ "NOFMV", 0, &CLNoFMV }
};

WINAPP App;
char* cutseqpakPtr;
long resChangeCounter;

bool WinRunCheck(LPSTR WindowName, LPSTR ClassName, HANDLE* mutex)
{
	HWND window;

	Log(2, "WinRunCheck");
	*mutex = CreateMutex(0, 1, WindowName);

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		window = FindWindow(ClassName, WindowName);

		if (window)
		{
			SendMessage(window, WM_ACTIVATE, WA_ACTIVE, 0);
			SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			SetWindowPos(window, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}

		return 1;
	}

	return 0;
}

void WinProcessCommandLine(LPSTR cmd)
{
	COMMANDLINES* command;
	char* pCommand;
	char* p;
	char* last;
	ulong l;
	long num;
	char parameter[20];

	Log(2, "WinProcessCommandLine");

	num = sizeof(commandlines) / sizeof(commandlines[0]);
	
	for (int i = 0; i < num; i++)
	{
		command = &commandlines[i];
		command->code((char*)"_INIT");
	}

	for (int i = 0; (ulong)i < strlen(cmd); i++)
	{
		if (toupper(cmd[i]))
			cmd[i] = toupper(cmd[i]);
	}

	for (int i = 0; i < num; i++)
	{
		command = &commandlines[i];
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
	FreeBinkStuff();

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

	DXDSClose();
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
	PrintString(x, y, 6, buf, 0);
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

void WinProcessCommands(long cmd)
{
	DXDISPLAYMODE* dm;
	long odm;

	if (cmd == KA_ALTENTER)
	{
		if (App.fmv || !(G_dxinfo->DDInfo[G_dxinfo->nDD].DDCaps.dwCaps2 & DDCAPS2_CANRENDERWINDOWED) || LevelLoadingThread.active)
			return;

		Log(6, "KA_ALTENTER");
		Log(5, "HangGameThread");
		while (App.dx.InScene) {};
		App.dx.WaitAtBeginScene = 1;
		while (!App.dx.InScene) {};
		SuspendThread((HANDLE)MainThread.handle);
		Log(5, "Game Thread Suspended");

		DXToggleFullScreen();
		HWInitialise();
		S_InitD3DMatrix();
		SetD3DViewMatrix();
		ResumeThread((HANDLE)MainThread.handle);
		App.dx.WaitAtBeginScene = 0;
		Log(5, "Game Thread Resumed");

		if (App.dx.Flags & DXF_FULLSCREEN)
		{
			SetCursor(0);
			ShowCursor(0);
		}
		else
		{
			SetCursor(LoadCursor(0, IDC_ARROW));
			ShowCursor(1);
		}
	}
	else if (cmd == KA_ALTP || cmd == KA_ALTM)
	{
		if (LevelLoadingThread.active || App.fmv)
			return;

		Log(5, "Change Video Mode");
		Log(5, "HangGameThread");
		while (App.dx.InScene) {};
		App.dx.WaitAtBeginScene = 1;
		while (!App.dx.InScene) {};
		SuspendThread((HANDLE)MainThread.handle);
		Log(5, "Game Thread Suspended");

		odm = App.DXInfo.nDisplayMode;

		if (cmd == KA_ALTP)
		{
			App.DXInfo.nDisplayMode++;

			if (App.DXInfo.nDisplayMode >= G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].nDisplayModes)
				App.DXInfo.nDisplayMode = G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].nDisplayModes - 1;

			dm = G_dxinfo->DDInfo[App.DXInfo.nDD].D3DDevices[App.DXInfo.nD3D].DisplayModes;

			while (dm[odm].bpp != dm[App.DXInfo.nDisplayMode].bpp)
			{
				App.DXInfo.nDisplayMode++;

				if (App.DXInfo.nDisplayMode >= G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].nDisplayModes)
				{
					App.DXInfo.nDisplayMode = odm;
					break;
				}
			}
		}
		else
		{
			App.DXInfo.nDisplayMode--;

			if (App.DXInfo.nDisplayMode < 0)
				App.DXInfo.nDisplayMode = 0;

			dm = G_dxinfo->DDInfo[App.DXInfo.nDD].D3DDevices[App.DXInfo.nD3D].DisplayModes;

			while (dm[odm].bpp != dm[App.DXInfo.nDisplayMode].bpp)
			{
				App.DXInfo.nDisplayMode--;

				if (App.DXInfo.nDisplayMode < 0)
				{
					App.DXInfo.nDisplayMode = odm;
					break;
				}
			}
		}

		if (odm != App.DXInfo.nDisplayMode)
		{
			if (!DXChangeVideoMode())
			{
				App.DXInfo.nDisplayMode = odm;
				DXChangeVideoMode();
			}

			HWInitialise();
			InitWindow(0, 0, App.dx.dwRenderWidth, App.dx.dwRenderHeight, 20, 20480, 80, App.dx.dwRenderWidth, App.dx.dwRenderHeight);
			InitFont();
			S_InitD3DMatrix();
			SetD3DViewMatrix();
		}

		ResumeThread((HANDLE)MainThread.handle);
		App.dx.WaitAtBeginScene = 0;
		Log(5, "Game Thread Resumed");
		resChangeCounter = 120;
	}
}

LRESULT CALLBACK WinMainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static long mouseX, mouseY, mouseB;
	static bool closing;

	switch (uMsg)
	{
	case WM_CREATE:
		resChangeCounter = 0;
		Log(6, "WM_CREATE");
		break;

	case WM_MOVE:
		Log(6, "WM_MOVE");
		DXMove((short)lParam, short((lParam >> 16) & 0xFFFF));
		break;

	case WM_ACTIVATE:

		if (!closing)
		{
			if (App.fmv)
				return 0;

			switch (wParam & 0xFFFF)
			{
			case WA_INACTIVE:
				Log(6, "WM_INACTIVE");

				if (App.SetupComplete)
				{
					Log(5, "Change Video Mode");
					Log(5, "HangGameThread");
					while (App.dx.InScene) {};
					App.dx.WaitAtBeginScene = 1;
					while (!App.dx.InScene) {};
					SuspendThread((HANDLE)MainThread.handle);
					Log(5, "Game Thread Suspended");
				}

				return 0;

			case WA_ACTIVE:
			case WA_CLICKACTIVE:
				Log(6, "WM_ACTIVE");

				if (App.SetupComplete)
				{
					ResumeThread((HANDLE)MainThread.handle);
					App.dx.WaitAtBeginScene = 0;
					Log(5, "Game Thread Resumed");
				}

				return 0;
			}
		}

		break;

	case WM_CLOSE:
		closing = 1;
		PostQuitMessage(0);
		break;

	case WM_COMMAND:
		Log(6, "WM_COMMAND");
		WinProcessCommands(wParam & 0xFFFF);
		break;

	case WM_MOUSEMOVE:
		mouseX = GET_X_LPARAM(lParam);
		mouseY = GET_Y_LPARAM(lParam);
		mouseB = wParam;
		break;

	case WM_APP:
		FillADPCMBuffer((char*)lParam, wParam);
		return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void ClearSurfaces()
{
	D3DRECT r;

	r.x1 = App.dx.rViewport.left;
	r.y1 = App.dx.rViewport.top;
	r.y2 = App.dx.rViewport.top + App.dx.rViewport.bottom;
	r.x2 = App.dx.rViewport.left + App.dx.rViewport.right;

	if (App.dx.Flags & DXF_HWR)
		DXAttempt(App.dx.lpViewport->Clear2(1, &r, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0F, 0));

	S_DumpScreen();

	if (App.dx.Flags & DXF_HWR)
		DXAttempt(App.dx.lpViewport->Clear2(1, &r, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0F, 0));

	S_DumpScreen();
}

bool WinRegisterWindow(HINSTANCE hinstance)
{
	App.hInstance = hinstance;
	App.WindowClass.hIcon = 0;
	App.WindowClass.lpszMenuName = 0;
	App.WindowClass.lpszClassName = "MainGameWindow";
	App.WindowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	App.WindowClass.hInstance = hinstance;
	App.WindowClass.style = CS_VREDRAW | CS_HREDRAW;
	App.WindowClass.lpfnWndProc = WinMainWndProc;
	App.WindowClass.cbClsExtra = 0;
	App.WindowClass.cbWndExtra = 0;
	App.WindowClass.hCursor = LoadCursor(0, IDC_ARROW);

	if (!RegisterClass(&App.WindowClass))
		return 0;

	return 1;
}

bool WinCreateWindow()
{
	App.hWnd = CreateWindowEx(WS_EX_APPWINDOW, "MainGameWindow", "Tomb Raider - The Last Revelation", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		0, 0, App.hInstance, 0);

	if (!App.hWnd)
		return 0;

	return 1;
}

void WinSetStyle(bool fullscreen, ulong& set)
{
	ulong style;

	style = GetWindowLong(App.hWnd, GWL_STYLE);

	if (fullscreen)
		style = (style & ~WS_OVERLAPPEDWINDOW) | WS_POPUP;
	else
		style = (style & ~WS_POPUP) | WS_OVERLAPPEDWINDOW;

	style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX | WS_SYSMENU);
	SetWindowLong(App.hWnd, GWL_STYLE, style);

	if (set)
		set = style;
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd)
{
	DXDISPLAYMODE* dm;
	HWND desktop;
	HDC hdc;
	DEVMODE devmode;
	char* buf;
	long size;

	start_setup = 0;
	App.mmx = 0;
	App.SetupComplete = 0;
	App.AutoTarget = 0;

	if (WinRunCheck((char*)"Tomb Raider - The Last Revelation", (char*)"MainGameWindow", &App.mutex))
		return 0;

	LoadGameflow();
	WinProcessCommandLine(lpCmdLine);

	if (!WinRegisterWindow(hInstance))
	{
		Log(1, "Unable To Register Window Class");
		return 0;
	}

	if (!WinCreateWindow())
	{
		Log(1, "Unable To Create Window");
		return 0;
	}

	ShowWindow(App.hWnd, SW_HIDE);
	UpdateWindow(App.hWnd);
	DXGetInfo(&App.DXInfo, App.hWnd);

	if (start_setup || !LoadSettings())
	{
		if (!DXSetupDialog())
		{
			free(gfScriptFile);
			free(gfLanguageFile);
			WinClose();
			return 0;
		}

		LoadSettings();
	}

#ifndef TIMES_LEVEL
	if (!fmvs_disabled)
	{
		if (!LoadBinkStuff())
		{
			MessageBox(0, "Failed to load Bink, disabling FMVs.", "Tomb Raider IV", 0);
			fmvs_disabled = 1;
		}
	}
#endif

	SetWindowPos(App.hWnd, 0, App.dx.rScreen.left, App.dx.rScreen.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	desktop = GetDesktopWindow();
	hdc = GetDC(desktop);
	App.Desktopbpp = GetDeviceCaps(hdc, BITSPIXEL);
	ReleaseDC(desktop, hdc);
	App.dx.WaitAtBeginScene = 0;
	App.dx.InScene = 0;
	App.fmv = 0;
	dm = &G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].DisplayModes[G_dxinfo->nDisplayMode];

	if (!DXCreate(dm->w, dm->h, dm->bpp, App.StartFlags, &App.dx, App.hWnd, WS_OVERLAPPEDWINDOW))
	{
		MessageBox(0, SCRIPT_TEXT(TXT_Failed_To_Setup_DirectX), "Tomb Raider IV", 0);
		return 0;
	}

	CoInitializeEx(0, COINIT_MULTITHREADED);

	WinSetStyle(G_dxptr->Flags & DXF_FULLSCREEN, G_dxptr->WindowStyle);
	UpdateWindow(App.hWnd);
	ShowWindow(App.hWnd, nShowCmd);

	if (App.dx.Flags & DXF_FULLSCREEN)
	{
		SetCursor(0);
		ShowCursor(0);
	}

	DXInitKeyboard(App.hWnd, App.hInstance);
	App.hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR));

	if (!App.SoundDisabled)
	{
		DXDSCreate();
		ACMInit();
	}

	cutseqpakPtr = 0;
	buf = 0;
	size = LoadFile("data\\cutseq.pak", &buf);

	if (size)
	{
		cutseqpakPtr = (char*)malloc(*(long*)buf);
		Decompress(cutseqpakPtr, buf + 4, size - 4, *(long*)buf);
		free(buf);
	}

	MainThread.active = 1;
	MainThread.ended = 0;
	MainThread.handle = _beginthreadex(0, 0, GameMain, 0, 0, (unsigned int*)&MainThread.address);
	WinProcMsg();
	MainThread.ended = 1;
	while (MainThread.active) {};

	if (cutseqpakPtr)
		free(cutseqpakPtr);

	WinClose();
	desktop = GetDesktopWindow();
	hdc = GetDC(desktop);
	devmode.dmSize = sizeof(DEVMODE);
	devmode.dmBitsPerPel = App.Desktopbpp;
	ReleaseDC(desktop, hdc);
	devmode.dmFields = DM_BITSPERPEL;
	ChangeDisplaySettings(&devmode, 0);
	CoUninitialize();
	return 0;
}
