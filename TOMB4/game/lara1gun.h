#pragma once
#include "../global/vars.h"

void inject_lara1gun(bool replace);

void DoGrenadeDamageOnBaddie(ITEM_INFO* baddie, ITEM_INFO* item);
void FireCrossbow(PHD_3DPOS* pos);
void draw_shotgun_meshes(long weapon_type);
void undraw_shotgun_meshes(long weapon_type);
void ready_shotgun(long weapon_type);
void FireShotgun();
void FireGrenade();

#define TriggerUnderwaterExplosion	( (void(__cdecl*)(ITEM_INFO*, long)) 0x0042B430 )
