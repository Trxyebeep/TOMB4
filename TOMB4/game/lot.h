#pragma once
#include "../global/vars.h"

void inject_lot(bool replace);

void InitialiseLOTarray(long allocmem);
void DisableBaddieAI(short item_number);
void ClearLOT(LOT_INFO* lot);

#define EnableBaddieAI	( (long(__cdecl*)(short, long)) 0x004552B0 )
#define CreateZone	( (void(__cdecl*)(ITEM_INFO*)) 0x004556A0 )
