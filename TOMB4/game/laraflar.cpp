#include "../tomb4/pch.h"
#include "laraflar.h"
#include "../specific/3dmath.h"
#include "objects.h"
#include "../specific/output.h"
#include "../specific/function_stubs.h"
#include "effect2.h"
#include "delstuff.h"
#ifdef GENERAL_FIXES
#include "draw.h"
#endif
#include "items.h"
#include "control.h"
#include "collide.h"

void DrawFlareInAir(ITEM_INFO* item)
{
#ifdef GENERAL_FIXES
	short* bounds;
#endif

	phd_PushMatrix();
#ifdef GENERAL_FIXES
	bounds = GetBoundsAccurate(item);
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos - bounds[3], item->pos.z_pos);
#else
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
#endif

	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	phd_PutPolygons_train(meshes[objects[FLARE_ITEM].mesh_index], 0);
	phd_PopMatrix();

	if (gfLevelFlags & GF_MIRROR)
	{
		if (item->room_number == gfMirrorRoom)
		{
			phd_PushMatrix();
			phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, 2 * gfMirrorZPlane - item->pos.z_pos);
			phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
			phd_PutPolygons_train(meshes[objects[FLARE_ITEM].mesh_index], 0);
			phd_PopMatrix();
		}
	}
}

void draw_flare_meshes()
{
	lara.mesh_ptrs[LM_LHAND] = meshes[objects[FLARE_ANIM].mesh_index + LM_LHAND * 2];
}

void undraw_flare_meshes()
{
	lara.mesh_ptrs[LM_LHAND] = meshes[objects[LARA].mesh_index + LM_LHAND * 2];
}

long DoFlareLight(PHD_VECTOR* pos, long flare_age)
{
	long x, y, z, r, g, b, rnd, falloff, ret;

	if (flare_age >= 900 || !flare_age)
		return 0;

	rnd = GetRandomControl();
	x = pos->x + ((rnd & 0xF) << 3);
	y = pos->y + (rnd >> 1 & 120) - 256;
	z = pos->z + (rnd >> 5 & 120);
	ret = 1;

	if (flare_age < 4)
	{
		falloff = (rnd & 3) + 4 + flare_age * 4;

		if (falloff > 16)
			falloff -= (rnd >> 12) & 3;

		r = (rnd >> 4 & 0x1F) + 8 * flare_age + 32;
		g = (rnd & 0x1F) + 16 * (flare_age + 10);
		b = 16 * flare_age + ((rnd >> 8) & 0x1F);
	}
	else if (flare_age < 16)
	{
		falloff = (rnd & 1) + flare_age + 2;
		r = ((rnd >> 4) & 0x1F) + 4 * flare_age + 64;
		g = (rnd & 0x3F) + 4 * flare_age + 128;
		b = ((rnd >> 8) & 0x1F) + 4 * flare_age + 16;
	}
	else if (flare_age < 810)
	{
		falloff = 16;
		r = (rnd >> 4 & 0x1F) + 128;
		g = (rnd & 0x3F) + 192;
		b = ((rnd >> 8) & 0x3F) + (((rnd >> 6) & 0x7F) >> 2);
	}
	else if (flare_age < 876)
	{
		if (rnd > 0x2000)
		{
			falloff = 16;
			r = (rnd & 0x1F) + 128;
			g = (rnd & 0x3F) + 192;
			b = ((rnd >> 8) & 0x3F) + (((rnd >> 6) & 0x7F) >> 2);
		}
		else
		{
			r = (GetRandomControl() & 0x3F) + 64;
			g = (GetRandomControl() & 0x3F) + 192;
			b = GetRandomControl() & 0x7F;
			falloff = (GetRandomControl() & 6) + 8;
			ret = 0;
		}
	}
	else
	{
		falloff = 16 - ((flare_age - 876) >> 1);
		r = (GetRandomControl() & 0x3F) + 64;
		g = (GetRandomControl() & 0x3F) + 192;
		b = GetRandomControl() & 0x1F;
		ret = rnd & 1;
	}

	TriggerDynamic(x, y, z, falloff, r, g, b);
	return ret;
}

void DoFlareInHand(long flare_age)
{
	PHD_VECTOR pos;

	pos.x = 11;
	pos.y = 32;
	pos.z = 41;
	GetLaraJointPos(&pos, 14);
	lara.left_arm.flash_gun = DoFlareLight(&pos, flare_age);

	if (gfLevelFlags & GF_MIRROR && lara_item->room_number == gfMirrorRoom)
	{
		pos.z = 2 * gfMirrorZPlane - pos.z;
		DoFlareLight(&pos, flare_age);
	}

	if (lara.flare_age < 900)
		lara.flare_age++;
	else if (lara.gun_status == LG_NO_ARMS)
		lara.gun_status = LG_UNDRAW_GUNS;
}

void CreateFlare(short object, long thrown)
{
	ITEM_INFO** itemlist;
	MESH_INFO** meshlist;
	ITEM_INFO* flare;
	FLOOR_INFO* floor;
	PHD_VECTOR pos;
	long collided;
	short flare_item, room_number;

	flare_item = CreateItem();

	if (flare_item != NO_ITEM)
	{
		collided = 0;
		flare = &items[flare_item];
		flare->object_number = object;
		flare->room_number = lara_item->room_number;

		pos.x = -16;
		pos.y = 32;
		pos.z = 42;
		GetLaraJointPos(&pos, 14);
		flare->pos.x_pos = pos.x;
		flare->pos.y_pos = pos.y;
		flare->pos.z_pos = pos.z;

		room_number = lara_item->room_number;
		floor = GetFloor(pos.x, pos.y, pos.z, &room_number);
		itemlist = (ITEM_INFO**)&tsv_buffer[0];
		meshlist = (MESH_INFO**)&tsv_buffer[1024];

		if (GetCollidedObjects(flare, 0, 1, itemlist, meshlist, 0) || pos.y > GetHeight(floor, pos.x, pos.y, pos.z))
		{
			collided = 1;
			flare->pos.y_rot = lara_item->pos.y_rot - 0x8000;
			flare->pos.x_pos = lara_item->pos.x_pos + (80 * phd_sin(flare->pos.y_rot) >> W2V_SHIFT);
			flare->pos.z_pos = lara_item->pos.z_pos + (80 * phd_cos(flare->pos.y_rot) >> W2V_SHIFT);
			flare->room_number = lara_item->room_number;
		}
		else
		{
			if (thrown)
				flare->pos.y_rot = lara_item->pos.y_rot;
			else
				flare->pos.y_rot = lara_item->pos.y_rot - 0x2000;

			flare->room_number = room_number;
		}

		InitialiseItem(flare_item);
		flare->pos.x_rot = 0;
		flare->pos.z_rot = 0;
		flare->shade = -1;

		if (thrown)
		{
			flare->speed = lara_item->speed + 50;
			flare->fallspeed = lara_item->fallspeed - 50;
		}
		else
		{
			flare->speed = lara_item->speed + 10;
			flare->fallspeed = lara_item->fallspeed + 50;
		}

		if (collided)
			flare->speed >>= 1;

		if (object == FLARE_ITEM)
		{
			if (DoFlareLight((PHD_VECTOR*)&flare->pos, lara.flare_age))
				flare->data = (void*)(lara.flare_age | 0x8000);
			else
				flare->data = (void*)(lara.flare_age & 0x7FFF);
		}
		else
			flare->item_flags[3] = lara.LitTorch;

		AddActiveItem(flare_item);
		flare->status = ITEM_ACTIVE;
	}
}

void inject_laraflar(bool replace)
{
	INJECT(0x0042F7B0, DrawFlareInAir, replace);
	INJECT(0x0042FF10, draw_flare_meshes, replace);
	INJECT(0x0042FF30, undraw_flare_meshes, replace);
	INJECT(0x0042F510, DoFlareLight, replace);
	INJECT(0x0042F6F0, DoFlareInHand, replace);
	INJECT(0x0042F880, CreateFlare, replace);
}
