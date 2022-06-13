#pragma once
#include "../global/vars.h"

#define KillItem	( (void(__cdecl*)(short)) 0x004541B0 )
#define RemoveActiveItem	( (void(__cdecl*)(short)) 0x004545F0 )
#define ItemNewRoom	( (void(__cdecl*)(short, short)) 0x004547B0 )
#define CreateEffect	( (short(__cdecl*)(short)) 0x00454900 )
#define InitialiseFXArray	( (void(__cdecl*)(long)) 0x004548B0 )
#define AddActiveItem	( (void(__cdecl*)(short)) 0x00454740 )
#define CreateItem	( (short(__cdecl*)()) 0x00454360 )
#define InitialiseItem	( (void(__cdecl*)(short)) 0x004543B0 )
#define RemoveDrawnItem	( (void(__cdecl*)(short)) 0x004546A0 )
#define InitialiseItemArray	( (void(__cdecl*)(long)) 0x00454140 )
