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
short CreateEffect(short room_num);
void KillEffect(short fx_num);

#define EffectNewRoom	( (void(__cdecl*)(short, short)) 0x00454A90 )
