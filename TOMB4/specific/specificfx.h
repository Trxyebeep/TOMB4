#pragma once
#include "../global/types.h"

void DrawTrainStrips();
void S_PrintShadow(short size, short* box, ITEM_INFO* item);
void S_DrawDrawSparks(SPARKS* sptr, long smallest_size, long* xyptr, long* zptr);
void DrawBikeSpeedo(long ux, long uy, long vel, long maxVel, long turboVel, long size, long unk);
void Draw2DSprite(long x, long y, long slot, long unused, long unused2);
void DrawJeepSpeedo(long ux, long uy, long vel, long maxVel, long turboVel, long size, long spriteSlot);
void DrawDebris();
void DoScreenFade();
void DrawPsxTile(long x_y, long height_width, long color, long u0, long u1);
void DrawFlash();
void S_DrawDarts(ITEM_INFO* item);
void ClipCheckPoint(D3DTLVERTEX* v, float x, float y, float z, short* clip);
void DrawFlatSky(ulong color, long zpos, long ypos, long drawtype);
void OutputSky();
void ProjectTriPoints(PHD_VECTOR* pos, long& x, long& y, long& z);
void setXY4(D3DTLVERTEX* v, long x1, long y1, long x2, long y2, long x3, long y3, long x4, long y4, long z, short* clip);
void setXY3(D3DTLVERTEX* v, long x1, long y1, long x2, long y2, long x3, long y3, long z, short* clip);
void setXYZ4(D3DTLVERTEX* v, long x1, long y1, long z1, long x2, long y2, long z2, long x3, long y3, long z3, long x4, long y4, long z4, short* clip);
void setXYZ3(D3DTLVERTEX* v, long x1, long y1, long z1, long x2, long y2, long z2, long x3, long y3, long z3, short* clip);
void SetFade(long start, long end);
void DrawLaserSightSprite();
void DrawSprite(long x, long y, long slot, long col, long size, long z);
void ShowTitle();
void SetUpLensFlare(long x, long y, long z, GAME_VECTOR* lfobj);
void InitTarget_2();
void InitBinoculars();
void DrawBinoculars();
void DrawWraithTrail(ITEM_INFO* item);
void DrawDrips();
void DrawBubbles();
void DrawShockwaves();
void DrawTrainFloorStrip(long x, long z, TEXTURESTRUCT* tex, long y_and_flags);
void S_DrawSplashes();
bool ClipLine(long& x1, long& y1, long z1, long& x2, long& y2, long z2, long xMin, long yMin, long w, long h);
void S_DrawFireSparks(long size, long life);
void DrawRope(ROPE_STRUCT* rope);
void DrawBlood();
void S_DrawSmokeSparks();
void DoUwEffect();
void DrawLightning();

extern MESH_DATA* targetMeshP;
extern long DoFade;
