#pragma once
#include "../global/vars.h"

void inject_switch(bool replace);

void FullBlockSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);

#define GetSwitchTrigger	( (long(__cdecl*)(ITEM_INFO*, short*, long)) 0x00461BD0 )
#define TestTriggersAtXYZ	( (void(__cdecl*)(long, long, long, short, short, short)) 0x00461CA0 )
