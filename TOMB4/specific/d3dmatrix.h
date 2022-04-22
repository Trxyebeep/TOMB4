#pragma once
#include "../global/vars.h"

void inject_d3dmatrix(bool replace);

D3DMATRIX* D3DIdentityMatrix(D3DMATRIX* mx);
void SetD3DMatrix(D3DMATRIX* mx, long* imx);
void SetD3DViewMatrix();
void D3DTransform(D3DVECTOR* vec, D3DMATRIX* mx);
D3DVECTOR* D3DNormalise(D3DVECTOR* vec);
void S_InitD3DMatrix();
