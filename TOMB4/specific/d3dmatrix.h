#pragma once
#include "../global/vars.h"

void inject_d3dmatrix(bool replace);

D3DMATRIX* D3DIdentityMatrix(D3DMATRIX* mx);
void SetD3DMatrix(D3DMATRIX* mx, long* imx);
void SetD3DViewMatrix();
void D3DTransform(D3DVECTOR* vec, D3DMATRIX* mx);

#define D3DNormalise	( (void(__cdecl*)(D3DVECTOR*)) 0x00490D50 )
#define S_InitD3DMatrix	( (void(__cdecl*)()) 0x00490CF0 )
