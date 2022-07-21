#pragma once
#include "../global/vars.h"

void inject_items(bool replace);

void InitialiseItemArray(short num);
void KillItem(short item_num);
short CreateItem();
void InitialiseItem(short item_num);
void RemoveActiveItem(short item_num);
void RemoveDrawnItem(short item_num);
void AddActiveItem(short item_num);
void ItemNewRoom(short item_num, short room_num);
void InitialiseFXArray(long allocmem);

#define KillEffect	( (void(__cdecl*)(short)) 0x00454970 )
#define EffectNewRoom	( (void(__cdecl*)(short, short)) 0x00454A90 )
#define CreateEffect	( (short(__cdecl*)(short)) 0x00454900 )
