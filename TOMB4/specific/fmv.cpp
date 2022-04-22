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

void ShowBinkFrame()
{
	DDSURFACEDESC2 surf;

	memset(&surf, 0, sizeof(surf));
	surf.dwSize = sizeof(DDSURFACEDESC2);
	DXAttempt(BinkSurface->Lock(0, &surf, DDLOCK_NOSYSLOCK, 0));
	BinkCopyToBuffer(Bink, surf.lpSurface, surf.lPitch, Bink->num, 0, 0, BinkSurfaceType);
	DXAttempt(BinkSurface->Unlock(0));

	if (App.dx.Flags & 2)
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

#ifndef NO_CD
	memset(path, 0, sizeof(path));
	path[0] = cd_drive;
	path[1] = ':';
	path[2] = '\\';
#endif
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

		FreeD3DLights();
		DXChangeVideoMode();
		HWInitialise();
		ClearSurfaces();
		rm = 1;
	}
	
	Bink = 0;
	BinkSetSoundSystem(BinkOpenDirectSound, App.dx.lpDS);
	Bink = (BINK_STRUCT*)BinkOpen(path, 0);

	if (App.dx.Flags & 2)
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
		CreateD3DLights();
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

void inject_fmv(bool replace)
{
	INJECT(0x0049C350, ShowBinkFrame, replace);
	INJECT(0x0049C3E0, PlayFmvNow, replace);
}
