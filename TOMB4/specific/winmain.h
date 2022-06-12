#pragma once
#include "../global/vars.h"

void inject_winmain(bool replace);

bool WinRunCheck(LPSTR WindowName, LPSTR ClassName, HANDLE* mutex);
void WinProcessCommandLine(LPSTR cmd);
void WinClose();
float WinFrameRate();
void WinDisplayString(long x, long y, char* string, ...);
long CheckMMXTechnology();
void WinProcMsg();
void WinProcessCommands(long cmd);
LRESULT CALLBACK WinMainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void ClearSurfaces();
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd);

#define RestoreFPCW	( (void(__cdecl*)(short)) 0x0048F960 )
#define MungeFPCW	( (long(__cdecl*)(short*)) 0x0048F910 )
