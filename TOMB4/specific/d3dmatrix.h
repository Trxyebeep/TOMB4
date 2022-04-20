#pragma once
#include "../global/vars.h"

void inject_d3dmatrix(bool replace);

D3DMATRIX* D3DIdentityMatrix(D3DMATRIX* mx);
void SetD3DMatrix(D3DMATRIX* mx, long* imx);

#define SetD3DViewMatrix	( (void(__cdecl*)()) 0x00490B30  )
#define	D3DTransform	( (void(__cdecl*)(D3DVECTOR*, D3DMATRIX*)) 0x00490E10 )
#define D3DNormalise	( (void(__cdecl*)(D3DVECTOR*)) 0x00490D50 )
#define S_InitD3DMatrix	( (void(__cdecl*)()) 0x00490CF0 )
