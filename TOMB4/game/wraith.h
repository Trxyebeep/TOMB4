#pragma once
#include "../global/vars.h"

void inject_wraith(bool replace);

void InitialiseWraith(short item_number);
void TriggerWraithFlame(long x, long y, long z, short xv, short yv, short zv, long objnum);
void TriggerWraithEffect(long x, long y, long z, short vel, long objnum);
void WraithControl(short item_number);
