#include "../tomb4/pch.h"
#include "dxshell.h"
#include "function_stubs.h"

long DDSCL_FLAGS[11] =	// for DXSetCooperativeLevel logging
{
	DDSCL_ALLOWMODEX,
	DDSCL_ALLOWREBOOT,
	DDSCL_CREATEDEVICEWINDOW,
	DDSCL_EXCLUSIVE,
	DDSCL_FPUSETUP,
	DDSCL_FULLSCREEN,
	DDSCL_MULTITHREADED,
	DDSCL_NORMAL,
	DDSCL_NOWINDOWCHANGES,
	DDSCL_SETDEVICEWINDOW,
	DDSCL_SETFOCUSWINDOW
};

const char* DDSCL_TEXT[11] =
{
	"allowmodex",
	"allowreboot",
	"createdevicewindow",
	"exclusive",
	"fpusetup",
	"fullscreen",
	"multithreaded",
	"normal",
	"nowindowchanges",
	"setdevicewindow",
	"setfocuswindow"
};

void DXBitMask2ShiftCnt(ulong mask, uchar* shift, uchar* count)
{
	uchar i;

	for (i = 0; !(mask & 1); i++)
		mask >>= 1;

	*shift = i;

	for (i = 0; mask & 1; i++)
		mask >>= 1;

	*count = i;
}

void DXReadKeyboard(char* KeyMap)
{
	HRESULT state;

	state = G_dxptr->Keyboard->GetDeviceState(256, KeyMap);

	if (FAILED(state))
	{
		if (state == DIERR_INPUTLOST)
			G_dxptr->Keyboard->Acquire();

		G_dxptr->Keyboard->GetDeviceState(256, KeyMap);
	}
}

long DXAttempt(HRESULT r)
{
	if (SUCCEEDED(r))
		return DD_OK;

	Log(1, "ERROR : %s", DXGetErrorString(r));
	return DD_FALSE;
}

void* AddStruct(void* p, long num, long size)
{
	void* ptr;

	if (!num)
		ptr = malloc(size);
	else
		ptr = realloc(p, size * (num + 1));

	memset((char*)ptr + size * num, 0, size);
	return ptr;
}

long DXDDCreate(LPGUID pGuid, void** pDD4)
{
	LPDIRECTDRAW pDD;

	Log(2, "DXDDCreate");

	if (DXAttempt(DirectDrawCreate(pGuid, &pDD, 0)) != DD_OK)
	{
		Log(1, "DXDDCreate Failed");
		return 0;
	}

	DXAttempt(pDD->QueryInterface(IID_IDirectDraw4, pDD4));

	if (pDD)
	{
		Log(4, "Released %s @ %x - RefCnt = %d", "DirectDraw", pDD, pDD->Release());
		pDD = 0;
	}
	else
		Log(1, "%s Attempt To Release NULL Ptr", "DirectDraw");

	Log(5, "DXDDCreate Successful");
	return 1;
}

long DXD3DCreate(LPDIRECTDRAW4 pDD4, void** pD3D)
{
	Log(2, "DXD3DCreate");

	if (DXAttempt(pDD4->QueryInterface(IID_IDirect3D3, pD3D)) != DD_OK)
	{
		Log(1, "DXD3DCreate Failed");
		return 0;
	}

	Log(5, "DXD3DCreate Successful");
	return 1;
}

long DXSetCooperativeLevel(LPDIRECTDRAW4 pDD4, HWND hwnd, long flags)
{
	char* ptr;
	char buf[1024];

	strcpy(buf, "DXSetCooperativeLevel - ");
	ptr = &buf[strlen(buf)];

	for (int i = 0; i < 11; i++)
	{
		if (DDSCL_FLAGS[i] & flags)
		{
			strcpy(ptr, DDSCL_TEXT[i]);
			ptr += strlen(DDSCL_TEXT[i]);
			*ptr++ = ' ';
			*ptr = '\0';
		}
	}

	*(ptr - 1) = '\0';
	Log(2, buf);

	if (DXAttempt(pDD4->SetCooperativeLevel(hwnd, flags)) != DD_OK)
		return 0;

	return 1;
}

BOOL __stdcall DXEnumDirectDraw(GUID FAR* lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext)
{
	DXINFO* dxinfo;
	DXDIRECTDRAWINFO* DDInfo;
	long nDDInfo;

	Log(2, "DXEnumDirectDraw");
	dxinfo = (DXINFO*)lpContext;
	nDDInfo = dxinfo->nDDInfo;
	dxinfo->DDInfo = (DXDIRECTDRAWINFO*)AddStruct(dxinfo->DDInfo, dxinfo->nDDInfo, sizeof(DXDIRECTDRAWINFO));
	DDInfo = &dxinfo->DDInfo[nDDInfo];

	if (lpGUID)
	{
		DDInfo->lpGuid = &DDInfo->Guid;
		DDInfo->Guid = *lpGUID;
	}
	else
		DDInfo->lpGuid = 0;

	lstrcpy(DDInfo->About, lpDriverDescription);
	lstrcpy(DDInfo->Name, lpDriverName);
	Log(5, "Obtaining Information For %s", lpDriverDescription);

	if (DXDDCreate(lpGUID, (void**)&G_ddraw))
	{
		DXAttempt(G_ddraw->GetDeviceIdentifier(&DDInfo->DDIdentifier, 0));

		Log(5, "Found - %s\nDriver %s Version %d.%d.%d.%d",
			DDInfo->DDIdentifier.szDescription,
			DDInfo->DDIdentifier.szDriver,
			HIWORD(DDInfo->DDIdentifier.liDriverVersion.HighPart),
			LOWORD(DDInfo->DDIdentifier.liDriverVersion.HighPart),
			HIWORD(DDInfo->DDIdentifier.liDriverVersion.LowPart),
			LOWORD(DDInfo->DDIdentifier.liDriverVersion.LowPart));

		memset(&DDInfo->DDCaps, 0, sizeof(DDInfo->DDCaps));
		DDInfo->DDCaps.dwSize = sizeof(DDCAPS);

		Log(5, "Getting Device Capabilities");
		DXAttempt(G_ddraw->GetCaps(&DDInfo->DDCaps, 0));

		Log(5, "Enumerating Display Modes");
		DXSetCooperativeLevel(G_ddraw, 0, DDSCL_FULLSCREEN | DDSCL_NOWINDOWCHANGES | DDSCL_NORMAL | DDSCL_ALLOWMODEX);

		Log(2, "DXEnumDisplayModes");
		DXAttempt(G_ddraw->EnumDisplayModes(0, 0, (void*)DDInfo, DXEnumDisplayModes));

		if (DXD3DCreate(G_ddraw, (void**)&G_d3d))
		{
			Log(2, "DXEnumDirect3D");
			DXAttempt(G_d3d->EnumDevices(DXEnumDirect3D, (void*)DDInfo));

			if (G_d3d)
			{
				Log(4, "Released %s @ %x - RefCnt = %d", "Direct3D", G_d3d, G_d3d->Release());
				G_d3d = 0;
			}
			else
				Log(1, "%s Attempt To Release NULL Ptr", "Direct3D");
		}

		DXSetCooperativeLevel(G_ddraw, 0, DDSCL_NORMAL);

		if (G_ddraw)
		{
			Log(4, "Released %s @ %x - RefCnt = %d", "DirectDraw", G_ddraw, G_ddraw->Release());
			G_ddraw = 0;
		}
		else
			Log(1, "%s Attempt To Release NULL Ptr", "DirectDraw");
	}

	dxinfo->nDDInfo++;
	return DDENUMRET_OK;
}

BOOL __stdcall DXEnumDirectSound(LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext)
{
	DXINFO* dxinfo;
	DXDIRECTSOUNDINFO* DSInfo;
	long nDSInfo;

	Log(2, "DXEnumDirectSound");
	dxinfo = (DXINFO*)lpContext;
	nDSInfo = dxinfo->nDSInfo;
	dxinfo->DSInfo = (DXDIRECTSOUNDINFO*)AddStruct(dxinfo->DSInfo, nDSInfo, sizeof(DXDIRECTSOUNDINFO));
	DSInfo = &dxinfo->DSInfo[nDSInfo];

	if (lpGuid)
	{
		DSInfo->lpGuid = &DSInfo->Guid;
		DSInfo->Guid = *lpGuid;
	}
	else
		DSInfo->lpGuid = 0;

	lstrcpy(DSInfo->About, lpcstrDescription);
	lstrcpy(DSInfo->Name, lpcstrModule);
	Log(5, "Found - %s %s", lpcstrDescription, lpcstrModule);
	dxinfo->nDSInfo++;
	return DDENUMRET_OK;
}

long DXGetInfo(DXINFO* dxinfo, HWND hwnd)
{
	Log(2, "DXInitialise");
	G_hwnd = hwnd;
	Log(5, "Enumerating DirectDraw Devices");
	DXAttempt(DirectDrawEnumerate(DXEnumDirectDraw, dxinfo));
	DXAttempt(DirectSoundEnumerate(DXEnumDirectSound, dxinfo));
	G_dxinfo = dxinfo;
	return 1;
}

void DXFreeInfo(DXINFO* dxinfo)
{
	DXDIRECTDRAWINFO* DDInfo;
	DXD3DDEVICE* d3d;

	Log(2, "DXFreeInfo");

	for (int i = 0; i < dxinfo->nDDInfo; i++)
	{
		DDInfo = &dxinfo->DDInfo[i];

		for (int j = 0; j < DDInfo->nD3DDevices; j++)
		{
			d3d = &DDInfo->D3DDevices[j];
			free(d3d->DisplayModes);
			free(d3d->TextureInfos);
			free(d3d->ZBufferInfos);
		}

		free(DDInfo->D3DDevices);
		free(DDInfo->DisplayModes);
	}

	free(dxinfo->DDInfo);
	free(dxinfo->DSInfo);
}

HRESULT __stdcall DXEnumDisplayModes(LPDDSURFACEDESC2 lpDDSurfaceDesc2, LPVOID lpContext)
{
	DXDIRECTDRAWINFO* DDInfo;
	DXDISPLAYMODE* DM;
	long nDisplayModes;

	DDInfo = (DXDIRECTDRAWINFO*)lpContext;
	nDisplayModes = DDInfo->nDisplayModes;
	DDInfo->DisplayModes = (DXDISPLAYMODE*)AddStruct(DDInfo->DisplayModes, nDisplayModes, sizeof(DXDISPLAYMODE));
	DM = &DDInfo->DisplayModes[nDisplayModes];
	DM->w = lpDDSurfaceDesc2->dwWidth;
	DM->h = lpDDSurfaceDesc2->dwHeight;
	DM->bpp = lpDDSurfaceDesc2->ddpfPixelFormat.dwRGBBitCount;
	DM->bPalette = (lpDDSurfaceDesc2->ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8) >> 5;
	DM->RefreshRate = lpDDSurfaceDesc2->dwRefreshRate;
	memcpy(&DM->ddsd, lpDDSurfaceDesc2, sizeof(DM->ddsd));

	if (DM->bPalette)
		Log(3, "%d x %d - %d Bit - Palette", DM->w, DM->h, DM->bpp);
	else
	{
		DXBitMask2ShiftCnt(lpDDSurfaceDesc2->ddpfPixelFormat.dwRBitMask, &DM->rshift, &DM->rbpp);
		DXBitMask2ShiftCnt(lpDDSurfaceDesc2->ddpfPixelFormat.dwGBitMask, &DM->gshift, &DM->gbpp);
		DXBitMask2ShiftCnt(lpDDSurfaceDesc2->ddpfPixelFormat.dwBBitMask, &DM->bshift, &DM->bbpp);
		Log(3, "%d x %d - %d Bit - %d%d%d", DM->w, DM->h, DM->bpp, DM->rbpp, DM->gbpp, DM->bbpp);
	}

	DDInfo->nDisplayModes++;
	return DDENUMRET_OK;
}

long BPPToDDBD(long BPP)
{
	switch (BPP)
	{
	case 1:
		return DDBD_1;

	case 2:
		return DDBD_2;

	case 4:
		return DDBD_4;

	case 8:
		return DDBD_8;

	case 16:
		return DDBD_16;

	case 24:
		return DDBD_24;

	case 32:
		return DDBD_32;

	default:
		return 0;
	}
}

HRESULT __stdcall DXEnumTextureFormats(LPDDPIXELFORMAT lpDDPixFmt, LPVOID lpContext)
{
	DXD3DDEVICE* d3d;
	DXTEXTUREINFO* tex;
	long nTextureInfos;

	if (!(lpDDPixFmt->dwFlags & DDPF_ALPHAPIXELS) || !(lpDDPixFmt->dwFlags & DDPF_RGB))
		return DDENUMRET_OK;

	d3d = (DXD3DDEVICE*)lpContext;
	nTextureInfos = d3d->nTextureInfos;
	d3d->TextureInfos = (DXTEXTUREINFO*)AddStruct(d3d->TextureInfos, nTextureInfos, sizeof(DXTEXTUREINFO));
	tex = &d3d->TextureInfos[nTextureInfos];
	memcpy(&tex->ddpf, lpDDPixFmt, sizeof(DDPIXELFORMAT));

	if (lpDDPixFmt->dwFlags & DDPF_PALETTEINDEXED8)
	{
		tex->bPalette = 1;
		tex->bpp = 8;
		Log(3, "8 Bit");
	}
	else if (lpDDPixFmt->dwFlags & DDPF_PALETTEINDEXED4)
	{
		tex->bPalette = 1;
		tex->bpp = 4;
		Log(3, "4 Bit");
	}
	else
	{
		tex->bPalette = 0;
		tex->bpp = lpDDPixFmt->dwRGBBitCount;

		if (lpDDPixFmt->dwFlags & DDPF_RGB)
		{
			if (lpDDPixFmt->dwFlags & DDPF_ALPHAPIXELS)
			{
				DXBitMask2ShiftCnt(lpDDPixFmt->dwRBitMask, &tex->rshift, &tex->rbpp);
				DXBitMask2ShiftCnt(lpDDPixFmt->dwGBitMask, &tex->gshift, &tex->gbpp);
				DXBitMask2ShiftCnt(lpDDPixFmt->dwBBitMask, &tex->bshift, &tex->bbpp);
				DXBitMask2ShiftCnt(lpDDPixFmt->dwRGBAlphaBitMask, &tex->ashift, &tex->abpp);
				Log(3, "%d Bit %d%d%d%d RGBA", tex->bpp, tex->rbpp, tex->gbpp, tex->bbpp, tex->abpp);
			}
			else
			{
				DXBitMask2ShiftCnt(lpDDPixFmt->dwRBitMask, &tex->rshift, &tex->rbpp);
				DXBitMask2ShiftCnt(lpDDPixFmt->dwGBitMask, &tex->gshift, &tex->gbpp);
				DXBitMask2ShiftCnt(lpDDPixFmt->dwBBitMask, &tex->bshift, &tex->bbpp);
				Log(3, "%d Bit %d%d%d RGB", tex->bpp, tex->rbpp, tex->gbpp, tex->bbpp);
			}
		}
	}

	d3d->nTextureInfos++;
	return D3DENUMRET_OK;
}

void inject_dxshell(bool replace)
{
	INJECT(0x00492240, DXBitMask2ShiftCnt, replace);
	INJECT(0x004944D0, DXReadKeyboard, replace);
	INJECT(0x00491C30, DXAttempt, replace);
	INJECT(0x00491E50, AddStruct, replace);
	INJECT(0x00491EA0, DXDDCreate, replace);
	INJECT(0x00491F60, DXD3DCreate, replace);
	INJECT(0x00492BE0, DXSetCooperativeLevel, replace);
	INJECT(0x00491FC0, DXEnumDirectDraw, replace);
	INJECT(0x00491CC0, DXEnumDirectSound, replace);
	INJECT(0x00491C60, DXGetInfo, replace);
	INJECT(0x00491D60, DXFreeInfo, replace);
	INJECT(0x00492280, DXEnumDisplayModes, replace);
	INJECT(0x004923A0, BPPToDDBD, replace);
	INJECT(0x00492920, DXEnumTextureFormats, replace);
}
