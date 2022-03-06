#include "../tomb4/pch.h"
#include "polyinsert.h"
#include "dxshell.h"
#include "drawroom.h"
#include "output.h"
#include "d3dmatrix.h"
#include "function_stubs.h"
#include "../game/tomb4fx.h"

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

void CreateFogPos(FOGBULB_STRUCT* FogBulb)
{
	FVECTOR d;
	long* mx;
	float dist;
	short bounds[6];

	if (GlobalFogOff)
		FogBulb->inRange = 0;
	else
	{
		d.x = FogBulb->WorldPos.x - camera.pos.x;
		d.y = FogBulb->WorldPos.y - camera.pos.y;
		d.z = FogBulb->WorldPos.z - camera.pos.z;
		dist = SQUARE(d.x) + SQUARE(d.y) + SQUARE(d.z);

		if (dist > 0x19000000)
			FogBulb->inRange = 0;
		else
		{
			FogBulb->inRange = 1;
			bounds[0] = short(FogBulb->WorldPos.x - camera.pos.x + FogBulb->rad);
			bounds[1] = short(FogBulb->WorldPos.x - camera.pos.x - FogBulb->rad);
			bounds[2] = short(FogBulb->WorldPos.y - camera.pos.y + FogBulb->rad);
			bounds[3] = short(FogBulb->WorldPos.y - camera.pos.y - FogBulb->rad);
			bounds[4] = short(FogBulb->WorldPos.z - camera.pos.z + FogBulb->rad);
			bounds[5] = short(FogBulb->WorldPos.z - camera.pos.z - FogBulb->rad);
			mx = phd_mxptr;
			phd_mxptr = w2v_matrix;

			if (S_GetObjectBounds(bounds))
				NumFogBulbsInRange++;
			else
				FogBulb->inRange = 0;

			phd_mxptr = mx;

			if (FogBulb->inRange)
			{
				FogBulb->vec.x = FogBulb->WorldPos.x - camera.pos.x;
				FogBulb->vec.y = FogBulb->WorldPos.y - camera.pos.y;
				FogBulb->vec.z = FogBulb->WorldPos.z - camera.pos.z;
				SetD3DMatrix(&D3DMView, w2v_matrix);
				mD3DTransform(&FogBulb->vec, &D3DMView);
				FogBulb->pos.x = FogBulb->vec.x;
				FogBulb->pos.y = FogBulb->vec.y;
				FogBulb->pos.z = FogBulb->vec.z;
				D3DNormalise((D3DVECTOR*)&FogBulb->vec);
				FogBulb->vec.x = FogBulb->rad * FogBulb->vec.x + FogBulb->pos.x;
				FogBulb->vec.y = FogBulb->rad * FogBulb->vec.y + FogBulb->pos.y;
				FogBulb->vec.z = FogBulb->rad * FogBulb->vec.z + FogBulb->pos.z;
				FogBulb->dist = sqrt(SQUARE(FogBulb->pos.x) + SQUARE(FogBulb->pos.y) + SQUARE(FogBulb->pos.z));
			}
		}
	}
}

void ControlFXBulb(FOGBULB_STRUCT* FogBulb)
{
	if (FogBulb->timer > 0)
	{
		FogBulb->timer--;

		FogBulb->rad += (GetRandomDraw() - 0x100) & 0x1FF;

		if (FogBulb->rad > FogBulb->FXRad)
			FogBulb->rad = float(FogBulb->FXRad + (GetRandomDraw() & 0xFF));
	}
	else
	{
		FogBulb->timer--;

		if (FogBulb->timer < -30)
			FogBulb->timer = 0;

		FogBulb->rad -= 255;

		if (FogBulb->rad < 0)
		{
			FogBulb->rad = 0;
			FogBulb->active = 0;
		}
	}

	TriggerFlashSmoke((long)FogBulb->WorldPos.x, (long)FogBulb->WorldPos.y, (long)FogBulb->WorldPos.z, FogBulb->room_number);
	TriggerFlashSmoke((long)FogBulb->WorldPos.x, (long)FogBulb->WorldPos.y, (long)FogBulb->WorldPos.z, FogBulb->room_number);
	FogBulb->sqrad = SQUARE(FogBulb->rad);
	FogBulb->inv_sqrad = 1.0F / FogBulb->sqrad;
}

void CreateFXBulbs()
{
	FOGBULB_STRUCT* FogBulb;

	NumFXFogBulbs = 0;

	for (int i = 0; i < 5; i++)
	{
		FogBulb = &FXFogBulbs[i];

		if (FogBulb->active)
		{
			CreateFogPos(FogBulb);
			ControlFXBulb(FogBulb);
			NumFXFogBulbs++;
		}
	}
}

void ClearFXFogBulbs()
{
	for (int i = 0; i < 5; i++)
		FXFogBulbs[i].active = 0;

	NumFXFogBulbs = 0;
}

void TriggerFXFogBulb(long x, long y, long z, long FXRad, long density, long r, long g, long b, long room_number)
{
	FOGBULB_STRUCT* FogBulb;
	long num;

	num = 0;

	while (FXFogBulbs[num].active)
	{
		num++;

		if (num > 4)
			return;
	}

	FogBulb = &FXFogBulbs[num];
	FogBulb->inRange = 1;
	FogBulb->density = density;
	FogBulb->WorldPos.x = (float)x;
	FogBulb->WorldPos.y = (float)y;
	FogBulb->WorldPos.z = (float)z;
	FogBulb->rad = 0;
	FogBulb->sqrad = 0;
	FogBulb->inv_sqrad = 1 / FogBulb->sqrad;	//how does this not crash
	FogBulb->timer = 50;
	FogBulb->active = 1;
	FogBulb->FXRad = FXRad;
	FogBulb->r = r;
	FogBulb->g = g;
	FogBulb->b = b;
	FogBulb->room_number = room_number;
	CreateFogPos(FogBulb);
	NumFXFogBulbs++;
}

long IsVolumetric()
{
	return App.Volumetric;
}

int DistCompare(const void* a, const void* b)
{
	FOGBULB_STRUCT* bulbA;
	FOGBULB_STRUCT* bulbB;
	FVECTOR dA;
	FVECTOR dB;

	bulbA = (FOGBULB_STRUCT*)a;
	bulbB = (FOGBULB_STRUCT*)b;
	dA.x = bulbA->WorldPos.x - camera.pos.x;
	dA.y = bulbA->WorldPos.y - camera.pos.y;
	dA.z = bulbA->WorldPos.z - camera.pos.z;
	dB.x = bulbB->WorldPos.x - camera.pos.x;
	dB.y = bulbB->WorldPos.y - camera.pos.y;
	dB.z = bulbB->WorldPos.z - camera.pos.z;
	bulbA->dist = SQUARE(dA.x) + SQUARE(dA.y) + SQUARE(dA.z);
	bulbB->dist = SQUARE(dB.x) + SQUARE(dB.y) + SQUARE(dB.z);

	if (bulbA->dist > bulbB->dist)
		return 1;

	if (bulbA->dist < bulbB->dist)
		return -1;

	return 0;
}

void InitialiseFogBulbs()
{
	FOGBULB_STRUCT* FogBulb;

	NumActiveFogBulbs = 0;
	qsort(&FogBulbs, NumLevelFogBulbs, sizeof(FOGBULB_STRUCT), DistCompare);

	for (int i = 0; i < NumLevelFogBulbs; i++)
	{
		FogBulb = &FogBulbs[i];
		CreateFogPos(FogBulb);

		if (FogBulb->inRange)
		{
			ActiveFogBulbs[NumActiveFogBulbs] = FogBulb;
			NumActiveFogBulbs++;

			if (NumActiveFogBulbs >= 5)
				return;
		}
	}
}

void OmniEffect(D3DTLVERTEX* v)
{
	FOGBULB_STRUCT* FogBulb;
	FVECTOR pos;
	FVECTOR dP;
	FVECTOR dV;
	float val, val2;
	long r, g, b, lVal;

	for (int i = 0; i < 5; i++)
	{
		FogBulb = &FXFogBulbs[i];

		if (FogBulb->active && FogBulb->inRange)
		{
			pos.x = v->tu;
			pos.y = v->tv;
			pos.z = v->sz;

			if (FogBulb->pos.z < pos.z)
			{
				pos.x *= FogBulb->dist * (1 / v->sz);
				pos.y *= FogBulb->dist * (1 / v->sz);
				pos.z *= FogBulb->dist * (1 / v->sz);
			}

			dP.x = pos.x - FogBulb->pos.x;
			dP.y = pos.y - FogBulb->pos.y;
			dP.z = pos.z - FogBulb->pos.z;
			dV.x = FogBulb->vec.x - FogBulb->pos.x;
			dV.y = FogBulb->vec.y - FogBulb->pos.y;
			dV.z = FogBulb->vec.z - FogBulb->pos.z;
			val = SQUARE(dV.x) + SQUARE(dV.y) + SQUARE(dV.z);

			if (val)
			{
				val2 = (dP.x * dV.x + dP.y * dV.y + dP.z * dV.z) / val;

				if (val2 >= -1)
				{
					if (val2 > 0)
					{
						dP.x -= val2 * dV.x;
						dP.y -= val2 * dV.y;
						dP.z -= val2 * dV.z;
					}

					val = SQUARE(dP.x) + SQUARE(dP.y) + SQUARE(dP.z);

					if (val && val < FogBulb->sqrad)
					{
						val *= FogBulb->inv_sqrad * FogBulb->density;
						lVal = (long)val;
						r = CLRR(v->specular) + (((FogBulb->density - lVal) * FogBulb->r) >> 8);
						g = CLRG(v->specular) + (((FogBulb->density - lVal) * FogBulb->g) >> 8);
						b = CLRB(v->specular) + (((FogBulb->density - lVal) * FogBulb->b) >> 8);

						if (r > 255)
							r = 255;

						if (g > 255)
							g = 255;

						if (b > 255)
							b = 255;

						v->specular = b | v->specular & 0xFF000000 | ((g | ((r | (r << 8)) << 8)) << 8);
					}
				}
			}
		}
	}
}

void OmniFog(D3DTLVERTEX* v)
{
	FOGBULB_STRUCT* FogBulb;
	FVECTOR pos;
	FVECTOR dP;
	FVECTOR dV;
	float val, val2;
	long s, r, g, b, lVal;

	if (InventoryActive || nPolyType == 6 || gfLevelFlags & GF_TRAIN)
		return;

	s = (v->specular & 0xFF000000) >> 24;

	if (gfCurrentLevel != 5 && gfCurrentLevel != 6 && s != 255)
	{
		v->specular |= 0xFF000000;
		r = (CLRR(v->color) * s) >> 8;
		g = (CLRG(v->color) * s) >> 8;
		b = (CLRB(v->color) * s) >> 8;
		v->color = RGBA(r, g, b, 0xFF);

		if (!(v->color & 0xFFFFFF))
			return;
	}

	OmniEffect(v);

	if (NumFogBulbsInRange && NumActiveFogBulbs)
	{
		for (int i = 0; i < NumActiveFogBulbs; i++)
		{
			FogBulb = ActiveFogBulbs[i];

			if (FogBulb->inRange)
			{
				pos.x = v->tu;
				pos.y = v->tv;
				pos.z = v->sz;

				if (FogBulb->pos.z < pos.z)
				{
					pos.x *= FogBulb->dist * (1 / v->sz);
					pos.y *= FogBulb->dist * (1 / v->sz);
					pos.z *= FogBulb->dist * (1 / v->sz);
				}

				dP.x = pos.x - FogBulb->pos.x;
				dP.y = pos.y - FogBulb->pos.y;
				dP.z = pos.z - FogBulb->pos.z;
				dV.x = FogBulb->vec.x - FogBulb->pos.x;
				dV.y = FogBulb->vec.y - FogBulb->pos.y;
				dV.z = FogBulb->vec.z - FogBulb->pos.z;
				val = SQUARE(dV.x) + SQUARE(dV.y) + SQUARE(dV.z);

				if (val)
				{
					val2 = (dP.x * dV.x + dP.y * dV.y + dP.z * dV.z) / val;

					if (val2 >= -1)
					{
						if (val2 > 0)
						{
							dP.x -= val2 * dV.x;
							dP.y -= val2 * dV.y;
							dP.z -= val2 * dV.z;
						}

						val = SQUARE(dP.x) + SQUARE(dP.y) + SQUARE(dP.z);

						if (val && val < FogBulb->sqrad)
						{
							val *= FogBulb->inv_sqrad * FogBulb->density;
							lVal = (long)val + (v->specular >> 24) - FogBulb->density;

							if (lVal < 0)
								lVal = 0;

							v->specular = (lVal << 24) | v->specular & 0xFFFFFF;
						}
					}
				}
			}
		}
	}
}

void inject_polyinsert(bool replace)
{
	INJECT(0x004812D0, HWR_DrawSortList, replace);
	INJECT(0x00480A50, DrawSortList, replace);
	INJECT(0x00481B50, CreateFogPos, replace);
	INJECT(0x004818C0, ControlFXBulb, replace);
	INJECT(0x004819B0, CreateFXBulbs, replace);
	INJECT(0x004818A0, ClearFXFogBulbs, replace);
	INJECT(0x004819F0, TriggerFXFogBulb, replace);
	INJECT(0x00481AD0, IsVolumetric, replace);
	INJECT(0x00481D20, DistCompare, replace);
	INJECT(0x00481DF0, InitialiseFogBulbs, replace);
	INJECT(0x00481E60, OmniEffect, replace);
	INJECT(0x004820C0, OmniFog, replace);
}
