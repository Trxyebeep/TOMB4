#pragma once
#include "../global/types.h"

D3DMATRIX* D3DIdentityMatrix(D3DMATRIX* mx);
void SetD3DMatrix(D3DMATRIX* mx, float* imx);
void SetD3DViewMatrix();
void D3DTransform(D3DVECTOR* vec, D3DMATRIX* mx);
D3DVECTOR* D3DNormalise(D3DVECTOR* vec);
void S_InitD3DMatrix();
LPD3DMATRIX D3DMultMatrix(LPD3DMATRIX d, LPD3DMATRIX a, LPD3DMATRIX b);

extern D3DMATRIX D3DMView;
extern D3DMATRIX D3DLightMatrix;
extern D3DMATRIX D3DInvCameraMatrix;
