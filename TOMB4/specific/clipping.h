#pragma once
#include "../global/vars.h"

void inject_clipping(bool replace);

long ZClipper(long n, D3DTLBUMPVERTEX* in, D3DTLBUMPVERTEX* out);

#define visible_zclip	( (long(__cdecl*)(D3DTLVERTEX*, D3DTLVERTEX*, D3DTLVERTEX*)) 0x0046FDE0 )
#define XYUVGClipper	( (long(__cdecl*)(long, D3DTLBUMPVERTEX*)) 0x0046E8D0 )