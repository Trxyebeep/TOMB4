#pragma once
#include "../global/vars.h"

void inject_lara1gun(bool replace);

void DoGrenadeDamageOnBaddie(ITEM_INFO* baddie, ITEM_INFO* item);
void FireCrossbow(PHD_3DPOS* pos);

#define TriggerUnderwaterExplosion	( (void(__cdecl*)(ITEM_INFO*, long)) 0x0042B430 )
#define undraw_shotgun_meshes	( (void(__cdecl*)(long)) 0x00428E70 )
#define draw_shotgun_meshes	( (void(__cdecl*)(long)) 0x00428E40 )
