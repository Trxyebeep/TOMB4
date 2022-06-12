#pragma once
#include "../global/vars.h"

void inject_winmain(bool replace);
void WinProcessCommandLine(LPSTR cmd);

bool WinRunCheck(LPSTR WindowName, LPSTR ClassName, HANDLE* mutex);

#define RestoreFPCW	( (void(__cdecl*)(short)) 0x0048F960 )
#define MungeFPCW	( (long(__cdecl*)(short*)) 0x0048F910 )
#define ClearSurfaces	( (void(__cdecl*)()) 0x0048E8D0 )
