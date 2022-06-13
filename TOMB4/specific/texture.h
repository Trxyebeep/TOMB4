#pragma once
#include "../global/vars.h"

void inject_texture(bool replace);

typedef void(__cdecl* rgbfunc)(uchar*, uchar*, uchar*);
LPDIRECTDRAWSURFACE4 CreateTexturePage(long w, long h, long MipMapCount, long* pSrc, rgbfunc RGBM, long format);
void FreeTextures();
