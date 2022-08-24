#include "../tomb4/pch.h"
#include "d3dmatrix.h"
#include "dxshell.h"
#include "3dmath.h"
#include "winmain.h"

D3DMATRIX D3DMView;

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

void mSetD3DMatrix(D3DMATRIX* mx, float* imx)
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

void SetD3DMatrix(D3DMATRIX* mx, long* imx)
{
	D3DIdentityMatrix(mx);

	mx->_11 = (float)imx[M00] / 16384;
	mx->_12 = (float)imx[M10] / 16384;
	mx->_13 = (float)imx[M20] / 16384;
	mx->_21 = (float)imx[M01] / 16384;
	mx->_22 = (float)imx[M11] / 16384;
	mx->_23 = (float)imx[M21] / 16384;
	mx->_31 = (float)imx[M02] / 16384;
	mx->_32 = (float)imx[M12] / 16384;
	mx->_33 = (float)imx[M22] / 16384;
	mx->_41 = float(imx[M03] >> W2V_SHIFT);
	mx->_42 = float(imx[M13] >> W2V_SHIFT);
	mx->_43 = float(imx[M23] >> W2V_SHIFT);
}

void SetD3DViewMatrix()
{
	mSetD3DMatrix(&D3DMView, mMXPtr);
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
