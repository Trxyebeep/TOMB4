#pragma once
#include "../global/vars.h"

void inject_laraflar(bool replace);

void DrawFlareInAir(ITEM_INFO* item);

#define CreateFlare	( (void(__cdecl*)(short, long)) 0x0042F880 )
#define undraw_flare_meshes	( (void(__cdecl*)()) 0x0042FF30 )
