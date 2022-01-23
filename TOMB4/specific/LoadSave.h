#pragma once
#include "../global/vars.h"

void inject_loadsave(bool replace);

void S_DrawHealthBar(long pos);
void S_DrawAirBar(long pos);
void S_DrawDashBar(long pos);
void S_InitLoadBar(long maxpos);
void S_LoadBar();
void DoBar(long x, long y, long width, long height, long pos, long clr1, long clr2);
void DoOptions();

#define S_LoadSave	( (long(__cdecl*)(long, long)) 0x0047CD20 )
#define S_PauseMenu	( (long(__cdecl*)()) 0x0047CC60 )
#define DoSlider	( (void(__cdecl*)(long, long, long, long, long, long, long, long)) 0x0047AB80 )
#define CheckKeyConflicts	( (void(__cdecl*)()) 0x0047B130 )
#define CreateMonoScreen	( (void(__cdecl*)()) 0x00479F20 )
#define S_DisplayMonoScreen	( (void(__cdecl*)()) 0x0047A500 )
#define FreeMonoScreen	( (void(__cdecl*)()) 0x00479F40 )
