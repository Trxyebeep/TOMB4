#pragma once
#include "../global/vars.h"

void inject_winmain(bool replace);

bool WinRunCheck(LPSTR WindowName, LPSTR ClassName, HANDLE* mutex);
void WinProcessCommandLine(LPSTR cmd);
void WinClose();
float WinFrameRate();
void WinDisplayString(long x, long y, char* string, ...);
long CheckMMXTechnology();

#define RestoreFPCW	( (void(__cdecl*)(short)) 0x0048F960 )
#define MungeFPCW	( (long(__cdecl*)(short*)) 0x0048F910 )
#define ClearSurfaces	( (void(__cdecl*)()) 0x0048E8D0 )
