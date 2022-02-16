#pragma once
#include "../global/vars.h"

void inject_demigod(bool replace);

void TriggerDemigodMissile(PHD_3DPOS* pos, short room_number, short type);
void TriggerDemigodMissileFlame(short fx_number, short xv, short yv, short zv);
void TriggerHammerSmoke(long x, long y, long z, long num);
