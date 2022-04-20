#include "../tomb4/pch.h"
#include "d3dmatrix.h"
#include "dxshell.h"

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
	mx->_41 = float(imx[M03] >> 14);
	mx->_42 = float(imx[M13] >> 14);
	mx->_43 = float(imx[M23] >> 14);
}

void SetD3DViewMatrix()
{
	D3DIdentityMatrix(&D3DMView);
	D3DMView._11 = (float)phd_mxptr[M00] / 16384;
	D3DMView._12 = (float)phd_mxptr[M10] / 16384;
	D3DMView._13 = (float)phd_mxptr[M20] / 16384;
	D3DMView._21 = (float)phd_mxptr[M01] / 16384;
	D3DMView._22 = (float)phd_mxptr[M11] / 16384;
	D3DMView._23 = (float)phd_mxptr[M21] / 16384;
	D3DMView._31 = (float)phd_mxptr[M02] / 16384;
	D3DMView._32 = (float)phd_mxptr[M12] / 16384;
	D3DMView._33 = (float)phd_mxptr[M22] / 16384;
	D3DMView._41 = float(phd_mxptr[M03] >> 14);
	D3DMView._42 = float(phd_mxptr[M13] >> 14);
	D3DMView._43 = float(phd_mxptr[M23] >> 14);
	DXAttempt(App.dx.lpD3DDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, &D3DMView));
}

void D3DTransform(D3DVECTOR* vec, D3DMATRIX* mx)
{
	vec->x = vec->x * mx->_11 + mx->_21 * vec->y + mx->_31 * vec->z;
	vec->y = vec->x * mx->_22 + mx->_22 * vec->y + mx->_32 * vec->z;
	vec->z = vec->x * mx->_13 + mx->_23 * vec->y + mx->_33 * vec->z;
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

void inject_d3dmatrix(bool replace)
{
	INJECT(0x00490DD0, D3DIdentityMatrix, replace);
	INJECT(0x00490C30, SetD3DMatrix, replace);
	INJECT(0x00490B30, SetD3DViewMatrix, replace);
	INJECT(0x00490E10, D3DTransform, replace);
	INJECT(0x00490D50, D3DNormalise, replace);
}
