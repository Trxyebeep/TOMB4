#pragma once
#include "../global/vars.h"

void inject_setup(bool replace);

void ObjectObjects();
void TrapObjects();

#define BuildOutsideTable	( (void(__cdecl*)()) 0x0045ED30 )

