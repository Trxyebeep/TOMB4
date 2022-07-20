#pragma once
#include "../global/vars.h"

void inject_text(bool replace);

void InitFont();
void UpdatePulseColour();
long GetStringLength(char* string, short* top, short* bottom);

#define PrintString	( (void(__cdecl*)(ushort, ushort, uchar, const char*, ushort)) 0x00463B50 )

#ifdef GENERAL_FIXES
extern long stash_font_height;
extern long smol_font_height;
#endif
