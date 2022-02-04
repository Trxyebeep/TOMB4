#pragma once
#include "../global/vars.h"

void inject_laramisc(bool replace);

void LaraCheatGetStuff();
void LaraCheatyBits();

#define LaraControl	( (void(__cdecl*)(short)) 0x00430250 )
#define LaraInitialiseMeshes	( (void(__cdecl*)()) 0x00430140 )
#define AnimateLara	( (void(__cdecl*)(ITEM_INFO*)) 0x00430B60 )
#define InitialiseLaraAnims	( (void(__cdecl*)(ITEM_INFO*)) 0x00430EE0 )
