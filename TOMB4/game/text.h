#pragma once
#include "../global/vars.h"

void inject_text(bool replace);

void InitFont();

#define UpdatePulseColour	( (void(__cdecl*)()) 0x00463930 )
#define PrintString	( (void(__cdecl*)(ushort, ushort, uchar, const char*, ushort)) 0x00463B50 )
#define GetStringLength	( (long(__cdecl*)(char*, short*, short*)) 0x004639E0 )

#ifdef GENERAL_FIXES
extern long stash_font_height;
extern long smol_font_height;
#endif
