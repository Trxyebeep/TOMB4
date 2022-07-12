#pragma once
#include "../global/vars.h"

extern short SPxzoffs[8];
extern short SPyoffs[8];

void inject_traps(bool replace);

void FlameEmitterControl(short item_number);
void TwoBlockPlatformFloor(ITEM_INFO* item, long x, long y, long z, long* height);
void TwoBlockPlatformCeiling(ITEM_INFO* item, long x, long y, long z, long* height);
void ControlTwoBlockPlatform(short item_number);
void ControlJobySpike(short item_number);
void DrawScaledSpike(ITEM_INFO* item);
void ControlSlicerDicer(short item_number);

#define LaraBurn	( (void(__cdecl*)()) 0x00414EE0 )
#define CloseTrapDoor	( (void(__cdecl*)(ITEM_INFO*)) 0x004135A0 )
#define TestBoundsCollideTeethSpikes	( (long(__cdecl*)(ITEM_INFO*)) 0x00415EC0 )
#define LavaBurn	( (void(__cdecl*)(ITEM_INFO*)) 0x00414F30 )
