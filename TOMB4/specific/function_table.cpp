#include "../tomb4/pch.h"
#include "function_table.h"
#include "function_stubs.h"
#include "dxshell.h"

void SetFogColor(long r, long g, long b)
{
	r &= 0xFF;
	g &= 0xFF;
	b &= 0xFF;
	CurrentFog = RGBA(r, g, b, 0xFF);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_FOGCOLOR, CurrentFog);
}

void HWInitialise()
{
	Log(2, "HWIntialise");	//nice typo
	App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
	App.dx.lpD3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	App.dx.lpD3DDevice->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);
	App.dx.lpD3DDevice->SetTextureStageState(3, D3DTSS_COLOROP, D3DTOP_DISABLE);
	App.dx.lpD3DDevice->SetTextureStageState(4, D3DTSS_COLOROP, D3DTOP_DISABLE);
	App.dx.lpD3DDevice->SetTextureStageState(5, D3DTSS_COLOROP, D3DTOP_DISABLE);
	App.dx.lpD3DDevice->SetTextureStageState(6, D3DTSS_COLOROP, D3DTOP_DISABLE);
	App.dx.lpD3DDevice->SetTextureStageState(7, D3DTSS_COLOROP, D3DTOP_DISABLE);
	App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);

	if (App.Filtering)
	{
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_LINEAR);
	}
	else
	{
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_POINT);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_POINT);
	}

	App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);

	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, 1);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);

	DXAttempt(App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE));
	DXAttempt(App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 1));
	DXAttempt(App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, 1));

	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DITHERENABLE, 1);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHAREF, 0);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_NOTEQUAL);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 0);

	DXAttempt(App.dx.lpD3DDevice->SetLightState(D3DLIGHTSTATE_AMBIENT, 0));
	DXAttempt(App.dx.lpD3DDevice->SetLightState(D3DLIGHTSTATE_COLORVERTEX, 0));
	DXAttempt(App.dx.lpD3DDevice->SetLightState(D3DLIGHTSTATE_COLORMODEL, D3DCOLOR_RGB));

	DXAttempt(App.dx.lpD3DDevice->SetLightState(D3DLIGHTSTATE_FOGMODE, D3DFOG_LINEAR));
	DXAttempt(App.dx.lpD3DDevice->SetLightState(D3DLIGHTSTATE_FOGSTART, *(DWORD*)(&FogStart)));
	DXAttempt(App.dx.lpD3DDevice->SetLightState(D3DLIGHTSTATE_FOGEND, *(DWORD*)(&FogEnd)));
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_FOGCOLOR, 0xFF000000);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, 1);
}

void inject_functbl(bool replace)
{
	INJECT(0x00476E80, SetFogColor, replace);
	INJECT(0x00476B30, HWInitialise, replace);
}
