#pragma once
#include "../global/vars.h"

void inject_people(bool replace);

short GunShot(long x, long y, long z, short speed, short yrot, short room_number);
short GunHit(long x, long y, long z, short speed, short yrot, short room_number);

#define TargetVisible	( (long(__cdecl*)(ITEM_INFO*, AI_INFO*)) 0x0040AEB0 )
#define Targetable	( (long(__cdecl*)(ITEM_INFO*, AI_INFO*)) 0x0040AF80 )
#define ShotLara	( (void(__cdecl*)(ITEM_INFO*, AI_INFO*, BITE_INFO*, short, long)) 0x0040B1D0 )
