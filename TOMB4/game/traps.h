#pragma once
#include "../global/vars.h"

void inject_traps(bool replace);

void FlameEmitterControl(short item_number);

#define LaraBurn	( (void(__cdecl*)()) 0x00414EE0 )
