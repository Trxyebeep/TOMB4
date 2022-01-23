#pragma once
#include "../global/vars.h"

#define game_malloc	( (void*(__cdecl*)(long)) 0x00490F80 )
#define GetRandomDraw	( (long(__cdecl*)()) 0x00490F00 )
#define GetRandomControl	( (long(__cdecl*)()) 0x00490EE0 )
#define Log	( (void(__cdecl*)(ulong, const char*, ...)) 0x0049C750 )//wrong place, idk where it goes
#define PlayFmvNow	( (long(__cdecl*)()) 0x0049C3E0 )//wrong place, idk where it goes