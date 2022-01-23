#pragma once
#include "../global/vars.h"

#define SetD3DViewMatrix	( (void(__cdecl*)()) 0x00490B30  )
#define	D3DTransform	( (void(__cdecl*)(D3DVECTOR*, D3DMATRIX*)) 0x00490E10 )
#define D3DNormalise	( (void(__cdecl*)(D3DVECTOR*)) 0x00490D50 )
