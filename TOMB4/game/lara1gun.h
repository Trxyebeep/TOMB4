#pragma once
#include "../global/types.h"

void DoGrenadeDamageOnBaddie(ITEM_INFO* baddie, ITEM_INFO* item);
void FireCrossbow(PHD_3DPOS* pos);
void draw_shotgun_meshes(long weapon_type);
void undraw_shotgun_meshes(long weapon_type);
void ready_shotgun(long weapon_type);
void FireShotgun();
void FireGrenade();
void AnimateShotgun(long weapon_type);
void RifleHandler(long weapon_type);
void CrossbowHitSwitchType78(ITEM_INFO* item, ITEM_INFO* target, long MustHitLastNode);
void TriggerUnderwaterExplosion(ITEM_INFO* item, long vehicle);
void draw_shotgun(long weapon_type);
void undraw_shotgun(long weapon_type);
void ControlCrossbow(short item_number);
void ControlGrenade(short item_number);
