#pragma once
#include "../global/types.h"

void TriggerSethMissileFlame(short fx_number, long xv, long yv, long zv);
void TriggerSethMissile(PHD_3DPOS* pos, short room_number, short type);
void TriggerSethSparks(long x, long y, long z, short xv, short yv, short zv);
void TriggerSethFlame(short item_number, uchar NodeNumber, short size);
void DoSethEffects(short item_number);
void InitialiseSeth(short item_number);
void SethControl(short item_number);
