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

#define GetCollisionInfo	( (void(__cdecl*)(COLL_INFO*, long, long, long, short, long)) 0x00446170 )
#define	TestBoundsCollide	( (long(__cdecl*)(ITEM_INFO*, ITEM_INFO*, long)) 0x00447B00 )
#define	TestBoundsCollideStatic	( (long(__cdecl*)(short*, PHD_3DPOS*, long)) 0x00447BE0 )
#define	ItemPushLara	( (long(__cdecl*)(ITEM_INFO*, ITEM_INFO*, COLL_INFO*, long, long)) 0x00447750 )
#define TestLaraPosition	( (long(__cdecl*)(short*, ITEM_INFO*, ITEM_INFO*)) 0x00447F30 )
#define MoveLaraPosition	( (long(__cdecl*)(PHD_VECTOR*, ITEM_INFO*, ITEM_INFO*)) 0x004483E0 )
#define AlignLaraPosition	( (void(__cdecl*)(PHD_VECTOR*, ITEM_INFO*, ITEM_INFO*)) 0x00448070 )
#define ItemPushLaraStatic	( (void(__cdecl*)(ITEM_INFO*, short*, PHD_3DPOS*, COLL_INFO*)) 0x00447CE0 )
