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
#include "time.h"
#include "winmain.h"
#include "../game/tomb4fx.h"
#include "../game/delstuff.h"
#include "../game/camera.h"
#include "gamemain.h"
#include "LoadSave.h"
#include "file.h"
#include "../game/lara.h"
#include "../game/deltapak.h"
#include "../game/health.h"
#include "../game/control.h"
#include "../game/text.h"
#include "../game/gameflow.h"
#include "../game/spotcam.h"
#include "../game/effect2.h"
#include "../tomb4/tomb4.h"

D3DTLVERTEX SkinVerts[40][12];
short SkinClip[40][12];
long GlobalAlpha = 0xFF000000;
long GlobalAmbient;

float AnimatingTexturesV[16][8][3];
static short AnimatingTexturesVOffset;

void ProcessObjectMeshVertices(MESH_DATA* mesh)
{
	POINTLIGHT_STRUCT* point;
	SUNLIGHT_STRUCT* sun;
	FVECTOR vPos;
	FVECTOR vtx;
	FVECTOR n;
	float* v;
	short* clip;
	static float DistanceFogStart;
	float zv, fR, fG, fB, val, num;
	long lp, cR, cG, cB, sA, sR, sG, sB;
	short clipFlag;

	clip = clipflags;

	if (gfLevelFlags & GF_TRAIN || gfCurrentLevel == 5 || gfCurrentLevel == 6)
		DistanceFogStart = 12.0F * 1024.0F;
	else
		DistanceFogStart = tomb4.distance_fog * 1024.0F;

	num = 255.0F / DistanceFogStart;
	mesh->SourceVB->Lock(DDLOCK_READONLY, (LPVOID*)&v, 0);

	for (int i = 0; i < mesh->nVerts; i++)
	{
		vtx.x = *v++;
		vtx.y = *v++;
		vtx.z = *v++;
		n.x = mesh->Normals[i].x;
		n.y = mesh->Normals[i].y;
		n.z = mesh->Normals[i].z;
		v += 5;

		vPos.x = vtx.x * D3DMView._11 + vtx.y * D3DMView._21 + vtx.z * D3DMView._31 + D3DMView._41;
		vPos.y = vtx.x * D3DMView._12 + vtx.y * D3DMView._22 + vtx.z * D3DMView._32 + D3DMView._42;
		vPos.z = vtx.x * D3DMView._13 + vtx.y * D3DMView._23 + vtx.z * D3DMView._33 + D3DMView._43;

		MyVertexBuffer[i].tu = vPos.x;
		MyVertexBuffer[i].tv = vPos.y;

		sA = 0xFF;
		sR = 0;
		sG = 0;
		sB = 0;

		if (nTotalLights)
		{
			fR = (float)ambientR;
			fG = (float)ambientG;
			fB = (float)ambientB;

			for (lp = 0; lp < nPointLights; lp++)
			{
				point = &PointLights[lp];
				val = point->vec.x * n.x + point->vec.y * n.y + point->vec.z * n.z;

				if (val > 0)
				{
					val *= point->rad;
					fR += val * point->r;
					fG += val * point->g;
					fB += val * point->b;
				}
			}

			for (lp = 0; lp < nSpotLights; lp++)
			{
				point = &SpotLights[lp];
				val = point->vec.x * n.x + point->vec.y * n.y + point->vec.z * n.z;

				if (val > 0)
				{
					val *= point->rad;
					fR += val * point->r;
					fG += val * point->g;
					fB += val * point->b;
				}
			}

			for (lp = 0; lp < nSunLights; lp++)
			{
				sun = &SunLights[lp];
				val = sun->vec.x * n.x + sun->vec.y * n.y + sun->vec.z * n.z;

				if (val > 0)
				{
					if (!InventoryActive)
						val *= 0.75F;
					else
						val += val;

					fR += val * sun->r;
					fG += val * sun->g;
					fB += val * sun->b;
				}
			}

			cR = (long)fR;
			cG = (long)fG;
			cB = (long)fB;
		}
		else
		{
			cR = ambientR;
			cG = ambientG;
			cB = ambientB;
		}
		
		if (vPos.z > DistanceFogStart)
		{
			val = (vPos.z - DistanceFogStart) * num;

			if (gfLevelFlags & GF_TRAIN || gfCurrentLevel == 5 || gfCurrentLevel == 6)
			{
				val = (vPos.z - DistanceFogStart) / 512.0F;
				sA -= long(val * (255.0F / 8.0F));

				if (sA < 0)
					sA = 0;
			}
			else
			{
				cR -= (long)val;
				cG -= (long)val;
				cB -= (long)val;
			}
		}

		if (cR - 128 <= 0)
			cR <<= 1;
		else
		{
			sR = (cR - 128) >> 1;
			cR = 255;
		}

		if (cG - 128 <= 0)
			cG <<= 1;
		else
		{
			sG = (cG - 128) >> 1;
			cG = 255;
		}

		if (cB - 128 <= 0)
			cB <<= 1;
		else
		{
			sB = (cB - 128) >> 1;
			cB = 255;
		}

		clipFlag = 0;

		if (vPos.z < f_mznear)
			clipFlag = -128;
		else
		{
			zv = f_mpersp / vPos.z;

			if (gfLevelFlags & GF_TRAIN || gfCurrentLevel == 5 || gfCurrentLevel == 6)
			{
				if (vPos.z > FogEnd)
				{
					clipFlag = 16;
					vPos.z = f_zfar;
				}
			}

			vPos.x = vPos.x * zv + f_centerx;
			vPos.y = vPos.y * zv + f_centery;

			if (camera.underwater)
			{
				vPos.x += vert_wibble_table[((wibble + (long)vPos.y) >> 3) & 0x1F];
				vPos.y += vert_wibble_table[((wibble + (long)vPos.x) >> 3) & 0x1F];
			}

			MyVertexBuffer[i].rhw = zv * f_moneopersp;

			if (vPos.x < clip_left)
				clipFlag++;
			else if (vPos.x > clip_right)
				clipFlag += 2;

			if (vPos.y < clip_top)
				clipFlag += 4;
			else if (vPos.y > clip_bottom)
				clipFlag += 8;
		}

		*clip++ = clipFlag;
		MyVertexBuffer[i].sx = vPos.x;
		MyVertexBuffer[i].sy = vPos.y;
		MyVertexBuffer[i].sz = vPos.z;

		if (current_item)
		{
			if (!(room[current_item->room_number].flags & ROOM_UNDERWATER) && camera.underwater)
			{
				cR = (cR * water_color_R) >> 8;
				cG = (cG * water_color_G) >> 8;
				cB = (cB * water_color_B) >> 8;
			}
		}

		if (sR > 255) sR = 255; else if (sR < 0) sR = 0;
		if (sG > 255) sG = 255; else if (sG < 0) sG = 0;
		if (sB > 255) sB = 255; else if (sB < 0) sB = 0;
		if (cR > 255) cR = 255; else if (cR < 0) cR = 0;
		if (cG > 255) cG = 255; else if (cG < 0) cG = 0;
		if (cB > 255) cB = 255; else if (cB < 0) cB = 0;

		MyVertexBuffer[i].color = GlobalAlpha | RGBONLY(cR, cG, cB);
		MyVertexBuffer[i].specular = RGBA(sR, sG, sB, sA);
	}

	mesh->SourceVB->Unlock();
}

void ProcessStaticMeshVertices(MESH_DATA* mesh)
{
	DYNAMIC* l;
	FVECTOR d;
	FVECTOR lPos;
	FVECTOR vPos;
	FVECTOR vtx;
	float* v;
	short* clip;
	static float DistanceFogStart;
	float zv, val, val2, num;
	long sA, sR, sG, sB, cR, cG, cB, pR, pG, pB;
	short clipFlag;

	clip = clipflags;

	if (gfLevelFlags & GF_TRAIN || gfCurrentLevel == 5 || gfCurrentLevel == 6)
		DistanceFogStart = 12.0F * 1024.0F;
	else
		DistanceFogStart = tomb4.distance_fog * 1024.0F;

	num = 255.0F / DistanceFogStart;
	pR = (StaticMeshShade & 0x1F) << 3;
	pG = ((StaticMeshShade >> 5) & 0x1F) << 3;
	pB = ((StaticMeshShade >> 10) & 0x1F) << 3;
	mesh->SourceVB->Lock(DDLOCK_READONLY, (LPVOID*)&v, 0);

	for (int i = 0; i < mesh->nVerts; i++)
	{
		vtx.x = *v++;
		vtx.y = *v++;
		vtx.z = *v++;
		v += 5;

		vPos.x = vtx.x * D3DMView._11 + vtx.y * D3DMView._21 + vtx.z * D3DMView._31 + D3DMView._41;
		vPos.y = vtx.x * D3DMView._12 + vtx.y * D3DMView._22 + vtx.z * D3DMView._32 + D3DMView._42;
		vPos.z = vtx.x * D3DMView._13 + vtx.y * D3DMView._23 + vtx.z * D3DMView._33 + D3DMView._43;
		MyVertexBuffer[i].tu = vPos.x;
		MyVertexBuffer[i].tv = vPos.y;

		cR = CLRR(mesh->prelight[i]);
		cG = CLRG(mesh->prelight[i]);
		cB = CLRB(mesh->prelight[i]);
		cR = (cR * pR) >> 8;
		cG = (cG * pG) >> 8;
		cB = (cB * pB) >> 8;
		sA = 0xFF;
		sR = 0;
		sG = 0;
		sB = 0;

		if (tomb4.static_lighting)
		{
			for (int j = 0; j < MAX_DYNAMICS; j++)
			{
				l = &dynamics[j];

				if (!l->on)
					continue;

				d.x = l->x - lGlobalMeshPos.x;
				d.y = l->y - lGlobalMeshPos.y;
				d.z = l->z - lGlobalMeshPos.z;
				lPos.x = D3DLightMatrix._11 * d.x + D3DLightMatrix._12 * d.y + D3DLightMatrix._13 * d.z;
				lPos.y = D3DLightMatrix._21 * d.x + D3DLightMatrix._22 * d.y + D3DLightMatrix._23 * d.z;
				lPos.z = D3DLightMatrix._31 * d.x + D3DLightMatrix._32 * d.y + D3DLightMatrix._33 * d.z;
				val = sqrt(SQUARE(lPos.x - vtx.x) + SQUARE(lPos.y - vtx.y) + SQUARE(lPos.z - vtx.z)) * 1.7F;

				if (val <= l->falloff)
				{
					val2 = (l->falloff - val) / l->falloff;
					cR += long(val2 * l->r);
					cG += long(val2 * l->g);
					cB += long(val2 * l->b);
				}
			}
		}

		if (vPos.z > DistanceFogStart)
		{
			val = (vPos.z - DistanceFogStart) * num;

			if (gfLevelFlags & GF_TRAIN || gfCurrentLevel == 5 || gfCurrentLevel == 6)
			{
				val = (vPos.z - DistanceFogStart) / 512.0F;
				sA -= long(val * (255.0F / 8.0F));

				if (sA < 0)
					sA = 0;
			}
			else
			{
				cR -= (long)val;
				cG -= (long)val;
				cB -= (long)val;
			}
		}

		if (cR - 128 <= 0)
			cR <<= 1;
		else
		{
			sR = (cR - 128) >> 1;
			cR = 255;
		}

		if (cG - 128 <= 0)
			cG <<= 1;
		else
		{
			sG = (cG - 128) >> 1;
			cG = 255;
		}

		if (cB - 128 <= 0)
			cB <<= 1;
		else
		{
			sB = (cB - 128) >> 1;
			cB = 255;
		}
		
		clipFlag = 0;

		if (vPos.z < f_mznear)
			clipFlag = -128;
		else
		{
			zv = f_mpersp / vPos.z;

			if (gfLevelFlags & GF_TRAIN || gfCurrentLevel == 5 || gfCurrentLevel == 6)
			{
				if (vPos.z > FogEnd)
				{
					clipFlag = 16;
					vPos.z = f_zfar;
				}
			}

			vPos.x = vPos.x * zv + f_centerx;
			vPos.y = vPos.y * zv + f_centery;

			if (camera.underwater)
			{
				vPos.x += vert_wibble_table[((wibble + (long)vPos.y) >> 3) & 0x1F];
				vPos.y += vert_wibble_table[((wibble + (long)vPos.x) >> 3) & 0x1F];
			}

			MyVertexBuffer[i].rhw = zv * f_moneopersp;

			if (vPos.x < clip_left)
				clipFlag++;
			else if (vPos.x > clip_right)
				clipFlag += 2;

			if (vPos.y < clip_top)
				clipFlag += 4;
			else if (vPos.y > clip_bottom)
				clipFlag += 8;
		}

		*clip++ = clipFlag;
		MyVertexBuffer[i].sx = vPos.x;
		MyVertexBuffer[i].sy = vPos.y;
		MyVertexBuffer[i].sz = vPos.z;

		if (!(room[current_item->room_number].flags & ROOM_UNDERWATER) && camera.underwater)
		{
			cR = (cR * water_color_R) >> 8;
			cG = (cG * water_color_G) >> 8;
			cB = (cB * water_color_B) >> 8;
		}

		if (sR > 255) sR = 255; else if (sR < 0) sR = 0;
		if (sG > 255) sG = 255; else if (sG < 0) sG = 0;
		if (sB > 255) sB = 255; else if (sB < 0) sB = 0;
		if (cR > 255) cR = 255; else if (cR < 0) cR = 0;
		if (cG > 255) cG = 255; else if (cG < 0) cG = 0;
		if (cB > 255) cB = 255; else if (cB < 0) cB = 0;

		MyVertexBuffer[i].color = GlobalAlpha | RGBONLY(cR, cG, cB);
		MyVertexBuffer[i].specular = RGBA(sR, sG, sB, sA);
	}

	mesh->SourceVB->Unlock();
}

void ProcessTrainMeshVertices(MESH_DATA* mesh)
{
	FVECTOR vPos;
	FVECTOR vtx;
	float* v;
	short* clip;
	static float DistanceFogStart;
	static float DistanceFogEnd;
	float zv, val, zbak, num, fR, fG, fB;
	long sA, sR, sG, sB, cR, cG, cB, dR, dG, dB;
	short clipFlag;

	clip = clipflags;
	DistanceFogStart = 17.0F * 1024.0F;	//does not listen to custom distance fog
	DistanceFogEnd = 25.0F * 1024.0F;
	num = 255.0F / DistanceFogStart;
	mesh->SourceVB->Lock(DDLOCK_READONLY, (LPVOID*)&v, 0);

	for (int i = 0; i < mesh->nVerts; i++)
	{
		vtx.x = *v++;
		vtx.y = *v++;
		vtx.z = *v++;
		v += 5;

		vPos.x = vtx.x * D3DMView._11 + vtx.y * D3DMView._21 + vtx.z * D3DMView._31 + D3DMView._41;
		vPos.y = vtx.x * D3DMView._12 + vtx.y * D3DMView._22 + vtx.z * D3DMView._32 + D3DMView._42;
		vPos.z = vtx.x * D3DMView._13 + vtx.y * D3DMView._23 + vtx.z * D3DMView._33 + D3DMView._43;
		MyVertexBuffer[i].tu = vPos.x;
		MyVertexBuffer[i].tv = vPos.y;
		zbak = vPos.z;

		cR = ambientR;
		cG = ambientG;
		cB = ambientB;
		sA = 0xFF;
		sR = 0;
		sG = 0;
		sB = 0;

		if (zbak > DistanceFogStart)
		{
			if (gfLevelFlags & GF_TRAIN || gfCurrentLevel == 5 || gfCurrentLevel == 6)
			{
				val = (zbak - DistanceFogStart) / 512.0F;
				sA -= long(val * (255.0F / 8.0F));

				if (sA < 0)
					sA = 0;

				dR = gfLevelFlags & GF_TRAIN ? 0xD2 : 0xE2;
				dG = gfLevelFlags & GF_TRAIN ? 0xB1 : 0x97;
				dB = gfLevelFlags & GF_TRAIN ? 0x63 : 0x76;
				val = (DistanceFogEnd - zbak) / (DistanceFogEnd - DistanceFogStart);

				fR = (float)dR / 255.0F;
				fG = (float)dG / 255.0F;
				fB = (float)dB / 255.0F;
				fR *= 1.0F - val;
				fG *= 1.0F - val;
				fB *= 1.0F - val;
				fR *= 255;
				fG *= 255;
				fB *= 255;
				sR = (long)fR;
				sG = (long)fG;
				sB = (long)fB;
				
				if (sR > dR) sR = dR;
				if (sG > dG) sG = dG;
				if (sB > dB) sB = dB;
			}
			else
			{
				val = (zbak - DistanceFogStart) * num;
				cR -= (long)val;
				cG -= (long)val;
				cB -= (long)val;
			}
		}
		
		clipFlag = 0;

		if (vPos.z < f_mznear)
			clipFlag = -128;
		else
		{
			zv = f_mpersp / vPos.z;

			if (vPos.z > FogEnd)
			{
				clipFlag = 16;
				vPos.z = f_zfar;
			}

			vPos.x = vPos.x * zv + f_centerx;
			vPos.y = vPos.y * zv + f_centery;
			MyVertexBuffer[i].rhw = zv * f_moneopersp;

			if (vPos.x < clip_left)
				clipFlag++;
			else if (vPos.x > clip_right)
				clipFlag += 2;

			if (vPos.y < clip_top)
				clipFlag += 4;
			else if (vPos.y > clip_bottom)
				clipFlag += 8;
		}

		*clip++ = clipFlag;
		MyVertexBuffer[i].sx = vPos.x;
		MyVertexBuffer[i].sy = vPos.y;
		MyVertexBuffer[i].sz = vPos.z;

		if (sR > 255) sR = 255; else if (sR < 0) sR = 0;
		if (sG > 255) sG = 255; else if (sG < 0) sG = 0;
		if (sB > 255) sB = 255; else if (sB < 0) sB = 0;
		if (cR > 255) cR = 255; else if (cR < 0) cR = 0;
		if (cG > 255) cG = 255; else if (cG < 0) cG = 0;
		if (cB > 255) cB = 255; else if (cB < 0) cB = 0;

		MyVertexBuffer[i].color = RGBA(cR, cG, cB, 0xFF);
		MyVertexBuffer[i].specular = RGBA(sR, sG, sB, sA);
	}

	mesh->SourceVB->Unlock();
}

void ProcessPickupMeshVertices(MESH_DATA* mesh)
{
	FVECTOR vPos;
	FVECTOR vtx;
	float* v;
	short* clip;
	float zv;
	long cR, cG, cB, sR, sG, sB;
	short clipFlag;

	clip = clipflags;
	mesh->SourceVB->Lock(DDLOCK_READONLY, (LPVOID*)&v, 0);

	for (int i = 0; i < mesh->nVerts; i++)
	{
		vtx.x = *v++;
		vtx.y = *v++;
		vtx.z = *v++;
		v += 5;

		vPos.x = vtx.x * D3DMView._11 + vtx.y * D3DMView._21 + vtx.z * D3DMView._31 + D3DMView._41;
		vPos.y = vtx.x * D3DMView._12 + vtx.y * D3DMView._22 + vtx.z * D3DMView._32 + D3DMView._42;
		vPos.z = vtx.x * D3DMView._13 + vtx.y * D3DMView._23 + vtx.z * D3DMView._33 + D3DMView._43;
		MyVertexBuffer[i].tu = vPos.x;
		MyVertexBuffer[i].tv = vPos.y;

		sR = 0;
		sG = 0;
		sB = 0;
		cR = ambientR;
		cG = ambientG;
		cB = ambientB;

		if (cR - 128 <= 0)
			cR <<= 1;
		else
		{
			sR = (cR - 128) >> 1;
			cR = 255;
		}

		if (cG - 128 <= 0)
			cG <<= 1;
		else
		{
			sG = (cG - 128) >> 1;
			cG = 255;
		}

		if (cB - 128 <= 0)
			cB <<= 1;
		else
		{
			sB = (cB - 128) >> 1;
			cB = 255;
		}

		clipFlag = 0;

		if (vPos.z < f_mznear)
			clipFlag = -128;
		else
		{
			zv = f_mpersp / vPos.z;
			vPos.x = vPos.x * zv + f_centerx;
			vPos.y = vPos.y * zv + f_centery;
			MyVertexBuffer[i].rhw = zv * f_moneopersp;

			if (vPos.x < clip_left)
				clipFlag++;
			else if (vPos.x > clip_right)
				clipFlag += 2;

			if (vPos.y < clip_top)
				clipFlag += 4;
			else if (vPos.y > clip_bottom)
				clipFlag += 8;
		}

		*clip++ = clipFlag;
		MyVertexBuffer[i].sx = vPos.x;
		MyVertexBuffer[i].sy = vPos.y;
		MyVertexBuffer[i].sz = vPos.z;

		if (sR > 255) sR = 255; else if (sR < 0) sR = 0;
		if (sG > 255) sG = 255; else if (sG < 0) sG = 0;
		if (sB > 255) sB = 255; else if (sB < 0) sB = 0;
		if (cR > 255) cR = 255; else if (cR < 0) cR = 0;
		if (cG > 255) cG = 255; else if (cG < 0) cG = 0;
		if (cB > 255) cB = 255; else if (cB < 0) cB = 0;

		MyVertexBuffer[i].color = RGBA(cR, cG, cB, CLRA(GlobalAlpha));
		MyVertexBuffer[i].specular = RGBA(sR, sG, sB, 0xFF);
	}

	mesh->SourceVB->Unlock();
}

static void RGB_M(ulong& c, long m)	//Original was a macro.
{
	long r, g, b, a;

	a = CLRA(c);
	r = (CLRR(c) * m) >> 8;
	g = (CLRG(c) * m) >> 8;
	b = (CLRB(c) * m) >> 8;
	c = RGBA(r, g, b, a);
}

void phd_PutPolygons(short* objptr, long clip)
{
	MESH_DATA* mesh;
	SPRITESTRUCT* envmap_sprite;
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

	if (!objptr)
		return;

	if (objptr == meshes[objects[LARA_DOUBLE].mesh_index] || objptr == meshes[objects[LARA_DOUBLE].mesh_index + 2])
		envmap_sprite = &spriteinfo[objects[SKY_GRAPHICS].mesh_index];
	else
		envmap_sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 11];

	ResetLighting();

	if (GlobalAmbient)
	{
		ambientR = CLRR(GlobalAmbient);
		ambientG = CLRG(GlobalAmbient);
		ambientB = CLRB(GlobalAmbient);
		GlobalAmbient = 0;
	}
	else if (mesh->prelight)
		InitItemDynamicLighting(current_item);
	else
		InitObjectLighting(current_item);

	clip_left = f_left;
	clip_top = f_top;
	clip_right = f_right;
	clip_bottom = f_bottom;

	if (mesh->nVerts)
	{
		if (mesh->prelight)
			ProcessStaticMeshVertices(mesh);
		else
			ProcessObjectMeshVertices(mesh);
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
			num <<= 3;
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
				AddQuadZBuffer(MyVertexBuffer, quad[0], quad[1], quad[2], quad[3], pTex, 0);
			else if (pTex->drawtype <= 2)
				AddQuadSorted(MyVertexBuffer, quad[0], quad[1], quad[2], quad[3], pTex, 0);

			if (envmap)
			{
				clrbak[0] = MyVertexBuffer[quad[0]].color;
				clrbak[1] = MyVertexBuffer[quad[1]].color;
				clrbak[2] = MyVertexBuffer[quad[2]].color;
				clrbak[3] = MyVertexBuffer[quad[3]].color;
				spcbak[0] = MyVertexBuffer[quad[0]].specular;
				spcbak[1] = MyVertexBuffer[quad[1]].specular;
				spcbak[2] = MyVertexBuffer[quad[2]].specular;
				spcbak[3] = MyVertexBuffer[quad[3]].specular;
				RGB_M(MyVertexBuffer[quad[0]].color, num);
				RGB_M(MyVertexBuffer[quad[1]].color, num);
				RGB_M(MyVertexBuffer[quad[2]].color, num);
				RGB_M(MyVertexBuffer[quad[3]].color, num);
				RGB_M(MyVertexBuffer[quad[0]].specular, num);
				RGB_M(MyVertexBuffer[quad[1]].specular, num);
				RGB_M(MyVertexBuffer[quad[2]].specular, num);
				RGB_M(MyVertexBuffer[quad[3]].specular, num);
				AddQuadSorted(MyVertexBuffer, quad[0], quad[1], quad[2], quad[3], &envmap_texture, 0);
				MyVertexBuffer[quad[0]].color = clrbak[0];
				MyVertexBuffer[quad[1]].color = clrbak[1];
				MyVertexBuffer[quad[2]].color = clrbak[2];
				MyVertexBuffer[quad[3]].color = clrbak[3];
				MyVertexBuffer[quad[0]].specular = spcbak[0];
				MyVertexBuffer[quad[1]].specular = spcbak[1];
				MyVertexBuffer[quad[2]].specular = spcbak[2];
				MyVertexBuffer[quad[3]].specular = spcbak[3];
			}
		}
		else
		{
			pTex->drawtype = 7;
			AddQuadSorted(MyVertexBuffer, quad[0], quad[1], quad[2], quad[3], pTex, 0);
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
			num <<= 3;
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
				AddTriZBuffer(MyVertexBuffer, tri[0], tri[1], tri[2], pTex, 0);
			else if (pTex->drawtype <= 2)
				AddTriSorted(MyVertexBuffer, tri[0], tri[1], tri[2], pTex, 0);

			if (envmap)
			{
				clrbak[0] = MyVertexBuffer[tri[0]].color;
				clrbak[1] = MyVertexBuffer[tri[1]].color;
				clrbak[2] = MyVertexBuffer[tri[2]].color;
				spcbak[0] = MyVertexBuffer[tri[0]].specular;
				spcbak[1] = MyVertexBuffer[tri[1]].specular;
				spcbak[2] = MyVertexBuffer[tri[2]].specular;
				RGB_M(MyVertexBuffer[tri[0]].color, num);
				RGB_M(MyVertexBuffer[tri[1]].color, num);
				RGB_M(MyVertexBuffer[tri[2]].color, num);
				RGB_M(MyVertexBuffer[tri[0]].specular, num);
				RGB_M(MyVertexBuffer[tri[1]].specular, num);
				RGB_M(MyVertexBuffer[tri[2]].specular, num);
				AddTriSorted(MyVertexBuffer, tri[0], tri[1], tri[2], &envmap_texture, 0);
				MyVertexBuffer[tri[0]].color = clrbak[0];
				MyVertexBuffer[tri[1]].color = clrbak[1];
				MyVertexBuffer[tri[2]].color = clrbak[2];
				MyVertexBuffer[tri[0]].specular = spcbak[0];
				MyVertexBuffer[tri[1]].specular = spcbak[1];
				MyVertexBuffer[tri[2]].specular = spcbak[2];
			}
		}
		else
		{
			pTex->drawtype = 7;
			AddTriSorted(MyVertexBuffer, tri[0], tri[1], tri[2], pTex, 0);
		}

		pTex->drawtype = drawbak;
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

	if (!objptr)
		return;

	phd_PushMatrix();
	phd_TranslateRel(x, 0, 0);
	SetD3DViewMatrix();
	mesh = (MESH_DATA*)objptr;
	phd_PopMatrix();

	v = MyVertexBuffer;
	clip_left = f_left;
	clip_top = f_top;
	clip_right = f_right;
	clip_bottom = f_bottom;
	ResetLighting();
	ambientR = 0xFF;
	ambientG = 0xFF;
	ambientB = 0xFF;
	ProcessTrainMeshVertices(mesh);
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
}

void _InsertRoom(ROOM_INFO* r)
{
	SetD3DViewMatrix();
	InsertRoom(r);
}

void RenderLoadPic(long unused)
{
	short poisoned;

	camera.pos.y = gfLoadCam.y;
	camera.pos.x = gfLoadCam.x;
	camera.pos.z = gfLoadCam.z;
	lara_item->pos.x_pos = camera.pos.x;
	lara_item->pos.y_pos = camera.pos.y;
	lara_item->pos.z_pos = camera.pos.z;
	camera.target.x = gfLoadTarget.x;
	camera.target.y = gfLoadTarget.y;
	camera.target.z = gfLoadTarget.z;
	camera.pos.room_number = gfLoadRoom;
	camera.underwater = room[gfLoadRoom].flags & ROOM_UNDERWATER;

	if (gfLoadRoom == 255)
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

		if (tomb4.loadingtxt && !tomb4.tr5_loadbar)
		{
			if (tomb4.bar_mode == 2 || tomb4.bar_mode == 3)
				PrintString(phd_centerx, long(float((480 - (font_height >> 1)) * float(phd_winymax / 480.0F))) - (font_height >> 1),
					5, SCRIPT_TEXT(TXT_LOADING2), FF_CENTER);
			else
				PrintString(phd_centerx, long(float(phd_winymax / 480.0F) + (phd_winymax - font_height)) - (font_height >> 1),
					5, SCRIPT_TEXT(TXT_LOADING2), FF_CENTER);
		}

		S_OutputPolyList();
		S_DumpScreen();

	} while (DoFade != 2);

	phd_LookAt(camera.pos.x, camera.pos.y, camera.pos.z, camera.target.x, camera.target.y, camera.target.z, 0);
	S_InitialisePolyList();
	RenderIt(camera.pos.room_number);

	if (tomb4.loadingtxt && !tomb4.tr5_loadbar)
	{
		if (tomb4.bar_mode == 2 || tomb4.bar_mode == 3)
			PrintString(phd_centerx, long(float((480 - (font_height >> 1)) * float(phd_winymax / 480.0F))) - (font_height >> 1),
				5, SCRIPT_TEXT(TXT_LOADING2), FF_CENTER);
		else
			PrintString(phd_centerx, long(float(phd_winymax / 480.0F) + (phd_winymax - font_height)) - (font_height >> 1),
				5, SCRIPT_TEXT(TXT_LOADING2), FF_CENTER);
	}

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
		col = 0xD2B163;
	else if (gfCurrentLevel == 5 || gfCurrentLevel == 6)
	{
		col = FogTableColor[19];
		SetFogColor(CLRR(col), CLRG(col), CLRB(col));
	}
	else
		col = 0;
	
	if (App.dx.Flags & DXF_HWR)
		DXAttempt(App.dx.lpViewport->Clear2(1, &rect, D3DCLEAR_TARGET, col, 1.0F, 0));

	_BeginScene();
	InitBuckets();
	InitialiseSortList();
}

void phd_PutPolygonsPickup(short* objptr, float x, float y, long color)
{
	MESH_DATA* mesh;
	SPRITESTRUCT* envmap_sprite;
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

	ResetLighting();
	ambientR = CLRR(color);
	ambientG = CLRG(color);
	ambientB = CLRB(color);
	clip_left = f_left;
	clip_top = f_top;
	clip_right = f_right;
	clip_bottom = f_bottom;
	fcx = f_centerx;
	fcy = f_centery;
	f_centerx = x;
	f_centery = y;

	if (mesh->nVerts)
		ProcessPickupMeshVertices(mesh);

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
			num <<= 3;
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

		AddQuadSorted(MyVertexBuffer, quad[0], quad[1], quad[2], quad[3], pTex, 0);

		if (envmap)
		{
			clrbak[0] = MyVertexBuffer[quad[0]].color;
			clrbak[1] = MyVertexBuffer[quad[1]].color;
			clrbak[2] = MyVertexBuffer[quad[2]].color;
			clrbak[3] = MyVertexBuffer[quad[3]].color;
			spcbak[0] = MyVertexBuffer[quad[0]].specular;
			spcbak[1] = MyVertexBuffer[quad[1]].specular;
			spcbak[2] = MyVertexBuffer[quad[2]].specular;
			spcbak[3] = MyVertexBuffer[quad[3]].specular;
			RGB_M(MyVertexBuffer[quad[0]].color, num);
			RGB_M(MyVertexBuffer[quad[1]].color, num);
			RGB_M(MyVertexBuffer[quad[2]].color, num);
			RGB_M(MyVertexBuffer[quad[3]].color, num);
			RGB_M(MyVertexBuffer[quad[0]].specular, num);
			RGB_M(MyVertexBuffer[quad[1]].specular, num);
			RGB_M(MyVertexBuffer[quad[2]].specular, num);
			RGB_M(MyVertexBuffer[quad[3]].specular, num);
			AddQuadSorted(MyVertexBuffer, quad[0], quad[1], quad[2], quad[3], &envmap_texture, 0);
			MyVertexBuffer[quad[0]].color = clrbak[0];
			MyVertexBuffer[quad[1]].color = clrbak[1];
			MyVertexBuffer[quad[2]].color = clrbak[2];
			MyVertexBuffer[quad[3]].color = clrbak[3];
			MyVertexBuffer[quad[0]].specular = spcbak[0];
			MyVertexBuffer[quad[1]].specular = spcbak[1];
			MyVertexBuffer[quad[2]].specular = spcbak[2];
			MyVertexBuffer[quad[3]].specular = spcbak[3];
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
			num <<= 3;
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
		
		AddTriSorted(MyVertexBuffer, tri[0], tri[1], tri[2], pTex, 0);

		if (envmap)
		{
			clrbak[0] = MyVertexBuffer[tri[0]].color;
			clrbak[1] = MyVertexBuffer[tri[1]].color;
			clrbak[2] = MyVertexBuffer[tri[2]].color;
			spcbak[0] = MyVertexBuffer[tri[0]].specular;
			spcbak[1] = MyVertexBuffer[tri[1]].specular;
			spcbak[2] = MyVertexBuffer[tri[2]].specular;
			RGB_M(MyVertexBuffer[tri[0]].color, num);
			RGB_M(MyVertexBuffer[tri[1]].color, num);
			RGB_M(MyVertexBuffer[tri[2]].color, num);
			RGB_M(MyVertexBuffer[tri[0]].specular, num);
			RGB_M(MyVertexBuffer[tri[1]].specular, num);
			RGB_M(MyVertexBuffer[tri[2]].specular, num);
			AddTriSorted(MyVertexBuffer, tri[0], tri[1], tri[2], &envmap_texture, 0);
			MyVertexBuffer[tri[0]].color = clrbak[0];
			MyVertexBuffer[tri[1]].color = clrbak[1];
			MyVertexBuffer[tri[2]].color = clrbak[2];
			MyVertexBuffer[tri[0]].specular = spcbak[0];
			MyVertexBuffer[tri[1]].specular = spcbak[1];
			MyVertexBuffer[tri[2]].specular = spcbak[2];
		}

		pTex->drawtype = drawbak;
	}
}

void phd_PutPolygonSkyMesh(short* objptr, long clipstatus)
{
	TEXTURESTRUCT* pTex;
	MESH_DATA* mesh;
	short* quad;
	short* tri;
	ushort drawbak;

	mesh = (MESH_DATA*)objptr;
	SetD3DViewMatrix();
	ResetLighting();
	ambientR = 128;
	ambientG = 128;
	ambientB = 128;
	clip_top = f_top;
	clip_bottom = f_bottom;
	clip_left = f_left;
	clip_right = f_right;
	current_item = 0;
	ProcessObjectMeshVertices(mesh);
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
					MyVertexBuffer[quad[0]].color = 0;
					MyVertexBuffer[quad[1]].color = 0;
					MyVertexBuffer[quad[2]].color = 0xFF000000;
					MyVertexBuffer[quad[3]].color = 0xFF000000;
					pTex->drawtype = 3;
				}
				else
				{
					MyVertexBuffer[quad[0]].color = 0;
					MyVertexBuffer[quad[1]].color = 0;
					MyVertexBuffer[quad[2]].color = 0;
					MyVertexBuffer[quad[3]].color = 0;
					pTex->drawtype = 0;
				}
			}
		}
		else
			pTex->drawtype = 4;

		if (gfLevelFlags & GF_TRAIN || gfCurrentLevel == 5 || gfCurrentLevel == 6)
		{
			MyVertexBuffer[quad[0]].color = 0xFFFFFFFF;
			MyVertexBuffer[quad[1]].color = 0xFFFFFFFF;
			MyVertexBuffer[quad[2]].color = 0xFFFFFFFF;
			MyVertexBuffer[quad[3]].color = 0xFFFFFFFF;

			if (i < 16)
			{
				MyVertexBuffer[quad[0]].specular = 0x7F000000;
				MyVertexBuffer[quad[1]].specular = 0x7F000000;
				MyVertexBuffer[quad[2]].specular = 0;
				MyVertexBuffer[quad[3]].specular = 0;
			}
		}

		AddQuadSorted(MyVertexBuffer, quad[0], quad[1], quad[2], quad[3], pTex, 0);
		pTex->drawtype = drawbak;
	}

	tri = mesh->gt3;

	for (int i = 0; i < mesh->ngt3; i++, tri += 5)
	{
		pTex = &textinfo[tri[3] & 0x7FFF];
		drawbak = pTex->drawtype;
		pTex->drawtype = 4;
		AddTriSorted(MyVertexBuffer, tri[0], tri[1], tri[2], pTex, 0);
		pTex->drawtype = drawbak;
	}
}

void S_DrawPickup(short object_number)
{
	long x, y;

	phd_LookAt(0, 1024, 0, 0, 0, 0, 0);
	SetD3DViewMatrix();

	x = phd_winwidth - GetFixedScale(80) + PickupX;
	y = phd_winheight - GetFixedScale(75);
	DrawThreeDeeObject2D(x, y, convert_obj_to_invobj(object_number), 128, 0, (GnFrameCounter & 0x7F) << 9, 0, 0, 1);
}

long S_GetObjectBounds(short* bounds)
{
	FVECTOR vtx[8];
	float xMin, xMax, yMin, yMax, zMin, zMax, numZ, xv, yv, zv;

	if (mMXPtr[M23] >= f_mzfar && !outside)
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
		zv = vtx[i].x * mMXPtr[M20] + vtx[i].y * mMXPtr[M21] + vtx[i].z * mMXPtr[M22] + mMXPtr[M23];

		if (zv > f_mznear && zv < f_mzfar)
		{
			numZ++;
			zv /= f_mpersp;

			if (!zv)
				zv = 1;

			zv = 1 / zv;
			xv = zv * (vtx[i].x * mMXPtr[M00] + vtx[i].y * mMXPtr[M01] + vtx[i].z * mMXPtr[M02] + mMXPtr[M03]);

			if (xv < xMin)
				xMin = xv;

			if (xv > xMax)
				xMax = xv;

			yv = zv * (vtx[i].x * mMXPtr[M10] + vtx[i].y * mMXPtr[M11] + vtx[i].z * mMXPtr[M12] + mMXPtr[M13]);

			if (yv < yMin)
				yMin = yv;

			if (yv > yMax)
				yMax = yv;
		}
	}

	xMin += f_centerx;
	xMax += f_centerx;
	yMin += f_centery;
	yMax += f_centery;

	if (numZ < 8 || xMin < 0 || yMin < 0 || phd_winxmax < xMax || phd_winymax < yMax)
		return -1;

	if (phd_right >= xMin && phd_bottom >= yMin && phd_left <= xMax && phd_top <= yMax)
		return 1;
	else
		return 0;
}

HRESULT DDCopyBitmap(LPDIRECTDRAWSURFACEX surf, HBITMAP hbm, long x, long y, long dx, long dy)
{
	HDC hdc;
	HDC hdc2;
	BITMAP bitmap;
	DDSURFACEDESCX desc;
	HRESULT result;
	long l, t;

	if (!hbm || !surf)
		return E_FAIL;

	surf->Restore();
	hdc = CreateCompatibleDC(0);

	if (!hdc)
		OutputDebugString("createcompatible dc failed\n");

	SelectObject(hdc, hbm);
	GetObject(hbm, sizeof(BITMAP), &bitmap);

	if (!dx)
		dx = bitmap.bmWidth;

	if (!dy)
		dy = bitmap.bmHeight;

	desc.dwSize = sizeof(DDSURFACEDESCX);
	desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT;
	surf->GetSurfaceDesc(&desc);
	l = 0;
	t = 0;

	if (!(App.dx.Flags & DXF_HWR))
	{
		surf = App.dx.lpPrimaryBuffer;

		if (App.dx.Flags & DXF_WINDOWED)
		{
			l = App.dx.rScreen.left;
			t = App.dx.rScreen.top;
		}
	}

	result = surf->GetDC(&hdc2);

	if (!result)
	{
		StretchBlt(hdc2, l, t, desc.dwWidth, desc.dwHeight, hdc, x, y, dx, dy, SRCCOPY);
		surf->ReleaseDC(hdc2);
	}

	DeleteDC(hdc);
	return result;
}

HRESULT _LoadBitmap(LPDIRECTDRAWSURFACEX surf, LPCSTR name)
{
	HBITMAP hBitmap;
	HRESULT result;

	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(0), name, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

	if (!hBitmap)
		hBitmap = (HBITMAP)LoadImage(0, name, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);

	if (!hBitmap)
	{
		OutputDebugString("handle is null\n");
		return E_FAIL;
	}

	result = DDCopyBitmap(surf, hBitmap, 0, 0, 0, 0);

	if (result != DD_OK)
		OutputDebugString("ddcopybitmap failed\n");

	DeleteObject(hBitmap);
	return result;
}

void do_boot_screen(long language)
{
	Log(2, "do_boot_screen");

	switch (language)
	{
		case ENGLISH:
		case DUTCH:
			_LoadBitmap(App.dx.lpBackBuffer, "uk.bmp");
			S_DumpScreen();
			_LoadBitmap(App.dx.lpBackBuffer, "uk.bmp");
			break;

		case FRENCH:
			_LoadBitmap(App.dx.lpBackBuffer, "france.bmp");
			S_DumpScreen();
			_LoadBitmap(App.dx.lpBackBuffer, "france.bmp");
			break;

		case GERMAN:
			_LoadBitmap(App.dx.lpBackBuffer, "germany.bmp");
			S_DumpScreen();
			_LoadBitmap(App.dx.lpBackBuffer, "germany.bmp");
			break;

		case ITALIAN:
			_LoadBitmap(App.dx.lpBackBuffer, "italy.bmp");
			S_DumpScreen();
			_LoadBitmap(App.dx.lpBackBuffer, "italy.bmp");
			break;

		case SPANISH:
			_LoadBitmap(App.dx.lpBackBuffer, "spain.bmp");
			S_DumpScreen();
			_LoadBitmap(App.dx.lpBackBuffer, "spain.bmp");
			break;

		case US:
			_LoadBitmap(App.dx.lpBackBuffer, "usa.bmp");
			S_DumpScreen();
			_LoadBitmap(App.dx.lpBackBuffer, "usa.bmp");
			break;

		case JAPAN:
			_LoadBitmap(App.dx.lpBackBuffer, "japan.bmp");
			S_DumpScreen();
			_LoadBitmap(App.dx.lpBackBuffer, "japan.bmp");
			break;
	}

#ifdef TIMES_LEVEL
	Sleep(2000);
#endif
}

void S_AnimateTextures(long n)
{
	TEXTURESTRUCT* tex;
	TEXTURESTRUCT tex2;
	short* range;
	float voff;
	static long comp;
	short nRanges, nRangeFrames;

	for (comp += n; comp > 5; comp -= 5)
	{
		nRanges = *aranges;
		range = aranges + 1;

		for (int i = 0; i < nRanges; i++)
		{
			nRangeFrames = *range++;

			if (i < nAnimUVRanges && gfUVRotate)
			{
				if (nRangeFrames > 0)
					range += nRangeFrames;
			}
			else
			{
				tex2 = textinfo[*range];

				while (nRangeFrames > 0)
				{
					textinfo[range[0]] = textinfo[range[1]];
					range++;
					nRangeFrames--;
				}

				textinfo[*range] = tex2;
			}

			range++;
		}
	}

	if (gfUVRotate)
	{
		range = aranges + 1;
		AnimatingTexturesVOffset = (AnimatingTexturesVOffset - gfUVRotate * (n >> 1)) & 0x1F;

		for (int i = 0; i < nAnimUVRanges; i++)
		{
			nRangeFrames = *range++;

			while (nRangeFrames >= 0)
			{
				tex = &textinfo[range[0]];
				voff = AnimatingTexturesVOffset * (1.0F / 256.0F);
				tex->v1 = voff + AnimatingTexturesV[i][nRangeFrames][0];
				tex->v2 = voff + AnimatingTexturesV[i][nRangeFrames][0];
				tex->v3 = voff + AnimatingTexturesV[i][nRangeFrames][0] + 0.125F;
				tex->v4 = voff + AnimatingTexturesV[i][nRangeFrames][0] + 0.125F;
				range++;
				nRangeFrames--;
			}
		}
	}
}

long S_DumpScreen()
{
	long n;

	n = Sync();

	while (n < 2)
	{
		while (!Sync());	//wait for sync
		n++;
	}

	GnFrameCounter++;
	_EndScene();
	DXShowFrame();
	App.dx.DoneBlit = 1;
	return n;
}

void S_OutputPolyList()
{
	D3DRECT r;
	long h;

	WinFrameRate();
	nPolys = 0;
	nClippedPolys = 0;
	DrawPrimitiveCnt = 0;
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);

	if (resChangeCounter)
	{
		WinDisplayString(8, App.dx.dwRenderHeight - 8, (char*)"%dx%d", App.dx.dwRenderWidth, App.dx.dwRenderHeight);
		resChangeCounter -= long(30 / App.fps);

		if (resChangeCounter < 0)
			resChangeCounter = 0;
	}

	if (App.dx.lpZBuffer)
		DrawBuckets();

	if (!gfCurrentLevel)
	{
		Fade();

		if (App.dx.lpZBuffer)
			DrawSortList();
	}

	SortPolyList(SortCount, SortList);
	DrawSortList();

	if (App.dx.lpZBuffer)
	{
		r.x1 = App.dx.rViewport.left;
		r.y1 = App.dx.rViewport.top;
		r.x2 = App.dx.rViewport.left + App.dx.rViewport.right;
		r.y2 = App.dx.rViewport.top + App.dx.rViewport.bottom;
		DXAttempt(App.dx.lpViewport->Clear2(1, &r, D3DCLEAR_ZBUFFER, 0, 1.0F, 0));
	}

	if (BinocularRange && !MonoScreenOn)
	{
		InitialiseSortList();
		DrawBinoculars();
		DrawSortList();
	}

	if (pickups[CurrentPickup].life != -1 && !MonoScreenOn && !GLOBAL_playing_cutseq && !bDisableLaraControl)
	{
		bWaterEffect = 0;
		InitialiseSortList();
		S_DrawPickup(pickups[CurrentPickup].object_number);
		SortPolyList(SortCount, SortList);
		DrawSortList();
	}

	InitialiseSortList();

	if (FadeScreenHeight)
	{
		h = long((float)phd_winymax / 256.0F) * FadeScreenHeight;
		DrawPsxTile(0, phd_winwidth | (h << 16), 0x62FFFFFF, 0, 0);
		DrawPsxTile(phd_winheight - h, phd_winwidth | (h << 16), 0x62FFFFFF, 0, 0);
	}

	if (gfCurrentLevel)
	{
		Fade();

		if (FlashFader)
		{
			DrawFlash();

			if (FlashFader)
				FlashFader -= 2;
		}

		DrawSortList();
	}

	if (DoFade == 1)
	{
		InitialiseSortList();
		DoScreenFade();
		DrawSortList();
	}
}

void StashSkinVertices(long node)
{
	D3DTLVERTEX* d;
	short* cf;
	char* vns;

	vns = (char*)&SkinVertNums[node];
	cf = (short*)&SkinClip[node];
	d = (D3DTLVERTEX*)&SkinVerts[node];

	while (1)
	{
		if (*vns < 0)
			break;

		d->sx = MyVertexBuffer[*vns].sx;
		d->sy = MyVertexBuffer[*vns].sy;
		d->sz = MyVertexBuffer[*vns].sz;
		d->rhw = MyVertexBuffer[*vns].rhw;
		d->color = MyVertexBuffer[*vns].color;
		d->specular = MyVertexBuffer[*vns].specular;
		d->tu = MyVertexBuffer[*vns].tu;
		d->tv = MyVertexBuffer[*vns].tv;
		*cf++ = clipflags[*vns];
		d++;
		vns++;
	}
}

void SkinVerticesToScratch(long node)
{
	D3DTLVERTEX* d;
	short* cf;
	char* vns;

	vns = (char*)&ScratchVertNums[node];
	cf = (short*)&SkinClip[node];
	d = (D3DTLVERTEX*)&SkinVerts[node];

	while (1)
	{
		if (*vns < 0)
			break;

		MyVertexBuffer[*vns].sx = d->sx;
		MyVertexBuffer[*vns].sy = d->sy;
		MyVertexBuffer[*vns].sz = d->sz;
		MyVertexBuffer[*vns].rhw = d->rhw;
		MyVertexBuffer[*vns].color = d->color;
		MyVertexBuffer[*vns].specular = d->specular;
		MyVertexBuffer[*vns].tu = d->tu;
		MyVertexBuffer[*vns].tv = d->tv;
		clipflags[*vns] = *cf++;
		d++;
		vns++;
	}
}

long GetRenderScale(long unit)	//User selected scale
{
	long w, h, x, y;

	w = long(640.0F /  tomb4.GUI_Scale);
	h = long(480.0F / tomb4.GUI_Scale);
	x = (phd_winwidth > w) ? MulDiv(phd_winwidth, unit, w) : unit;
	y = (phd_winheight > h) ? MulDiv(phd_winheight, unit, h) : unit;
	return x < y ? x : y;
}

long GetFixedScale(long unit)	//Fixed scale
{
	long w, h, x, y;

	w = 640;
	h = 480;
	x = (phd_winwidth > w) ? MulDiv(phd_winwidth, unit, w) : unit;
	y = (phd_winheight > h) ? MulDiv(phd_winheight, unit, h) : unit;
	return x < y ? x : y;
}
