#pragma once
#include "../global/vars.h"

void inject_sphere(bool replace);

long GetSpheres(ITEM_INFO* item, SPHERE* ptr, long WorldSpace);
long TestCollision(ITEM_INFO* item, ITEM_INFO* l);
void InitInterpolate2(long frac, long rate);
void GetJointAbsPosition(ITEM_INFO* item, PHD_VECTOR* pos, long joint);

extern SPHERE Slist[34];
extern char GotLaraSpheres;
