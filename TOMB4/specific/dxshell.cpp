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
}
