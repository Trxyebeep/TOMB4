#pragma once
#include "../global/vars.h"

void inject_switch(bool replace);

void FullBlockSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
long SwitchTrigger(short item_number, short timer);
long GetSwitchTrigger(ITEM_INFO* item, short* ItemNos, long AttatchedToSwitch);

#define TestTriggersAtXYZ	( (void(__cdecl*)(long, long, long, short, short, short)) 0x00461CA0 )
