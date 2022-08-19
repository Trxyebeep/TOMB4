#pragma once
#include "../global/types.h"

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
void EffectNewRoom(short fx_num, short room_num);

extern short next_fx_active;
extern short next_item_active;
