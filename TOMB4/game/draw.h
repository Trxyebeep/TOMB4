#pragma once
#include "../global/vars.h"

void inject_draw(bool replace);

void InitInterpolate(long frac, long rate);
void phd_PopMatrix_I();
void phd_PushMatrix_I();
void phd_RotY_I(short ang);
void phd_RotX_I(short ang);
void phd_RotZ_I(short ang);
void phd_TranslateRel_I(long x, long y, long z);
void phd_TranslateRel_ID(long x, long y, long z, long x2, long y2, long z2);
void phd_RotYXZ_I(short y, short x, short z);
void gar_RotYXZsuperpack_I(short** pprot1, short** pprot2, long skip);
void gar_RotYXZsuperpack(short** pprot, long skip);
void phd_PutPolygons_I(short* ptr, long clip);
void InterpolateMatrix();
void InterpolateArmMatrix(long* mx);

#define GetFrames	( (long(__cdecl*)(ITEM_INFO*, short*[], long*)) 0x00450DC0 )
#define GetBestFrame	( (short*(__cdecl*)(ITEM_INFO*)) 0x00450EE0 )
#define GetBoundsAccurate	( (short*(__cdecl*)(ITEM_INFO*)) 0x00450E60 )
#define CalculateObjectLighting	( (void(__cdecl*)(ITEM_INFO*, short*)) 0x00450BB0 )
#define CalculateObjectLightingLara	( (void(__cdecl*)()) 0x00450CB0 )
