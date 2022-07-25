#pragma once
#include "../global/vars.h"

void inject_lot(bool replace);

void InitialiseLOTarray(long allocmem);

#define DisableBaddieAI	( (void(__cdecl*)(short)) 0x00455270 )
#define EnableBaddieAI	( (long(__cdecl*)(short, long)) 0x004552B0 )
#define CreateZone	( (void(__cdecl*)(ITEM_INFO*)) 0x004556A0 )
