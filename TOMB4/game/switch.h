#pragma once
#include "../global/vars.h"

#define GetSwitchTrigger	( (long(__cdecl*)(ITEM_INFO*, short*, long)) 0x00461BD0 )
#define TestTriggersAtXYZ	( (void(__cdecl*)(long, long, long, short, short, short)) 0x00461CA0 )
