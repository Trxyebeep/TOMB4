#pragma once
#include "../global/vars.h"

void inject_lara2gun(bool replace);

void undraw_pistol_mesh_left(long weapon_type);
void undraw_pistol_mesh_right(long weapon_type);
void ready_pistols(long weapon_type);
void draw_pistol_meshes(long weapon_type);
void draw_pistols(long weapon_type);
void undraw_pistols(long weapon_type);
