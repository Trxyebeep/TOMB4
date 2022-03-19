#pragma once
#include "../global/vars.h"

#define visible_zclip	( (long(__cdecl*)(D3DTLVERTEX*, D3DTLVERTEX*, D3DTLVERTEX*)) 0x0046FDE0 )
#define ZClipper	( (long(__cdecl*)(long, D3DTLBUMPVERTEX*, D3DTLBUMPVERTEX*)) 0x0046E4D0 )
#define XYUVGClipper	( (long(__cdecl*)(long, D3DTLBUMPVERTEX*)) 0x0046E8D0 )