#pragma once
#include "../global/vars.h"

void inject_input(bool replace);

long Key(long number);
long S_UpdateInput();

#define ReadJoystick	( (long(__cdecl*)(long&, long&)) 0x004777E0 )
