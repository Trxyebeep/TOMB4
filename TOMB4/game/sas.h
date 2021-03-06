#pragma once
#include "../global/vars.h"

void inject_sas(bool replace);

void InitialiseInjuredSas(short item_number);
void InjuredSasControl(short item_number);
void DragSASCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void InitialiseSas(short item_number);
void SasControl(short item_number);
