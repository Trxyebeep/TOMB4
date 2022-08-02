#pragma once
#include "../global/vars.h"

void inject_switch(bool replace);

void FullBlockSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
long SwitchTrigger(short item_number, short timer);
long GetSwitchTrigger(ITEM_INFO* item, short* ItemNos, long AttatchedToSwitch);
void TestTriggersAtXYZ(long x, long y, long z, short room_number, short heavy, short flags);
void SwitchControl(short item_number);
void SwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void SwitchCollision2(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void UnderwaterSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
