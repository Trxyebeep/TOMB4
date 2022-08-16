#pragma once
#include "../global/types.h"

void inject_texture(bool replace);

typedef void(__cdecl* rgbfunc)(uchar*, uchar*, uchar*);
LPDIRECTDRAWSURFACEX CreateTexturePage(long w, long h, long MipMapCount, long* pSrc, rgbfunc RGBM, long format);
void FreeTextures();

extern TEXTURE* Textures;
extern long nTextures;
