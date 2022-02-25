#include "../tomb4/pch.h"
#include "function_table.h"

void SetFogColor(long r, long g, long b)
{
	r &= 0xFF;
	g &= 0xFF;
	b &= 0xFF;
	CurrentFog = RGBA(r, g, b, 0xFF);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_FOGCOLOR, CurrentFog);
}

void inject_functbl(bool replace)
{
	INJECT(0x00476E80, SetFogColor, replace);
}
