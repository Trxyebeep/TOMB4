#pragma once
#include "../global/types.h"

void TriggerTorchFlame(short item_number, long node);
void FireCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void DoFlameTorch();
void GetFlameTorch();
void FlameTorchControl(short item_number);
