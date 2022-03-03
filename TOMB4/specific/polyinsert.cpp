#include "../tomb4/pch.h"
#include "polyinsert.h"
#include "dxshell.h"
#include "drawroom.h"

#define FVF (D3DFVF_TEX2 | D3DFVF_SPECULAR | D3DFVF_DIFFUSE | D3DFVF_XYZRHW)

void HWR_DrawSortList(D3DTLBUMPVERTEX* info, short num_verts, short texture, short type)
{
	switch (type)
	{
	case 0:

		if (App.dx.lpZBuffer)
			App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 1);

		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 0);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
		DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[texture].tex));
		App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, info, num_verts, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 1);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 1);
		break;

	case 1:
#ifdef GENERAL_FIXES
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, 0);
#endif
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 1);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 1);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
		DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[texture].tex));
		App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, info, num_verts, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
#ifdef GENERAL_FIXES
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, 1);
#endif
		break;

	case 2:

		if (App.dx.lpZBuffer)
			App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 0);

#ifdef GENERAL_FIXES
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, 0);
#endif
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 0);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, 0);
		DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[texture].tex));
		App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, info, num_verts, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, 1);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
#ifdef GENERAL_FIXES
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, 1);
#endif
		break;

	case 3:

		if (App.dx.lpZBuffer)
			App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 0);

		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 1);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[texture].tex));
		App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, info, num_verts, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		break;

	case 4:
		DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[texture].tex));

		if (App.dx.lpZBuffer)
		{
			App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, 0);
			App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 0);
		}

		App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, info, num_verts, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);

		if (App.dx.lpZBuffer)
		{
			App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 1);
			App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, 1);
		}

		break;

#ifdef GENERAL_FIXES

	case 5:

		if (App.dx.lpZBuffer)
			App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 0);

		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 1);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ZERO);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCCOLOR);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE4X);
		DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[texture].tex));
		App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, info, num_verts, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		break;

#endif

	case 6:

		if (App.dx.lpZBuffer)
			App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 0);

		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 0);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
		DXAttempt(App.dx.lpD3DDevice->SetTexture(0, 0));
		App.dx.lpD3DDevice->DrawPrimitive(D3DPT_LINELIST, FVF, info, num_verts, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
		break;

	case 7:

		if (App.dx.lpZBuffer)
			App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 1);

		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 1);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[texture].tex));
		App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, info, num_verts, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		break;
	}

	DrawPrimitiveCnt++;
}

void DrawSortList()
{
	SORTLIST* pSort;
	D3DTLBUMPVERTEX* vtx;
	D3DTLBUMPVERTEX* bVtx;
	D3DTLBUMPVERTEX* bVtxbak;
	long num;
	short nVtx, tpage, drawtype, total_nVtx;

	nVtx = 0;

	if (!SortCount)
		return;

	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 1);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 1);

	if (!App.dx.lpZBuffer)
	{
		for (int i = 0; i < SortCount; i++)
		{
			pSort = SortList[i];
			vtx = (D3DTLBUMPVERTEX*)(pSort + 1);

			if (pSort->polytype == 4)
				App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, 0);

			HWR_DrawSortList(vtx, pSort->nVtx, pSort->tpage, pSort->drawtype);

			if (pSort->polytype == 4)
				App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, 1);

		}
	}
	else
	{
#ifdef GENERAL_FIXES				//just to shut VS up
		pSort = SortList[0];		//if SortCount is < 0 then pSort will be uninitialized in the original, but I don't think that ever happens
#endif

		for (num = 0; num < SortCount; num++)
		{
			pSort = SortList[num];

			if (pSort->drawtype == 0 || pSort->drawtype == 1 || pSort->drawtype == 4)
				break;
		}

		bVtxbak = Bucket[0].Vertex;
		bVtx = bVtxbak;
		tpage = pSort->tpage;
		drawtype = pSort->drawtype;

		for (; num < SortCount; num++)
		{
			pSort = SortList[num];

			if (pSort->drawtype == 0 || pSort->drawtype == 1 || pSort->drawtype == 4)
			{
				if (pSort->drawtype == drawtype && pSort->tpage == tpage)
				{
					vtx = (D3DTLBUMPVERTEX*)(pSort + 1);

					for (int i = 0; i < pSort->nVtx; i++, vtx++, bVtx++)
					{
						bVtx->sx = vtx->sx;
						bVtx->sy = vtx->sy;
						bVtx->sz = vtx->sz;
						bVtx->rhw = vtx->rhw;
						bVtx->color = vtx->color;
						bVtx->specular = vtx->specular;
						bVtx->tu = vtx->tu;
						bVtx->tv = vtx->tv;
						nVtx++;
					}
				}
				else
				{
					HWR_DrawSortList(bVtxbak, nVtx, tpage, drawtype);	//inlined in asm
					drawtype = pSort->drawtype;
					tpage = pSort->tpage;
					bVtx = bVtxbak;
					nVtx = 0;
					num--;
				}
			}
		}

		if (nVtx)
			HWR_DrawSortList(bVtxbak, nVtx, tpage, drawtype);

		for (num = SortCount - 1; num >= 0; num--)
		{
			pSort = SortList[num];

#ifdef GENERAL_FIXES
			if (pSort->drawtype == 2 || pSort->drawtype == 3 || pSort->drawtype == 5 || pSort->drawtype == 6 || pSort->drawtype == 7)
#else
			if (pSort->drawtype == 2 || pSort->drawtype == 3 || pSort->drawtype == 6 || pSort->drawtype == 7)
#endif
				break;
		}

		tpage = pSort->tpage;
		drawtype = pSort->drawtype;
		bVtx = bVtxbak;
		nVtx = 0;
		total_nVtx = 0;

		for (; num >= 0; num--)
		{
			pSort = SortList[num];

#ifdef GENERAL_FIXES
			if (pSort->drawtype == 2 || pSort->drawtype == 3 || pSort->drawtype == 5 || pSort->drawtype == 6 || pSort->drawtype == 7)
#else
			if (pSort->drawtype == 2 || pSort->drawtype == 3 || pSort->drawtype == 6 || pSort->drawtype == 7)
#endif
			{
				if (pSort->tpage == tpage && pSort->drawtype == drawtype)
				{
					vtx = (D3DTLBUMPVERTEX*)(pSort + 1);

					for (int i = 0; i < pSort->nVtx; i++, vtx++, bVtx++)
					{
						bVtx->sx = vtx->sx;
						bVtx->sy = vtx->sy;
						bVtx->sz = vtx->sz;
						bVtx->rhw = vtx->rhw;
						bVtx->color = vtx->color;
						bVtx->specular = vtx->specular;
						bVtx->tu = vtx->tu;
						bVtx->tv = vtx->tv;
						nVtx++;
					}
				}
				else
				{
					HWR_DrawSortList(bVtxbak, nVtx, tpage, drawtype);
					tpage = pSort->tpage;
					nVtx = 0;
					drawtype = pSort->drawtype;
					bVtx = bVtxbak;
					num++;
				}
			}
		}

		if (nVtx)
			HWR_DrawSortList(bVtxbak, nVtx, tpage, drawtype);
	}

	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 1);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 0);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);
	InitBuckets();
}

void inject_polyinsert(bool replace)
{
	INJECT(0x004812D0, HWR_DrawSortList, replace);
	INJECT(0x00480A50, DrawSortList, replace);
}
