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
void ScaleCurrentMatrix(PHD_VECTOR* vec);
void SetupZRange(long znear, long zfar);
void InitWindow(long x, long y, long w, long h, long znear, long zfar, long fov, long a, long b);

#define phd_LookAt	( (void(__cdecl*)(long, long, long, long, long, long, short)) 0x00490110 )
