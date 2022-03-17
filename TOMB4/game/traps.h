#pragma once
#include "../global/vars.h"

void inject_traps(bool replace);

void FlameEmitterControl(short item_number);

#define LaraBurn	( (void(__cdecl*)()) 0x00414EE0 )
#define CloseTrapDoor	( (void(__cdecl*)(ITEM_INFO*)) 0x004135A0 )
