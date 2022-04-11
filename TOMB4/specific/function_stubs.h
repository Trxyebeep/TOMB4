#pragma once
#include "../global/vars.h"

void inject_funcstubs(bool replace);

void other_log(char* format, ...);
void S_ExitSystem(const char* string);
long GetRandomControl();
void SeedRandomControl(long seed);
long GetRandomDraw();
void SeedRandomDraw(long seed);
void init_game_malloc();
void* game_malloc(long size);

#define Log	( (void(__cdecl*)(ulong, const char*, ...)) 0x0049C750 )//wrong place, idk where it goes
#define ClearFakeDevice	( (HRESULT(__cdecl*)(LPDIRECT3DDEVICE3, long, D3DRECT*, long, long, float, long)) 0x00494D20 )
