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
void phd_GenerateW2V(PHD_3DPOS* viewPos);
void phd_LookAt(long sx, long sy, long sz, long tx, long ty, long tz, short roll);

__inline short phd_sin(long angle)
{
	angle >>= 3;
	return 4 * rcossin_tbl[angle & 0x1FFE];
}

__inline short phd_cos(long angle)
{
	angle >>= 3;
	return 4 * rcossin_tbl[(angle & 0x1FFE) + 1];
}

__inline void phd_PopMatrix()
{
	phd_mxptr -= indices_count;
}
