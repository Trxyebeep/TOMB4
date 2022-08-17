#pragma once
#include "../global/types.h"

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
long MungeFPCW(short* fpcw);
void RestoreFPCW(short fpcw);

extern WINAPP App;
extern char* cutseqpakPtr;
extern long resChangeCounter;
