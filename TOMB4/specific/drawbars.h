#pragma once
#include "../global/types.h"

void S_DrawHealthBar(long pos);
void S_DrawAirBar(long pos);
void S_DrawDashBar(long pos);
void S_DrawEnemyBar(long pos);
void DoSlider(long x, long y, long width, long height, long pos, long c1, long c2, long c3);
void S_InitLoadBar(long maxpos);
void S_LoadBar();
