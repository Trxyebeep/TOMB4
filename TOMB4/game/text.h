#pragma once
#include "../global/vars.h"

#define UpdatePulseColour	( (void(__cdecl*)()) 0x00463930 )
#define PrintString	( (void(__cdecl*)(ushort, ushort, uchar, const char*, ushort)) 0x00463B50 )
#define InitFont	( (void(__cdecl*)()) 0x00463650 )
#define GetStringLength	( (long(__cdecl*)(char*, short*, short*)) 0x004639E0 )
