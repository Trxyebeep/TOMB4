#pragma once
#include "../global/vars.h"

void inject_laraclmb(bool replace);

void lara_as_climbstnc(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_climbleft(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_climbleft(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_climbright(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_climbright(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_climbing(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_climbdown(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_climbend(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_climbstnc(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_climbing(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_climbdown(ITEM_INFO* item, COLL_INFO* coll);
short GetClimbTrigger(long x, long y, long z, short room_number);
long LaraTestClimb(long x, long y, long z, long xfront, long zfront, long item_height, short item_room, long* shift);

#define LaraCheckForLetGo	( (long(__cdecl*)(ITEM_INFO*, COLL_INFO*)) 0x0042C980 )
#define	LaraDoClimbLeftRight	( (void(__cdecl*)(ITEM_INFO*, COLL_INFO*, long, long)) 0x0042CD20 )
#define LaraTestClimbPos	( (long(__cdecl*)(ITEM_INFO*, long, long, long, long, long*)) 0x0042C3C0 )
#define	LaraTestClimbUpPos	( (long(__cdecl*)(ITEM_INFO*, long, long, long*, long*)) 0x0042CA60 )
