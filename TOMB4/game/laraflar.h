#pragma once
#include "../global/types.h"

void DrawFlareInAir(ITEM_INFO* item);
void draw_flare_meshes();
void undraw_flare_meshes();
long DoFlareLight(PHD_VECTOR* pos, long flare_age);
void DoFlareInHand(long flare_age);
void CreateFlare(short object, long thrown);
void set_flare_arm(long frame);
void ready_flare();
void draw_flare();
void undraw_flare();
void FlareControl(short item_number);
