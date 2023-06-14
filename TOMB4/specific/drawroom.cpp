#include "../tomb4/pch.h"
#include "drawroom.h"
#include "function_stubs.h"
#include "dxshell.h"
#include "polyinsert.h"
#include "function_table.h"
#include "d3dmatrix.h"
#include "lighting.h"
#include "../game/camera.h"
#include "../game/draw.h"
#include "../game/effect2.h"
#include "gamemain.h"
#include "texture.h"
#include "3dmath.h"
#include "winmain.h"
#include "file.h"
#include "../game/control.h"
#include "../game/gameflow.h"
#include "../tomb4/tomb4.h"

static ROOM_DYNAMIC RoomDynamics[MAX_DYNAMICS];
static long nRoomDynamics;

MESH_DATA** mesh_vtxbuf;
TEXTUREBUCKET Bucket[20];
float clip_left;
float clip_top;
float clip_right;
float clip_bottom;
long bWaterEffect;
long num_level_meshes;

long water_color_R = 128;
long water_color_G = 224;
long water_color_B = 255;

void ProcessRoomDynamics(ROOM_INFO* r)
{
	//Collect dynamic lights for room lighting

	ROOM_DYNAMIC* l;
	DYNAMIC* d;
	float falloff;
	
	nRoomDynamics = 0;
	l = RoomDynamics;

	for (int i = 0; i < MAX_DYNAMICS; i++)
	{
		d = &dynamics[i];

		if (!d->on)
			continue;

		falloff = float((d->falloff >> 1) + (d->falloff >> 3));
		l->x = d->x - r->posx;
		l->y = d->y - r->posy;
		l->z = d->z - r->posz;
		l->r = (float)d->r * (1.0F / 255.0F);
		l->g = (float)d->g * (1.0F / 255.0F);
		l->b = (float)d->b * (1.0F / 255.0F);
		l->falloff = falloff;
		l->inv_falloff = 1.0F / l->falloff;
		l->sqr_falloff = SQUARE(l->falloff);
		l++;
		nRoomDynamics++;
	}
}

void ProcessRoomVertices(ROOM_INFO* r)
{
	//Transform, project, and light vertices, and store them in MyVertexBuffer.

	ROOM_DYNAMIC* l;
	FVECTOR lPos;
	FVECTOR vPos;
	FVECTOR vtx;
	FVECTOR n;
	short* clip;
	static float DistanceFogStart;
	float zv, fR, fG, fB, val, val2, num;
	long cR, cG, cB, sA, sR, sG, sB, rndoff, col;
	short clipFlag;
	uchar rnd, abs;
	char shimmer;

	clip = clipflags;

	if (gfLevelFlags & GF_TRAIN || gfCurrentLevel == 5 || gfCurrentLevel == 6)
		DistanceFogStart = 12.0F * 1024.0F;
	else
		DistanceFogStart = tomb4.distance_fog * 1024.0F;

	num = 255.0F / DistanceFogStart;

	for (int i = 0; i < r->nVerts; i++)
	{
		vtx.x = r->x + r->verts[i].x;
		vtx.y = r->y + r->verts[i].y;
		vtx.z = r->z + r->verts[i].z;
		n.x = r->vnormals[i].x;
		n.y = r->vnormals[i].y;
		n.z = r->vnormals[i].z;

		rndoff = long((vtx.x / 64.0F) + (vtx.y / 64.0F) + (vtx.z / 128.0F)) & 0xFC;

		if (i < r->nWaterVerts)
		{
			rnd = WaterTable[r->MeshEffect][rndoff & 0x3F].random;
			vtx.y += WaterTable[r->MeshEffect][((wibble >> 2) + rnd) & 0x3F].choppy;
		}

		vPos.x = vtx.x * D3DMView._11 + vtx.y * D3DMView._21 + vtx.z * D3DMView._31 + D3DMView._41;
		vPos.y = vtx.x * D3DMView._12 + vtx.y * D3DMView._22 + vtx.z * D3DMView._32 + D3DMView._42;
		vPos.z = vtx.x * D3DMView._13 + vtx.y * D3DMView._23 + vtx.z * D3DMView._33 + D3DMView._43;

		MyVertexBuffer[i].tu = vPos.x;
		MyVertexBuffer[i].tv = vPos.y;

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

			if (i >= r->nWaterVerts && camera.underwater)
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

		if (i >= r->nShoreVerts && camera.underwater)
		{
			cR = CLRR(r->prelightwater[i]);
			cG = CLRG(r->prelightwater[i]);
			cB = CLRB(r->prelightwater[i]);
		}
		else
		{
			cR = CLRR(r->prelight[i]);
			cG = CLRG(r->prelight[i]);
			cB = CLRB(r->prelight[i]);
		}

		sA = 0xFF;
		sR = 0;
		sG = 0;
		sB = 0;
		fR = 0;
		fG = 0;
		fB = 0;

		for (int j = 0; j < nRoomDynamics; j++)
		{
			l = &RoomDynamics[j];

			lPos.x = vtx.x - r->posx - l->x;
			lPos.y = vtx.y - r->posy - l->y;
			lPos.z = vtx.z - r->posz - l->z;
			val = SQUARE(lPos.x) + SQUARE(lPos.y) + SQUARE(lPos.z);

			if (val < l->sqr_falloff)
			{
				val = sqrt(val);
				val2 = l->inv_falloff * (l->falloff - val);
				lPos.x = (n.x * D3DMView._11 + n.y * D3DMView._21 + n.z * D3DMView._31) * (1.0F / val * lPos.x);
				lPos.y = (n.x * D3DMView._12 + n.y * D3DMView._22 + n.z * D3DMView._32) * (1.0F / val * lPos.y);
				lPos.z = (n.x * D3DMView._13 + n.y * D3DMView._23 + n.z * D3DMView._33) * (1.0F / val * lPos.z);
				val = val2 * (1.0F - (lPos.x + lPos.y + lPos.z));
				fR += l->r * val;
				fG += l->g * val;
				fB += l->b * val;
			}
		}

		cR += long(fR * 128.0F);
		cG += long(fG * 128.0F);
		cB += long(fB * 128.0F);

		if (i < r->nWaterVerts + r->nShoreVerts)
		{
			rndoff = long((vtx.x / 64.0F) + (vtx.y / 64.0F) + (vtx.z / 128.0F)) & 0xFC;

			rnd = WaterTable[r->MeshEffect][rndoff & 0x3C].random;
			shimmer = WaterTable[r->MeshEffect][((wibble >> 2) + rnd) & 0x3F].shimmer;
			abs = WaterTable[r->MeshEffect][((wibble >> 2) + rnd) & 0x3F].abs;
			col = (shimmer + abs) << 3;
			cR += col;
			cG += col;
			cB += col;
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

		if (sR > 255) sR = 255; else if (sR < 0) sR = 0;
		if (sG > 255) sG = 255; else if (sG < 0) sG = 0;
		if (sB > 255) sB = 255; else if (sB < 0) sB = 0;
		if (cR > 255) cR = 255; else if (cR < 0) cR = 0;
		if (cG > 255) cG = 255; else if (cG < 0) cG = 0;
		if (cB > 255) cB = 255; else if (cB < 0) cB = 0;

		MyVertexBuffer[i].color = RGBA(cR, cG, cB, 0xFF);
		MyVertexBuffer[i].specular = RGBA(sR, sG, sB, sA);
	}
}

void ProcessRoomData(ROOM_INFO* r)
{
	D3DVERTEX* vptr;
	LIGHTINFO* light;
	PCLIGHT_INFO* pclight;
	FOGBULB_STRUCT* bulb;
	D3DVERTEXBUFFERDESC vb;
	short* data_ptr;
	short* faces;
	short* prelight;
	float intensity;
	long nWaterVerts, nShoreVerts, nRestOfVerts, nLights, nBulbs;
	ushort cR, cG, cB;

	data_ptr = r->data;
	r->nVerts = *data_ptr++;

	if (!r->nVerts)
	{
		r->num_lights = 0;
		r->SourceVB = 0;
		return;
	}

	data_ptr += r->nVerts * 6;
	r->FaceData = data_ptr;
	r->gt4cnt = *data_ptr++;
	data_ptr += r->gt4cnt * 5;
	r->gt3cnt = *data_ptr;
	r->verts = (D3DVECTOR*)game_malloc(sizeof(D3DVECTOR) * r->nVerts);
	faces = (short*)malloc(2 * r->nVerts);
	prelight = (short*)malloc(2 * r->nVerts);
	data_ptr = r->data + 1;	//go to vert data
	nWaterVerts = 0;

	for (int i = 0; i < r->nVerts; i++)	//get water verts
	{
		if (data_ptr[4] & 0x2000)
		{
			r->verts[nWaterVerts].x = (float)data_ptr[0];
			r->verts[nWaterVerts].y = (float)data_ptr[1];
			r->verts[nWaterVerts].z = (float)data_ptr[2];
			prelight[nWaterVerts] = data_ptr[5];
			faces[i] = short(nWaterVerts | 0x8000);
			nWaterVerts++;
		}

		data_ptr += 6;
	}

	data_ptr = r->data + 1;
	nShoreVerts = 0;

	for (int i = 0; i < r->nVerts; i++)	//again for shore verts
	{
		if (data_ptr[4] & 0x4000 && !(data_ptr[4] & 0x2000))
		{
			r->verts[nShoreVerts + nWaterVerts].x = (float)data_ptr[0];
			r->verts[nShoreVerts + nWaterVerts].y = (float)data_ptr[1];
			r->verts[nShoreVerts + nWaterVerts].z = (float)data_ptr[2];
			prelight[nShoreVerts + nWaterVerts] = data_ptr[5];
			faces[i] = short(nShoreVerts + nWaterVerts);
			nShoreVerts++;
		}

		data_ptr += 6;
	}

	data_ptr = r->data + 1;
	nRestOfVerts = 0;

	for (int i = 0; i < r->nVerts; i++)	//one more for everything else
	{
		if (!(data_ptr[4] & 0x4000) && !(data_ptr[4] & 0x2000))
		{
			r->verts[nRestOfVerts + nShoreVerts + nWaterVerts].x = (float)data_ptr[0];
			r->verts[nRestOfVerts + nShoreVerts + nWaterVerts].y = (float)data_ptr[1];
			r->verts[nRestOfVerts + nShoreVerts + nWaterVerts].z = (float)data_ptr[2];
			prelight[nRestOfVerts + nShoreVerts + nWaterVerts] = data_ptr[5];
			faces[i] = short(nRestOfVerts + nShoreVerts + nWaterVerts);
			nRestOfVerts++;
		}

		data_ptr += 6;
	}

	data_ptr = r->FaceData + 1;
	r->nWaterVerts = nWaterVerts;
	r->nShoreVerts = nShoreVerts;

	for (int i = 0; i < r->gt4cnt; i++)	//get quad data
	{
		if (faces[data_ptr[0]] & 0x8000 || faces[data_ptr[1]] & 0x8000 || faces[data_ptr[2]] & 0x8000 || faces[data_ptr[3]] & 0x8000)
			data_ptr[4] |= 0x4000;

		data_ptr[0] = faces[data_ptr[0]] & 0x7FFF;
		data_ptr[1] = faces[data_ptr[1]] & 0x7FFF;
		data_ptr[2] = faces[data_ptr[2]] & 0x7FFF;
		data_ptr[3] = faces[data_ptr[3]] & 0x7FFF;
		data_ptr += 5;	//onto the next quad
	}

	data_ptr++;//skip over tri count

	for (int i = 0; i < r->gt3cnt; i++)	//tris
	{
		data_ptr[0] = faces[data_ptr[0]] & 0x7FFF;
		data_ptr[1] = faces[data_ptr[1]] & 0x7FFF;
		data_ptr[2] = faces[data_ptr[2]] & 0x7FFF;
		data_ptr += 4;
	}

	free(faces);
	CreateVertexNormals(r);
	r->prelight = (long*)game_malloc(4 * r->nVerts);
	r->prelightwater = (long*)game_malloc(4 * r->nVerts);
	r->watercalc = 0;
	vb.dwNumVertices = r->nVerts;
	vb.dwSize = sizeof(D3DVERTEXBUFFERDESC);
	vb.dwCaps = 0;
	vb.dwFVF = D3DFVF_VERTEX;
	DXAttempt(App.dx.lpD3D->CreateVertexBuffer(&vb, &r->SourceVB, D3DDP_DONOTCLIP, 0));
	r->SourceVB->Lock(DDLOCK_WRITEONLY, (void**)&vptr, 0);
	r->posx = (float)r->x;
	r->posy = (float)r->y;
	r->posz = (float)r->z;
	data_ptr = r->data + 1;

	for (int i = 0; i < r->nVerts; i++)
	{
		vptr->x = r->verts[i].x + (float)r->x;
		vptr->y = r->verts[i].y + (float)r->y;
		vptr->z = r->verts[i].z + (float)r->z;
		vptr->nx = r->vnormals[i].x;
		vptr->ny = r->vnormals[i].y;
		vptr->nz = r->vnormals[i].z;
		cR = ((prelight[i] & 0x7C00) >> 10) << 3;
		cG = ((prelight[i] & 0x3E0) >> 5) << 3;
		cB = (prelight[i] & 0x1F) << 3;
		r->prelight[i] = RGBA(cR, cG, cB, 0xFF);
		cR = ushort((cR * water_color_R) >> 8);
		cG = ushort((cG * water_color_G) >> 8);
		cB = ushort((cB * water_color_B) >> 8);
		r->prelightwater[i] = RGBA(cR, cG, cB, 0xFF);
		vptr++;
		data_ptr += 6;
	}

	r->SourceVB->Unlock();
	free(prelight);
	r->pclight = 0;

	if (r->num_lights)
	{
		r->pclight = (PCLIGHT_INFO*)game_malloc(sizeof(PCLIGHT_INFO) * r->num_lights);
		nLights = 0;
		nBulbs = NumLevelFogBulbs;

		for (int i = 0; i < r->num_lights; i++)
		{
			light = &r->light[i];

			if (light->Type == LIGHT_FOG)
			{
				if (NumLevelFogBulbs >= 20)
				{
					Log(1, "Fog Bulb Discarded - More Than %d", 20);
					continue;
				}

				bulb = &FogBulbs[nBulbs];
				bulb->inRange = 0;
				bulb->density = light->r;
				bulb->WorldPos.x = (float)light->x;
				bulb->WorldPos.y = (float)light->y;
				bulb->WorldPos.z = (float)light->z;
				bulb->rad = light->Outer;
				bulb->sqrad = SQUARE(bulb->rad);
				bulb->inv_sqrad = 1 / bulb->sqrad;
				nBulbs++;
				NumLevelFogBulbs = nBulbs;
			}
			else
			{
				if (!light->r && !light->g && !light->b && light->Type == LIGHT_SPOT)
					continue;

				pclight = &r->pclight[nLights];
				pclight->r = light->r * (1.0F / 255.0F);
				pclight->g = light->g * (1.0F / 255.0F);
				pclight->b = light->b * (1.0F / 255.0F);
				intensity = r->light[nLights].Intensity;

				if (intensity < 0)
					intensity = -intensity;

				intensity *= 1.0F / 8191.0F;
				pclight->r *= intensity;
				pclight->g *= intensity;
				pclight->b *= intensity;

				if (r->light[nLights].Type)
					pclight->shadow = long(intensity * 255);

				pclight->x = (float)light->x;
				pclight->y = (float)light->y;
				pclight->z = (float)light->z;
				pclight->ix = light->x;
				pclight->iy = light->y;
				pclight->iz = light->z;
				pclight->nx = -light->nx;
				pclight->ny = -light->ny;
				pclight->nz = -light->nz;
				pclight->inx = long(light->nx * -16384.0F);
				pclight->iny = long(light->ny * -16384.0F);
				pclight->inz = long(light->nz * -16384.0F);
				pclight->Inner = light->Inner;
				pclight->Outer = light->Outer;
				pclight->InnerAngle = 2 * acos(light->Inner);
				pclight->OuterAngle = 2 * acos(light->Outer);

				if (r->light[nLights].Type == LIGHT_SPOT && pclight->OuterAngle > 3.1415927F)
				{
					Log(1, "SpotLight Corrected");
					pclight->OuterAngle = 3.1415927F;
				}

				pclight->Cutoff = light->Cutoff;
				pclight->Type = light->Type;
				nLights++;
			}
		}
	}

	r->SourceVB->Optimize(App.dx._lpD3DDevice, 0);
}

void InsertRoom(ROOM_INFO* r)
{
	TEXTURESTRUCT* pTex; 
	short* data;
	short numQuads, numTris;
	bool doublesided;

	clip_left = r->left;
	clip_right = r->right;
	clip_bottom = r->bottom;
	clip_top = r->top;

	if (r->nVerts)
	{
		ProcessRoomDynamics(r);
		ProcessRoomVertices(r);

		data = r->FaceData;
		numQuads = *data++;

		for (int i = 0; i < numQuads; i++, data += 5)
		{
			pTex = &textinfo[data[4] & 0x3FFF];
			doublesided = (data[4] >> 15) & 1;

			if (!pTex->drawtype)
				AddQuadZBuffer(MyVertexBuffer, data[0], data[1], data[2], data[3], pTex, doublesided);
			else if (pTex->drawtype <= 2)
				AddQuadSorted(MyVertexBuffer, data[0], data[1], data[2], data[3], pTex, doublesided);
		}

		numTris = *data++;

		for (int i = 0; i < numTris; i++, data += 4)
		{
			pTex = &textinfo[data[3] & 0x3FFF];
			doublesided = (data[3] >> 15) & 1;

			if (!pTex->drawtype)
				AddTriZBuffer(MyVertexBuffer, data[0], data[1], data[2], pTex, doublesided);
			else if (pTex->drawtype <= 2)
				AddTriSorted(MyVertexBuffer, data[0], data[1], data[2], pTex, doublesided);
		}
	}
}

void CalcTriFaceNormal(D3DVECTOR* p1, D3DVECTOR* p2, D3DVECTOR* p3, D3DVECTOR* N)
{
	FVECTOR u, v;

	u.x = p1->x - p2->x;
	u.y = p1->y - p2->y;
	u.z = p1->z - p2->z;
	v.x = p3->x - p2->x;
	v.y = p3->y - p2->y;
	v.z = p3->z - p2->z;
	N->x = v.z * u.y - v.y * u.z;
	N->y = v.x * u.z - v.z * u.x;
	N->z = v.y * u.x - v.x * u.y;
}

void ProcessMeshData(long num_meshes)
{
	MESH_DATA* mesh;
	D3DVERTEX* vtx;
	D3DVERTEXBUFFERDESC buf;
	short* mesh_ptr;
	short* last_mesh_ptr;
	long lp;
	short c;

	Log(2, "ProcessMeshData %d", num_meshes);
	num_level_meshes = num_meshes;
	mesh_vtxbuf = (MESH_DATA**)game_malloc(4 * num_meshes);
	mesh_base = (short*)malloc_ptr;
	last_mesh_ptr = 0;
	mesh = (MESH_DATA*)num_meshes;

	for (int i = 0; i < num_meshes; i++)
	{
		mesh_ptr = meshes[i];

		if (mesh_ptr == last_mesh_ptr)
		{
			meshes[i] = (short*)mesh;
			mesh_vtxbuf[i] = mesh;
		}
		else
		{
			last_mesh_ptr = mesh_ptr;
			mesh = (MESH_DATA*)game_malloc(sizeof(MESH_DATA));
			memset(mesh, 0, sizeof(MESH_DATA));
			meshes[i] = (short*)mesh;
			mesh_vtxbuf[i] = mesh;
			mesh->x = mesh_ptr[0];
			mesh->y = mesh_ptr[1];
			mesh->z = mesh_ptr[2];
			mesh->r = mesh_ptr[3];
			mesh->flags = mesh_ptr[4];
			mesh->nVerts = mesh_ptr[5] & 0xFF;
			lp = 0;

			if (!mesh->nVerts)
				lp = mesh_ptr[5] >> 8;

			mesh_ptr += 6;

			if (mesh->nVerts)
			{
				buf.dwNumVertices = mesh->nVerts;
				buf.dwSize = sizeof(D3DVERTEXBUFFERDESC);
				buf.dwCaps = 0;
				buf.dwFVF = D3DFVF_TEX1 | D3DFVF_NORMAL | D3DFVF_XYZ;
				DXAttempt(App.dx.lpD3D->CreateVertexBuffer(&buf, &mesh->SourceVB, 0, 0));
				mesh->SourceVB->Lock(DDLOCK_WRITEONLY, (LPVOID*)&vtx, 0);

				for (int j = 0; j < mesh->nVerts; j++)
				{
					vtx[j].x = mesh_ptr[0];
					vtx[j].y = mesh_ptr[1];
					vtx[j].z = mesh_ptr[2];
					mesh_ptr += 3;
				}

				mesh->nNorms = mesh_ptr[0];
				mesh_ptr++;

				if (!mesh->nNorms)
					mesh->nNorms = mesh->nVerts;

				if (mesh->nNorms > 0)
				{
					mesh->Normals = (D3DVECTOR*)game_malloc(mesh->nNorms * sizeof(D3DVECTOR));

					for (int j = 0; j < mesh->nVerts; j++)
					{
						vtx[j].nx = mesh_ptr[0];
						vtx[j].ny = mesh_ptr[1];
						vtx[j].nz = mesh_ptr[2];
						mesh_ptr += 3;
						D3DNormalise((D3DVECTOR*)&vtx[j].nx);
						mesh->Normals[j].x = vtx[j].nx;
						mesh->Normals[j].y = vtx[j].ny;
						mesh->Normals[j].z = vtx[j].nz;
					}

					mesh->prelight = 0;
				}
				else
				{
					mesh->Normals = 0;
					mesh->prelight = (long*)game_malloc(4 * mesh->nVerts);

					for (int j = 0; j < mesh->nVerts; j++)
					{
						c = 255 - (mesh_ptr[0] >> 5);
						mesh->prelight[j] = RGBONLY(c, c, c);
						mesh_ptr++;
					}
				}

				mesh->SourceVB->Unlock();
			}
			else
				mesh_ptr += 6 * lp + 1;

			mesh->ngt4 = mesh_ptr[0];
			mesh_ptr++;

			if (mesh->ngt4)
			{
				mesh->gt4 = (short*)game_malloc(12 * mesh->ngt4);
				lp = 6 * mesh->ngt4;

				for (int j = 0; j < lp; j++)
					mesh->gt4[j] = mesh_ptr[j];

				mesh_ptr += lp;
			}

			mesh->ngt3 = mesh_ptr[0];
			mesh_ptr++;

			if (mesh->ngt3)
			{
				mesh->gt3 = (short*)game_malloc(10 * mesh->ngt3);
				lp = 5 * mesh->ngt3;

				for (int j = 0; j < lp; j++)
					mesh->gt3[j] = mesh_ptr[j];
			}
		}
	}

	Log(2, "End ProcessMeshData");
}

void InitBuckets()
{
	TEXTUREBUCKET* bucket;

	for (int i = 0; i < 20; i++)
	{
		bucket = &Bucket[i];
		bucket->tpage = -1;
		bucket->nVtx = 0;
	}
}

void DrawBucket(TEXTUREBUCKET* bucket)
{
	if (bucket->tpage == 1)
		bucket->tpage = 1;

	if (!bucket->nVtx)
		return;

	if (Textures[bucket->tpage].bump && App.BumpMapping)
	{
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, 0);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
		DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[Textures[bucket->tpage].bumptpage].tex));
		App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, bucket->vtx, bucket->nVtx, D3DDP_DONOTCLIP);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, 1);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 1);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_DESTCOLOR);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_SRCCOLOR);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		DrawPrimitiveCnt++;
	}

	DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[bucket->tpage].tex));
	App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, bucket->vtx, bucket->nVtx, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);

	if (App.BumpMapping)
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);

	bucket->nVtx = 0;
	bucket->tpage = -1;
	DrawPrimitiveCnt++;
}

void FindBucket(long tpage, D3DTLBUMPVERTEX** Vpp, long** nVtxpp)
{
	TEXTUREBUCKET* bucket;
	long nVtx, biggest;

	for (int i = 0; i < 20; i++)
	{
		bucket = &Bucket[i];

		if (bucket->tpage == tpage && bucket->nVtx < 512)
		{
			*Vpp = &bucket->vtx[bucket->nVtx];
			*nVtxpp = &bucket->nVtx;
			return;
		}

		if (bucket->nVtx > 512)
		{
			DrawBucket(bucket);
			bucket->tpage = tpage;
			bucket->nVtx = 0;
			*Vpp = bucket->vtx;
			*nVtxpp = &bucket->nVtx;
			return;
		}
	}

	nVtx = 0;
	biggest = 0;

	for (int i = 0; i < 20; i++)
	{
		bucket = &Bucket[i];

		if (bucket->tpage == -1)
		{
			bucket->tpage = tpage;
			*Vpp = bucket->vtx;
			*nVtxpp = &bucket->nVtx;
			return;
		}

		if (bucket->nVtx > nVtx)
		{
			nVtx = bucket->nVtx;
			biggest = i;
		}
	}

	bucket = &Bucket[biggest];
	DrawBucket(bucket);
	bucket->tpage = tpage;
	bucket->nVtx = 0;
	*Vpp = bucket->vtx;
	*nVtxpp = &bucket->nVtx;
}

void DrawBuckets()
{
	TEXTUREBUCKET* bucket;

	if (App.BumpMapping)
	{
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, 0);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

		for (int i = 0; i < 20; i++)
		{
			bucket = &Bucket[i];

			if (Textures[bucket->tpage].bump && bucket->nVtx)
			{
				DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[Textures[bucket->tpage].bumptpage].tex));
				App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, bucket->vtx, bucket->nVtx, D3DDP_DONOTCLIP);
				DrawPrimitiveCnt++;
			}
		}

		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, 1);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 1);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_DESTCOLOR);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_SRCCOLOR);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		for (int i = 0; i < 20; i++)
		{
			bucket = &Bucket[i];

			if (Textures[bucket->tpage].bump && bucket->nVtx)
			{
				DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[bucket->tpage].tex));
				App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, bucket->vtx, bucket->nVtx, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
				bucket->nVtx = 0;
				bucket->tpage = -1;
				DrawPrimitiveCnt++;
			}
		}

		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);

		for (int i = 0; i < 20; i++)
		{
			bucket = &Bucket[i];

			if (!Textures[bucket->tpage].bump && bucket->nVtx)
			{
				DXAttempt(App.dx.lpD3DDevice->SetTexture(0, Textures[bucket->tpage].tex));
				App.dx.lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, FVF, bucket->vtx, bucket->nVtx, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
				bucket->nVtx = 0;
				bucket->tpage = -1;
				DrawPrimitiveCnt++;
			}
		}
	}
	else
	{
		for (int i = 0; i < 20; i++)
		{
			bucket = &Bucket[i];
			DrawBucket(bucket);
		}
	}
}

void CreateVertexNormals(ROOM_INFO* r)
{
	D3DVECTOR p1;
	D3DVECTOR p2;
	D3DVECTOR p3;
	D3DVECTOR n1;
	D3DVECTOR n2;
	short* data;
	short nQuads;
	short nTris;

	data = r->FaceData;
	r->fnormals = (D3DVECTOR*)game_malloc(sizeof(D3DVECTOR) * (r->gt3cnt + r->gt4cnt));
	nQuads = *data++;

	for (int i = 0; i < nQuads; i++)
	{
		p1 = r->verts[data[0]];
		p2 = r->verts[data[1]];
		p3 = r->verts[data[2]];
		CalcTriFaceNormal(&p1, &p2, &p3, &n1);

		p1 = r->verts[data[0]];
		p2 = r->verts[data[2]];
		p3 = r->verts[data[3]];
		CalcTriFaceNormal(&p1, &p2, &p3, &n2);

		n1.x += n2.x;
		n1.y += n2.y;
		n1.z += n2.z;
		D3DNormalise(&n1);
		r->fnormals[i] = n1;
		data += 5;
	}

	nTris = *data++;

	for (int i = 0; i < nTris; i++)
	{
		p1 = r->verts[data[0]];
		p2 = r->verts[data[1]];
		p3 = r->verts[data[2]];
		CalcTriFaceNormal(&p1, &p2, &p3, &n1);
		D3DNormalise(&n1);
		r->fnormals[nQuads + i] = n1;
		data += 4;
	}

	r->vnormals = (D3DVECTOR*)game_malloc(sizeof(D3DVECTOR) * r->nVerts);

	data = r->FaceData;
	nQuads = *data++;

	data += nQuads * 5;
	nTris = *data;

	for (int i = 0; i < r->nVerts; i++)
	{
		n1.x = 0;
		n1.y = 0;
		n1.z = 0;

		data = r->FaceData + 1;

		for (int j = 0; j < nQuads; j++)
		{
			if (data[0] == i || data[1] == i || data[2] == i || data[3] == i)
			{
				n1.x += r->fnormals[j].x;
				n1.y += r->fnormals[j].y;
				n1.z += r->fnormals[j].z;
			}

			data += 5;
		}

		data++;

		for (int j = 0; j < nTris; j++)
		{
			if (data[0] == i || data[1] == i || data[2] == i)
			{
				n1.x += r->fnormals[nQuads + j].x;
				n1.y += r->fnormals[nQuads + j].y;
				n1.z += r->fnormals[nQuads + j].z;
			}

			data += 4;
		}

		D3DNormalise(&n1);
		r->vnormals[i] = n1;
	}
}
