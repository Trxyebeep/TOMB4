#pragma once
#include "../global/vars.h"

void inject_laraflar(bool replace);

void DrawFlareInAir(ITEM_INFO* item);
void draw_flare_meshes();
void undraw_flare_meshes();
long DoFlareLight(PHD_VECTOR* pos, long flare_age);
void DoFlareInHand(long flare_age);
void CreateFlare(short object, long thrown);
void set_flare_arm(long frame);
