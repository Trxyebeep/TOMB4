#pragma once
#include "../global/vars.h"

void inject_senet(bool replace);

void InitialiseSenet(short item_number);
void MakeMove(long piece, long displacement);
void SenetControl(short item_number);
long CheckSenetWinner(long won);

#define ShockwaveExplosion	( (void(__cdecl*)(ITEM_INFO*, ulong, long)) 0x0040FBD0 )
