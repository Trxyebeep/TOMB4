#pragma once
#include "../global/types.h"

void InitialiseJeep(short item_number);
void DrawJeepExtras(ITEM_INFO* item);
void JeepExplode(ITEM_INFO* item);
void JeepCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
long GetCollisionAnim(ITEM_INFO* item, PHD_VECTOR* pos, BIKEINFO* vehicle);
long DoShift(ITEM_INFO* item, PHD_VECTOR* newPos, PHD_VECTOR* oldPos);
void JeepBaddieCollision(ITEM_INFO* item);
void JeepCollideStaticObjects(long x, long y, long z, short room_number, long height);
long JeepDynamics(ITEM_INFO* item);
void JeepControl(short item_number);
void JeepStart(ITEM_INFO* item, ITEM_INFO* l);
void JeepFireGrenade(ITEM_INFO* item);
void InitialiseEnemyJeep(short item_number);
void EnemyJeepControl(short item_number);
