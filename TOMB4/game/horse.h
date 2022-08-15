#pragma once
#include "../global/vars.h"

void inject_horse(bool replace);

void InitialiseHorseman(short item_number);
void HorsemanControl(short item_number);
void TriggerHorsemanRicochets(PHD_VECTOR* pos, long yrot, long num);
void InitialiseHorse(short item_number);
