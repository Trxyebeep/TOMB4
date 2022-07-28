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
void ControlSprinkler(short item_number);
void ControlMineHelicopter(short item_number);
void MineCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void FallingSquishyBlockCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void ControlFallingSquishyBlock(short item_number);
void ControlLRSquishyBlock(short item_number);
void ControlSethBlade(short item_number);
void ControlPlinthBlade(short item_number);
void ControlMovingBlade(short item_number);
void ControlCatwalkBlade(short item_number);
void ControlBirdBlade(short item_number);
void Control4xFloorRoofBlade(short item_number);
void ControlSpikeball(short item_number);
void ControlHammer(short item_number);
void ControlStargate(short item_number);
void ControlPlough(short item_number);
void ControlChain(short item_number);
void ControlBurningFloor(short item_number);
void ControlRaisingBlock(short item_number);
void ControlScaledSpike(short item_number);
void FlameEmitter3Control(short item_number);
void FlameControl(short fx_number);
void FlameEmitter2Control(short item_number);

#define LaraBurn	( (void(__cdecl*)()) 0x00414EE0 )
#define CloseTrapDoor	( (void(__cdecl*)(ITEM_INFO*)) 0x004135A0 )
#define TestBoundsCollideTeethSpikes	( (long(__cdecl*)(ITEM_INFO*)) 0x00415EC0 )
#define LavaBurn	( (void(__cdecl*)(ITEM_INFO*)) 0x00414F30 )
