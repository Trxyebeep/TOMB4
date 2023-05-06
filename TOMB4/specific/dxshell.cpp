#include "../tomb4/pch.h"
#include "dxshell.h"
#include "function_stubs.h"
#include "winmain.h"

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

char tga_header[18] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, 1, 0, 1, 16, 0 };

DXPTR* G_dxptr;
DXINFO* G_dxinfo;
LPDIRECTDRAWX G_ddraw;
LPDIRECT3DX G_d3d;
HWND G_hwnd;
char keymap[256];

static char keymap2[256];

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

	DXAttempt(pDD->QueryInterface(DDGUID, pDD4));

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

long DXD3DCreate(LPDIRECTDRAWX pDD4, void** pD3D)
{
	Log(2, "DXD3DCreate");

	if (DXAttempt(pDD4->QueryInterface(D3DGUID, pD3D)) != DD_OK)
	{
		Log(1, "DXD3DCreate Failed");
		return 0;
	}

	Log(5, "DXD3DCreate Successful");
	return 1;
}

long DXSetCooperativeLevel(LPDIRECTDRAWX pDD4, HWND hwnd, long flags)
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

HRESULT __stdcall DXEnumDisplayModes(LPDDSURFACEDESCX lpDDSurfaceDesc2, LPVOID lpContext)
{
	DXDIRECTDRAWINFO* DDInfo;
	DXDISPLAYMODE* DM;
	long nDisplayModes;

	if (lpDDSurfaceDesc2->ddpfPixelFormat.dwRGBBitCount == 16)
		return DDENUMRET_OK;

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

	if (lpDDPixFmt->dwRGBBitCount == 16)
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

HRESULT __stdcall DXEnumZBufferFormats(LPDDPIXELFORMAT lpDDPixFmt, LPVOID lpContext)
{
	DXD3DDEVICE* d3d;
	DXZBUFFERINFO* zbuffer;
	long nZBufferInfos;

	d3d = (DXD3DDEVICE*)lpContext;
	nZBufferInfos = d3d->nZBufferInfos;
	d3d->ZBufferInfos = (DXZBUFFERINFO*)AddStruct(d3d->ZBufferInfos, nZBufferInfos, sizeof(DXZBUFFERINFO));
	zbuffer = &d3d->ZBufferInfos[nZBufferInfos];
	memcpy(&zbuffer->ddpf, lpDDPixFmt, sizeof(DDPIXELFORMAT));
	zbuffer->bpp = lpDDPixFmt->dwRGBBitCount;
	Log(3, "%d Bit", zbuffer->bpp);
	d3d->nZBufferInfos++;
	return D3DENUMRET_OK;
}

long DXCreateSurface(LPDIRECTDRAWX dd, LPDDSURFACEDESCX desc, LPDIRECTDRAWSURFACEX* surf)
{
	Log(2, "DXCreateSurface");

	if (DXAttempt(dd->CreateSurface(desc, surf, 0)) == DD_OK)
		return 1;

	Log(1, "DXCreateSurface Failed");
	return 0;
}

long DXSetVideoMode(LPDIRECTDRAWX dd, long dwWidth, long dwHeight, long dwBPP)
{
	Log(2, "DXSetVideoMode");
	Log(5, "SetDisplayMode - %dx%dx%d", dwWidth, dwHeight, dwBPP);

	if (DXAttempt(dd->SetDisplayMode(dwWidth, dwHeight, dwBPP, 0, 0)) != DD_OK)
		return 0;

	return 1;
}

long DXCreateD3DDevice(LPDIRECT3DX d3d, GUID guid, LPDIRECTDRAWSURFACEX surf, LPDIRECT3DDEVICEX* device)
{
	Log(2, "DXCreateD3DDevice");

	if (DXAttempt(d3d->CreateDevice(guid, surf, device, 0)) != DD_OK)
	{
		Log(1, "DXCreateD3DDevice Failed");
		return 0;
	}
	else
	{
		Log(2, "DXCreateD3DDevice Successful");
		return 1;
	}
}

long DXCreateViewport(LPDIRECT3DX d3d, LPDIRECT3DDEVICEX device, long w, long h, LPDIRECT3DVIEWPORTX* viewport)
{
	D3DVIEWPORT2 vp2;

	Log(2, "DXCreateViewport");


	if (DXAttempt(d3d->CreateViewport(viewport, 0)) != DD_OK)
		return 0;

	if (DXAttempt(device->AddViewport(*viewport)) != DD_OK)
		return 0;

	memset(&vp2, 0, sizeof(D3DVIEWPORT2));
	vp2.dwSize = sizeof(D3DVIEWPORT2);
	vp2.dvClipWidth = (float)w;
	vp2.dvClipHeight = (float)h;
	vp2.dwX = 0;
	vp2.dwY = 0;
	vp2.dvClipX = 0;
	vp2.dvClipY = 0;
	vp2.dvMinZ = 0;
	vp2.dvMaxZ = 1;
	vp2.dwWidth = w;
	vp2.dwHeight = h;

	if (DXAttempt((*viewport)->SetViewport2(&vp2)) != DD_OK)
		return 0;

	DXAttempt(device->SetCurrentViewport(*viewport));
	return 1;
}

HRESULT DXShowFrame()
{
	if (G_dxptr->lpPrimaryBuffer->IsLost())
	{
		Log(3, "Restored Primary Buffer");
		DXAttempt(G_dxptr->lpPrimaryBuffer->Restore());
	}

	if (G_dxptr->lpBackBuffer->IsLost())
	{
		Log(3, "Restored Back Buffer");
		DXAttempt(G_dxptr->lpBackBuffer->Restore());
	}

	if (!(App.dx.Flags & (DXF_HWR | DXF_WINDOWED)))
		return 0;

	if (G_dxptr->Flags & DXF_WINDOWED)
		return DXAttempt(G_dxptr->lpPrimaryBuffer->Blt(&G_dxptr->rScreen, G_dxptr->lpBackBuffer, &G_dxptr->rViewport, DDBLT_WAIT, 0));
	else
		return DXAttempt(G_dxptr->lpPrimaryBuffer->Flip(0, DDFLIP_WAIT));
}

void DXMove(long x, long y)
{
	Log(2, "DXMove : x %d y %d", x, y);

	if (G_dxptr && !(G_dxptr->Flags & DXF_FULLSCREEN))
		SetRect(&G_dxptr->rScreen, x, y, x + G_dxptr->dwRenderWidth, y + G_dxptr->dwRenderHeight);
}

void DXInitKeyboard(HWND hwnd, HINSTANCE hinstance)
{
	IDirectInput* dinput;
	IDirectInputDevice* Keyboard;

	DXAttempt(DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&dinput, 0));
	dinput->QueryInterface(DIGUID, (void**)&G_dxptr->lpDirectInput);

	if (dinput)
	{
		Log(4, "Released %s @ %x - RefCnt = %d", "DirectInput", dinput, dinput->Release());
		dinput = 0;
	}
	else
		Log(1, "%s Attempt To Release NULL Ptr", "DirectInput");

	DXAttempt(G_dxptr->lpDirectInput->CreateDevice(GUID_SysKeyboard, &Keyboard, 0));
	Keyboard->QueryInterface(DIDGUID, (void**)&G_dxptr->Keyboard);

	if (Keyboard)
	{
		Log(4, "Released %s @ %x - RefCnt = %d", "Keyboard", Keyboard, Keyboard->Release());
		Keyboard = 0;
	}
	else
		Log(1, "%s Attempt To Release NULL Ptr", "Keyboard");

	DXAttempt(G_dxptr->Keyboard->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND));
	DXAttempt(G_dxptr->Keyboard->SetDataFormat(&c_dfDIKeyboard));
	DXAttempt(G_dxptr->Keyboard->Acquire());
	memset(keymap, 0, sizeof(keymap));
	memset(keymap2, 0, sizeof(keymap2));
}

void DXSaveScreen(LPDIRECTDRAWSURFACEX surf, const char* name)
{
	FILE* file;
	DDSURFACEDESCX desc;
	short* pSurf;
	short* pDest;
	char* pM;
	ulong val;
	static long num = 0;
	long r, g, b;
	char buf[16];

	memset(&desc, 0, sizeof(DDSURFACEDESCX));
	desc.dwSize = sizeof(DDSURFACEDESCX);
	DXAttempt(surf->GetSurfaceDesc(&desc));
	DXAttempt(surf->Lock(0, &desc, DDLOCK_WAIT, 0));
	pSurf = (short*)desc.lpSurface;
	sprintf(buf, "%s%04d.tga", name, num);
	num++;
	file = fopen(buf, "wb");

	if (file)
	{
		*(short*)&tga_header[12] = (short)desc.dwWidth;
		*(short*)&tga_header[14] = (short)desc.dwHeight;
		fwrite(tga_header, sizeof(tga_header), 1, file);
		pM = (char*)malloc(2 * desc.dwWidth * desc.dwHeight);
		pDest = (short*)pM;
		pSurf += desc.dwHeight * (desc.lPitch / 2);

		for (ulong h = 0; h < desc.dwHeight; h++)
		{
			for (ulong w = 0; w < desc.dwWidth; w++)
			{
				val = pSurf[w];

				if (desc.ddpfPixelFormat.dwRBitMask == 0xF800)
				{
					r = (val >> 11) & 0x1F;
					g = (val >> 6) & 0x1F;
					b = val & 0x1F;
					*pDest++ = short((r << 10) + (g << 5) + b);
				}
				else
					*pDest++ = (short)val;
			}

			pSurf -= desc.lPitch / 2;
		}

		fwrite(pM, 2 * desc.dwWidth * desc.dwHeight, 1, file);
		fclose(file);
		free(pM);
		buf[7]++;

		if (buf[7] > '9')
		{
			buf[7] = '0';
			buf[6]++;
		}
	}

	DXAttempt(surf->Unlock(0));
}

void DXClose()
{
	Log(2, "CloseDirectX");

	if (G_dxptr)
	{
		if (G_dxptr->lpViewport)
		{
			Log(4, "Released %s @ %x - RefCnt = %d", "Viewport", G_dxptr->lpViewport, G_dxptr->lpViewport->Release());
			G_dxptr->lpViewport = 0;
		}
		else
			Log(1, "%s Attempt To Release NULL Ptr", "Viewport");

		if (G_dxptr->lpD3DDevice)
		{
			Log(4, "Released %s @ %x - RefCnt = %d", "Direct3DDevice", G_dxptr->lpD3DDevice, G_dxptr->lpD3DDevice->Release());
			G_dxptr->lpD3DDevice = 0;
		}
		else
			Log(1, "%s Attempt To Release NULL Ptr", "Direct3DDevice");

		if (G_dxptr->lpZBuffer)
		{
			Log(4, "Released %s @ %x - RefCnt = %d", "Z Buffer", G_dxptr->lpZBuffer, G_dxptr->lpZBuffer->Release());
			G_dxptr->lpZBuffer = 0;
		}
		else
			Log(1, "%s Attempt To Release NULL Ptr", "Z Buffer");

		if (G_dxptr->lpBackBuffer)
		{
			Log(4, "Released %s @ %x - RefCnt = %d", "Back Buffer", G_dxptr->lpBackBuffer, G_dxptr->lpBackBuffer->Release());
			G_dxptr->lpBackBuffer = 0;
		}
		else
			Log(1, "%s Attempt To Release NULL Ptr", "Back Buffer");

		if (G_dxptr->lpPrimaryBuffer)
		{
			Log(4, "Released %s @ %x - RefCnt = %d", "Primary Buffer", G_dxptr->lpPrimaryBuffer, G_dxptr->lpPrimaryBuffer->Release());
			G_dxptr->lpPrimaryBuffer = 0;
		}
		else
			Log(1, "%s Attempt To Release NULL Ptr", "Primary Buffer");

		if (!(G_dxptr->Flags & DXF_NOFREE))
		{
			if (G_dxptr->lpDD)
			{
				Log(4, "Released %s @ %x - RefCnt = %d", "DirectDraw", G_dxptr->lpDD, G_dxptr->lpDD->Release());
				G_dxptr->lpDD = 0;
			}
			else
				Log(1, "%s Attempt To Release NULL Ptr", "DirectDraw");

			if (G_dxptr->lpD3D)
			{
				Log(4, "Released %s @ %x - RefCnt = %d", "Direct3D", G_dxptr->lpD3D, G_dxptr->lpD3D->Release());
				G_dxptr->lpD3D = 0;
			}
			else
				Log(1, "%s Attempt To Release NULL Ptr", "Direct3D");
		}
	}
}

long DXCreate(long w, long h, long bpp, long Flags, DXPTR* dxptr, HWND hWnd, long WindowStyle)
{
	DXDISPLAYMODE* dm;
	LPDIRECTDRAWCLIPPER clipper;
	HWND desktop;
	DEVMODE dev;
	HDC hDC;
	DDSURFACEDESCX desc;
	RECT r;
	long flag, CoopLevel;

	flag = 0;
	Log(2, "DXCreate");
	G_dxptr = dxptr;
	G_dxptr->Flags = Flags;
	G_dxptr->hWnd = hWnd;
	G_dxptr->WindowStyle = WindowStyle;

	if (Flags & 64)
		flag = 1;

	DXClose();

	if (!flag)
	{
		if (!DXDDCreate(G_dxinfo->DDInfo[G_dxinfo->nDD].lpGuid, (void**)&G_dxptr->lpDD) || !DXD3DCreate(G_dxptr->lpDD, (void**)&G_dxptr->lpD3D))
		{
			DXClose();
			return 0;
		}
	}

	if (Flags & DXF_FULLSCREEN)
		CoopLevel = DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT | DDSCL_EXCLUSIVE;
	else
		CoopLevel = DDSCL_NORMAL;

	if (Flags & DXF_FPUSETUP)
		CoopLevel |= DDSCL_FPUSETUP;

	G_dxptr->CoopLevel = CoopLevel;

	if (!DXSetCooperativeLevel(G_dxptr->lpDD, hWnd, CoopLevel))
	{
		DXClose();
		return 0;
	}

	if (Flags & DXF_FULLSCREEN)
	{
		dm = &G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].DisplayModes[G_dxinfo->nDisplayMode];
		DXSetVideoMode(G_dxptr->lpDD, dm->w, dm->h, dm->bpp);
	}
	else
	{
		desktop = GetDesktopWindow();
		hDC = GetDC(desktop);
		ReleaseDC(desktop, hDC);
		dev.dmBitsPerPel = G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].DisplayModes[G_dxinfo->nDisplayMode].bpp;
		dev.dmSize = 148;	//sizeof(DEVMODE) is 156????
		dev.dmFields = DM_BITSPERPEL;
		ChangeDisplaySettings(&dev, 0);
	}

	memset(&desc, 0, sizeof(DDSURFACEDESCX));
	desc.dwSize = sizeof(DDSURFACEDESCX);

	if (Flags & DXF_FULLSCREEN)
	{
		desc.dwBackBufferCount = 1;
		desc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		desc.ddsCaps.dwCaps = DDSCAPS_COMPLEX | DDSCAPS_FLIP | DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE | DDSCAPS_VIDEOMEMORY;

		if (!(Flags & DXF_HWR))
		{
			desc.dwBackBufferCount = 0;
			desc.dwFlags = DDSD_CAPS;
			desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE | DDSCAPS_VIDEOMEMORY;
		}

		Log(3, "Create Primary Surface");

		if (!DXCreateSurface(G_dxptr->lpDD, &desc, &G_dxptr->lpPrimaryBuffer))
		{
			DXClose();
			return 0;
		}

		if (Flags & DXF_HWR)
		{
			Log(3, "Get Attached Back Buffer");
			desc.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
			G_dxptr->lpPrimaryBuffer->GetAttachedSurface(&desc.ddsCaps, &G_dxptr->lpBackBuffer);
		}
		else
			G_dxptr->lpBackBuffer = G_dxptr->lpPrimaryBuffer;

		dm = &G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].DisplayModes[G_dxinfo->nDisplayMode];
		G_dxptr->dwRenderWidth = dm->w;
		G_dxptr->dwRenderHeight = dm->h;
		G_dxptr->rViewport.top = 0;
		G_dxptr->rViewport.left = 0;
		G_dxptr->rViewport.right = dm->w;
		G_dxptr->rViewport.bottom = dm->h;
	}
	else
	{
		Log(5, "DXCreate: Windowed Mode");
		dm = &G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].DisplayModes[G_dxinfo->nDisplayMode];
		r.top = 0;
		r.left = 0;
		r.right = dm->w;
		r.bottom = dm->h;
		AdjustWindowRect(&r, WindowStyle, 0);
		SetWindowPos(hWnd, 0, 0, 0, r.right - r.left, r.bottom - r.top, SWP_NOMOVE | SWP_NOZORDER);
		GetClientRect(hWnd, &G_dxptr->rViewport);
		GetClientRect(hWnd, &G_dxptr->rScreen);
		ClientToScreen(hWnd, (LPPOINT)&G_dxptr->rScreen);
		ClientToScreen(hWnd, (LPPOINT)&G_dxptr->rScreen.right);
		G_dxptr->dwRenderWidth = G_dxptr->rViewport.right;
		G_dxptr->dwRenderHeight = G_dxptr->rViewport.bottom;
		Log(5, "w %d h %d", G_dxptr->dwRenderWidth, G_dxptr->dwRenderHeight);
		desc.dwFlags = DDSD_CAPS;
		desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

		if (!DXCreateSurface(G_dxptr->lpDD, &desc, &G_dxptr->lpPrimaryBuffer))
		{
			DXClose();
			return 0;
		}

		if (DXAttempt(G_dxptr->lpDD->CreateClipper(0, &clipper, 0)) != DD_OK)
		{
			DXClose();
			return 0;
		}

		DXAttempt(clipper->SetHWnd(0, hWnd));
		DXAttempt(G_dxptr->lpPrimaryBuffer->SetClipper(clipper));

		if (clipper)
		{
			Log(4, "Released %s @ %x - RefCnt = %d", "Clipper", clipper, clipper->Release());
			clipper = 0;
		}
		else
			Log(1, "%s Attempt To Release NULL Ptr", "Clipper");

		desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
		desc.dwWidth = G_dxptr->dwRenderWidth;
		desc.dwHeight = G_dxptr->dwRenderHeight;
		desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;

		if (DXAttempt(G_dxptr->lpDD->CreateSurface(&desc, &G_dxptr->lpBackBuffer, 0)) != DD_OK)
		{
			DXClose();
			return 0;
		}
	}

	if (Flags & DXF_ZBUFFER && Flags & DXF_HWR)
	{
		Log(3, "Creating ZBuffer");
		memset(&desc, 0, sizeof(DDSURFACEDESCX));
		desc.dwSize = sizeof(DDSURFACEDESCX);
		desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
		desc.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_ZBUFFER;
		desc.dwWidth = G_dxptr->dwRenderWidth;
		desc.dwHeight = G_dxptr->dwRenderHeight;
		memcpy(&desc.ddpfPixelFormat, &G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].ZBufferInfos->ddpf, sizeof(DDPIXELFORMAT));

		if (DXAttempt(G_dxptr->lpDD->CreateSurface(&desc, &G_dxptr->lpZBuffer, 0)) != DD_OK)
		{
			DXClose();
			return 0;
		}

		DXAttempt(G_dxptr->lpBackBuffer->AddAttachedSurface(G_dxptr->lpZBuffer));
		Log(3, "ZBuffer Created %x", G_dxptr->lpZBuffer);
	}

	if (!DXCreateD3DDevice(G_dxptr->lpD3D, G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].Guid, G_dxptr->lpBackBuffer, &G_dxptr->lpD3DDevice))
	{
		DXClose();
		return 0;
	}

	G_dxptr->_lpD3DDevice = G_dxptr->lpD3DDevice;

	if (!DXCreateViewport(G_dxptr->lpD3D, G_dxptr->lpD3DDevice, G_dxptr->dwRenderWidth, G_dxptr->dwRenderHeight, &G_dxptr->lpViewport))
	{
		DXClose();
		return 0;
	}

	DXAttempt(G_dxptr->lpD3DDevice->SetRenderTarget(G_dxptr->lpBackBuffer, 0));
	return 1;
}

long DXChangeVideoMode()
{
	long val;

	Log(2, "DXChangeVideoMode");
	G_dxptr->Flags |= DXF_NOFREE;
	G_dxptr->lpD3D->EvictManagedTextures();
	val = DXCreate(0, 0, 0, G_dxptr->Flags, G_dxptr, G_dxptr->hWnd, G_dxptr->WindowStyle);
	G_dxptr->Flags ^= DXF_NOFREE;
	Log(2, "Exited DXChangeVideoMode %d", val);
	return val;
}

long DXToggleFullScreen()
{
	DXDISPLAYMODE* dm;

	Log(2, "DXToggleFullScreen");

	if (G_dxptr->Flags & 2)
	{
		Log(5, "Switching To Full Screen");
		G_dxptr->Flags ^= DXF_WINDOWED;
		G_dxptr->Flags |= DXF_NOFREE | DXF_FULLSCREEN;
	}
	else
	{
		Log(5, "Switching To A Window");
		G_dxptr->Flags ^= DXF_FULLSCREEN;
		G_dxptr->Flags |= DXF_NOFREE | DXF_WINDOWED;
	}

	G_dxptr->lpD3D->EvictManagedTextures();
	dm = &G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].DisplayModes[G_dxinfo->nDisplayMode];
	DXCreate(dm->w, dm->h, dm->bpp, G_dxptr->Flags, G_dxptr, G_dxptr->hWnd, G_dxptr->WindowStyle);
	WinSetStyle(G_dxptr->Flags & DXF_FULLSCREEN, G_dxptr->WindowStyle);
	G_dxptr->Flags ^= DXF_NOFREE;
	return 1;
}

HRESULT __stdcall DXEnumDirect3D(LPGUID lpGuid, LPSTR lpDeviceDescription, LPSTR lpDeviceName, LPD3DDEVICEDESC lpHWDesc, LPD3DDEVICEDESC lpHELDesc, LPVOID lpContext)
{
	DXDIRECTDRAWINFO* ddi;
	DXD3DDEVICE* device;
	LPDIRECT3DDEVICEX d3dDevice;
	DXDISPLAYMODE* dm;
	LPDIRECTDRAWSURFACEX surf;
	DDSURFACEDESCX desc;
	long nD3DDevices;

	ddi = (DXDIRECTDRAWINFO*)lpContext;
	nD3DDevices = ddi->nD3DDevices;
	ddi->D3DDevices = (DXD3DDEVICE*)AddStruct(ddi->D3DDevices, nD3DDevices, sizeof(DXD3DDEVICE));
	device = &ddi->D3DDevices[nD3DDevices];

	if (lpGuid)
	{
		device->lpGuid = &device->Guid;
		device->Guid = *lpGuid;
	}
	else
		device->lpGuid = 0;

	lstrcpy(device->About, lpDeviceDescription);
	lstrcpy(device->Name, lpDeviceName);
	Log(5, "Found - %s", lpDeviceDescription);

	if (lpHWDesc->dwFlags)
	{
		device->bHardware = 1;
		memcpy(&device->DeviceDesc, lpHWDesc, sizeof(D3DDEVICEDESC));
	}
	else
	{
		device->bHardware = 0;
		memcpy(&device->DeviceDesc, lpHELDesc, sizeof(D3DDEVICEDESC));

		if (!App.mmx)
			strcpy(device->About, "Core Design Hardware Card Emulation");
		else
			strcpy(device->About, "Core Design MMX Hardware Card Emulation");
	}

	Log(5, "Finding Compatible Display Modes");
	device->nDisplayModes = 0;

	for (int i = 0; i < ddi->nDisplayModes; i++)
	{
		if (BPPToDDBD(ddi->DisplayModes[i].bpp) & device->DeviceDesc.dwDeviceRenderBitDepth)
		{
			device->DisplayModes = (DXDISPLAYMODE*)AddStruct(device->DisplayModes, ddi->nDisplayModes, sizeof(DXDISPLAYMODE));
			dm = &device->DisplayModes[device->nDisplayModes];
			memcpy(dm, &ddi->DisplayModes[i], sizeof(DXDISPLAYMODE));

			if (dm->bPalette)
				Log(3, "%d x %d - %d Bit - Palette", dm->w, dm->h, dm->bpp);
			else
				Log(3, "%d x %d - %d Bit - %d%d%d", dm->w, dm->h, dm->bpp, dm->rbpp, dm->gbpp, dm->bbpp);

			device->nDisplayModes++;
		}
	}

	Log(5, "Enumerate Texture Formats");
	memset(&desc, 0, sizeof(DDSURFACEDESCX));
	desc.dwSize = sizeof(DDSURFACEDESCX);
	desc.dwFlags = DDSD_CAPS;
	desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE;
	DXSetCooperativeLevel(G_ddraw, G_hwnd, DDSCL_FULLSCREEN | DDSCL_NOWINDOWCHANGES | DDSCL_EXCLUSIVE);
	DXCreateSurface(G_ddraw, &desc, &surf);

	if (surf)
	{
		DXCreateD3DDevice(G_d3d, device->Guid, surf, &d3dDevice);

		if (!d3dDevice)	//fail
		{
			Log(1, "Create D3DDevice Failed");

			//release surface, recreate it and try again

			if (surf)
			{
				Log(4, "Released %s @ %x - RefCnt = %d", "DirectDrawSurface", surf, surf->Release());
				surf = 0;
			}
			else
				Log(1, "%s Attempt To Release NULL Ptr", "DirectDrawSurface");

			DXSetVideoMode(G_ddraw, device->DisplayModes->w, device->DisplayModes->h, device->DisplayModes->bpp);
			DXCreateSurface(G_ddraw, &desc, &surf);

			if (surf)
				DXCreateD3DDevice(G_d3d, device->Guid, surf, &d3dDevice);
		}

		if (d3dDevice)	//did it work?
		{
			device->nTextureInfos = 0;
			Log(2, "DXEnumTextureFormats");
			DXAttempt(d3dDevice->EnumTextureFormats(DXEnumTextureFormats, (void*)device));

			if (d3dDevice)
			{
				Log(4, "Released %s @ %x - RefCnt = %d", "D3DDevice", d3dDevice, d3dDevice->Release());
				d3dDevice = 0;
			}
			else
				Log(1, "%s Attempt To Release NULL Ptr", "D3DDevice");
		}

		if (surf)
		{
			Log(4, "Released %s @ %x - RefCnt = %d", "DirectDrawSurface", surf, surf->Release());
			surf = 0;
		}
		else
			Log(1, "%s Attempt To Release NULL Ptr", "DirectDrawSurface");
	}

	DXSetCooperativeLevel(G_ddraw, G_hwnd, DDSCL_NORMAL);
	Log(5, "Enumerating ZBuffer Formats");
	Log(2, "DXEnumZBufferFormats");
	DXAttempt(G_d3d->EnumZBufferFormats(device->Guid, DXEnumZBufferFormats, (void*)device));
	ddi->nD3DDevices++;
	return D3DENUMRET_OK;
}

const char* DXGetErrorString(HRESULT hr)
{
	switch (hr)
	{
	case DD_OK:
		return "The request completed successfully.";

	case DDERR_ALREADYINITIALIZED:
		return "The object has already been initialized.";

	case DDERR_BLTFASTCANTCLIP:
		return "A DirectDrawClipper object is attached to a source surface that has passed into a call to the IDirectDrawSurface4::BltFast method.";

	case DDERR_CANNOTATTACHSURFACE:
		return "A surface cannot be attached to another requested surface.";

	case DDERR_CANNOTDETACHSURFACE:
		return "A surface cannot be detached from another requested surface.";

	case DDERR_CANTCREATEDC:
		return "Windows can not create any more device contexts (DCs), or a DC was requested for a palette-indexed surface when the surface had no palette and the display mode was not palette-indexed (in this case DirectDraw cannot select a proper palette into the DC).";

	case DDERR_CANTDUPLICATE:
		return "Primary and 3-D surfaces, or surfaces that are implicitly created, cannot be duplicated. ";

	case DDERR_CANTLOCKSURFACE:
		return "Access to this surface is refused because an attempt was made to lock the primary surface without DCI support. ";

	case DDERR_CANTPAGELOCK:
		return "An attempt to page lock a surface failed. Page lock will not work on a display-memory surface or an emulated primary surface. ";

	case DDERR_CANTPAGEUNLOCK:
		return "An attempt to page unlock a surface failed. Page unlock will not work on a display-memory surface or an emulated primary surface. ";

	case DDERR_CLIPPERISUSINGHWND:
		return "An attempt was made to set a clip list for a DirectDrawClipper object that is already monitoring a window handle. ";

	case DDERR_COLORKEYNOTSET:
		return "No source color key is specified for this operation. ";

	case DDERR_CURRENTLYNOTAVAIL:
		return "No support is currently available. ";

	case DDERR_DCALREADYCREATED:
		return "A device context (DC) has already been returned for this surface. Only one DC can be retrieved for each surface. ";

	case DDERR_DEVICEDOESNTOWNSURFACE:
		return "Surfaces created by one DirectDraw device cannot be used directly by another DirectDraw device. ";

	case DDERR_DIRECTDRAWALREADYCREATED:
		return "A DirectDraw object representing this driver has already been created for this process. ";

	case DDERR_EXCEPTION:
		return "An exception was encountered while performing the requested operation. ";

	case DDERR_EXCLUSIVEMODEALREADYSET:
		return "An attempt was made to set the cooperative level when it was already set to exclusive. ";

	case DDERR_EXPIRED:
		return "The data has expired and is therefore no longer valid. ";

	case DDERR_GENERIC:
		return "There is an undefined error condition. ";

	case DDERR_HEIGHTALIGN:
		return "The height of the provided rectangle is not a multiple of the required alignment. ";

	case DDERR_HWNDALREADYSET:
		return "The DirectDraw cooperative level window handle has already been set. It cannot be reset while the process has surfaces or palettes created. ";

	case DDERR_HWNDSUBCLASSED:
		return "DirectDraw is prevented from restoring state because the DirectDraw cooperative level window handle has been subclassed. ";

	case DDERR_IMPLICITLYCREATED:
		return "The surface cannot be restored because it is an implicitly created surface. ";

	case DDERR_INCOMPATIBLEPRIMARY:
		return "The primary surface creation request does not match with the existing primary surface. ";

	case DDERR_INVALIDCAPS:
		return "One or more of the capability bits passed to the callback function are incorrect. ";

	case DDERR_INVALIDCLIPLIST:
		return "DirectDraw does not support the provided clip list. ";

	case DDERR_INVALIDDIRECTDRAWGUID:
		return "The globally unique identifier (GUID) passed to the DirectDrawCreate function is not a valid DirectDraw driver identifier. ";

	case DDERR_INVALIDMODE:
		return "DirectDraw does not support the requested mode. ";

	case DDERR_INVALIDOBJECT:
		return "DirectDraw received a pointer that was an invalid DirectDraw object. ";

	case DDERR_INVALIDPARAMS:
		return "One or more of the parameters passed to the method are incorrect. ";

	case DDERR_INVALIDPIXELFORMAT:
		return "The pixel format was invalid as specified. ";

	case DDERR_INVALIDPOSITION:
		return "The position of the overlay on the destination is no longer legal. ";

	case DDERR_INVALIDRECT:
		return "The provided rectangle was invalid. ";

	case DDERR_INVALIDSTREAM:
		return "The specified stream contains invalid data. ";

	case DDERR_INVALIDSURFACETYPE:
		return "The requested operation could not be performed because the surface was of the wrong type. ";

	case DDERR_LOCKEDSURFACES:
		return "One or more surfaces are locked, causing the failure of the requested operation. ";

	case DDERR_MOREDATA:
		return "There is more data available than the specified buffer size can hold. ";

	case DDERR_NO3D:
		return "No 3-D hardware or emulation is present. ";

	case DDERR_NOALPHAHW:
		return "No alpha acceleration hardware is present or available, causing the failure of the requested operation. ";

	case DDERR_NOBLTHW:
		return "No blitter hardware is present. ";

	case DDERR_NOCLIPLIST:
		return "No clip list is available. ";

	case DDERR_NOCLIPPERATTACHED:
		return "No DirectDrawClipper object is attached to the surface object. ";

	case DDERR_NOCOLORCONVHW:
		return "The operation cannot be carried out because no color-conversion hardware is present or available. ";

	case DDERR_NOCOLORKEY:
		return "The surface does not currently have a color key. ";

	case DDERR_NOCOLORKEYHW:
		return "The operation cannot be carried out because there is no hardware support for the destination color key. ";

	case DDERR_NOCOOPERATIVELEVELSET:
		return "A create function is called without the IDirectDraw4::SetCooperativeLevel method being called. ";

	case DDERR_NODC:
		return "No DC has ever been created for this surface. ";

	case DDERR_NODDROPSHW:
		return "No DirectDraw raster operation (ROP) hardware is available. ";

	case DDERR_NODIRECTDRAWHW:
		return "Hardware-only DirectDraw object creation is not possible; the driver does not support any hardware. ";

	case DDERR_NODIRECTDRAWSUPPORT:
		return "DirectDraw support is not possible with the current display driver. ";

	case DDERR_NOEMULATION:
		return "Software emulation is not available. ";

	case DDERR_NOEXCLUSIVEMODE:
		return "The operation requires the application to have exclusive mode, but the application does not have exclusive mode. ";

	case DDERR_NOFLIPHW:
		return "Flipping visible surfaces is not supported. ";

	case DDERR_NOFOCUSWINDOW:
		return "An attempt was made to create or set a device window without first setting the focus window. ";

	case DDERR_NOGDI:
		return "No GDI is present. ";

	case DDERR_NOHWND:
		return "Clipper notification requires a window handle, or no window handle has been previously set as the cooperative level window handle. ";

	case DDERR_NOMIPMAPHW:
		return "The operation cannot be carried out because no mipmap capable texture mapping hardware is present or available. ";

	case DDERR_NOMIRRORHW:
		return "The operation cannot be carried out because no mirroring hardware is present or available. ";

	case DDERR_NONONLOCALVIDMEM:
		return "An attempt was made to allocate non-local video memory from a device that does not support non-local video memory. ";

	case DDERR_NOOPTIMIZEHW:
		return "The device does not support optimized surfaces. ";

	case DDERR_NOOVERLAYDEST:
		return "The IDirectDrawSurface4::GetOverlayPosition method is called on an overlay that the IDirectDrawSurface4::UpdateOverlay method has not been called on to establish a destination. ";

	case DDERR_NOOVERLAYHW:
		return "The operation cannot be carried out because no overlay hardware is present or available. ";

	case DDERR_NOPALETTEATTACHED:
		return "No palette object is attached to this surface. ";

	case DDERR_NOPALETTEHW:
		return "There is no hardware support for 16- or 256-color palettes. ";

	case DDERR_NORASTEROPHW:
		return "The operation cannot be carried out because no appropriate raster operation hardware is present or available. ";

	case DDERR_NOROTATIONHW:
		return "The operation cannot be carried out because no rotation hardware is present or available. ";

	case DDERR_NOSTRETCHHW:
		return "The operation cannot be carried out because there is no hardware support for stretching. ";

	case DDERR_NOT4BITCOLOR:
		return "The DirectDrawSurface object is not using a 4-bit color palette and the requested operation requires a 4-bit color palette. ";

	case DDERR_NOT4BITCOLORINDEX:
		return "The DirectDrawSurface object is not using a 4-bit color index palette and the requested operation requires a 4-bit color index palette. ";

	case DDERR_NOT8BITCOLOR:
		return "The DirectDrawSurface object is not using an 8-bit color palette and the requested operation requires an 8-bit color palette. ";

	case DDERR_NOTAOVERLAYSURFACE:
		return "An overlay component is called for a non-overlay surface. ";

	case DDERR_NOTEXTUREHW:
		return "The operation cannot be carried out because no texture-mapping hardware is present or available. ";

	case DDERR_NOTFLIPPABLE:
		return "An attempt has been made to flip a surface that cannot be flipped. ";

	case DDERR_NOTFOUND:
		return "The requested item was not found. ";

	case DDERR_NOTINITIALIZED:
		return "An attempt was made to call an interface method of a DirectDraw object created by CoCreateInstance before the object was initialized. ";

	case DDERR_NOTLOADED:
		return "The surface is an optimized surface, but it has not yet been allocated any memory. ";

	case DDERR_NOTLOCKED:
		return "An attempt is made to unlock a surface that was not locked. ";

	case DDERR_NOTPAGELOCKED:
		return "An attempt is made to page unlock a surface with no outstanding page locks. ";

	case DDERR_NOTPALETTIZED:
		return "The surface being used is not a palette-based surface. ";

	case DDERR_NOVSYNCHW:
		return "The operation cannot be carried out because there is no hardware support for vertical blank synchronized operations. ";

	case DDERR_NOZBUFFERHW:
		return "The operation to create a z-buffer in display memory or to perform a blit using a z-buffer cannot be carried out because there is no hardware support for z-buffers. ";

	case DDERR_NOZOVERLAYHW:
		return "The overlay surfaces cannot be z-layered based on the z-order because the hardware does not support z-ordering of overlays. ";

	case DDERR_OUTOFCAPS:
		return "The hardware needed for the requested operation has already been allocated. ";

	case DDERR_OUTOFMEMORY:
		return "DirectDraw does not have enough memory to perform the operation. ";

	case DDERR_OUTOFVIDEOMEMORY:
		return "DirectDraw does not have enough display memory to perform the operation. ";

	case DDERR_OVERLAPPINGRECTS:
		return "Operation could not be carried out because the source and destination rectangles are on the same surface and overlap each other. ";

	case DDERR_OVERLAYCANTCLIP:
		return "The hardware does not support clipped overlays. ";

	case DDERR_OVERLAYCOLORKEYONLYONEACTIVE:
		return "An attempt was made to have more than one color key active on an overlay. ";

	case DDERR_OVERLAYNOTVISIBLE:
		return "The IDirectDrawSurface4::GetOverlayPosition method is called on a hidden overlay. ";

	case DDERR_PALETTEBUSY:
		return "Access to this palette is refused because the palette is locked by another thread. ";

	case DDERR_PRIMARYSURFACEALREADYEXISTS:
		return "This process has already created a primary surface. ";

	case DDERR_REGIONTOOSMALL:
		return "The region passed to the IDirectDrawClipper::GetClipList method is too small. ";

	case DDERR_SURFACEALREADYATTACHED:
		return "An attempt was made to attach a surface to another surface to which it is already attached. ";

	case DDERR_SURFACEALREADYDEPENDENT:
		return "An attempt was made to make a surface a dependency of another surface to which it is already dependent. ";

	case DDERR_SURFACEBUSY:
		return "Access to the surface is refused because the surface is locked by another thread. ";

	case DDERR_SURFACEISOBSCURED:
		return "Access to the surface is refused because the surface is obscured. ";

	case DDERR_SURFACELOST:
		return "Access to the surface is refused because the surface memory is gone. Call the IDirectDrawSurface4::Restore method on this surface to restore the memory associated with it. ";

	case DDERR_SURFACENOTATTACHED:
		return "The requested surface is not attached. ";

	case DDERR_TOOBIGHEIGHT:
		return "The height requested by DirectDraw is too large. ";

	case DDERR_TOOBIGSIZE:
		return "The size requested by DirectDraw is too large. However, the individual height and width are valid sizes. ";

	case DDERR_TOOBIGWIDTH:
		return "The width requested by DirectDraw is too large. ";

	case DDERR_UNSUPPORTED:
		return "The operation is not supported. ";

	case DDERR_UNSUPPORTEDFORMAT:
		return "The FourCC format requested is not supported by DirectDraw. ";

	case DDERR_UNSUPPORTEDMASK:
		return "The bitmask in the pixel format requested is not supported by DirectDraw. ";

	case DDERR_UNSUPPORTEDMODE:
		return "The display is currently in an unsupported mode. ";

	case DDERR_VERTICALBLANKINPROGRESS:
		return "A vertical blank is in progress. ";

	case DDERR_VIDEONOTACTIVE:
		return "The video port is not active. ";

	case DDERR_WASSTILLDRAWING:
		return "The previous blit operation that is transferring information to or from this surface is incomplete. ";

	case DDERR_WRONGMODE:
		return "This surface cannot be restored because it was created in a different mode. ";

	case DDERR_XALIGN:
		return "The provided rectangle was not horizontally aligned on a required boundary. ";

	case D3DERR_BADMAJORVERSION:
		return "Bad major version";

	case D3DERR_BADMINORVERSION:
		return "Bad minor version";

	case D3DERR_COLORKEYATTACHED:
		return "Colorkey attached";

	case D3DERR_CONFLICTINGTEXTUREFILTER:
		return "Conflicting texture filter";

	case D3DERR_CONFLICTINGTEXTUREPALETTE:
		return "Conflicting texture palette";

	case D3DERR_CONFLICTINGRENDERSTATE:
		return "Conflicting render state";

	case D3DERR_DEVICEAGGREGATED:
		return "Device aggregated";

	case D3DERR_EXECUTE_CLIPPED_FAILED:
		return "Execute clipped failed";

	case D3DERR_EXECUTE_CREATE_FAILED:
		return "Execute create failed";

	case D3DERR_EXECUTE_DESTROY_FAILED:
		return "Execute destroy failed";

	case D3DERR_EXECUTE_FAILED:
		return "Execute failed";

	case D3DERR_EXECUTE_LOCK_FAILED:
		return "Execute lock failed";

	case D3DERR_EXECUTE_LOCKED:
		return "Execute locked";

	case D3DERR_EXECUTE_NOT_LOCKED:
		return "Execute not locked";

	case D3DERR_EXECUTE_UNLOCK_FAILED:
		return "Execute unlock failed";

	case D3DERR_INITFAILED:
		return "Init failed";

	case D3DERR_INBEGIN:
		return "Already in begin scene";

	case D3DERR_INVALID_DEVICE:
		return "Invalid device";

	case D3DERR_INVALIDCURRENTVIEWPORT:
		return "Invalid current viewport";

	case D3DERR_INVALIDMATRIX:
		return "Invalid matrix";

	case D3DERR_INVALIDPALETTE:
		return "Invalid palette";

	case D3DERR_INVALIDPRIMITIVETYPE:
		return "Invalid primitive type";

	case D3DERR_INVALIDRAMPTEXTURE:
		return "Invalid ramp texture";

	case D3DERR_INVALIDVERTEXFORMAT:
		return "Invalid vertex format";

	case D3DERR_INVALIDVERTEXTYPE:
		return "Invalid vertex type";

	case D3DERR_LIGHT_SET_FAILED:
		return "Light set failed";

	case D3DERR_LIGHTHASVIEWPORT:
		return "Light has viewport";

	case D3DERR_LIGHTNOTINTHISVIEWPORT:
		return "Light not in this viewport";

	case D3DERR_MATERIAL_CREATE_FAILED:
		return "Material create failed";

	case D3DERR_MATERIAL_DESTROY_FAILED:
		return "Material destroy failed";

	case D3DERR_MATERIAL_GETDATA_FAILED:
		return "Material get data failed";

	case D3DERR_MATERIAL_SETDATA_FAILED:
		return "Material set data failed";

	case D3DERR_MATRIX_CREATE_FAILED:
		return "Matrix create failed";

	case D3DERR_MATRIX_DESTROY_FAILED:
		return "Matrix destroy failed";

	case D3DERR_MATRIX_GETDATA_FAILED:
		return "Matrix get data failed";

	case D3DERR_MATRIX_SETDATA_FAILED:
		return "Matrix set data failed";

	case D3DERR_NOCURRENTVIEWPORT:
		return "No current viewport";

	case D3DERR_NOTINBEGIN:
		return "Not in begin scene";

	case D3DERR_NOVIEWPORTS:
		return "No viewports";

	case D3DERR_SCENE_BEGIN_FAILED:
		return "Begin scene failed";

	case D3DERR_SCENE_END_FAILED:
		return "End scene failed";

	case D3DERR_SCENE_IN_SCENE:
		return "Scene in scene";

	case D3DERR_SCENE_NOT_IN_SCENE:
		return "Scene not in scene";

	case D3DERR_SETVIEWPORTDATA_FAILED:
		return "Set viewport data failed";

	case D3DERR_STENCILBUFFER_NOTPRESENT:
		return "Stencil buffer not present";

	case D3DERR_SURFACENOTINVIDMEM:
		return "Surface not in video memory";

	case D3DERR_TEXTURE_BADSIZE:
		return "Bad texture size";

	case D3DERR_TEXTURE_CREATE_FAILED:
		return "Texture create failed";

	case D3DERR_TEXTURE_DESTROY_FAILED:
		return "Texture destroy failed";

	case D3DERR_TEXTURE_GETSURF_FAILED:
		return "Texture get surface failed";

	case D3DERR_TEXTURE_LOAD_FAILED:
		return "Texture load failed";

	case D3DERR_TEXTURE_LOCK_FAILED:
		return "Texture lock failed";

	case D3DERR_TEXTURE_LOCKED:
		return "Texture lock failed";

	case D3DERR_TEXTURE_NO_SUPPORT:
		return "Texture no support";

	case D3DERR_TEXTURE_NOT_LOCKED:
		return "Texture not locked";

	case D3DERR_TEXTURE_SWAP_FAILED:
		return "Texture swap failed";

	case D3DERR_TEXTURE_UNLOCK_FAILED:
		return "Texture unlock failed";

	case D3DERR_TOOMANYOPERATIONS:
		return "Too many operations";

	case D3DERR_TOOMANYPRIMITIVES:
		return "Too many primitives";

	case D3DERR_UNSUPPORTEDALPHAARG:
		return "Unsupported alpha argument";

	case D3DERR_UNSUPPORTEDALPHAOPERATION:
		return "Unsupported alpha operation";

	case D3DERR_UNSUPPORTEDCOLORARG:
		return "Unsupported color argument";

	case D3DERR_UNSUPPORTEDCOLOROPERATION:
		return "Unsupported color opertation";

	case D3DERR_UNSUPPORTEDFACTORVALUE:
		return "Unsupported factor value";

	case D3DERR_UNSUPPORTEDTEXTUREFILTER:
		return "Unsupported texture filter";

	case D3DERR_VBUF_CREATE_FAILED:
		return "Vertex buffer create failed";

	case D3DERR_VERTEXBUFFERLOCKED:
		return "Vertex buffer locked";

	case D3DERR_VERTEXBUFFEROPTIMIZED:
		return "Vertex buffer optimised";

	case D3DERR_VIEWPORTDATANOTSET:
		return "Viewport data not set";

	case D3DERR_VIEWPORTHASNODEVICE:
		return "Viewport has no device";

	case D3DERR_WRONGTEXTUREFORMAT:
		return "Wrong texture format";

	case D3DERR_ZBUFF_NEEDS_SYSTEMMEMORY:
		return "ZBuffer needs system memory";

	case D3DERR_ZBUFF_NEEDS_VIDEOMEMORY:
		return "ZBuffer needs video memory";

	case D3DERR_ZBUFFER_NOTPRESENT:
		return "ZBuffer not present";

	case DSERR_ALLOCATED:
		return "The request failed because resources, such as a priority level, were already in use by another caller. ";

	case DSERR_ALREADYINITIALIZED:
		return "The object is already initialized. ";

	case DSERR_BADFORMAT:
		return "The specified wave format is not supported. ";

	case DSERR_BUFFERLOST:
		return "The buffer memory has been lost and must be restored. ";

	case DSERR_CONTROLUNAVAIL:
		return "The buffer control (volume, pan, and so on) requested by the caller is not available. ";

	case DSERR_INVALIDCALL:
		return "This function is not valid for the current state of this object. ";

	case DSERR_NOAGGREGATION:
		return "The object does not support aggregation. ";

	case DSERR_NODRIVER:
		return "No sound driver is available for use. ";

	case DSERR_NOINTERFACE:
		return "The requested COM interface is not available. ";

	case DSERR_OTHERAPPHASPRIO:
		return "Another application has a higher priority level, preventing this call from succeeding ";

	case DSERR_PRIOLEVELNEEDED:
		return "The caller does not have the priority level required for the function to succeed. ";

	case DSERR_UNINITIALIZED:
		return "The IDirectSound::Initialize method has not been called or has not been called successfully before other methods were called. ";
	}

	return "Undefined Error";
}
