#pragma once
#include "../global/vars.h"

void inject_setup(bool replace);

void ObjectObjects();
void TrapObjects();

#define BuildOutsideTable	( (void(__cdecl*)()) 0x0045ED30 )
#define InitialiseObjects	( (void(__cdecl*)()) 0x0045C0D0 )
#define SetupGame	( (void(__cdecl*)()) 0x0045F0A0 )
#define reset_cutseq_vars	( (void(__cdecl*)()) 0x0045F1C0 )

