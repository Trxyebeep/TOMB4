#pragma once
#include "../global/types.h"

void TriggerHarpyMissileFlame(short fx_number, long xv, long yv, long zv);
void TriggerHarpyMissile(PHD_3DPOS* pos, short room_number, short mesh);
void TriggerHarpySparks(long x, long y, long z, short xv, short yv, short zv);
void TriggerHarpyFlame(short item_number, uchar NodeNumber, short size);
void DoHarpyEffects(ITEM_INFO* item, short item_number);
void InitialiseHarpy(short item_number);
void HarpyControl(short item_number);
