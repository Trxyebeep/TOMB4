#include "../tomb4/pch.h"
#include "d3dmatrix.h"

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

void inject_d3dmatrix(bool replace)
{
	INJECT(0x00490DD0, D3DIdentityMatrix, replace);
}
