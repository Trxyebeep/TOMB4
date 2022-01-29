#pragma once
#include "../global/vars.h"

void inject_collide(bool replace);

void ShiftItem(ITEM_INFO* item, COLL_INFO* coll);
long GetCollidedObjects(ITEM_INFO* item, long rad, long noInvisible, ITEM_INFO** StoredItems, MESH_INFO** StoredStatics, long StoreLara);

#define LaraBaddieCollision	( (void(__cdecl*)(ITEM_INFO*, COLL_INFO*)) 0x004471B0 )
#define UpdateLaraRoom	( (void(__cdecl*)(ITEM_INFO*, long)) 0x00446FB0 )
#define GetCollisionInfo	( (void(__cdecl*)(COLL_INFO*, long, long, long, short, long)) 0x00446170 )
#define	TestBoundsCollide	( (long(__cdecl*)(ITEM_INFO*, ITEM_INFO*, long)) 0x00447B00 )
#define	ItemPushLara	( (long(__cdecl*)(ITEM_INFO*, ITEM_INFO*, COLL_INFO*, long, long)) 0x00447750 )
#define TestLaraPosition	( (long(__cdecl*)(short*, ITEM_INFO*, ITEM_INFO*)) 0x00447F30 )
