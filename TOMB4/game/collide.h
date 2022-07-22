#pragma once
#include "../global/vars.h"

void inject_collide(bool replace);

void ShiftItem(ITEM_INFO* item, COLL_INFO* coll);
long GetCollidedObjects(ITEM_INFO* item, long rad, long noInvisible, ITEM_INFO** StoredItems, MESH_INFO** StoredStatics, long StoreLara);
void GenericDeadlyBoundingBoxCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void GenericSphereBoxCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void CreatureCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
long FindGridShift(long src, long dst);
short GetTiltType(FLOOR_INFO* floor, long x, long y, long z);
long CollideStaticObjects(COLL_INFO* coll, long x, long y, long z, short room_number, long hite);
void UpdateLaraRoom(ITEM_INFO* item, long height);
void LaraBaddieCollision(ITEM_INFO* l, COLL_INFO* coll);
void ObjectCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void ObjectCollisionNoBigPush(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void TrapCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
long ItemPushLara(ITEM_INFO* item, ITEM_INFO* l, COLL_INFO* coll, long spaz, long BigPush);
long TestBoundsCollide(ITEM_INFO* item, ITEM_INFO* l, long rad);
long TestBoundsCollideStatic(short* bounds, PHD_3DPOS* pos, long rad);
long ItemPushLaraStatic(ITEM_INFO* l, short* bounds, PHD_3DPOS* pos, COLL_INFO* coll);
long TestLaraPosition(short* bounds, ITEM_INFO* item, ITEM_INFO* l);

#define GetCollisionInfo	( (void(__cdecl*)(COLL_INFO*, long, long, long, short, long)) 0x00446170 )
#define MoveLaraPosition	( (long(__cdecl*)(PHD_VECTOR*, ITEM_INFO*, ITEM_INFO*)) 0x004483E0 )
#define AlignLaraPosition	( (void(__cdecl*)(PHD_VECTOR*, ITEM_INFO*, ITEM_INFO*)) 0x00448070 )
