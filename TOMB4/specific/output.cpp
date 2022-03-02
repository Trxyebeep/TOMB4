#include "../tomb4/pch.h"
#include "output.h"
#include "d3dmatrix.h"
#include "../game/objects.h"
#include "function_table.h"
#include "lighting.h"
#include "dxshell.h"
#include "drawroom.h"
#include "3dmath.h"
#include "polyinsert.h"
#include "../game/effects.h"
#include "../game/draw.h"
#include "specificfx.h"
#include "function_stubs.h"
#ifdef GENERAL_FIXES
#include "../game/text.h"
#include "../game/gameflow.h"
#endif

void phd_PutPolygons(short* objptr, long clip)	//whore
{
	MESH_DATA* mesh;
	SPRITESTRUCT* envmap_sprite;
	D3DTLVERTEX* v;
	TEXTURESTRUCT* pTex;
	D3DVECTOR normals[4];
	TEXTURESTRUCT envmap_texture;
	short* quad;
	short* tri;
	long clrbak[4];
	long spcbak[4];
	long num;
	ushort drawbak;
	bool envmap;

	SetD3DViewMatrix();
	mesh = (MESH_DATA*)objptr;

	if (objptr)
	{
#ifdef GENERAL_FIXES
		if (objptr == meshes[objects[LARA_DOUBLE].mesh_index] || objptr == meshes[objects[LARA_DOUBLE].mesh_index + 2])
#else
		if (objptr == meshes[objects[LARA_DOUBLE].mesh_index])	//forgets mesh 1, also somehow applies to Lara's 'hips'.
#endif
			envmap_sprite = &spriteinfo[objects[SKY_GRAPHICS].mesh_index];
		else
			envmap_sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 11];

		if (GlobalAmbient)
		{
			ClearObjectLighting();
			ClearDynamicLighting();
			App.dx.lpD3DDevice->SetLightState(D3DLIGHTSTATE_AMBIENT, GlobalAmbient);
			GlobalAmbient = 0;
		}
		else if (mesh->prelight)
		{
			ClearObjectLighting();
			InitItemDynamicLighting(current_item);
		}
		else
			InitObjectLighting(current_item);

		if (mesh->nVerts)
			DXAttempt(DestVB->ProcessVertices(D3DVOP_LIGHT | D3DVOP_TRANSFORM, 0, mesh->nVerts, mesh->SourceVB, 0, App.dx._lpD3DDevice, 0));

		DestVB->Lock(DDLOCK_READONLY, (void**)&v, 0);
		clip_left = f_left;
		clip_top = f_top;
		clip_right = f_right;
		clip_bottom = f_bottom;

		if (mesh->nVerts)
		{
			bWaterEffect = camera.underwater;
			ProjectVerts(mesh->nVerts, v, clipflags);

			if (mesh->prelight)
				PrelightVerts(mesh->nVerts, v, mesh);
			else
				CalcVertsColorSplitMMX(mesh->nVerts, v);
		}

		quad = mesh->gt4;

		for (int i = 0; i < mesh->ngt4; i++, quad += 6)
		{
			pTex = &textinfo[quad[4] & 0x7FFF];
			envmap = 0;
			drawbak = pTex->drawtype;

			if (quad[5] & 1)
				pTex->drawtype = 2;

			if (quad[5] & 2)
			{
				envmap = 1;
				num = (quad[5] >> 2) & 0x1F;
				normals[0] = mesh->Normals[quad[0]];
				normals[1] = mesh->Normals[quad[1]];
				normals[2] = mesh->Normals[quad[2]];
				normals[3] = mesh->Normals[quad[3]];
				D3DTransform(&normals[0], &D3DMView);
				D3DTransform(&normals[1], &D3DMView);
				D3DTransform(&normals[2], &D3DMView);
				D3DTransform(&normals[3], &D3DMView);

				for (int i = 0; i < 4; i++)
				{
					normals[i].x *= 0.125F;
					normals[i].y *= 0.125F;
					normals[i].z *= 0.125F;
				}

				envmap_texture.drawtype = 2;
				envmap_texture.flag = pTex->flag;
				envmap_texture.tpage = envmap_sprite->tpage;
				envmap_texture.u1 = normals[0].x + envmap_sprite->x1 + 0.125F;
				envmap_texture.v1 = normals[0].y + envmap_sprite->y1 + 0.125F;
				envmap_texture.u2 = normals[1].x + envmap_sprite->x1 + 0.125F;
				envmap_texture.v2 = normals[1].y + envmap_sprite->y1 + 0.125F;
				envmap_texture.u3 = normals[2].x + envmap_sprite->x1 + 0.125F;
				envmap_texture.v3 = normals[2].y + envmap_sprite->y1 + 0.125F;
				envmap_texture.u4 = normals[3].x + envmap_sprite->x1 + 0.125F;
				envmap_texture.v4 = normals[3].y + envmap_sprite->y1 + 0.125F;
			}

			if (GlobalAlpha == 0xFF000000)
			{
				if (!pTex->drawtype)
					AddQuadZBuffer(v, quad[0], quad[1], quad[2], quad[3], pTex, 0);
				else if (pTex->drawtype <= 2)
					AddQuadSorted(v, quad[0], quad[1], quad[2], quad[3], pTex, 0);

				if (envmap)
				{
					clrbak[0] = v[quad[0]].color;
					clrbak[1] = v[quad[1]].color;
					clrbak[2] = v[quad[2]].color;
					clrbak[3] = v[quad[3]].color;
					spcbak[0] = v[quad[0]].specular;
					spcbak[1] = v[quad[1]].specular;
					spcbak[2] = v[quad[2]].specular;
					spcbak[3] = v[quad[3]].specular;
					RGB_M(v[quad[0]].color, num);
					RGB_M(v[quad[1]].color, num);
					RGB_M(v[quad[2]].color, num);
					RGB_M(v[quad[3]].color, num);
					RGB_M(v[quad[0]].specular, num);
					RGB_M(v[quad[1]].specular, num);
					RGB_M(v[quad[2]].specular, num);
					RGB_M(v[quad[3]].specular, num);
					AddQuadSorted(v, quad[0], quad[1], quad[2], quad[3], &envmap_texture, 0);
					v[quad[0]].color = clrbak[0];
					v[quad[1]].color = clrbak[1];
					v[quad[2]].color = clrbak[2];
					v[quad[3]].color = clrbak[3];
					v[quad[0]].specular = spcbak[0];
					v[quad[1]].specular = spcbak[1];
					v[quad[2]].specular = spcbak[2];
					v[quad[3]].specular = spcbak[3];
				}
			}
			else
			{
				pTex->drawtype = 7;
				AddQuadSorted(v, quad[0], quad[1], quad[2], quad[3], pTex, 0);
			}

			pTex->drawtype = drawbak;
		}

		tri = mesh->gt3;

		for (int i = 0; i < mesh->ngt3; i++, tri += 5)
		{
			pTex = &textinfo[tri[3] & 0x7FFF];
			envmap = 0;
			drawbak = pTex->drawtype;

			if (tri[4] & 1)
				pTex->drawtype = 2;

			if (tri[4] & 2)
			{
				envmap = 1;
				num = (tri[4] >> 2) & 0x1F;
				normals[0] = mesh->Normals[tri[0]];
				normals[1] = mesh->Normals[tri[1]];
				normals[2] = mesh->Normals[tri[2]];
				D3DTransform(&normals[0], &D3DMView);
				D3DTransform(&normals[1], &D3DMView);
				D3DTransform(&normals[2], &D3DMView);

				for (int i = 0; i < 3; i++)
				{
					normals[i].x *= 0.125F;
					normals[i].y *= 0.125F;
					normals[i].z *= 0.125F;
				}

				envmap_texture.drawtype = 2;
				envmap_texture.flag = pTex->flag;
				envmap_texture.tpage = envmap_sprite->tpage;
				envmap_texture.u1 = normals[0].x + envmap_sprite->x1 + 0.125F;
				envmap_texture.v1 = normals[0].y + envmap_sprite->y1 + 0.125F;
				envmap_texture.u2 = normals[1].x + envmap_sprite->x1 + 0.125F;
				envmap_texture.v2 = normals[1].y + envmap_sprite->y1 + 0.125F;
				envmap_texture.u3 = normals[2].x + envmap_sprite->x1 + 0.125F;
				envmap_texture.v3 = normals[2].y + envmap_sprite->y1 + 0.125F;
			}

			if (GlobalAlpha == 0xFF000000)
			{
				if (!pTex->drawtype)
					AddTriZBuffer(v, tri[0], tri[1], tri[2], pTex, 0);
				else if (pTex->drawtype <= 2)
					AddTriSorted(v, tri[0], tri[1], tri[2], pTex, 0);

				if (envmap)
				{
					clrbak[0] = v[tri[0]].color;
					clrbak[1] = v[tri[1]].color;
					clrbak[2] = v[tri[2]].color;
					spcbak[0] = v[tri[0]].specular;
					spcbak[1] = v[tri[1]].specular;
					spcbak[2] = v[tri[2]].specular;
					RGB_M(v[tri[0]].color, num);
					RGB_M(v[tri[1]].color, num);
					RGB_M(v[tri[2]].color, num);
					RGB_M(v[tri[0]].specular, num);
					RGB_M(v[tri[1]].specular, num);
					RGB_M(v[tri[2]].specular, num);
					AddTriSorted(v, tri[0], tri[1], tri[2], &envmap_texture, 0);
					v[tri[0]].color = clrbak[0];
					v[tri[1]].color = clrbak[1];
					v[tri[2]].color = clrbak[2];
					v[tri[0]].specular = spcbak[0];
					v[tri[1]].specular = spcbak[1];
					v[tri[2]].specular = spcbak[2];
				}
			}
			else
			{
				pTex->drawtype = 7;
				AddTriSorted(v, tri[0], tri[1], tri[2], pTex, 0);
			}

			pTex->drawtype = drawbak;
		}

		DestVB->Unlock();
	}
}

void phd_PutPolygons_train(short* objptr, long x)
{
	MESH_DATA* mesh;
	D3DTLVERTEX* v;
	TEXTURESTRUCT* pTex;
	short* quad;
	short* tri;
	ushort drawbak;

	phd_PushMatrix();
	phd_TranslateRel(x, 0, 0);
	SetD3DViewMatrix();
	mesh = (MESH_DATA*)objptr;
	phd_PopMatrix();

	if (objptr)
	{
		ClearObjectLighting();
		ClearDynamicLighting();
		App.dx.lpD3DDevice->SetLightState(D3DLIGHTSTATE_AMBIENT, 0xFFFFFF);

		if (mesh->nVerts)
			DXAttempt(DestVB->ProcessVertices(D3DVOP_LIGHT | D3DVOP_TRANSFORM, 0, mesh->nVerts, mesh->SourceVB, 0, App.dx._lpD3DDevice, 0));

		DestVB->Lock(DDLOCK_READONLY, (void**)&v, 0);
		clip_left = f_left;
		clip_top = f_top;
		clip_right = f_right;
		clip_bottom = f_bottom;

		if (mesh->nVerts)
			ProjectTrainVerts(mesh->nVerts, v, clipflags, x);

		quad = mesh->gt4;

		for (int i = 0; i < mesh->ngt4; i++, quad += 6)
		{
			pTex = &textinfo[quad[4] & 0x7FFF];
			drawbak = pTex->drawtype;

			if (quad[5] & 1)
				pTex->drawtype = 2;

			if (!pTex->drawtype)
				AddQuadZBuffer(v, quad[0], quad[1], quad[2], quad[3], pTex, 0);
			else if (pTex->drawtype <= 2)
				AddQuadSorted(v, quad[0], quad[1], quad[2], quad[3], pTex, 0);

			pTex->drawtype = drawbak;
		}

		tri = mesh->gt3;

		for (int i = 0; i < mesh->ngt3; i++, tri += 5)
		{
			pTex = &textinfo[tri[3] & 0x7FFF];

			drawbak = pTex->drawtype;

			if (tri[4] & 1)
				pTex->drawtype = 2;

			if (!pTex->drawtype)
				AddTriZBuffer(v, tri[0], tri[1], tri[2], pTex, 0);
			else if (pTex->drawtype <= 2)
				AddTriSorted(v, tri[0], tri[1], tri[2], pTex, 0);
		}

		DestVB->Unlock();
	}
}

void ProjectTrainVerts(short nVerts, D3DTLVERTEX* v, short* clip, long x)
{
	float zv;
	short clip_distance;

	for (int i = 0; i < nVerts; i++)
	{
		clip_distance = 0;
		v->tu = v->sx;
		v->tv = v->sy;

		if (v->sz < f_mznear)
			clip_distance = -128;
		else
		{
			zv = f_mpersp / v->sz;

			if (v->sz > FogEnd)
			{
				v->sz = f_zfar;
				clip_distance = 256;
			}

			v->sx = zv * v->sx + f_centerx;
			v->sy = zv * v->sy + f_centery;
			v->rhw = f_moneopersp * zv;

			if (v->sx < clip_left)
				clip_distance++;
			else if (v->sx > clip_right)
				clip_distance += 2;

			if (v->sy < clip_top)
				clip_distance += 4;
			else if (v->sy > clip_bottom)
				clip_distance += 8;
		}

		clip[0] = clip_distance;
		clip++;
		v++;
	}
}

void PrelightVerts(long nVerts, D3DTLVERTEX* v, MESH_DATA* mesh)
{
	long r, g, b, sr, sg, sb;

	sr = (StaticMeshShade & 0x1F) << 3;
	sg = ((StaticMeshShade >> 5) & 0x1F) << 3;
	sb = ((StaticMeshShade >> 10) & 0x1F) << 3;

	for (int i = 0; i < nVerts; i++)
	{
		r = CLRR(v->color) + ((sr * (mesh->prelight[i] & 0xFF)) >> 8);
		g = CLRG(v->color) + ((sg * (mesh->prelight[i] & 0xFF)) >> 8);
		b = CLRB(v->color) + ((sb * (mesh->prelight[i] & 0xFF)) >> 8);

		if (r > 255)
			r = 255;

		if (g > 255)
			g = 255;

		if (b > 255)
			b = 255;

		if (!(room[current_item->room_number].flags & 1) && bWaterEffect)
		{
			r = (r * water_color_R) >> 8;
			g = (g * water_color_G) >> 8;
			b = (b * water_color_B) >> 8;
		}

		v->color = RGBA(r, g, b, 0xFF);
		CalcColorSplit(v->color, &v->color);
		v++;
	}
}

void _InsertRoom(ROOM_INFO* r)
{
	SetD3DViewMatrix();
	InsertRoom(r);
}

void RenderLoadPic(long unused)
{
	short poisoned;

	camera.pos.y = load_cam.y;
	camera.pos.x = load_cam.x;
	camera.pos.z = load_cam.z;
	lara_item->pos.x_pos = camera.pos.x;
	lara_item->pos.y_pos = camera.pos.y;
	lara_item->pos.z_pos = camera.pos.z;
	camera.target.x = load_target.x;
	camera.target.y = load_target.y;
	camera.target.z = load_target.z;
	camera.pos.room_number = load_roomnum;

	if (load_roomnum == 255)
		return;

	KillActiveBaddies((ITEM_INFO*)0xABCDEF);
	SetFade(255, 0);
	poisoned = lara.poisoned;
	FadeScreenHeight = 0;
	lara.poisoned = 0;
	GlobalFogOff = 1;
	BinocularRange = 0;

	if (App.dx.InScene)
		_EndScene();

	do
	{
		phd_LookAt(camera.pos.x, camera.pos.y, camera.pos.z, camera.target.x, camera.target.y, camera.target.z, 0);
		S_InitialisePolyList();
		RenderIt(camera.pos.room_number);
		S_OutputPolyList();
		S_DumpScreen();

	} while (DoFade != 2);

	phd_LookAt(camera.pos.x, camera.pos.y, camera.pos.z, camera.target.x, camera.target.y, camera.target.z, 0);
	S_InitialisePolyList();
	RenderIt(camera.pos.room_number);

#ifdef GENERAL_FIXES
	PrintString((ushort)phd_centerx, ushort(phd_winymax - (font_height << 1)), 5, SCRIPT_TEXT(TXT_LOADING2), 0x8000);
#endif

	S_OutputPolyList();
	S_DumpScreen();
	lara.poisoned = poisoned;
	GlobalFogOff = 0;
}

void S_InitialisePolyList()
{
	D3DRECT rect;
	long col;

	rect.x1 = App.dx.rViewport.left;
	rect.y1 = App.dx.rViewport.top;
	rect.x2 = App.dx.rViewport.left + App.dx.rViewport.right;
	rect.y2 = App.dx.rViewport.top + App.dx.rViewport.bottom;

	if (gfLevelFlags & GF_TRAIN)
		col = 0xCEAE60;
	else if (gfCurrentLevel == 5 || gfCurrentLevel == 6)
	{
		col = FogTableColor[19];
		SetFogColor(CLRR(col), CLRG(col), CLRB(col));
	}
	else
		col = 0;
	
	if (App.dx.Flags & 0x80)
		DXAttempt(App.dx.lpViewport->Clear2(1, &rect, D3DCLEAR_TARGET, col, 1.0F, 0));
	else
		ClearFakeDevice(App.dx.lpD3DDevice, 1, &rect, D3DCLEAR_TARGET, col, 1.0F, 0);

	_BeginScene();
	InitBuckets();
	InitialiseSortList();
}

void inject_output(bool replace)
{
	INJECT(0x0047DA60, phd_PutPolygons, replace);
	INJECT(0x00480310, phd_PutPolygons_train, replace);
	INJECT(0x00480220, ProjectTrainVerts, replace);
	INJECT(0x0047D900, PrelightVerts, replace);
	INJECT(0x0047F950, _InsertRoom, replace);
	INJECT(0x00480570, RenderLoadPic, replace);
	INJECT(0x0047D5B0, S_InitialisePolyList, replace);
}
