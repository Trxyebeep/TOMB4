#pragma once
#include "../global/vars.h"

void inject_input(bool replace);

long Key(long number);

#define S_UpdateInput	( (long(__cdecl*)()) 0x004778B0 )
#define ReadJoystick	( (long(__cdecl*)(long&, long&)) 0x004777E0 )
