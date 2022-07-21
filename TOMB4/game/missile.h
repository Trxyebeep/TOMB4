#pragma once
#include "../global/vars.h"

void inject_missile(bool replace);

long ExplodeFX(FX_INFO* fx, long NoXZVel, short Num);
void ControlBodyPart(short fx_number);
