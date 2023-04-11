#include "../tomb4/pch.h"
#include "d3dmatrix.h"
#include "dxshell.h"
#include "3dmath.h"
#include "winmain.h"

D3DMATRIX D3DMView;
D3DMATRIX D3DLightMatrix;
D3DMATRIX D3DInvCameraMatrix;

static D3DMATRIX D3DMWorld;
static D3DMATRIX D3DMProjection;

D3DMATRIX* D3DIdentityMatrix(D3DMATRIX* mx)
{
	mx->_11 = 1;
	mx->_12 = 0;
	mx->_13 = 0;
	mx->_14 = 0;

	mx->_21 = 0;
	mx->_22 = 1;
	mx->_23 = 0;
	mx->_24 = 0;

	mx->_31 = 0;
	mx->_32 = 0;
	mx->_33 = 1;
	mx->_34 = 0;

	mx->_41 = 0;
	mx->_42 = 0;
	mx->_43 = 0;
	mx->_44 = 1;
	return mx;
}

void SetD3DMatrix(D3DMATRIX* mx, float* imx)
{
	D3DIdentityMatrix(mx);
	mx->_11 = imx[M00];
	mx->_12 = imx[M10];
	mx->_13 = imx[M20];
	mx->_21 = imx[M01];
	mx->_22 = imx[M11];
	mx->_23 = imx[M21];
	mx->_31 = imx[M02];
	mx->_32 = imx[M12];
	mx->_33 = imx[M22];
	mx->_41 = imx[M03];
	mx->_42 = imx[M13];
	mx->_43 = imx[M23];
}

void SetD3DViewMatrix()
{
	SetD3DMatrix(&D3DMView, mMXPtr);
	DXAttempt(App.dx.lpD3DDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, &D3DMView));
}

void D3DTransform(D3DVECTOR* vec, D3DMATRIX* mx)
{
	float x, y, z;

	x = mx->_11 * vec->x + mx->_21 * vec->y + mx->_31 * vec->z;
	y = mx->_12 * vec->x + mx->_22 * vec->y + mx->_32 * vec->z;
	z = mx->_13 * vec->x + mx->_23 * vec->y + mx->_33 * vec->z;
	vec->x = x;
	vec->y = y;
	vec->z = z;
}

D3DVECTOR* D3DNormalise(D3DVECTOR* vec)
{
	float val;

	if (vec->x != 0 || vec->y != 0 || vec->z != 0)
	{
		val = 1.0F / sqrt(SQUARE(vec->x) + SQUARE(vec->y) + SQUARE(vec->z));
		vec->x = val * vec->x;
		vec->y = val * vec->y;
		vec->z = val * vec->z;
	}

	return vec;
}

void S_InitD3DMatrix()
{
	D3DIdentityMatrix(&D3DMWorld);
	D3DIdentityMatrix(&D3DMProjection);
	D3DMProjection._22 = -1;
	DXAttempt(App.dx.lpD3DDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &D3DMWorld));
	DXAttempt(App.dx.lpD3DDevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &D3DMProjection));
}

LPD3DMATRIX D3DMultMatrix(LPD3DMATRIX d, LPD3DMATRIX a, LPD3DMATRIX b)
{
	d->_11 = a->_11 * b->_11;
	d->_11 += a->_12 * b->_21;
	d->_11 += a->_13 * b->_31;
	d->_11 += a->_14 * b->_41;

	d->_12 = a->_11 * b->_12;
	d->_12 += a->_12 * b->_22;
	d->_12 += a->_13 * b->_32;
	d->_12 += a->_14 * b->_42;

	d->_13 = a->_11 * b->_13;
	d->_13 += a->_12 * b->_23;
	d->_13 += a->_13 * b->_33;
	d->_13 += a->_14 * b->_43;

	d->_14 = a->_11 * b->_14;
	d->_14 += a->_12 * b->_24;
	d->_14 += a->_13 * b->_34;
	d->_14 += a->_14 * b->_44;

	d->_21 = a->_21 * b->_11;
	d->_21 += a->_22 * b->_21;
	d->_21 += a->_23 * b->_31;
	d->_21 += a->_24 * b->_41;

	d->_22 = a->_21 * b->_12;
	d->_22 += a->_22 * b->_22;
	d->_22 += a->_23 * b->_32;
	d->_22 += a->_24 * b->_42;

	d->_23 = a->_21 * b->_13;
	d->_23 += a->_22 * b->_23;
	d->_23 += a->_23 * b->_33;
	d->_23 += a->_24 * b->_43;

	d->_24 = a->_21 * b->_14;
	d->_24 += a->_22 * b->_24;
	d->_24 += a->_23 * b->_34;
	d->_24 += a->_24 * b->_44;

	d->_31 = a->_31 * b->_11;
	d->_31 += a->_32 * b->_21;
	d->_31 += a->_33 * b->_31;
	d->_31 += a->_34 * b->_41;

	d->_32 = a->_31 * b->_12;
	d->_32 += a->_32 * b->_22;
	d->_32 += a->_33 * b->_32;
	d->_32 += a->_34 * b->_42;

	d->_33 = a->_31 * b->_13;
	d->_33 += a->_32 * b->_23;
	d->_33 += a->_33 * b->_33;
	d->_33 += a->_34 * b->_43;

	d->_34 = a->_31 * b->_14;
	d->_34 += a->_32 * b->_24;
	d->_34 += a->_33 * b->_34;
	d->_34 += a->_34 * b->_44;

	d->_41 = a->_41 * b->_11;
	d->_41 += a->_42 * b->_21;
	d->_41 += a->_43 * b->_31;
	d->_41 += a->_44 * b->_41;

	d->_42 = a->_41 * b->_12;
	d->_42 += a->_42 * b->_22;
	d->_42 += a->_43 * b->_32;
	d->_42 += a->_44 * b->_42;

	d->_43 = a->_41 * b->_13;
	d->_43 += a->_42 * b->_23;
	d->_43 += a->_43 * b->_33;
	d->_43 += a->_44 * b->_43;

	d->_44 = a->_41 * b->_14;
	d->_44 += a->_42 * b->_24;
	d->_44 += a->_43 * b->_34;
	d->_44 += a->_44 * b->_44;

	return d;
}
