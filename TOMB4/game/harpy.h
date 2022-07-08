#pragma once
#include "../global/vars.h"

void inject_harpy(bool replace);

void TriggerHarpyMissileFlame(short fx_number, long xv, long yv, long zv);
void TriggerHarpyMissile(PHD_3DPOS* pos, short room_number, short mesh);
