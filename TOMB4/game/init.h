#pragma once
#include "../global/types.h"

void InitialiseMapper(short item_number);
void InitialiseLightningConductor(short item_number);
void InitialiseDoor(short item_number);
void InitialiseTrapDoor(short item_number);
void InitialiseFallingBlock2(short item_number);
void InitialiseFlameEmitter(short item_number);
void InitialiseFlameEmitter2(short item_number);
void InitialiseFlameEmitter3(short item_number);
void InitialiseJobySpike(short item_number);
void InitialiseTwoBlockPlatform(short item_number);
void InitialiseSlicerDicer(short item_number);
void InitialiseScaledSpike(short item_number);
void InitialiseRaisingBlock(short item_number);
void InitialiseBurningFloor(short item_number);
void InitialiseSethBlade(short item_number);
void InitialiseObelisk(short item_number);
void InitialiseMineHelicopter(short item_number);
void InitialiseSmashObject(short item_number);
void InitialiseStatuePlinth(short item_number);
void InitialiseSmokeEmitter(short item_number);
void InitialisePulley(short item_number);
void InitialisePickUp(short item_number);
void CreateRope(ROPE_STRUCT* rope, PHD_VECTOR* pos, PHD_VECTOR* dir, long slength, ITEM_INFO* item);
void InitialiseRope(short item_number);
void init_all_ropes();
void InitialiseEffects();
