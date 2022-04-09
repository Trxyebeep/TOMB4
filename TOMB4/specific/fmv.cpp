#include "../tomb4/pch.h"
#include "fmv.h"
#include "dxshell.h"

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

void inject_fmv(bool replace)
{
	INJECT(0x0049C350, ShowBinkFrame, replace);
}
