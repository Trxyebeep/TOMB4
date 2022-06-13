#pragma once
#include "../global/vars.h"

void inject_texture(bool replace);

typedef void(__cdecl* rgbfunc)(uchar*, uchar*, uchar*);
LPDIRECTDRAWSURFACE4 CreateTexturePage(long w, long h, long MipMapCount, long* pSrc, rgbfunc RGBM, long format);

#define FreeTextures	( (void(__cdecl*)()) 0x0048E6E0 )
