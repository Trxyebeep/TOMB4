#include "../tomb4/pch.h"
#include "texture.h"
#include "dxshell.h"

LPDIRECTDRAWSURFACE4 CreateTexturePage(long w, long h, long MipMapCount, long* pSrc, rgbfunc RGBM, long format)
{
	DXTEXTUREINFO* tex;
	LPDIRECTDRAWSURFACE4 tSurf;
	DDSURFACEDESC2 desc;
	long* lS;
	long* lD;
	short* sS;
	short* sD;
	char* cD;
	ulong c, o, ro, go, bo, ao;
	uchar r, g, b, a;

	memset(&desc, 0, sizeof(DDSURFACEDESC2));
	desc.dwSize = sizeof(DDSURFACEDESC2);
	desc.dwWidth = w;
	desc.dwHeight = h;

	if (w < 32 || h < 32)
		MipMapCount = 0;

	desc.ddpfPixelFormat = G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].TextureInfos[G_dxinfo->nTexture].ddpf;
	desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
	desc.ddsCaps.dwCaps = DDSCAPS_TEXTURE;

	if (App.dx.Flags & 0x80)
		desc.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
	else
		desc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

	if (MipMapCount)
	{
		desc.dwFlags |= DDSD_MIPMAPCOUNT;
		desc.dwMipMapCount = MipMapCount + 1;
		desc.ddsCaps.dwCaps |= DDSCAPS_COMPLEX | DDSCAPS_MIPMAP;
	}

	DXCreateSurface(App.dx.lpDD, &desc, &tSurf);
	DXAttempt(tSurf->Lock(0, &desc, DDLOCK_NOSYSLOCK, 0));

	if (!format)
	{
		lS = pSrc;
		cD = (char*)desc.lpSurface;

		for (ulong y = 0; y < desc.dwHeight; y++)
		{
			for (ulong x = 0; x < desc.dwWidth; x++)
			{
				c = *(lS + x * 256 / w + y * 0x10000 / h);
				r = CLRR(c);
				g = CLRG(c);
				b = CLRB(c);
				a = CLRA(c);

				if (RGBM)
					RGBM(&r, &g, &b);

				tex = &G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].TextureInfos[G_dxinfo->nTexture];
				ro = r >> (8 - tex->rbpp) << (tex->rshift);
				go = g >> (8 - tex->gbpp) << (tex->gshift);
				bo = b >> (8 - tex->bbpp) << (tex->bshift);
				ao = a >> (8 - tex->abpp) << (tex->ashift);
				o = ro | go | bo | ao;

				for (int i = tex->bpp; i > 0; i -= 8)
				{
					*cD++ = (char)o;
					o >>= 8;
				}
			}
		}
	}
	else if (format == 2)
	{
		sS = (short*)pSrc;
		sD = (short*)desc.lpSurface;

		for (ulong y = 0; y < desc.dwHeight; y++)
		{
			for (ulong x = 0; x < desc.dwWidth; x++)
				*sD++ = *(sS + x * 256 / w + y * 0x10000 / h);
		}
	}
	else if (format == 1)
	{
		lS = pSrc;
		lD = (long*)desc.lpSurface;

		for (ulong y = 0; y < desc.dwHeight; y++)
		{
			for (ulong x = 0; x < desc.dwWidth; x++)
				*lD++ = *(lS + x * 256 / w + y * 0x10000 / h);
		}
	}

	DXAttempt(tSurf->Unlock(0));
	return tSurf;
}

void inject_texture(bool replace)
{
	INJECT(0x0048E2F0, CreateTexturePage, replace);
}
