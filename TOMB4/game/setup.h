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
void InitialiseObjects();
void GetAIPickups();
void GetCarriedItems();

#define SetupGame	( (void(__cdecl*)()) 0x0045F0A0 )
