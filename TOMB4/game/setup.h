#pragma once
#include "../global/vars.h"

void inject_setup(bool replace);

void ObjectObjects();
void TrapObjects();
void BaddyObjects();
void BuildOutsideTable();
void reset_cutseq_vars();
void ClearFootPrints();
void InitialiseGameFlags();
void InitialiseLara();

#define InitialiseObjects	( (void(__cdecl*)()) 0x0045C0D0 )
#define SetupGame	( (void(__cdecl*)()) 0x0045F0A0 )
