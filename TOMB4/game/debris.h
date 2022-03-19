#pragma once
#include "../global/vars.h"

void inject_debris(bool replace);

void UpdateDebris();
void TriggerDebris(GAME_VECTOR* pos, void* TextInfo, short* Offsets, long* Vels, short rgb);
long GetFreeDebris();

#define ShatterObject	( (void(__cdecl*)(SHATTER_ITEM*, MESH_INFO*, short, short, long)) 0x00433410 )
