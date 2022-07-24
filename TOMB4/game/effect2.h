#pragma once
#include "../global/vars.h"

void inject_effect2(bool replace);

void ControlSmokeEmitter(short item_number);
void TriggerExplosionSmokeEnd(long x, long y, long z, long uw);
void TriggerExplosionSmoke(long x, long y, long z, long uw);
void TriggerFlareSparks(long x, long y, long z, long xvel, long yvel, long zvel, long smoke);
void TriggerDynamic(long x, long y, long z, long falloff, long r, long g, long b);
void TriggerDynamic_MIRROR(long x, long y, long z, long falloff, long r, long g, long b);
void ClearDynamics();
void ControlEnemyMissile(short fx_number);
void SetupRipple(long x, long y, long z, long size, long flags);
void TriggerUnderwaterBlood(long x, long y, long z, long size);
void TriggerWaterfallMist(long x, long y, long z, long ang);
void TriggerDartSmoke(long x, long y, long z, long xv, long zv, long hit);
void KillAllCurrentItems(short item_number);
void KillEverything();
void TriggerExplosionBubble(long x, long y, long z, short room_number);
void ControlColouredLights(short item_number);
void DetatchSpark(long num, long type);
long GetFreeSpark();
void UpdateSparks();
void TriggerRicochetSpark(GAME_VECTOR* pos, long ang, long num, long smoke_only);
void TriggerExplosionSparks(long x, long y, long z, long extras, long dynamic, long uw, short room_number);

#define UpdateSplashes	( (void(__cdecl*)()) 0x004359E0 )
#define TriggerFireFlame	( (void(__cdecl*)(long, long, long, long, long)) 0x00435130 )
#define TriggerSuperJetFlame	( (void(__cdecl*)(ITEM_INFO*, long, long)) 0x00435570 )
#define SetupSplash	( (void(__cdecl*)(SPLASH_SETUP*)) 0x00435920 )
