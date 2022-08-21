#include "../tomb4/pch.h"
#include "lighting.h"
#include "dxshell.h"
#include "3dmath.h"
#include "function_stubs.h"
#include "d3dmatrix.h"
#include "../game/objects.h"
#include "../game/control.h"
#include "../game/delstuff.h"
#include "../game/effect2.h"
#include "../game/camera.h"
#include "function_table.h"
#include "../game/draw.h"
#include "winmain.h"
#include "../game/lara.h"

ITEM_INFO* current_item;
long StaticMeshShade;
long MaxRoomLights;

static D3DLIGHT_STRUCT* D3DDynamics;
static D3DLIGHT_STRUCT* D3DLights;
static ITEM_INFO StaticMeshLightItem;
static long SetupLight_thing;

#pragma warning(push)
#pragma warning(disable : 4244)
void S_CalculateStaticMeshLight(long x, long y, long z, long shade, ROOM_INFO* r)
{
	StaticMeshLightItem.il.ambient = r->ambient;
	StaticMeshLightItem.il.item_pos.x = x;
	StaticMeshLightItem.il.item_pos.y = y;
	StaticMeshLightItem.il.item_pos.z = z;
	StaticMeshLightItem.room_number = current_room;
	StaticMeshShade = shade;
	current_item = &StaticMeshLightItem;
}

void InitItemDynamicLighting(ITEM_INFO* item)
{
	DYNAMIC* dptr;
	PHD_VECTOR d;
	PHD_VECTOR vec;
	FVECTOR l;
	long last_off;

	last_off = -1;

	for (int i = 0; i < MAX_DYNAMICS; i++)
	{
		dptr = &dynamics[i];

		if (dptr->on)
		{
			d.x = dptr->x - item->il.item_pos.x;
			d.y = dptr->y - item->il.item_pos.y;
			d.z = dptr->z - item->il.item_pos.z;
			ApplyMatrix(w2v_matrix, &d, &vec);
			ApplyTransposeMatrix(phd_mxptr, &vec, &d);
			D3DDynamics[i].D3DLightx.dvPosition.x = d.x;
			D3DDynamics[i].D3DLightx.dvPosition.y = d.y;
			D3DDynamics[i].D3DLightx.dvPosition.z = d.z;
			D3DDynamics[i].D3DLightx.dwFlags = D3DLIGHT_ALL;
			D3DDynamics[i].D3DLightx.dcvColor.r = dptr->r / 255.0F;
			D3DDynamics[i].D3DLightx.dcvColor.g = dptr->g / 255.0F;
			D3DDynamics[i].D3DLightx.dcvColor.b = dptr->b / 255.0F;
			D3DDynamics[i].D3DLightx.dltType = D3DLIGHT_POINT;
			D3DDynamics[i].D3DLightx.dvAttenuation1 = 1;
			D3DDynamics[i].D3DLightx.dvRange = (dptr->falloff >> 1) + (dptr->falloff >> 3);
			D3DDynamics[i].D3DLight->SetLight((LPD3DLIGHT)&D3DDynamics[i].D3DLightx);
		}
		else 
		{
			if (D3DDynamics[i].D3DLightx.dwFlags & D3DLIGHT_ACTIVE)	//active but shouldn't be
			{
				D3DDynamics[i].D3DLightx.dwFlags = D3DLIGHT_NO_SPECULAR;
				D3DDynamics[i].D3DLightx.dltType = D3DLIGHT_POINT;
				D3DDynamics[i].D3DLight->SetLight((LPD3DLIGHT)&D3DDynamics[i].D3DLightx);
			}

			last_off = i;
		}
	}

	if (last_off >= 0 && bLaraTorch)
	{
		d.x = LaraTorchEnd.x - LaraTorchStart.x;
		d.y = LaraTorchEnd.y - LaraTorchStart.y;
		d.z = LaraTorchEnd.z - LaraTorchStart.z;
		ApplyMatrix(w2v_matrix, &d, &vec);
		ApplyTransposeMatrix(phd_mxptr, &vec, &d);
		l.x = d.x;
		l.y = d.y;
		l.z = d.z;
		d.x = LaraTorchStart.x - item->il.item_pos.x;
		d.y = LaraTorchStart.y - item->il.item_pos.y;
		d.z = LaraTorchStart.z - item->il.item_pos.z;
		ApplyMatrix(w2v_matrix, &d, &vec);
		ApplyTransposeMatrix(phd_mxptr, &vec, &d);
		D3DDynamics[last_off].D3DLightx.dcvColor.r = LaraTorchIntensity / 255.0F;
		D3DDynamics[last_off].D3DLightx.dcvColor.g = LaraTorchIntensity / 255.0F;
		D3DDynamics[last_off].D3DLightx.dcvColor.b = 0;
		D3DDynamics[last_off].D3DLightx.dvPosition.x = d.x;
		D3DDynamics[last_off].D3DLightx.dvPosition.y = d.y;
		D3DDynamics[last_off].D3DLightx.dvPosition.z = d.z;
		D3DDynamics[last_off].D3DLightx.dvDirection.x = l.x;
		D3DDynamics[last_off].D3DLightx.dvDirection.y = l.y;
		D3DDynamics[last_off].D3DLightx.dvDirection.z = l.z;
		D3DDynamics[last_off].D3DLightx.dwFlags = D3DLIGHT_ALL;
		D3DDynamics[last_off].D3DLightx.dltType = D3DLIGHT_SPOT;
		D3DDynamics[last_off].D3DLightx.dvAttenuation1 = 1;
		D3DDynamics[last_off].D3DLightx.dvFalloff = 1;
		D3DDynamics[last_off].D3DLightx.dvRange = 20480;
		D3DDynamics[last_off].D3DLightx.dvTheta = 0.25F;
		D3DDynamics[last_off].D3DLightx.dvPhi = 1.25F;
		D3DDynamics[last_off].D3DLight->SetLight((LPD3DLIGHT)&D3DDynamics[last_off].D3DLightx);
	}
}

void InitDynamicLighting()
{
	DYNAMIC* dptr;
	long last_off;

	last_off = -1;
	ClearObjectLighting();
	App.dx.lpD3DDevice->SetLightState(D3DLIGHTSTATE_AMBIENT, 0);

	for (int i = 0; i < MAX_DYNAMICS; i++)
	{
		dptr = &dynamics[i];

		if (dptr->on)
		{
			D3DDynamics[i].D3DLightx.dcvColor.r = dptr->r / 255.0F;
			D3DDynamics[i].D3DLightx.dcvColor.g = dptr->g / 255.0F;
			D3DDynamics[i].D3DLightx.dcvColor.b = dptr->b / 255.0F;
			D3DDynamics[i].D3DLightx.dvPosition.x = dptr->x;
			D3DDynamics[i].D3DLightx.dvPosition.y = dptr->y;
			D3DDynamics[i].D3DLightx.dvPosition.z = dptr->z;
			D3DDynamics[i].D3DLightx.dwFlags = D3DLIGHT_ALL;
			D3DDynamics[i].D3DLightx.dltType = D3DLIGHT_POINT;
			D3DDynamics[i].D3DLightx.dvAttenuation1 = 1;
			D3DDynamics[i].D3DLightx.dvRange = (dptr->falloff >> 1) + (dptr->falloff >> 3);
			D3DDynamics[i].D3DLight->SetLight((LPD3DLIGHT)&D3DDynamics[i].D3DLightx);
		}
		else
		{
			if (D3DDynamics[i].D3DLightx.dwFlags & D3DLIGHT_ACTIVE)	//active but shouldn't be
			{
				D3DDynamics[i].D3DLightx.dwFlags = D3DLIGHT_NO_SPECULAR;
				D3DDynamics[i].D3DLightx.dltType = D3DLIGHT_POINT;
				D3DDynamics[i].D3DLight->SetLight((LPD3DLIGHT)&D3DDynamics[i].D3DLightx);
			}

			last_off = i;
		}
	}

	if (last_off >= 0 && bLaraTorch)
	{
		D3DDynamics[last_off].D3DLightx.dcvColor.r = LaraTorchIntensity / 255.0F;
		D3DDynamics[last_off].D3DLightx.dcvColor.g = LaraTorchIntensity / 255.0F;
		D3DDynamics[last_off].D3DLightx.dcvColor.b = 0;
		D3DDynamics[last_off].D3DLightx.dvPosition.x = LaraTorchStart.x;
		D3DDynamics[last_off].D3DLightx.dvPosition.y = LaraTorchStart.y;
		D3DDynamics[last_off].D3DLightx.dvPosition.z = LaraTorchStart.z;
		D3DDynamics[last_off].D3DLightx.dvDirection.x = LaraTorchEnd.x - LaraTorchStart.x;
		D3DDynamics[last_off].D3DLightx.dvDirection.y = LaraTorchEnd.y - LaraTorchStart.y;
		D3DDynamics[last_off].D3DLightx.dvDirection.z = LaraTorchEnd.z - LaraTorchStart.z;
		D3DDynamics[last_off].D3DLightx.dwFlags = D3DLIGHT_ALL;
		D3DDynamics[last_off].D3DLightx.dltType = D3DLIGHT_SPOT;
		D3DDynamics[last_off].D3DLightx.dvAttenuation1 = 1;
		D3DDynamics[last_off].D3DLightx.dvFalloff = 1;
		D3DDynamics[last_off].D3DLightx.dvRange = 20480;
		D3DDynamics[last_off].D3DLightx.dvTheta = 0.25F;
		D3DDynamics[last_off].D3DLightx.dvPhi = 1.25F;
		D3DDynamics[last_off].D3DLight->SetLight((LPD3DLIGHT)&D3DDynamics[last_off].D3DLightx);
	}
}

void SetupLight(D3DLIGHT_STRUCT* d3dlight, PCLIGHT* light, long* ambient)
{
	PHD_VECTOR d;
	PHD_VECTOR vec;
	FVECTOR l;
	float fVal;
	long r, g, b, val, val2;;

	d = light->rlp;
	d3dlight->D3DLightx.dcvColor.r = light->r;
	d3dlight->D3DLightx.dcvColor.g = light->g;
	d3dlight->D3DLightx.dcvColor.b = light->b;
	ApplyMatrix(w2v_matrix, &d, &vec);
	ApplyTransposeMatrix(phd_mxptr, &vec, &d);
	d3dlight->D3DLightx.dvPosition.x = d.x;
	d3dlight->D3DLightx.dvPosition.y = d.y;
	d3dlight->D3DLightx.dvPosition.z = d.z;

	if (light->Type == LIGHT_SUN || light->Type == LIGHT_SPOT)
	{
		d.x = light->inx;
		d.y = light->iny;
		d.z = light->inz;
		ApplyMatrix(w2v_matrix, &d, &vec);
		ApplyTransposeMatrix(phd_mxptr, &vec, &d);
		l.x = d.x;
		l.y = d.y;
		l.z = d.z;
	}

	d3dlight->D3DLightx.dwFlags = D3DLIGHT_ALL;

	switch (light->Type)
	{
	case LIGHT_SUN:
		d3dlight->D3DLightx.dltType = D3DLIGHT_DIRECTIONAL;
		d3dlight->D3DLightx.dvDirection.x = l.x;
		d3dlight->D3DLightx.dvDirection.y = l.y;
		d3dlight->D3DLightx.dvDirection.z = l.z;
		break;

	case LIGHT_POINT:
		d3dlight->D3DLightx.dltType = D3DLIGHT_POINT;
		d3dlight->D3DLightx.dvAttenuation1 = 2;
		d3dlight->D3DLightx.dvRange = light->Outer;

		if (SetupLight_thing)
		{
			fVal = (light->Outer - phd_sqrt(light->Range)) / light->Outer;

			if (fVal < 1)
			{
				r = CLRR(*ambient) + (fVal * light->r * 255);
				g = CLRG(*ambient) + (fVal * light->g * 255);
				b = CLRB(*ambient) + (fVal * light->b * 255);

				if (r > 255)
					r = 255;

				if (g > 255)
					g = 255;

				if (b > 255)
					b = 255;

				*ambient = RGBONLY(r, g, b);
			}
		}

		break;

	case LIGHT_SPOT:
		d3dlight->D3DLightx.dltType = D3DLIGHT_SPOT;
		d3dlight->D3DLightx.dvDirection.x = l.x;
		d3dlight->D3DLightx.dvDirection.y = l.y;
		d3dlight->D3DLightx.dvDirection.z = l.z;
		d3dlight->D3DLightx.dvFalloff = 1;

		if (SetupLight_thing)
			d3dlight->D3DLightx.dvAttenuation1 = 2;
		else
			d3dlight->D3DLightx.dvAttenuation1 = 1;
		
		d3dlight->D3DLightx.dvRange = light->Cutoff;
		d3dlight->D3DLightx.dvTheta = light->OuterAngle;
		d3dlight->D3DLightx.dvPhi = light->OuterAngle;
		break;

	case LIGHT_SHADOW:
		r = CLRR(*ambient);
		g = CLRG(*ambient);
		b = CLRB(*ambient);
		val = phd_sqrt(light->Range);
		val2 = light->shadow >> 3;

		if (val >= light->Inner)
			val2 = (val - light->Outer) / ((light->Outer - light->Inner) / -val2);

		if (val2 < 0)
			val2 = 0;

		r -= val2;
		g -= val2;
		b -= val2;

		if (r < 0)
			r = 0;

		if (g < 0)
			g = 0;

		if (b < 0)
			b = 0;

		*ambient = RGBONLY(r, g, b);
		d3dlight->D3DLightx.dwFlags = D3DLIGHT_NO_SPECULAR;
		d3dlight->D3DLightx.dltType = D3DLIGHT_POINT;
		break;
	}

	DXAttempt(d3dlight->D3DLight->SetLight((LPD3DLIGHT)&d3dlight->D3DLightx));
}

void ClearDynamicLighting()
{
	App.dx.lpD3DDevice->SetLightState(D3DLIGHTSTATE_AMBIENT, 0);

	for (int i = 0; i < MAX_DYNAMICS; i++)
	{
		if (D3DDynamics[i].D3DLightx.dwFlags & D3DLIGHT_ACTIVE)
		{
			D3DDynamics[i].D3DLightx.dwFlags = D3DLIGHT_NO_SPECULAR;
			D3DDynamics[i].D3DLightx.dltType = D3DLIGHT_POINT;
			D3DDynamics[i].D3DLight->SetLight((LPD3DLIGHT)&D3DDynamics[i].D3DLightx);
		}
	}
}

void ApplyMatrix(long* matrix, PHD_VECTOR* start, PHD_VECTOR* dest)
{
	dest->x = (start->x * matrix[M00] + start->y * matrix[M01] + start->z * matrix[M02]) >> W2V_SHIFT;
	dest->y = (start->x * matrix[M10] + start->y * matrix[M11] + start->z * matrix[M12]) >> W2V_SHIFT;
	dest->z = (start->x * matrix[M20] + start->y * matrix[M21] + start->z * matrix[M22]) >> W2V_SHIFT;
}

void ApplyTransposeMatrix(long* matrix, PHD_VECTOR* start, PHD_VECTOR* dest)
{
	dest->x = (start->x * matrix[M00] + start->y * matrix[M10] + start->z * matrix[M20]) >> W2V_SHIFT;
	dest->y = (start->x * matrix[M01] + start->y * matrix[M11] + start->z * matrix[M21]) >> W2V_SHIFT;
	dest->z = (start->x * matrix[M02] + start->y * matrix[M12] + start->z * matrix[M22]) >> W2V_SHIFT;
}

void MallocD3DLights()
{
	if (MaxRoomLights > 21)
		Log(1, "MAX Room Lights of %d Exceeded - %d", 21, MaxRoomLights);

	MaxRoomLights *= 2;
	D3DLights = (D3DLIGHT_STRUCT*)game_malloc(sizeof(D3DLIGHT_STRUCT) * MaxRoomLights);
	D3DDynamics = (D3DLIGHT_STRUCT*)game_malloc(sizeof(D3DLIGHT_STRUCT) * MAX_DYNAMICS);
}

void CreateD3DLights()
{
	D3DMATERIAL material;

	Log(2, "CreateD3DLights");
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_FOGCOLOR, CurrentFog);

	for (int i = 0; i < MaxRoomLights; i++)
	{
		DXAttempt(App.dx.lpD3D->CreateLight(&D3DLights[i].D3DLight, 0));
		memset(&D3DLights[i].D3DLightx, 0, sizeof(D3DLights[i].D3DLightx));
		D3DLights[i].D3DLightx.dwSize = sizeof(D3DLights[i].D3DLightx);
		D3DLights[i].D3DLightx.dltType = D3DLIGHT_POINT;
		D3DLights[i].D3DLightx.dwFlags = D3DLIGHT_NO_SPECULAR;
		DXAttempt(D3DLights[i].D3DLight->SetLight((LPD3DLIGHT)&D3DLights[i].D3DLightx));
		DXAttempt(App.dx.lpViewport->AddLight(D3DLights[i].D3DLight));
	}

	if (D3DDynamics)
	{
		for (int i = 0; i < MAX_DYNAMICS; i++)
		{
			DXAttempt(App.dx.lpD3D->CreateLight(&D3DDynamics[i].D3DLight, 0));
			memset(&D3DDynamics[i].D3DLightx, 0, sizeof(D3DDynamics[i].D3DLightx));
			D3DDynamics[i].D3DLightx.dwSize = sizeof(D3DDynamics[i].D3DLightx);
			D3DDynamics[i].D3DLightx.dltType = D3DLIGHT_POINT;
			D3DDynamics[i].D3DLightx.dwFlags = D3DLIGHT_NO_SPECULAR;
			DXAttempt(D3DDynamics[i].D3DLight->SetLight((LPD3DLIGHT)&D3DDynamics[i].D3DLightx));
			DXAttempt(App.dx.lpViewport->AddLight(D3DDynamics[i].D3DLight));
		}

		DXAttempt(App.dx.lpD3D->CreateMaterial(&App.GlobalMaterial, 0));
		memset(&material, 0, sizeof(material));
		material.dwSize = sizeof(material);
		material.ambient.a = 0;
		material.diffuse.a = 0;
		material.ambient.r = 1;
		material.ambient.g = 1;
		material.ambient.b = 1;
		material.diffuse.r = 1;
		material.diffuse.g = 1;
		material.diffuse.b = 1;
		DXAttempt(App.GlobalMaterial->SetMaterial(&material));
		DXAttempt(App.GlobalMaterial->GetHandle(App.dx._lpD3DDevice, &App.GlobalMaterialHandle));
		DXAttempt(App.dx.lpD3DDevice->SetLightState(D3DLIGHTSTATE_MATERIAL, App.GlobalMaterialHandle));
	}
}

void FreeD3DLights()
{
	if (MaxRoomLights)
	{
		for (int i = 0; i < MaxRoomLights; i++)
		{
			App.dx.lpViewport->DeleteLight(D3DLights[i].D3DLight);

			if (D3DLights[i].D3DLight)
			{
				Log(4, "Released %s @ %x - RefCnt = %d", "Room Light", D3DLights[i].D3DLight, D3DLights[i].D3DLight->Release());
				D3DLights[i].D3DLight = 0;
			}
			else
				Log(1, "%s Attempt To Release NULL Ptr", "Room Light");
		}
	}

	if (D3DDynamics)
	{
		for (int i = 0; i < MAX_DYNAMICS; i++)
		{
			DXAttempt(App.dx.lpViewport->DeleteLight(D3DDynamics[i].D3DLight));

			if (D3DDynamics[i].D3DLight)
			{
				Log(4, "Released %s @ %x - RefCnt = %d", "Dynamic Light", D3DDynamics[i].D3DLight, D3DDynamics[i].D3DLight->Release());
				D3DDynamics[i].D3DLight = 0;
			}
			else
				Log(1, "%s Attempt To Release NULL Ptr", "Dynamic Light");
		}
	}

	if (App.GlobalMaterial)
	{
		Log(4, "Released %s @ %x - RefCnt = %d", "Global Material", App.GlobalMaterial, App.GlobalMaterial->Release());
		App.GlobalMaterial = 0;
	}
	else
		Log(1, "%s Attempt To Release NULL Ptr", "Global Material");
}

void CreateLightList(ITEM_INFO* item)
{
	ROOM_INFO* r;
	PCLIGHT* current_lights;
	PCLIGHT* prev_lights;
	PCLIGHT_INFO* room_light;
	D3DVECTOR vec;
	void* bakPtr;
	long bakNum, dx, dy, dz, range;
	bool in_range;

	r = &room[item->room_number];

	if (item->il.room_number != item->room_number)
	{
		bakPtr = item->il.pCurrentLights;
		bakNum = item->il.nCurrentLights;
		item->il.pCurrentLights = item->il.pPrevLights;
		item->il.nCurrentLights = item->il.nPrevLights;
		item->il.pPrevLights = bakPtr;
		item->il.nPrevLights = bakNum;
		item->il.room_number = item->room_number;
		prev_lights = (PCLIGHT*)item->il.pPrevLights;

		for (int i = 0; i < item->il.nPrevLights; i++)
		{
			if (prev_lights[i].Active)
			{
				if (prev_lights[i].Type == LIGHT_SHADOW)
					prev_lights[i].iny = -prev_lights[i].shadow >> 3;
				else
				{
					prev_lights[i].rs = prev_lights[i].r * -0.125F;
					prev_lights[i].gs = prev_lights[i].g * -0.125F;
					prev_lights[i].bs = prev_lights[i].b * -0.125F;
					prev_lights[i].tr = 0;
					prev_lights[i].tg = 0;
					prev_lights[i].tb = 0;
				}

				prev_lights[i].fcnt = 8;
			}
		}

		item->il.nCurrentLights = r->num_lights;
		current_lights = (PCLIGHT*)item->il.pCurrentLights;
		room_light = r->pclight;

		for (int i = 0; i < r->num_lights; i++)
		{
			current_lights->r = room_light->r;
			current_lights->g = room_light->g;
			current_lights->b = room_light->b;
			current_lights->tr = room_light->r;
			current_lights->tg = room_light->g;
			current_lights->tb = room_light->b;
			current_lights->x = room_light->x;
			current_lights->y = room_light->y;
			current_lights->z = room_light->z;
			current_lights->shadow = room_light->shadow << 3;
			current_lights->Inner = room_light->Inner;
			current_lights->Outer = room_light->Outer;
			current_lights->InnerAngle = room_light->InnerAngle;
			current_lights->OuterAngle = room_light->OuterAngle;
			current_lights->Cutoff = room_light->Cutoff;
			current_lights->inx = room_light->inx;
			current_lights->iny = room_light->iny;
			current_lights->inz = room_light->inz;
			current_lights->ix = room_light->ix;
			current_lights->iy = room_light->iy;
			current_lights->iz = room_light->iz;
			current_lights->nx = room_light->nx;
			current_lights->ny = room_light->ny;
			current_lights->nz = room_light->nz;
			current_lights->Type = room_light->Type;
			current_lights->Active = 0;

			if (room_light->Type == LIGHT_SHADOW)
				current_lights->inx = room_light->shadow;

			current_lights++;
			room_light++;
		}
	}

	current_lights = (PCLIGHT*)item->il.pCurrentLights;
	prev_lights = (PCLIGHT*)item->il.pPrevLights;

	for (int i = 0; i < item->il.nCurrentLights; i++)
	{
		in_range = 1;
		dx = current_lights[i].ix - item->il.item_pos.x;
		dy = current_lights[i].iy - item->il.item_pos.y;
		dz = current_lights[i].iz - item->il.item_pos.z;
		range = SQUARE(dx) + SQUARE(dy) + SQUARE(dz);

		if (current_lights[i].Type == LIGHT_POINT || current_lights[i].Type == LIGHT_SHADOW)
		{
			range = SQUARE(dx) + SQUARE(dy) + SQUARE(dz);

			if (range > SQUARE(current_lights[i].Outer))
				in_range = 0;
		}
		else if (current_lights[i].Type == LIGHT_SPOT)
		{
			range = SQUARE(dx) + SQUARE(dy) + SQUARE(dz);

			if (range > SQUARE(current_lights[i].Cutoff))
				in_range = 0;
			else
			{
				vec.x = -dx;
				vec.y = -dy;
				vec.z = -dz;
				D3DNormalise(&vec);

				if (current_lights[i].nx * vec.x + current_lights[i].ny * vec.y + current_lights[i].nz * vec.z <= current_lights[i].Outer)
					in_range = 0;
			}
		}

		current_lights[i].rlp.x = dx;
		current_lights[i].rlp.y = dy;
		current_lights[i].rlp.z = dz;
		current_lights[i].Range = range;

		if (in_range)
		{
			if (!current_lights[i].Active)
			{
				if (current_lights[i].Type == LIGHT_SHADOW)
				{
					current_lights[i].Active = 1;
					current_lights[i].iny = current_lights[i].inx;
					current_lights[i].shadow = 0;
				}
				else
				{
					current_lights[i].rs = current_lights[i].tr * 0.125F;
					current_lights[i].gs = current_lights[i].tg * 0.125F;
					current_lights[i].bs = current_lights[i].tb * 0.125F;
					current_lights[i].r = 0;
					current_lights[i].g = 0;
					current_lights[i].b = 0;
					current_lights[i].Active = 1;
				}

				current_lights[i].fcnt = 8;
			}
		}
		else
		{
			if (current_lights[i].Active && !current_lights[i].fcnt)
			{
				if (current_lights[i].Type == LIGHT_SHADOW)
					current_lights[i].iny = -current_lights[i].shadow >> 3;
				else
				{
					current_lights[i].rs = current_lights[i].r * -0.125F;
					current_lights[i].gs = current_lights[i].g * -0.125F;
					current_lights[i].bs = current_lights[i].b * -0.125F;
				}

				current_lights[i].fcnt = 8;
			}
		}
	}

	FadeLightList(current_lights, item->il.nCurrentLights);
	FadeLightList(prev_lights, item->il.nPrevLights);
}

void FadeLightList(PCLIGHT* lights, long nLights)
{
	for (int i = 0; i < nLights; i++)
	{
		if (lights[i].Active && lights[i].fcnt)
		{
			if (lights[i].Type == LIGHT_SHADOW)
				lights[i].shadow += lights[i].iny;
			else
			{
				lights[i].r += lights[i].rs;
				lights[i].g += lights[i].gs;
				lights[i].b += lights[i].bs;
			}

			lights[i].fcnt--;

			if (lights[i].Type == LIGHT_SHADOW)
			{
				if (lights[i].shadow <= 0)
					lights[i].Active = 0;
			}
			else
			{
				if (lights[i].r <= 0 && lights[i].g <= 0 && lights[i].b <= 0)
					lights[i].Active = 0;
			}
		}
	}
}

void InitObjectLighting(ITEM_INFO* item)
{
	PCLIGHT* light;
	long node_ambient, num_active, r, g, b;

	num_active = 0;
	node_ambient = item->il.ambient;
	SetupLight_thing = item->object_number >= GAME_PIECE1;
	light = (PCLIGHT*)item->il.pCurrentLights;

	for (int i = 0; i < item->il.nCurrentLights; i++)
	{
		if (light[i].Active)
		{
			SetupLight(&D3DLights[num_active], &light[i], &node_ambient);
			num_active++;
		}
	}

	light = (PCLIGHT*)item->il.pPrevLights;

	for (int i = 0; i < item->il.nPrevLights; i++)
	{
		if (light[i].Active)
		{
			SetupLight(&D3DLights[num_active], &light[i], &node_ambient);
			num_active++;
		}
	}

	for (int i = num_active; i < MaxRoomLights; i++)
	{
		if (D3DLights[i].D3DLightx.dwFlags & D3DLIGHT_ACTIVE)
		{
			D3DLights[i].D3DLightx.dwFlags = D3DLIGHT_NO_SPECULAR;
			D3DLights[i].D3DLightx.dltType = D3DLIGHT_POINT;
			D3DLights[i].D3DLight->SetLight((LPD3DLIGHT)&D3DLights[i].D3DLightx);
		}
	}

	InitItemDynamicLighting(item);

	if (item == lara_item && bLaraInWater)
	{
		if (bLaraUnderWater < 0)
		{
			node_ambient = LaraNodeAmbient[0];
			item->il.fcnt = 0;
			item->il.ambient = node_ambient;
		}
		else
			node_ambient = LaraNodeAmbient[1];
	}

	if (item->object_number == BURNING_FLOOR && item->required_anim_state)
	{
		r = (CLRR(node_ambient) * (item->required_anim_state << 1)) >> 8;
		g = (CLRG(node_ambient) * (item->required_anim_state << 1)) >> 8;
		b = (CLRB(node_ambient) * (item->required_anim_state << 1)) >> 8;
		node_ambient = RGBA(r, g, b, 0xFF);
	}

	App.dx.lpD3DDevice->SetLightState(D3DLIGHTSTATE_AMBIENT, node_ambient);
}

void ClearObjectLighting()
{
	App.dx.lpD3DDevice->SetLightState(D3DLIGHTSTATE_AMBIENT, 0);

	for (int i = 0; i < MaxRoomLights; i++)
	{
		if (D3DLights[i].D3DLightx.dwFlags & D3DLIGHT_ACTIVE)
		{
			D3DLights[i].D3DLightx.dwFlags = D3DLIGHT_NO_SPECULAR;
			D3DLights[i].D3DLightx.dltType = D3DLIGHT_POINT;
			D3DLights[i].D3DLight->SetLight((LPD3DLIGHT)&D3DLights[i].D3DLightx);
		}
	}
}

void CalcAmbientLight(ITEM_INFO* item)
{
	ROOM_INFO* r;
	short room_number;

	room_number = item->room_number;
	GetFloor(item->il.item_pos.x, item->il.item_pos.y, item->il.item_pos.z, &room_number);
	r = &room[room_number];

	if (item->il.ambient != r->ambient)
	{
		if (item->il.fcnt == -1)
		{
			item->il.ambient = r->ambient;
			item->il.fcnt = 0;
		}
		else
		{
			if (!item->il.fcnt)
			{
				item->il.r = CLRR(item->il.ambient);
				item->il.g = CLRG(item->il.ambient);
				item->il.b = CLRB(item->il.ambient);
				item->il.rs = CLRR(r->ambient) - item->il.r;
				item->il.gs = CLRG(r->ambient) - item->il.g;
				item->il.bs = CLRB(r->ambient) - item->il.b;
				item->il.r <<= 3;
				item->il.g <<= 3;
				item->il.b <<= 3;
				item->il.fcnt = 8;
			}

			if (item->il.fcnt)
			{
				item->il.r += item->il.rs;
				item->il.g += item->il.gs;
				item->il.b += item->il.bs;
				item->il.ambient = RGBONLY(item->il.r >> 3, item->il.g >> 3, item->il.b >> 3);
				item->il.fcnt--;
			}
		}
	}
}
#pragma warning(pop)
