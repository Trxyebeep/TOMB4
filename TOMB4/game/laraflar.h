#pragma once
#include "../global/vars.h"

void inject_laraflar(bool replace);

void DrawFlareInAir(ITEM_INFO* item);
void draw_flare_meshes();
void undraw_flare_meshes();
long DoFlareLight(PHD_VECTOR* pos, long flare_age);

#define CreateFlare	( (void(__cdecl*)(short, long)) 0x0042F880 )
