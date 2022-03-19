#pragma once
#include "../global/vars.h"

extern short SPxzoffs[8];
extern short SPyoffs[8];

void inject_traps(bool replace);

void FlameEmitterControl(short item_number);

#define LaraBurn	( (void(__cdecl*)()) 0x00414EE0 )
#define CloseTrapDoor	( (void(__cdecl*)(ITEM_INFO*)) 0x004135A0 )
