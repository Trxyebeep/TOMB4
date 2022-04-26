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
#include "../game/newinv.h"
#ifdef GENERAL_FIXES
#include "../game/text.h"
#include "../game/gameflow.h"
#include "../tomb4/tomb4.h"
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
#ifdef GENERAL_FIXES
	D3DVERTEX* vtx;
	DYNAMIC* dptr;
	PHD_VECTOR t, d, u, w;
	float fVal;
#endif
	long r, g, b, sr, sg, sb;

	sr = (StaticMeshShade & 0x1F) << 3;
	sg = ((StaticMeshShade >> 5) & 0x1F) << 3;
	sb = ((StaticMeshShade >> 10) & 0x1F) << 3;
#ifdef GENERAL_FIXES
	u.x = phd_mxptr[M03] >> 14;
	u.y = phd_mxptr[M13] >> 14;
	u.z = phd_mxptr[M23] >> 14;
	ApplyTransposeMatrix(w2v_matrix, &u, &t);
	t.x += w2v_matrix[M03];
	t.y += w2v_matrix[M13];
	t.z += w2v_matrix[M23];
	mesh->SourceVB->Lock(DDLOCK_READONLY, (void**)&vtx, NULL);
#endif

	for (int i = 0; i < nVerts; i++)
	{
		r = CLRR(v->color) + ((sr * (mesh->prelight[i] & 0xFF)) >> 8);
		g = CLRG(v->color) + ((sg * (mesh->prelight[i] & 0xFF)) >> 8);
		b = CLRB(v->color) + ((sb * (mesh->prelight[i] & 0xFF)) >> 8);

#ifdef GENERAL_FIXES
		for (int j = 0; j < 32; j++)
		{
			dptr = &dynamics[j];

			if (dptr->on)
			{
				d.x = dptr->x - t.x;
				d.y = dptr->y - t.y;
				d.z = dptr->z - t.z;
				ApplyMatrix(w2v_matrix, &d, &w);
				ApplyTransposeMatrix(phd_mxptr, &w, &u);
				fVal = sqrt(SQUARE(u.x - vtx[i].x) + SQUARE(u.y - vtx[i].y) + SQUARE(u.z - vtx[i].z)) * 1.7F;
				
				if (fVal <= dptr->falloff)
				{
					fVal = (dptr->falloff - fVal) / dptr->falloff;
					r += (long)(fVal * dptr->r);
					g += (long)(fVal * dptr->g);
					b += (long)(fVal * dptr->b);
				}
			}
		}
#endif

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

#ifdef GENERAL_FIXES
	mesh->SourceVB->Unlock();
#endif
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

#ifdef GENERAL_FIXES
		if (tomb4.loadingtxt && !tomb4.tr5_loadbar)
		{
			if (tomb4.bar_mode == 2 || tomb4.bar_mode == 3)
				PrintString((ushort)phd_centerx, ushort((float((480 - (font_height >> 1)) * float(phd_winymax / 480.0F))) - (font_height >> 1)),
					5, SCRIPT_TEXT(TXT_LOADING2), FF_CENTER);
			else
				PrintString((ushort)phd_centerx, ushort((float(phd_winymax / 480.0F) + (phd_winymax - font_height)) - (font_height >> 1)),
					5, SCRIPT_TEXT(TXT_LOADING2), FF_CENTER);
		}
#endif

		S_OutputPolyList();
		S_DumpScreen();

	} while (DoFade != 2);

	phd_LookAt(camera.pos.x, camera.pos.y, camera.pos.z, camera.target.x, camera.target.y, camera.target.z, 0);
	S_InitialisePolyList();
	RenderIt(camera.pos.room_number);

#ifdef GENERAL_FIXES
	if (tomb4.loadingtxt && !tomb4.tr5_loadbar)
	{
		if (tomb4.bar_mode == 2 || tomb4.bar_mode == 3)
			PrintString((ushort)phd_centerx, ushort((float((480 - (font_height >> 1)) * float(phd_winymax / 480.0F))) - (font_height >> 1)),
				5, SCRIPT_TEXT(TXT_LOADING2), FF_CENTER);
		else
			PrintString((ushort)phd_centerx, ushort((float(phd_winymax / 480.0F) + (phd_winymax - font_height)) - (font_height >> 1)),
				5, SCRIPT_TEXT(TXT_LOADING2), FF_CENTER);
	}
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

void phd_PutPolygonsPickup(short* objptr, float x, float y, long color)
{
	MESH_DATA* mesh;
	SPRITESTRUCT* envmap_sprite;
	D3DTLVERTEX* v;
	TEXTURESTRUCT* pTex;
	D3DVECTOR normals[4];
	TEXTURESTRUCT envmap_texture;
	short* quad;
	short* tri;
	float fcx, fcy;
	long clrbak[4];
	long spcbak[4];
	long num;
	ushort drawbak;
	bool envmap;

	bWaterEffect = 0;
	SetD3DViewMatrix();
	mesh = (MESH_DATA*)objptr;
	envmap_sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 11];
	ClearDynamicLighting();
	ClearObjectLighting();
	App.dx.lpD3DDevice->SetLightState(D3DLIGHTSTATE_AMBIENT, color);

	if (mesh->nVerts)
		DXAttempt(DestVB->ProcessVertices(D3DVOP_TRANSFORM | D3DVOP_LIGHT, 0, mesh->nVerts, mesh->SourceVB, 0, App.dx._lpD3DDevice, 0));

	DestVB->Lock(DDLOCK_READONLY, (void**)&v, 0);
	clip_left = f_left;
	clip_top = f_top;
	clip_right = f_right;
	clip_bottom = f_bottom;
	fcx = f_centerx;
	fcy = f_centery;
	f_centerx = x;
	f_centery = y;

	if (mesh->nVerts)
	{
		ProjectVerts(mesh->nVerts, v, clipflags);

		if (mesh->prelight)
			PrelightVerts(mesh->nVerts, v, mesh);
		else
			CalcVertsColorSplitMMX(mesh->nVerts, v);
	}

	f_centerx = fcx;
	f_centery = fcy;
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

		if (GlobalAlpha != 0xFF000000)
			pTex->drawtype = 3;

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

		if (GlobalAlpha != 0xFF000000)
			pTex->drawtype = 3;
		
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

		pTex->drawtype = drawbak;
	}

	DestVB->Unlock();
}

void phd_PutPolygonSkyMesh(short* objptr, long clipstatus)
{
	D3DTLVERTEX* v;
	TEXTURESTRUCT* pTex;
	MESH_DATA* mesh;
	short* quad;
	short* tri;
	ushort drawbak;

	mesh = (MESH_DATA*)objptr;
	SetD3DViewMatrix();
	DXAttempt(DestVB->ProcessVertices(D3DVOP_TRANSFORM, 0, mesh->nVerts, mesh->SourceVB, 0, App.dx._lpD3DDevice, 0));
	DestVB->Lock(DDLOCK_READONLY, (void**)&v, 0);
	clip_top = f_top;
	clip_bottom = f_bottom;
	clip_left = f_left;
	clip_right = f_right;
	ProjectVerts(mesh->nVerts, v, clipflags);
	quad = mesh->gt4;

	for (int i = 0; i < mesh->ngt4; i++, quad += 6)
	{
		pTex = &textinfo[quad[4] & 0x7FFF];
		drawbak = pTex->drawtype;

		if (quad[5] & 1)
		{
			if (gfLevelFlags & GF_HORIZONCOLADD)
				pTex->drawtype = 2;
			else
			{
				if (App.dx.lpZBuffer)
				{
					v[quad[0]].color = 0;
					v[quad[1]].color = 0;
					v[quad[2]].color = 0xFF000000;
					v[quad[3]].color = 0xFF000000;
					pTex->drawtype = 3;
				}
				else
				{
					v[quad[0]].color = 0;
					v[quad[1]].color = 0;
					v[quad[2]].color = 0;
					v[quad[3]].color = 0;
					pTex->drawtype = 0;
				}
			}
		}
		else
			pTex->drawtype = 4;

		if (gfLevelFlags & GF_TRAIN || gfCurrentLevel == 5 || gfCurrentLevel == 6)
		{
			v[quad[0]].color = 0xFFFFFFFF;
			v[quad[1]].color = 0xFFFFFFFF;
			v[quad[2]].color = 0xFFFFFFFF;
			v[quad[3]].color = 0xFFFFFFFF;

			if (i < 16)
			{
				v[quad[0]].specular = 0x7F000000;
				v[quad[1]].specular = 0x7F000000;
				v[quad[2]].specular = 0;
				v[quad[3]].specular = 0;
			}
		}

		AddQuadSorted(v, quad[0], quad[1], quad[2], quad[3], pTex, 0);
		pTex->drawtype = drawbak;
	}

	tri = mesh->gt3;

	for (int i = 0; i < mesh->ngt3; i++, tri += 5)
	{
		pTex = &textinfo[tri[3] & 0x7FFF];
		drawbak = pTex->drawtype;
		pTex->drawtype = 4;
		AddTriSorted(v, tri[0], tri[1], tri[2], pTex, 0);
		pTex->drawtype = drawbak;
	}

	DestVB->Unlock();
}

void S_DrawPickup(short object_number)
{
	phd_LookAt(0, 1024, 0, 0, 0, 0, 0);
	SetD3DViewMatrix();
	DrawThreeDeeObject2D(long((float)phd_winxmax / 512.0F * 448.0F + PickupX), long((float)phd_winymax / 256.0F * 216.0F),
		convert_obj_to_invobj(object_number), 128, 0, (GnFrameCounter & 0x7F) << 9, 0, 0, 1);
}

long S_GetObjectBounds(short* bounds)
{
	FVECTOR vtx[8];
	float xMin, xMax, yMin, yMax, zMin, zMax, numZ, xv, yv, zv;

	if (phd_mxptr[11] >= phd_zfar && !outside)
		return 0;

	xMin = bounds[0];
	xMax = bounds[1];
	yMin = bounds[2];
	yMax = bounds[3];
	zMin = bounds[4];
	zMax = bounds[5];

	vtx[0].x = xMin;
	vtx[0].y = yMin;
	vtx[0].z = zMin;

	vtx[1].x = xMax;
	vtx[1].y = yMin;
	vtx[1].z = zMin;

	vtx[2].x = xMax;
	vtx[2].y = yMax;
	vtx[2].z = zMin;

	vtx[3].x = xMin;
	vtx[3].y = yMax;
	vtx[3].z = zMin;

	vtx[4].x = xMin;
	vtx[4].y = yMin;
	vtx[4].z = zMax;

	vtx[5].x = xMax;
	vtx[5].y = yMin;
	vtx[5].z = zMax;

	vtx[6].x = xMax;
	vtx[6].y = yMax;
	vtx[6].z = zMax;

	vtx[7].x = xMin;
	vtx[7].y = yMax;
	vtx[7].z = zMax;

	xMin = (float)0x3FFFFFFF;
	xMax = (float)-0x3FFFFFFF;
	yMin = (float)0x3FFFFFFF;
	yMax = (float)-0x3FFFFFFF;
	numZ = 0;

	for (int i = 0; i < 8; i++)
	{
		zv = vtx[i].x * phd_mxptr[M20] + vtx[i].y * phd_mxptr[M21] + vtx[i].z * phd_mxptr[M22] + phd_mxptr[M23];

		if (zv > phd_znear && phd_zfar > zv)
		{
			numZ++;
			zv /= phd_persp;

			if (!zv)
				zv = 1;

			zv = 1 / zv;
			xv = zv * (vtx[i].x * phd_mxptr[M00] + vtx[i].y * phd_mxptr[M01] + vtx[i].z * phd_mxptr[M02] + phd_mxptr[M03]);

			if (xv < xMin)
				xMin = xv;

			if (xv > xMax)
				xMax = xv;

			yv = zv * (vtx[i].x * phd_mxptr[M10] + vtx[i].y * phd_mxptr[M11] + vtx[i].z * phd_mxptr[M12] + phd_mxptr[M13]);

			if (yv < yMin)
				yMin = yv;

			if (yv > yMax)
				yMax = yv;
		}
	}

	xMin += phd_centerx;
	xMax += phd_centerx;
	yMin += phd_centery;
	yMax += phd_centery;

	if (numZ < 8 || xMin < 0 || yMin < 0 || phd_winxmax < xMax || phd_winymax < yMax)
		return -1;

	if (phd_right >= xMin && phd_bottom >= yMin && phd_left <= xMax && phd_top <= yMax)
		return 1;
	else
		return 0;
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
	INJECT(0x0047E8B0, phd_PutPolygonsPickup, replace);
	INJECT(0x0047F620, phd_PutPolygonSkyMesh, replace);
	INJECT(0x0047F970, S_DrawPickup, replace);
	INJECT(0x0047FCF0, S_GetObjectBounds, replace);
}
