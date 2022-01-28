#pragma once
#include "../global/vars.h"

void inject_footprint(bool replace);

void AddFootPrint(ITEM_INFO* item);
#ifdef FOOTPRINTS
void S_DrawFootPrints();
#endif