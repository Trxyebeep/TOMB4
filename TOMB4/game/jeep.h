#pragma once
#include "../global/vars.h"

void inject_jeep(bool replace);

void InitialiseJeep(short item_number);
void DrawJeepExtras(ITEM_INFO* item);
void JeepExplode(ITEM_INFO* item);
void JeepCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
long GetCollisionAnim(ITEM_INFO* item, PHD_VECTOR* pos, BIKEINFO* vehicle);
long DoShift(ITEM_INFO* item, PHD_VECTOR* newPos, PHD_VECTOR* oldPos);

#define JeepControl	( (void(__cdecl*)(short)) 0x00467380 )
#define JeepStart	( (void(__cdecl*)(ITEM_INFO*, ITEM_INFO*)) 0x0046A620 )
