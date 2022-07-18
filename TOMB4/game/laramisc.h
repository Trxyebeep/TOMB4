#pragma once
#include "../global/vars.h"

void inject_laramisc(bool replace);

void LaraCheatGetStuff();
void LaraCheatyBits();
void InitialiseLaraLoad(short item_number);
void InitialiseLaraAnims(ITEM_INFO* item);
void LaraInitialiseMeshes();

#define LaraControl	( (void(__cdecl*)(short)) 0x00430250 )
#define AnimateLara	( (void(__cdecl*)(ITEM_INFO*)) 0x00430B60 )
