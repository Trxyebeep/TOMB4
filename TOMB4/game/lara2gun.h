#pragma once
#include "../global/vars.h"

void inject_lara2gun(bool replace);

void undraw_pistol_mesh_left(long weapon_type);
void undraw_pistol_mesh_right(long weapon_type);

#define draw_pistol_meshes	( (void(__cdecl*)(long)) 0x0042BB20 )
