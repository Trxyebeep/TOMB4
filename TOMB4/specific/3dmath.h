#pragma once
#include "../global/vars.h"

void inject_3dmath(bool replace);

void phd_PushMatrix();
void phd_PushUnitMatrix();
long phd_TranslateRel(long x, long y, long z);
void phd_RotX(short angle);
void phd_RotY(short angle);
void phd_RotZ(short angle);
void phd_RotYXZpack(long angles);
void phd_RotYXZ(short y, short x, short z);
void phd_TranslateAbs(long x, long y, long z);
void phd_GetVectorAngles(long x, long y, long z, short* angles);
ulong mGetAngle(long x, long z, long x1, long z1);
void AlterFOV(short fov);
long phd_atan(long x, long y);
ulong phd_sqrt(ulong num);

#define phd_LookAt	( (void(__cdecl*)(long, long, long, long, long, long, short)) 0x00490110 )
#define ScaleCurrentMatrix	( (void(__cdecl*)(PHD_VECTOR*)) 0x0048FB60 )
#define InitWindow	(  (void(__cdecl*)(long, long, long, long, long, long, long, long, long)) 0x0048FC10 )
