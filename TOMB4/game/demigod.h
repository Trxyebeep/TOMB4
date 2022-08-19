#pragma once
#include "../global/types.h"

void TriggerDemigodMissile(PHD_3DPOS* pos, short room_number, short type);
void TriggerDemigodMissileFlame(short fx_number, long xv, long yv, long zv);
void TriggerHammerSmoke(long x, long y, long z, long num);
void DoDemigodEffects(short item_number);
void InitialiseDemigod(short item_number);
void DemigodControl(short item_number);
