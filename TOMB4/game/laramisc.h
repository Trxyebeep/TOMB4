#pragma once
#include "../global/vars.h"

void inject_laramisc(bool replace);

void LaraCheatGetStuff();
void LaraCheatyBits();
void InitialiseLaraLoad(short item_number);
void InitialiseLaraAnims(ITEM_INFO* item);
void LaraInitialiseMeshes();
void AnimateLara(ITEM_INFO* item);

#define LaraControl	( (void(__cdecl*)(short)) 0x00430250 )
