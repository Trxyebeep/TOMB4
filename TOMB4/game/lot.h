#pragma once
#include "../global/types.h"

void InitialiseLOTarray(long allocmem);
void DisableBaddieAI(short item_number);
void ClearLOT(LOT_INFO* lot);
void CreateZone(ITEM_INFO* item);
void InitialiseSlot(short item_number, long slot);
long EnableBaddieAI(short item_number, long Always);

extern CREATURE_INFO* baddie_slots;
