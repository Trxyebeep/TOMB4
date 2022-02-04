#pragma once
#include "../global/vars.h"

void inject_specificfx(bool replace);

void DrawTrainStrips();
void S_PrintShadow(short size, short* box, ITEM_INFO* item);
void S_DrawDrawSparks(SPARKS* sptr, long smallest_size, short* xyptr, long* zptr);

#define setXY4	( (void(__cdecl*)(D3DTLVERTEX*, long, long, long, long, long, long, long, long, long, short*)) 0x00485600 )
#define setXYZ3	( (void(__cdecl*)(D3DTLVERTEX*, long, long, long, long, long, long, long, long, long, short*)) 0x00485D90 )

#define DrawBikeSpeedo	( (void(__cdecl*)(long, long, long, long, long, long, long)) 0x0048C6C0 )
#define DrawTrainFloorStrip	( (void(__cdecl*)(long, long, TEXTURESTRUCT*, long)) 0x0048B480 )
#define DrawRope	( (void(__cdecl*)(ROPE_STRUCT*)) 0x00489540 )
#define DrawFlatSky	( (void(__cdecl*)(ulong, long, long, long)) 0x00488950 )
#define OutputSky	( (void(__cdecl*)()) 0x00489480 )
#define SetUpLensFlare	( (void(__cdecl*)(long, long, long, GAME_VECTOR*)) 0x0048BC30 )
#define DrawLaserSightSprite	( (void(__cdecl*)()) 0x00489950 )
#define S_DrawSparks	( (void(__cdecl*)()) 0x0043A1B0 )
#define DoUwEffect	( (void(__cdecl*)()) 0x0048A840 )
#define S_DrawSmokeSparks	( (void(__cdecl*)()) 0x00487260 )
#define S_DrawSplashes	( (void(__cdecl*)()) 0x00486430 )
#define DrawBubbles	( (void(__cdecl*)()) 0x004860D0 )
#define DrawDebris	( (void(__cdecl*)()) 0x00489B90 )
#define DrawBlood	( (void(__cdecl*)()) 0x0048A0C0 )
#define DrawDrips	( (void(__cdecl*)()) 0x0048A430 )
#define DrawShockwaves	( (void(__cdecl*)()) 0x0048CAA0 )
#define DrawLightning	( (void(__cdecl*)()) 0x0048D9B0 )
#define SetFade	( (void(__cdecl*)(long, long)) 0x0048C240 )
#define ClipLine	( (long(__cdecl*)(long&, long&, long, long&, long&, long, long, long, long, long)) 0x00488690 )
#define ShowTitle	( (void(__cdecl*)()) 0x0048B130 )
