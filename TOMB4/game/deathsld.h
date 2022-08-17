#pragma once
#include "../global/types.h"

void inject_deathslide(bool replace);

void InitialiseDeathSlide(short item_number);
void DeathSlideCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void ControlDeathSlide(short item_number);
