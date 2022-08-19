#pragma once
#include "../global/types.h"

void InitialiseInjuredSas(short item_number);
void InjuredSasControl(short item_number);
void DragSASCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void InitialiseSas(short item_number);
void SasControl(short item_number);
