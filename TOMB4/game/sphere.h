#pragma once
#include "../global/vars.h"

void inject_sphere(bool replace);

long GetSpheres(ITEM_INFO* item, SPHERE* ptr, long WorldSpace);

#define GetJointAbsPosition	( (void(__cdecl*)(ITEM_INFO*, PHD_VECTOR*, long)) 0x0045FF80 )
#define TestCollision	( (long(__cdecl*)(ITEM_INFO*, ITEM_INFO*)) 0x0045FB10 )
