#pragma once
#include "../global/vars.h"

void inject_jeep(bool replace);

void InitialiseJeep(short item_number);
void DrawJeepExtras(ITEM_INFO* item);
void JeepExplode(ITEM_INFO* item);

#define JeepControl	( (void(__cdecl*)(short)) 0x00467380 )
#define JeepStart	( (void(__cdecl*)(ITEM_INFO*, ITEM_INFO*)) 0x0046A620 )
#define DoShift	( (long(__cdecl*)(ITEM_INFO*, PHD_VECTOR*, PHD_VECTOR*)) 0x00468B80 )
#define GetCollisionAnim	( (long(__cdecl*)(ITEM_INFO*, PHD_VECTOR*, BIKEINFO*)) 0x00468AE0 )
