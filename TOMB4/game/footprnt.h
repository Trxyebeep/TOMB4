#pragma once
#include "../global/types.h"

void AddFootPrint(ITEM_INFO* item);
#ifdef FOOTPRINTS
void S_DrawFootPrints();
#endif

extern FOOTPRINT FootPrint[];
extern long FootPrintNum;