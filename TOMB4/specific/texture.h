#pragma once
#include "../global/vars.h"

typedef void(__cdecl* rgbfunc)(uchar*, uchar*, uchar*);
#define CreateTexturePage	( (LPDIRECTDRAWSURFACE4(__cdecl*)(long, long, long, long*, rgbfunc, long)) 0x0048E2F0 )
