#include "../tomb4/pch.h"
#include "texture.h"
#include "dxshell.h"
#include "function_stubs.h"
#include "winmain.h"

TEXTURE* Textures;
long nTextures;

LPDIRECTDRAWSURFACEX CreateTexturePage(long w, long h, long MipMapCount, long* pSrc, rgbfunc RGBM, long format)
{
	DXTEXTUREINFO* tex;
	LPDIRECTDRAWSURFACEX tSurf;
	DDSURFACEDESCX desc;
	long* lS;
	long* lD;
	short* sS;
	short* sD;
	char* cD;
	ulong c, o, ro, go, bo, ao;
	uchar r, g, b, a;

	memset(&desc, 0, sizeof(DDSURFACEDESCX));
	desc.dwSize = sizeof(DDSURFACEDESCX);
	desc.dwWidth = w;
	desc.dwHeight = h;

	desc.ddpfPixelFormat = G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].TextureInfos[G_dxinfo->nTexture].ddpf;
	desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
	desc.ddsCaps.dwCaps = DDSCAPS_TEXTURE;

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

	LPDIRECTDRAWSURFACEX nextMip = NULL;
	DDSCAPS2 mipCaps;
	memset(&mipCaps, 0, sizeof(DDSCAPS2));
	mipCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;
	LPDIRECTDRAWSURFACEX currentMip;
	HRESULT mipResult = tSurf->GetAttachedSurface(&mipCaps, &currentMip);
	//Traverse Mip Chain and fill them with data
	while (mipResult == DD_OK)
	{
		currentMip->AddRef();
		DDSURFACEDESCX mipDesc;
		memset(&mipDesc, 0, sizeof(mipDesc));
		mipDesc.dwSize = sizeof(DDSURFACEDESCX);
		DXAttempt(currentMip->GetSurfaceDesc(&mipDesc));
		DXAttempt(currentMip->Lock(0, &mipDesc, DDLOCK_NOSYSLOCK, 0));

		if (!format)
		{
			lS = pSrc;
			cD = (char*)mipDesc.lpSurface;

			for (ulong y = 0; y < mipDesc.dwHeight; y++)
			{
				for (ulong x = 0; x < mipDesc.dwWidth; x++)
				{
					c = *(lS + x * 256 / mipDesc.dwWidth + y * 0x10000 / mipDesc.dwHeight);
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
			sD = (short*)mipDesc.lpSurface;

			for (ulong y = 0; y < mipDesc.dwHeight; y++)
			{
				for (ulong x = 0; x < mipDesc.dwWidth; x++)
					*sD++ = *(sS + x * 256 / mipDesc.dwWidth + y * 0x10000 / mipDesc.dwHeight);
			}
		}
		else if (format == 1)
		{
			lS = pSrc;
			lD = (long*)mipDesc.lpSurface;

			for (ulong y = 0; y < mipDesc.dwHeight; y++)
			{
				for (ulong x = 0; x < mipDesc.dwWidth; x++)
					*lD++ = *(lS + x * 256 / mipDesc.dwWidth + y * 0x10000 / mipDesc.dwHeight);
			}
		}
		currentMip->Unlock(0);
		mipResult = currentMip->GetAttachedSurface(
				&mipCaps, &nextMip);
		currentMip->Release();
		currentMip = nextMip;
	}



	DXAttempt(tSurf->Unlock(0));
	return tSurf;
}

void FreeTextures()
{
	TEXTURE* tex;

	DXAttempt(App.dx.lpD3DDevice->SetTexture(0, 0));
	DXAttempt(App.dx.lpD3D->EvictManagedTextures());

	for (int i = 0; i < nTextures; i++)
	{
		tex = &Textures[i];

		if (tex->tex)
		{
			Log(4, "Released %s @ %x - RefCnt = %d", "Texture", tex->tex, tex->tex->Release());
			tex->tex = 0;
		}
		else
			Log(1, "%s Attempt To Release NULL Ptr", "Texture");

		if (tex->surface)
		{
			Log(4, "Released %s @ %x - RefCnt = %d", "Surface", tex->surface, tex->surface->Release());
			tex->surface = 0;
		}
		else
			Log(1, "%s Attempt To Release NULL Ptr", "Surface");
	}

	free(Textures);
	Textures = 0;
}
