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
long ambientR, ambientG, ambientB;

FVECTOR lGlobalMeshPos;
SUNLIGHT_STRUCT SunLights[64];
POINTLIGHT_STRUCT PointLights[64];
POINTLIGHT_STRUCT SpotLights[64];
long nSunLights, nPointLights, nSpotLights, nShadowLights, nTotalLights;

static ITEM_INFO StaticMeshLightItem;
static long SetupLight_thing;

void S_CalculateStaticMeshLight(long x, long y, long z, long shade, ROOM_INFO* r)
{
	StaticMeshLightItem.il.ambient = r->ambient;
	StaticMeshLightItem.il.item_pos.x = x;
	StaticMeshLightItem.il.item_pos.y = y;
	StaticMeshLightItem.il.item_pos.z = z;
	StaticMeshLightItem.room_number = (short)current_room;
	StaticMeshShade = shade;
	current_item = &StaticMeshLightItem;
}

void InitItemDynamicLighting(ITEM_INFO* item)
{
	DYNAMIC* dptr;

	for (int i = 0; i < MAX_DYNAMICS; i++)
	{
		dptr = &dynamics[i];

		if (dptr->on)
			SetupDynamicLight(dptr, item);
	}

	ambientR = CLRR(item->il.ambient);
	ambientG = CLRG(item->il.ambient);
	ambientB = CLRB(item->il.ambient);
}

void SetupDynamicLight(DYNAMIC* light, ITEM_INFO* item)
{
	POINTLIGHT_STRUCT* point;
	float x, y, z, falloff, dist, val;

	x = light->x - lGlobalMeshPos.x;
	y = light->y - lGlobalMeshPos.y;
	z = light->z - lGlobalMeshPos.z;
	falloff = float((light->falloff >> 1) + (light->falloff >> 3));
	dist = sqrt(SQUARE(z) + SQUARE(y) + SQUARE(x));
	point = &PointLights[nPointLights];

	if (dist <= falloff)
	{
		val = 1.0F / dist;
		point->vec.x = val * (x * D3DLightMatrix._11 + y * D3DLightMatrix._12 + z * D3DLightMatrix._13);
		point->vec.y = val * (x * D3DLightMatrix._21 + y * D3DLightMatrix._22 + z * D3DLightMatrix._23);
		point->vec.z = val * (x * D3DLightMatrix._31 + y * D3DLightMatrix._32 + z * D3DLightMatrix._33);
		point->r = light->r;
		point->g = light->g;
		point->b = light->b;
		point->rad = (falloff - dist) / falloff;
		nPointLights++;
		nTotalLights++;
	}
}

void SetupLight(PCLIGHT* light, ITEM_INFO* item, long* ambient)
{
	SUNLIGHT_STRUCT* sun;
	POINTLIGHT_STRUCT* point;
	float x, y, z, num, num2;
	long r, g, b, val, val2;

	switch (light->Type)
	{
	case LIGHT_SUN:
		sun = &SunLights[nSunLights];
		x = light->nx;
		y = light->ny;
		z = light->nz;
		num = -1.0F / sqrt(SQUARE(x) + SQUARE(y) + SQUARE(z));

		sun->vec.x = (D3DLightMatrix._11 * x + D3DLightMatrix._12 * y + D3DLightMatrix._13 * z) * num;
		sun->vec.y = (D3DLightMatrix._21 * x + D3DLightMatrix._22 * y + D3DLightMatrix._23 * z) * num;
		sun->vec.z = (D3DLightMatrix._31 * x + D3DLightMatrix._32 * y + D3DLightMatrix._33 * z) * num;
		sun->r = light->r * 255.0F;
		sun->g = light->g * 255.0F;
		sun->b = light->b * 255.0F;
		nSunLights++;
		nTotalLights++;
		break;

	case LIGHT_POINT:
		x = light->x - lGlobalMeshPos.x;
		y = light->y - lGlobalMeshPos.y;
		z = light->z - lGlobalMeshPos.z;

		if (!x || !y || !z)
		{
			//in the rare case of the light being placed on the exact same spot as the mesh, make sure it gets some lighting, otherwise weird stuff happen
			x++;
			y++;
			z++;
		}

		point = &PointLights[nPointLights];
		num2 = sqrt(SQUARE(x) + SQUARE(y) + SQUARE(z));
		num = 2.0F / num2;
		point->vec.x = (D3DLightMatrix._11 * x + D3DLightMatrix._12 * y + D3DLightMatrix._13 * z) * num;
		point->vec.y = (D3DLightMatrix._21 * x + D3DLightMatrix._22 * y + D3DLightMatrix._23 * z) * num;
		point->vec.z = (D3DLightMatrix._31 * x + D3DLightMatrix._32 * y + D3DLightMatrix._33 * z) * num;
		point->r = light->r * 255.0F;
		point->g = light->g * 255.0F;
		point->b = light->b * 255.0F;
		point->rad = (light->Outer - num2) / light->Outer;

		if (point->rad < 0)
			point->rad = 0;

		if (SetupLight_thing && point->rad < 1)
		{
			r = CLRR(*ambient) + long(point->rad * point->r);
			g = CLRG(*ambient) + long(point->rad * point->g);
			b = CLRB(*ambient) + long(point->rad * point->b);

			if (r > 255)
				r = 255;

			if (g > 255)
				g = 255;

			if (b > 255)
				b = 255;

			*ambient = RGBONLY(r, g, b);
			point->rad /= 2;	//if it's this close and we're already changing ambience, ramp down radius to avoid double color
		}

		nPointLights++;
		nTotalLights++;
		break;

	case LIGHT_SPOT:
		x = light->x - lGlobalMeshPos.x;
		y = light->y - lGlobalMeshPos.y;
		z = light->z - lGlobalMeshPos.z;

		if (!x || !y || !z)
		{
			x++;
			y++;
			z++;
		}

		point = &SpotLights[nSpotLights];
		num2 = sqrt(SQUARE(x) + SQUARE(y) + SQUARE(z));

		if (SetupLight_thing)
			num = 2.0F / num2;
		else
			num = 1.0F / num2;

		point->vec.x = (D3DLightMatrix._11 * x + D3DLightMatrix._12 * y + D3DLightMatrix._13 * z) * num;
		point->vec.y = (D3DLightMatrix._21 * x + D3DLightMatrix._22 * y + D3DLightMatrix._23 * z) * num;
		point->vec.z = (D3DLightMatrix._31 * x + D3DLightMatrix._32 * y + D3DLightMatrix._33 * z) * num;
		point->r = light->r * 255.0F;
		point->g = light->g * 255.0F;
		point->b = light->b * 255.0F;
		point->rad = 1.0F - num2 / light->Cutoff;

		if (point->rad < 0)
			point->rad = 0;

		nSpotLights++;
		nTotalLights++;
		break;

	case LIGHT_SHADOW:
		r = CLRR(*ambient);
		g = CLRG(*ambient);
		b = CLRB(*ambient);
		val = phd_sqrt(light->Range);
		val2 = light->shadow >> 3;

		if (val >= light->Inner)
			val2 = long((val - light->Outer) / ((light->Outer - light->Inner) / -val2));

		if (val2 < 0)
			val2 = 0;

		val2 >>= 1;
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
		nShadowLights++;
		break;
	}
}

void mApplyMatrix(float* matrix, FVECTOR* start, FVECTOR* dest)
{
	dest->x = start->x * matrix[M00] + start->y * matrix[M01] + start->z * matrix[M02];
	dest->y = start->x * matrix[M10] + start->y * matrix[M11] + start->z * matrix[M12];
	dest->z = start->x * matrix[M20] + start->y * matrix[M21] + start->z * matrix[M22];
}

void mApplyTransposeMatrix(float* matrix, FVECTOR* start, FVECTOR* dest)
{
	dest->x = start->x * matrix[M00] + start->y * matrix[M10] + start->z * matrix[M20];
	dest->y = start->x * matrix[M01] + start->y * matrix[M11] + start->z * matrix[M21];
	dest->z = start->x * matrix[M02] + start->y * matrix[M12] + start->z * matrix[M22];
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
				vec.x = (float)-dx;
				vec.y = (float)-dy;
				vec.z = (float)-dz;
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
	long node_ambient, r, g, b;

	node_ambient = item->il.ambient;
	SetupLight_thing = item->object_number >= GAME_PIECE1;
	light = (PCLIGHT*)item->il.pCurrentLights;

	for (int i = 0; i < item->il.nCurrentLights; i++)
	{
		if (light[i].Active)
			SetupLight(&light[i], item, &node_ambient);
	}

	light = (PCLIGHT*)item->il.pPrevLights;

	for (int i = 0; i < item->il.nPrevLights; i++)
	{
		if (light[i].Active)
			SetupLight(&light[i], item, &node_ambient);
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

	ambientR = CLRR(node_ambient);
	ambientG = CLRG(node_ambient);
	ambientB = CLRB(node_ambient);
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

void ResetLighting()
{
	D3DMATRIX view;
	D3DMATRIX cam;
	
	ambientR = 0;
	ambientG = 0;
	ambientB = 0;

	nSunLights = 0;
	nPointLights = 0;
	nSpotLights = 0;
	nShadowLights = 0;
	nTotalLights = 0;

	view = D3DMView;
	cam = D3DInvCameraMatrix;
	D3DMultMatrix(&D3DLightMatrix, &view, &cam);
	lGlobalMeshPos.x = CamPos.x + D3DLightMatrix._41;
	lGlobalMeshPos.y = CamPos.y + D3DLightMatrix._42;
	lGlobalMeshPos.z = CamPos.z + D3DLightMatrix._43;
}
