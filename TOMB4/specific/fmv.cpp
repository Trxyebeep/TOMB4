#include "../tomb4/pch.h"
#include "fmv.h"
#include "dxshell.h"
#include "audio.h"
#include "lighting.h"
#include "function_table.h"
#include "winmain.h"
#include "input.h"
#include "3dmath.h"
#include "../game/text.h"
#include "d3dmatrix.h"
#include "dxsound.h"
#include "../game/control.h"
#include "cmdline.h"
#include "gamemain.h"
#include "LoadSave.h"

static void (__stdcall* BinkCopyToBuffer)(BINK_STRUCT*, LPVOID, LONG, long, long, long, long);
static void(__stdcall* BinkOpenDirectSound)(ulong);
static void (__stdcall* BinkSetSoundSystem)(LPVOID, LPDIRECTSOUND);
static LPVOID (__stdcall* BinkOpen)(char*, ulong);
static long (__stdcall* BinkDDSurfaceType)(LPDIRECTDRAWSURFACEX);
static long (__stdcall* BinkDoFrame)(BINK_STRUCT*);
static void (__stdcall* BinkNextFrame)(BINK_STRUCT*);
static long (__stdcall* BinkWait)(BINK_STRUCT*);
static void (__stdcall* BinkClose)(BINK_STRUCT*);
static HMODULE hBinkW32;

static BINK_STRUCT* Bink;
static LPDIRECTDRAWSURFACEX BinkSurface;
static long BinkSurfaceType;

#define GET_DLL_PROC(dll, proc, n) \
{ \
	*(FARPROC *)&(proc) = GetProcAddress((dll), n); \
	if(!proc) throw #proc; \
}

bool LoadBinkStuff()
{
	hBinkW32 = LoadLibrary("binkw32.dll");

	if (!hBinkW32)
		return 0;

	try
	{	
		GET_DLL_PROC(hBinkW32, BinkCopyToBuffer, "_BinkCopyToBuffer@28");
		GET_DLL_PROC(hBinkW32, BinkOpenDirectSound, "_BinkOpenDirectSound@4");
		GET_DLL_PROC(hBinkW32, BinkSetSoundSystem, "_BinkSetSoundSystem@8");
		GET_DLL_PROC(hBinkW32, BinkOpen, "_BinkOpen@8");
		GET_DLL_PROC(hBinkW32, BinkDDSurfaceType, "_BinkDDSurfaceType@4");
		GET_DLL_PROC(hBinkW32, BinkDoFrame, "_BinkDoFrame@4");
		GET_DLL_PROC(hBinkW32, BinkNextFrame, "_BinkNextFrame@4");
		GET_DLL_PROC(hBinkW32, BinkWait, "_BinkWait@4");
		GET_DLL_PROC(hBinkW32, BinkClose, "_BinkClose@4");
	}
	catch (LPCTSTR)
	{
		FreeLibrary(hBinkW32);
		hBinkW32 = 0;
		return 0;
	}

	return 1;
}

void FreeBinkStuff()
{
	if (hBinkW32)
	{
		FreeLibrary(hBinkW32);
		hBinkW32 = 0;
	}
}

void ShowBinkFrame()
{
	DDSURFACEDESCX surf;

	memset(&surf, 0, sizeof(surf));
	surf.dwSize = sizeof(DDSURFACEDESCX);
	DXAttempt(BinkSurface->Lock(0, &surf, DDLOCK_NOSYSLOCK, 0));
	BinkCopyToBuffer(Bink, surf.lpSurface, surf.lPitch, Bink->num, 0, 0, BinkSurfaceType);
	DXAttempt(BinkSurface->Unlock(0));

	if (App.dx.Flags & DXF_WINDOWED)
		DXShowFrame();
}

long PlayFmvNow(long num)
{
	DXDISPLAYMODE* modes;
	DXDISPLAYMODE* current;
	long dm, rm, ndms;
	char name[80];
	char path[80];

	if (MainThread.ended)
		return 0;

	if ((1 << num) & FmvSceneTriggered)
		return 1;

	FmvSceneTriggered |= 1 << num;
	S_CDStop();

	if (fmvs_disabled)
		return 0;

	sprintf(name, "fmv\\fmv%02d.bik", num);
	memset(path, 0, sizeof(path));
	strcat(path, name);
	App.fmv = 1;
	modes = G_dxinfo->DDInfo[App.DXInfo.nDD].D3DDevices[App.DXInfo.nD3D].DisplayModes;
	rm = 0;
	dm = App.DXInfo.nDisplayMode;
	current = &modes[dm];

	if (current->bpp != 32 || current->w != 640 || current->h != 480)
	{
		ndms = G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].nDisplayModes;

		for (int i = 0; i < ndms; i++, modes++)
		{
			if (modes->bpp == 32 && modes->w == 640 && modes->h == 480)
			{
				App.DXInfo.nDisplayMode = i;
				break;
			}
		}

		DXChangeVideoMode();
		HWInitialise();
		ClearSurfaces();
		rm = 1;
	}
	
	Bink = 0;
	BinkSetSoundSystem(BinkOpenDirectSound, App.dx.lpDS);
	Bink = (BINK_STRUCT*)BinkOpen(path, 0);

	if (App.dx.Flags & DXF_WINDOWED)
		BinkSurface = App.dx.lpBackBuffer;
	else
		BinkSurface = App.dx.lpPrimaryBuffer;

	BinkSurfaceType = BinkDDSurfaceType(BinkSurface);

	if (Bink)
	{
		BinkDoFrame(Bink);
		S_UpdateInput();

		for (int i = 0; i != Bink->num2; i++)
		{
			if (input & IN_OPTION || MainThread.ended)
				break;

			BinkNextFrame(Bink);

			while (BinkWait(Bink));

			ShowBinkFrame();
			BinkDoFrame(Bink);
			S_UpdateInput();
		}


		BinkClose(Bink);
		Bink = 0;
	}

	if (rm)
	{
		App.DXInfo.nDisplayMode = dm;
		DXChangeVideoMode();
		InitWindow(0, 0, App.dx.dwRenderWidth, App.dx.dwRenderHeight, 20, 20480, 80, App.dx.dwRenderWidth, App.dx.dwRenderHeight);
		InitFont();
		S_InitD3DMatrix();
		SetD3DViewMatrix();
	}

	DXChangeOutputFormat(sfx_frequencies[SoundQuality], 1);
	HWInitialise();
	ClearSurfaces();
	App.fmv = 0;
	return 0;
}
