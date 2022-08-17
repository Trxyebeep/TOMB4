#pragma once
#include "../global/types.h"

void inject_baboon(bool replace);

void InitialiseBaboon(short item_number);
void BaboonControl(short item_number);
void FindCrowbarSwitch(ITEM_INFO* item, short switch_index);
void ReTriggerBaboon(short item_number);
void ExplodeBaboon(ITEM_INFO* item);
