#pragma once
#include "../global/vars.h"

void inject_effects(bool replace);

void SetFog(ITEM_INFO* item);

#define SoundEffects	( (void(__cdecl*)()) 0x004370E0 )
#define KillActiveBaddies	( (void(__cdecl*)(ITEM_INFO*)) 0x00437930 )
#define DoBloodSplat	( (short(__cdecl*)(long, long, long, short, short, short)) 0x00437180 )
