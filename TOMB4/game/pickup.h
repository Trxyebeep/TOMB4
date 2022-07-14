#pragma once
#include "../global/vars.h"

void inject_pickup(bool replace);

void SarcophagusCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void KeyHoleCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void PuzzleDoneCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll);
void PuzzleDone(ITEM_INFO* item, short item_number);
void AnimatingPickUp(short item_number);

#define RegeneratePickups	( (void(__cdecl*)()) 0x00457650 )
#define PickUpCollision	( (void(__cdecl*)(short, ITEM_INFO*, COLL_INFO*)) 0x00457720 )
#define KeyTrigger	( (long(__cdecl*)(short)) 0x00458710 )
#define PickupTrigger	( (long(__cdecl*)(short)) 0x00458780 )
