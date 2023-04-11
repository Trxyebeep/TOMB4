#include "../tomb4/pch.h"
#include "3dmath.h"
#include "d3dmatrix.h"
#include "../game/spotcam.h"
#include "../game/control.h"
#include "function_stubs.h"
#include "../game/lara.h"

float one = 33554432.0F;
float mone = 2048.0F;
float FogStart = float(1024 * 12);
float FogEnd = float(1024 * 20);

float* mMXPtr;
float mW2V[indices_count];
float mMXStack[20 * indices_count];
float fcossin_tbl[65536];

long* phd_mxptr;
long w2v_matrix[indices_count];
long matrix_stack[20 * indices_count];

float f_centerx;
float f_centery;
float f_top;
float f_left;
float f_bottom;
float f_right;
float f_znear;
float f_zfar;
float f_mznear;
float f_mzfar;
float f_persp;
float f_mpersp;
float f_oneopersp;
float f_moneopersp;
float f_perspoznear;
float f_mperspoznear;
float f_moneoznear;
float f_a;
float f_b;
float f_boo;

long phd_winheight;
long phd_winwidth;
long phd_centerx;
long phd_centery;
long phd_top;
long phd_left;
long phd_bottom;
long phd_right;
long phd_znear;
long phd_zfar;
long phd_persp;
short phd_winxmax;
short phd_winxmin;
short phd_winymax;
short phd_winymin;

static D3DMATRIX D3DMW2VMatrix;
static float LfAspectCorrection;

static void mInit()
{
	float* ptr;
	float ang;

	for (int i = 0; i < 65536; i++)
	{
		ptr = &fcossin_tbl[i];
		ang = i * float(M_PI * 2.0F / 65536.0F);
		*ptr = sin(ang);
	}

	mMXPtr = mMXStack;
}

static void mPushMatrix()
{
	mMXPtr[indices_count + M00] = mMXPtr[M00];
	mMXPtr[indices_count + M01] = mMXPtr[M01];
	mMXPtr[indices_count + M02] = mMXPtr[M02];
	mMXPtr[indices_count + M03] = mMXPtr[M03];
	mMXPtr[indices_count + M10] = mMXPtr[M10];
	mMXPtr[indices_count + M11] = mMXPtr[M11];
	mMXPtr[indices_count + M12] = mMXPtr[M12];
	mMXPtr[indices_count + M13] = mMXPtr[M13];
	mMXPtr[indices_count + M20] = mMXPtr[M20];
	mMXPtr[indices_count + M21] = mMXPtr[M21];
	mMXPtr[indices_count + M22] = mMXPtr[M22];
	mMXPtr[indices_count + M23] = mMXPtr[M23];
	mMXPtr += indices_count;
}

static void mPushUnitMatrix()
{
	mMXPtr += indices_count;
	mMXPtr[M00] = 1;
	mMXPtr[M01] = 0;
	mMXPtr[M02] = 0;
	mMXPtr[M03] = 0;
	mMXPtr[M10] = 0;
	mMXPtr[M11] = 1;
	mMXPtr[M12] = 0;
	mMXPtr[M13] = 0;
	mMXPtr[M20] = 0;
	mMXPtr[M21] = 0;
	mMXPtr[M22] = 1;
	mMXPtr[M23] = 0;
}

static void mSetTrans(long x, long y, long z)
{
	mMXPtr[M03] = (float)x;
	mMXPtr[M13] = (float)y;
	mMXPtr[M23] = (float)z;
}

static void mTranslateRel(long x, long y, long z)
{
	mMXPtr[M03] += x * mMXPtr[M00] + y * mMXPtr[M01] + z * mMXPtr[M02];
	mMXPtr[M13] += x * mMXPtr[M10] + y * mMXPtr[M11] + z * mMXPtr[M12];
	mMXPtr[M23] += x * mMXPtr[M20] + y * mMXPtr[M21] + z * mMXPtr[M22];
}

static void mRotX(short angle)
{
	float sin, cos, mx1, mx2;

	if (angle)
	{
		sin = fSin(angle);
		cos = fCos(angle);

		mx1 = cos * mMXPtr[M01] + sin * mMXPtr[M02];
		mx2 = cos * mMXPtr[M02] - sin * mMXPtr[M01];
		mMXPtr[M01] = mx1;
		mMXPtr[M02] = mx2;

		mx1 = cos * mMXPtr[M11] + sin * mMXPtr[M12];
		mx2 = cos * mMXPtr[M12] - sin * mMXPtr[M11];
		mMXPtr[M11] = mx1;
		mMXPtr[M12] = mx2;

		mx1 = cos * mMXPtr[M21] + sin * mMXPtr[M22];
		mx2 = cos * mMXPtr[M22] - sin * mMXPtr[M21];
		mMXPtr[M21] = mx1;
		mMXPtr[M22] = mx2;
	}
}

static void mRotY(short angle)
{
	float sin, cos, mx1, mx2;

	if (angle)
	{
		sin = fSin(angle);
		cos = fCos(angle);

		mx1 = cos * mMXPtr[M00] - sin * mMXPtr[M02];
		mx2 = cos * mMXPtr[M02] + sin * mMXPtr[M00];
		mMXPtr[M00] = mx1;
		mMXPtr[M02] = mx2;

		mx1 = cos * mMXPtr[M10] - sin * mMXPtr[M12];
		mx2 = cos * mMXPtr[M12] + sin * mMXPtr[M10];
		mMXPtr[M10] = mx1;
		mMXPtr[M12] = mx2;

		mx1 = cos * mMXPtr[M20] - sin * mMXPtr[M22];
		mx2 = cos * mMXPtr[M22] + sin * mMXPtr[M20];
		mMXPtr[M20] = mx1;
		mMXPtr[M22] = mx2;
	}
}

static void mRotZ(short angle)
{
	float sin, cos, mx1, mx2;

	if (angle)
	{
		sin = fSin(angle);
		cos = fCos(angle);

		mx1 = cos * mMXPtr[M00] + sin * mMXPtr[M01];
		mx2 = cos * mMXPtr[M01] - sin * mMXPtr[M00];
		mMXPtr[M00] = mx1;
		mMXPtr[M01] = mx2;

		mx1 = cos * mMXPtr[M10] + sin * mMXPtr[M11];
		mx2 = cos * mMXPtr[M11] - sin * mMXPtr[M10];
		mMXPtr[M10] = mx1;
		mMXPtr[M11] = mx2;

		mx1 = cos * mMXPtr[M20] + sin * mMXPtr[M21];
		mx2 = cos * mMXPtr[M21] - sin * mMXPtr[M20];;
		mMXPtr[M20] = mx1;
		mMXPtr[M21] = mx2;
	}
}

static void mRotYXZpack(long angles)
{
	short angle;

	angle = (angles >> 10) & 0x3FF;
	angle <<= 6;

	if (angle)
		mRotY(angle);

	angle = (angles >> 20) & 0x3FF;
	angle <<= 6;

	if (angle)
		mRotX(angle);

	angle = angles & 0x3FF;
	angle <<= 6;

	if (angle)
		mRotZ(angle);
}

static void mRotYXZ(short y, short x, short z)
{
	if (y)
		mRotY(y);

	if (x)
		mRotX(x);

	if (z)
		mRotZ(z);
}

static void mTranslateAbs(long x, long y, long z)
{
	float fx, fy, fz;

	fx = x - mW2V[M03];
	fy = y - mW2V[M13];
	fz = z - mW2V[M23];
	mMXPtr[M03] = fx * mMXPtr[M00] + fy * mMXPtr[M01] + fz * mMXPtr[M02];
	mMXPtr[M13] = fx * mMXPtr[M10] + fy * mMXPtr[M11] + fz * mMXPtr[M12];
	mMXPtr[M23] = fx * mMXPtr[M20] + fy * mMXPtr[M21] + fz * mMXPtr[M22];
}

static void mGenerateW2V(PHD_3DPOS* viewPos)
{
	PHD_VECTOR scalar;
	float sx, cx, sy, cy, sz, cz;

	sx = fSin(viewPos->x_rot);
	cx = fCos(viewPos->x_rot);
	sy = fSin(viewPos->y_rot);
	cy = fCos(viewPos->y_rot);
	sz = fSin(viewPos->z_rot);
	cz = fCos(viewPos->z_rot);
	mMXPtr = mMXStack;

	mW2V[M00] = FTRIGMULT3(sx, sy, sz) + FTRIGMULT2(cy, cz);
	mW2V[M01] = FTRIGMULT2(cx, sz);
	mW2V[M02] = FTRIGMULT3(sx, cy, sz) - FTRIGMULT2(sy, cz);
	mMXPtr[M00] = mW2V[M00];
	mMXPtr[M01] = mW2V[M01];
	mMXPtr[M02] = mW2V[M02];

	mW2V[M10] = FTRIGMULT3(sx, sy, cz) - FTRIGMULT2(cy, sz);
	mW2V[M11] = FTRIGMULT2(cx, cz);
	mW2V[M12] = FTRIGMULT3(sx, cy, cz) + FTRIGMULT2(sy, sz);
	mMXPtr[M10] = mW2V[M10];
	mMXPtr[M11] = mW2V[M11];
	mMXPtr[M12] = mW2V[M12];

	mW2V[M20] = FTRIGMULT2(cx, sy);
	mW2V[M21] = -sx;
	mW2V[M22] = FTRIGMULT2(cx, cy);
	mMXPtr[M20] = mW2V[M20];
	mMXPtr[M21] = mW2V[M21];
	mMXPtr[M22] = mW2V[M22];

	if (lara.dpoisoned != lara.poisoned)
	{
		lara.poisoned += (lara.dpoisoned - lara.poisoned) >> 4;

		if (abs(lara.dpoisoned - lara.poisoned) < 16)
			lara.poisoned = lara.dpoisoned;
	}

	if (lara.poisoned >= 256)
	{
		scalar.x = (lara.poisoned - 256) * ((phd_sin(XSoff1) + phd_sin(XSoff2)) >> 2);
		scalar.y = (lara.poisoned - 256) * ((phd_sin(YSoff1) + phd_sin(YSoff2)) >> 2);
		scalar.z = (lara.poisoned - 256) * ((phd_sin(ZSoff1) + phd_sin(ZSoff2)) >> 2);

		if (scalar.x || scalar.y || scalar.z)
		{
			scalar.x = (scalar.x >> 12) + 0x4000;
			scalar.y = (scalar.y >> 12) + 0x4000;
			scalar.z = (scalar.z >> 12) + 0x4000;
			ScaleCurrentMatrix(&scalar);
		}
	}

	mW2V[M03] = (float)viewPos->x_pos;
	mW2V[M13] = (float)viewPos->y_pos;
	mW2V[M23] = (float)viewPos->z_pos;
	mMXPtr[M03] = mW2V[M03];
	mMXPtr[M13] = mW2V[M13];
	mMXPtr[M23] = mW2V[M23];

	mW2V[M10] = LfAspectCorrection * mMXPtr[M10];
	mW2V[M11] = LfAspectCorrection * mMXPtr[M11];
	mW2V[M12] = LfAspectCorrection * mMXPtr[M12];
	mMXPtr[M10] = mW2V[M10];
	mMXPtr[M11] = mW2V[M11];
	mMXPtr[M12] = mW2V[M12];

	SetD3DMatrix(&D3DMW2VMatrix, mW2V);
	D3DInvCameraMatrix._11 = D3DMW2VMatrix._11;
	D3DInvCameraMatrix._12 = D3DMW2VMatrix._21;
	D3DInvCameraMatrix._13 = D3DMW2VMatrix._31;
	D3DInvCameraMatrix._14 = D3DMW2VMatrix._41;
	D3DInvCameraMatrix._21 = D3DMW2VMatrix._12;
	D3DInvCameraMatrix._22 = D3DMW2VMatrix._22;
	D3DInvCameraMatrix._23 = D3DMW2VMatrix._32;
	D3DInvCameraMatrix._24 = D3DMW2VMatrix._42;
	D3DInvCameraMatrix._31 = D3DMW2VMatrix._13;
	D3DInvCameraMatrix._32 = D3DMW2VMatrix._23;
	D3DInvCameraMatrix._33 = D3DMW2VMatrix._33;
	D3DInvCameraMatrix._34 = D3DMW2VMatrix._43;
	D3DInvCameraMatrix._41 = D3DMW2VMatrix._14;
	D3DInvCameraMatrix._42 = D3DMW2VMatrix._24;
	D3DInvCameraMatrix._43 = D3DMW2VMatrix._34;
	D3DInvCameraMatrix._44 = D3DMW2VMatrix._44;
}

static void mScaleCurrentMatrix(PHD_VECTOR* vec)
{
	float x, y, z;

	x = vec->x * (1.0F / float(1 << W2V_SHIFT));
	y = vec->y * (1.0F / float(1 << W2V_SHIFT));
	z = vec->z * (1.0F / float(1 << W2V_SHIFT));

	mMXPtr[M00] = mMXPtr[M00] * x;
	mMXPtr[M10] = mMXPtr[M10] * x;
	mMXPtr[M20] = mMXPtr[M20] * x;

	mMXPtr[M01] = mMXPtr[M01] * y;
	mMXPtr[M11] = mMXPtr[M11] * y;
	mMXPtr[M21] = mMXPtr[M21] * y;

	mMXPtr[M02] = mMXPtr[M02] * z;
	mMXPtr[M12] = mMXPtr[M12] * z;
	mMXPtr[M22] = mMXPtr[M22] * z;
}

void phd_PushMatrix()
{
	phd_mxptr[indices_count + M00] = phd_mxptr[M00];
	phd_mxptr[indices_count + M01] = phd_mxptr[M01];
	phd_mxptr[indices_count + M02] = phd_mxptr[M02];
	phd_mxptr[indices_count + M03] = phd_mxptr[M03];
	phd_mxptr[indices_count + M10] = phd_mxptr[M10];
	phd_mxptr[indices_count + M11] = phd_mxptr[M11];
	phd_mxptr[indices_count + M12] = phd_mxptr[M12];
	phd_mxptr[indices_count + M13] = phd_mxptr[M13];
	phd_mxptr[indices_count + M20] = phd_mxptr[M20];
	phd_mxptr[indices_count + M21] = phd_mxptr[M21];
	phd_mxptr[indices_count + M22] = phd_mxptr[M22];
	phd_mxptr[indices_count + M23] = phd_mxptr[M23];
	phd_mxptr += indices_count;

	mPushMatrix();
}

void phd_PushUnitMatrix()
{
	phd_mxptr += indices_count;
	phd_mxptr[M00] = 1 << W2V_SHIFT;
	phd_mxptr[M01] = 0;
	phd_mxptr[M02] = 0;
	phd_mxptr[M03] = 0;
	phd_mxptr[M10] = 0;
	phd_mxptr[M11] = 1 << W2V_SHIFT;
	phd_mxptr[M12] = 0;
	phd_mxptr[M13] = 0;
	phd_mxptr[M20] = 0;
	phd_mxptr[M21] = 0;
	phd_mxptr[M22] = 1 << W2V_SHIFT;
	phd_mxptr[M23] = 0;

	mPushUnitMatrix();
}

void phd_SetTrans(long x, long y, long z)
{
	phd_mxptr[M03] = x << 14;
	phd_mxptr[M13] = y << 14;
	phd_mxptr[M23] = z << 14;
	mSetTrans(x, y, z);
}

long phd_TranslateRel(long x, long y, long z)
{
	phd_mxptr[M03] += x * phd_mxptr[M00] + y * phd_mxptr[M01] + z * phd_mxptr[M02];
	phd_mxptr[M13] += x * phd_mxptr[M10] + y * phd_mxptr[M11] + z * phd_mxptr[M12];
	phd_mxptr[M23] += x * phd_mxptr[M20] + y * phd_mxptr[M21] + z * phd_mxptr[M22];
	
	mTranslateRel(x, y, z);
	return 1;
}

void phd_RotX(short angle)
{
	long sin, cos, mx1, mx2;

	if (angle)
	{
		sin = phd_sin(angle);
		cos = phd_cos(angle);

		mx1 = cos * phd_mxptr[M01] + sin * phd_mxptr[M02];
		mx2 = cos * phd_mxptr[M02] - sin * phd_mxptr[M01];
		phd_mxptr[M01] = mx1 >> W2V_SHIFT;
		phd_mxptr[M02] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M11] + sin * phd_mxptr[M12];
		mx2 = cos * phd_mxptr[M12] - sin * phd_mxptr[M11];
		phd_mxptr[M11] = mx1 >> W2V_SHIFT;
		phd_mxptr[M12] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M21] + sin * phd_mxptr[M22];
		mx2 = cos * phd_mxptr[M22] - sin * phd_mxptr[M21];
		phd_mxptr[M21] = mx1 >> W2V_SHIFT;
		phd_mxptr[M22] = mx2 >> W2V_SHIFT;
	}

	mRotX(angle);
}

void phd_RotY(short angle)
{
	long sin, cos, mx1, mx2;

	if (angle)
	{
		sin = phd_sin(angle);
		cos = phd_cos(angle);

		mx1 = cos * phd_mxptr[M00] - sin * phd_mxptr[M02];
		mx2 = cos * phd_mxptr[M02] + sin * phd_mxptr[M00];
		phd_mxptr[M00] = mx1 >> W2V_SHIFT;
		phd_mxptr[M02] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M10] - sin * phd_mxptr[M12];
		mx2 = cos * phd_mxptr[M12] + sin * phd_mxptr[M10];
		phd_mxptr[M10] = mx1 >> W2V_SHIFT;
		phd_mxptr[M12] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M20] - sin * phd_mxptr[M22];
		mx2 = cos * phd_mxptr[M22] + sin * phd_mxptr[M20];
		phd_mxptr[M20] = mx1 >> W2V_SHIFT;
		phd_mxptr[M22] = mx2 >> W2V_SHIFT;
	}

	mRotY(angle);
}

void phd_RotZ(short angle)
{
	long sin, cos, mx1, mx2;

	if (angle)
	{
		sin = phd_sin(angle);
		cos = phd_cos(angle);

		mx1 = cos * phd_mxptr[M00] + sin * phd_mxptr[M01];
		mx2 = cos * phd_mxptr[M01] - sin * phd_mxptr[M00];
		phd_mxptr[M00] = mx1 >> W2V_SHIFT;
		phd_mxptr[M01] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M10] + sin * phd_mxptr[M11];
		mx2 = cos * phd_mxptr[M11] - sin * phd_mxptr[M10];
		phd_mxptr[M10] = mx1 >> W2V_SHIFT;
		phd_mxptr[M11] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M20] + sin * phd_mxptr[M21];
		mx2 = cos * phd_mxptr[M21] - sin * phd_mxptr[M20];
		phd_mxptr[M20] = mx1 >> W2V_SHIFT;
		phd_mxptr[M21] = mx2 >> W2V_SHIFT;
	}

	mRotZ(angle);
}

void phd_RotYXZpack(long angles)
{
	long sin, cos, mx1, mx2;
	short angle;

	angle = (angles >> 10) & 0x3FF;
	angle <<= 6;

	if (angle)
	{
		sin = phd_sin(angle);
		cos = phd_cos(angle);

		mx1 = cos * phd_mxptr[M00] - sin * phd_mxptr[M02];
		mx2 = cos * phd_mxptr[M02] + sin * phd_mxptr[M00];
		phd_mxptr[M00] = mx1 >> W2V_SHIFT;
		phd_mxptr[M02] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M10] - sin * phd_mxptr[M12];
		mx2 = cos * phd_mxptr[M12] + sin * phd_mxptr[M10];
		phd_mxptr[M10] = mx1 >> W2V_SHIFT;
		phd_mxptr[M12] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M20] - sin * phd_mxptr[M22];
		mx2 = cos * phd_mxptr[M22] + sin * phd_mxptr[M20];
		phd_mxptr[M20] = mx1 >> W2V_SHIFT;
		phd_mxptr[M22] = mx2 >> W2V_SHIFT;
	}

	angle = (angles >> 20) & 0x3FF;
	angle <<= 6;

	if (angle)
	{
		sin = phd_sin(angle);
		cos = phd_cos(angle);

		mx1 = cos * phd_mxptr[M01] + sin * phd_mxptr[M02];
		mx2 = cos * phd_mxptr[M02] - sin * phd_mxptr[M01];
		phd_mxptr[M01] = mx1 >> W2V_SHIFT;
		phd_mxptr[M02] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M11] + sin * phd_mxptr[M12];
		mx2 = cos * phd_mxptr[M12] - sin * phd_mxptr[M11];
		phd_mxptr[M11] = mx1 >> W2V_SHIFT;
		phd_mxptr[M12] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M21] + sin * phd_mxptr[M22];
		mx2 = cos * phd_mxptr[M22] - sin * phd_mxptr[M21];
		phd_mxptr[M21] = mx1 >> W2V_SHIFT;
		phd_mxptr[M22] = mx2 >> W2V_SHIFT;
	}

	angle = angles & 0x3FF;
	angle <<= 6;

	if (angle)
	{
		sin = phd_sin(angle);
		cos = phd_cos(angle);

		mx1 = cos * phd_mxptr[M00] + sin * phd_mxptr[M01];
		mx2 = cos * phd_mxptr[M01] - sin * phd_mxptr[M00];
		phd_mxptr[M00] = mx1 >> W2V_SHIFT;
		phd_mxptr[M01] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M10] + sin * phd_mxptr[M11];
		mx2 = cos * phd_mxptr[M11] - sin * phd_mxptr[M10];
		phd_mxptr[M10] = mx1 >> W2V_SHIFT;
		phd_mxptr[M11] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M20] + sin * phd_mxptr[M21];
		mx2 = cos * phd_mxptr[M21] - sin * phd_mxptr[M20];
		phd_mxptr[M20] = mx1 >> W2V_SHIFT;
		phd_mxptr[M21] = mx2 >> W2V_SHIFT;
	}

	mRotYXZpack(angles);
}

void phd_RotYXZ(short y, short x, short z)
{
	long sin, cos, mx1, mx2;

	if (y)
	{
		sin = phd_sin(y);
		cos = phd_cos(y);

		mx1 = cos * phd_mxptr[M00] - sin * phd_mxptr[M02];
		mx2 = cos * phd_mxptr[M02] + sin * phd_mxptr[M00];
		phd_mxptr[M00] = mx1 >> W2V_SHIFT;
		phd_mxptr[M02] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M10] - sin * phd_mxptr[M12];
		mx2 = cos * phd_mxptr[M12] + sin * phd_mxptr[M10];
		phd_mxptr[M10] = mx1 >> W2V_SHIFT;
		phd_mxptr[M12] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M20] - sin * phd_mxptr[M22];
		mx2 = cos * phd_mxptr[M22] + sin * phd_mxptr[M20];
		phd_mxptr[M20] = mx1 >> W2V_SHIFT;
		phd_mxptr[M22] = mx2 >> W2V_SHIFT;
	}

	if (x)
	{
		sin = phd_sin(x);
		cos = phd_cos(x);

		mx1 = cos * phd_mxptr[M01] + sin * phd_mxptr[M02];
		mx2 = cos * phd_mxptr[M02] - sin * phd_mxptr[M01];
		phd_mxptr[M01] = mx1 >> W2V_SHIFT;
		phd_mxptr[M02] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M11] + sin * phd_mxptr[M12];
		mx2 = cos * phd_mxptr[M12] - sin * phd_mxptr[M11];
		phd_mxptr[M11] = mx1 >> W2V_SHIFT;
		phd_mxptr[M12] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M21] + sin * phd_mxptr[M22];
		mx2 = cos * phd_mxptr[M22] - sin * phd_mxptr[M21];
		phd_mxptr[M21] = mx1 >> W2V_SHIFT;
		phd_mxptr[M22] = mx2 >> W2V_SHIFT;
	}

	if (z)
	{
		sin = phd_sin(z);
		cos = phd_cos(z);

		mx1 = cos * phd_mxptr[M00] + sin * phd_mxptr[M01];
		mx2 = cos * phd_mxptr[M01] - sin * phd_mxptr[M00];
		phd_mxptr[M00] = mx1 >> W2V_SHIFT;
		phd_mxptr[M01] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M10] + sin * phd_mxptr[M11];
		mx2 = cos * phd_mxptr[M11] - sin * phd_mxptr[M10];
		phd_mxptr[M10] = mx1 >> W2V_SHIFT;
		phd_mxptr[M11] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M20] + sin * phd_mxptr[M21];
		mx2 = cos * phd_mxptr[M21] - sin * phd_mxptr[M20];
		phd_mxptr[M20] = mx1 >> W2V_SHIFT;
		phd_mxptr[M21] = mx2 >> W2V_SHIFT;
	}

	mRotYXZ(y, x, z);
}

void phd_TranslateAbs(long x, long y, long z)
{
	long fx, fy, fz;

	fx = x - w2v_matrix[M03];
	fy = y - w2v_matrix[M13];
	fz = z - w2v_matrix[M23];
	phd_mxptr[M03] = fx * phd_mxptr[M00] + fy * phd_mxptr[M01] + fz * phd_mxptr[M02];
	phd_mxptr[M13] = fx * phd_mxptr[M10] + fy * phd_mxptr[M11] + fz * phd_mxptr[M12];
	phd_mxptr[M23] = fx * phd_mxptr[M20] + fy * phd_mxptr[M21] + fz * phd_mxptr[M22];

	mTranslateAbs(x, y, z);
}

void phd_GetVectorAngles(long x, long y, long z, short* angles)
{
	short atan;

	angles[0] = (short)phd_atan(z, x);

	while ((short)x != x || (short)y != y || (short)z != z)
	{
		x >>= 2;
		y >>= 2;
		z >>= 2;
	}

	atan = (short)phd_atan(phd_sqrt(SQUARE(z) + SQUARE(x)), y);

	if ((y > 0 && atan > 0) || (y < 0 && atan < 0))
		atan = -atan;

	angles[1] = atan;
}

ulong mGetAngle(long x, long z, long x1, long z1)
{
	long dx, dz, octant, swap, angle;

	dx = x1 - x;
	dz = z1 - z;

	if (!dx && !dz)
		return 0;

	octant = 0;

	if (dx < 0)
	{
		octant = 4;
		dx = -dx;
	}

	if (dz < 0)
	{
		octant += 2;
		dz = -dz;
	}

	if (dz > dx)
	{
		octant++;
		swap = dx;
		dx = dz;
		dz = swap;
	}

	while (short(dz) != dz)
	{
		dx >>= 1;
		dz >>= 1;
	}

	angle = phdtan2[octant] + phdtantab[(dz << 11) / dx];

	if (angle < 0)
		angle = -angle;

	return -angle & 0xFFFF;
}

void AlterFOV(short fov)
{
	long fov_width;

	CurrentFov = fov;
	fov /= 2;
	fov_width = phd_winheight * 320 / 240;
	LfAspectCorrection = 1.0F; // must always be 1.0 for unstretched view
	phd_persp = (fov_width / 2) * phd_cos(fov) / phd_sin(fov);
	f_persp = (float)phd_persp;
	f_oneopersp = one / f_persp;
	f_perspoznear = f_persp / f_znear;
	f_mpersp = f_persp;
	f_moneopersp = mone / f_persp;
	f_mperspoznear = f_persp / f_mznear;
}

long phd_atan(long x, long y)
{
	long octant, n, result;

	result = 0;
	octant = 0;

	if (x || y)
	{
		if (x < 0)
		{
			octant += 4;
			x = -x;
		}

		if (y < 0)
		{
			octant += 2;
			y = -y;
		}

		if (y > x)
		{
			octant++;
			n = x;
			x = y;
			y = n;
		}

		while ((short)y != y)
		{
			x >>= 1;
			y >>= 1;
		}

		result = phdtan2[octant] + phdtantab[(y << 11) / x];

		if (result < 0)
			result = -result;
	}

	return result;
}

ulong phd_sqrt(ulong num)
{
	ulong base, result, tmp;

	base = 0x40000000;
	result = 0;

	do
	{
		tmp = result;
		result += base;
		tmp >>= 1;

		if (result > num)
			result = tmp;
		else
		{
			num -= result;
			result = base | tmp;
		}

		base >>= 2;

	} while (base);

	return result;
}

void ScaleCurrentMatrix(PHD_VECTOR* vec)
{
	phd_mxptr[M00] = (phd_mxptr[M00] * vec->x) >> W2V_SHIFT;
	phd_mxptr[M10] = (phd_mxptr[M10] * vec->x) >> W2V_SHIFT;
	phd_mxptr[M20] = (phd_mxptr[M20] * vec->x) >> W2V_SHIFT;

	phd_mxptr[M01] = (phd_mxptr[M01] * vec->y) >> W2V_SHIFT;
	phd_mxptr[M11] = (phd_mxptr[M11] * vec->y) >> W2V_SHIFT;
	phd_mxptr[M21] = (phd_mxptr[M21] * vec->y) >> W2V_SHIFT;

	phd_mxptr[M02] = (phd_mxptr[M02] * vec->z) >> W2V_SHIFT;
	phd_mxptr[M12] = (phd_mxptr[M12] * vec->z) >> W2V_SHIFT;
	phd_mxptr[M22] = (phd_mxptr[M22] * vec->z) >> W2V_SHIFT;

	mScaleCurrentMatrix(vec);
}

void SetupZRange(long znear, long zfar)
{
	phd_znear = znear;
	phd_zfar = zfar;
	f_zfar = (float)zfar;
	f_znear = (float)znear;
	f_perspoznear = f_persp / f_znear;
	f_mznear = float(znear >> W2V_SHIFT);
	f_mzfar = float(zfar >> W2V_SHIFT);
	f_mperspoznear = f_mpersp / f_mznear;
	f_moneoznear = mone / f_mznear;
	f_b = f_mzfar * f_mznear * 0.99F / (f_mznear - f_mzfar);
	f_a = 0.005F - f_b / f_mznear;
	f_b = -f_b;
	f_boo = f_b / mone;
}

void InitWindow(long x, long y, long w, long h, long znear, long zfar, long fov, long a, long b)
{
	phd_winwidth = w;
	phd_winxmax = short(w - 1);
	phd_winxmin = (short)x;
	phd_winheight = h;
	phd_winymax = short(h - 1);
	phd_winymin = (short)y;
	phd_centerx = w / 2;
	phd_centery = h / 2;
	phd_znear = znear << W2V_SHIFT;
	f_centerx = float(w / 2);
	phd_zfar = zfar << W2V_SHIFT;
	f_centery = float(h / 2);
	AlterFOV(short(182 * fov));
	SetupZRange(phd_znear, phd_zfar);
	phd_right = phd_winxmax;
	phd_bottom = phd_winymax;
	phd_left = x;
	phd_top = y;
	f_right = float(phd_winxmax + 1);
	f_bottom = float(phd_winymax + 1);
	f_top = (float)phd_winymin;
	f_left = (float)phd_winxmin;
	phd_mxptr = matrix_stack;
	mInit();
}

void phd_GenerateW2V(PHD_3DPOS* viewPos)
{
	PHD_VECTOR scalar;
	long sx, cx, sy, cy, sz, cz;

	sx = phd_sin(viewPos->x_rot);
	cx = phd_cos(viewPos->x_rot);
	sy = phd_sin(viewPos->y_rot);
	cy = phd_cos(viewPos->y_rot);
	sz = phd_sin(viewPos->z_rot);
	cz = phd_cos(viewPos->z_rot);
	phd_mxptr = matrix_stack;

	w2v_matrix[M00] = TRIGMULT3(sx, sy, sz) + TRIGMULT2(cy, cz);
	w2v_matrix[M01] = TRIGMULT2(cx, sz);
	w2v_matrix[M02] = TRIGMULT3(sx, cy, sz) - TRIGMULT2(sy, cz);
	phd_mxptr[M00] = w2v_matrix[M00];
	phd_mxptr[M01] = w2v_matrix[M01];
	phd_mxptr[M02] = w2v_matrix[M02];

	w2v_matrix[M10] = TRIGMULT3(sx, sy, cz) - TRIGMULT2(cy, sz);
	w2v_matrix[M11] = TRIGMULT2(cx, cz);
	w2v_matrix[M12] = TRIGMULT3(sx, cy, cz) + TRIGMULT2(sy, sz);
	phd_mxptr[M10] = w2v_matrix[M10];
	phd_mxptr[M11] = w2v_matrix[M11];
	phd_mxptr[M12] = w2v_matrix[M12];

	w2v_matrix[M20] = TRIGMULT2(cx, sy);
	w2v_matrix[M21] = -sx;
	w2v_matrix[M22] = TRIGMULT2(cx, cy);
	phd_mxptr[M20] = w2v_matrix[M20];
	phd_mxptr[M21] = w2v_matrix[M21];
	phd_mxptr[M22] = w2v_matrix[M22];

	if (lara.dpoisoned != lara.poisoned)
	{
		lara.poisoned += (lara.dpoisoned - lara.poisoned) >> 4;

		if (abs(lara.dpoisoned - lara.poisoned) < 16)
			lara.poisoned = lara.dpoisoned;
	}

	if (lara.poisoned >= 256)
	{
		scalar.x = (lara.poisoned - 256) * ((phd_sin(XSoff1) + phd_sin(XSoff2)) >> 2);
		scalar.y = (lara.poisoned - 256) * ((phd_sin(YSoff1) + phd_sin(YSoff2)) >> 2);
		scalar.z = (lara.poisoned - 256) * ((phd_sin(ZSoff1) + phd_sin(ZSoff2)) >> 2);

		if (scalar.x || scalar.y || scalar.z)
		{
			scalar.x = (scalar.x >> 12) + 0x4000;
			scalar.y = (scalar.y >> 12) + 0x4000;
			scalar.z = (scalar.z >> 12) + 0x4000;
			ScaleCurrentMatrix(&scalar);
		}
	}

	w2v_matrix[M03] = phd_mxptr[M03] = viewPos->x_pos;;
	w2v_matrix[M13] = phd_mxptr[M13] = viewPos->y_pos;
	w2v_matrix[M23] = phd_mxptr[M23] = viewPos->z_pos;
	phd_mxptr[M03] = w2v_matrix[M03];
	phd_mxptr[M13] = w2v_matrix[M13];
	phd_mxptr[M23] = w2v_matrix[M23];

	w2v_matrix[M10] = long(LfAspectCorrection * float(phd_mxptr[M10]));
	w2v_matrix[M11] = long(LfAspectCorrection * float(phd_mxptr[M11]));
	w2v_matrix[M12] = long(LfAspectCorrection * float(phd_mxptr[M12]));
	phd_mxptr[M10] = w2v_matrix[M10];
	phd_mxptr[M11] = w2v_matrix[M11];
	phd_mxptr[M12] = w2v_matrix[M12];
	mGenerateW2V(viewPos);
}

void phd_LookAt(long sx, long sy, long sz, long tx, long ty, long tz, short roll)
{
	PHD_3DPOS viewPos;
	long dx, dy, dz, val;
	short angles[2];

	phd_GetVectorAngles(tx - sx, ty - sy, tz - sz, angles);
	viewPos.x_pos = sx;
	viewPos.y_pos = sy;
	viewPos.z_pos = sz;
	viewPos.x_rot = angles[1];
	viewPos.y_rot = angles[0];
	viewPos.z_rot = roll;
	dx = sx - tx;
	dy = sy - ty;
	dz = sz - tz;
	val = phd_sqrt(SQUARE(dx) + SQUARE(dz));
	CamRot.x = (mGetAngle(0, 0, val, dy) >> 4) & 0xFFF;
	CamRot.y = (mGetAngle(sz, sx, tz, tx) >> 4) & 0xFFF;
	CamRot.z = 0;
	CamPos.x = sx;
	CamPos.y = sy;
	CamPos.z = sz;
	phd_GenerateW2V(&viewPos);
	S_InitD3DMatrix();
}
