#pragma once
#include "../global/vars.h"

void inject_specificfx(bool replace);

void DrawTrainStrips();
void S_PrintShadow(short size, short* box, ITEM_INFO* item);

#define DrawBikeSpeedo	( (void(__cdecl*)(long, long, long, long, long, long, long)) 0x0048C6C0 )
#define DrawTrainFloorStrip	( (void(__cdecl*)(long, long, TEXTURESTRUCT*, long)) 0x0048B480 )
#define setXYZ3	( (void(__cdecl*)(D3DTLVERTEX*, long, long, long, long, long, long, long, long, long, short*)) 0x00485D90 )
