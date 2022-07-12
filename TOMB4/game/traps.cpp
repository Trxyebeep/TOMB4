#include "../tomb4/pch.h"
#include "traps.h"
#include "control.h"
#include "effect2.h"
#include "../specific/function_stubs.h"
#include "sound.h"
#include "tomb4fx.h"
#include "effects.h"
#include "items.h"
#include "draw.h"
#include "objects.h"
#include "../specific/3dmath.h"
#include "../specific/output.h"

short SPxzoffs[8] = { 0, 0, 0x200, 0, 0, 0, -0x200, 0 };
short SPyoffs[8] = { -0x400, 0, -0x200, 0, 0, 0, -0x200, 0 };
short SPDETyoffs[8] = { 0x400, 0x200, 0x200, 0x200, 0, 0x200, 0x200, 0x200 };

void FlameEmitterControl(short item_number)
{
	ITEM_INFO* item;
	ulong distance;
	long x, z;

	item = &items[item_number];

	if (!TriggerActive(item))
	{
		if (item->trigger_flags >= 0)
			LibraryTab[item->trigger_flags] = 0;

		return;
	}

	if (item->trigger_flags < 0)
	{
		if ((-item->trigger_flags & 7) == 2 || (-item->trigger_flags & 7) == 7)
		{
			SoundEffect(SFX_FLAME_EMITTER, &item->pos, 0);
			TriggerSuperJetFlame(item, -256 - (3072 * GlobalCounter & 0x1C00), GlobalCounter & 1);
			TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos,
				(GetRandomControl() & 3) + 20, (GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 96, 0);
		}
		else
		{
			if (item->item_flags[0])
			{
				if (item->item_flags[1])
					item->item_flags[1] -= item->item_flags[1] >> 2;

				if (item->item_flags[2])
					item->item_flags[2] += 8;

				item->item_flags[0]--;

				if (!item->item_flags[0])
					item->item_flags[3] = (GetRandomControl() & 0x3F) + 150;
			}
			else
			{
				item->item_flags[3]--;

				if (!item->item_flags[3])
				{
					if (-item->trigger_flags >> 3)
						item->item_flags[0] = (GetRandomControl() & 0x1F) + 30 * (-item->trigger_flags >> 3);
					else
						item->item_flags[0] = (GetRandomControl() & 0x3F) + 60;
				}

				if (item->item_flags[2])
					item->item_flags[2] -= 8;

				if (item->item_flags[1] > -8192)
					item->item_flags[1] -= 512;
			}

			if (item->item_flags[2])
				AddFire(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 0, item->room_number, item->item_flags[2] & 0xFF);

			if (item->item_flags[1])
			{
				SoundEffect(SFX_FLAME_EMITTER, &item->pos, 0);

				if (item->item_flags[1] > -8192)
					TriggerSuperJetFlame(item, item->item_flags[1], GlobalCounter & 1);
				else
					TriggerSuperJetFlame(item, -256 - (3072 * GlobalCounter & 0x1C00), GlobalCounter & 1);

				TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, (-item->item_flags[1] >> 10) - (GetRandomControl() & 1) + 16, (GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 96, 0);
			}
			else
				TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 10 - (GetRandomControl() & 1), (GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 96, 0);
		}

		SoundEffect(SFX_LOOP_FOR_SMALL_FIRES, &item->pos, 0);
	}
	else
	{
		LibraryTab[item->trigger_flags] = 1;
		AddFire(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 2, item->room_number, 0);
		TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 16 - (GetRandomControl() & 1),
			(GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 96, 0);
		SoundEffect(SFX_LOOP_FOR_SMALL_FIRES, &item->pos, SFX_DEFAULT);

		if (!lara.burn && ItemNearLara(&item->pos, 600))
		{
			x = lara_item->pos.x_pos - item->pos.x_pos;
			z = lara_item->pos.z_pos - item->pos.z_pos;
			distance = SQUARE(x) + SQUARE(z);

			if (distance < 0x40000)
				LaraBurn();
		}
	}
}

static long OnTwoBlockPlatform(ITEM_INFO* item, long x, long z)
{
	long tx, tz;

	if (!item->mesh_bits)
		return 0;

	x >>= 10;
	z >>= 10;
	tx = item->pos.x_pos >> 10;
	tz = item->pos.z_pos >> 10;

	if (!item->pos.y_rot && (x == tx || x == tx - 1) && (z == tz || z == tz + 1))
		return 1;

	if (item->pos.y_rot == 0x8000 && (x == tx || x == tx + 1) && (z == tz || z == tz - 1))
		return 1;

	if (item->pos.y_rot == 0x4000 && (z == tz || z == tz - 1) && (x == tx || x == tx + 1))
		return 1;

	if (item->pos.y_rot == -0x4000 && (z == tz || z == tz - 1) && (x == tx || x == tx - 1))
		return 1;

	return 0;
}

void TwoBlockPlatformFloor(ITEM_INFO* item, long x, long y, long z, long* height)
{
	if (OnTwoBlockPlatform(item, x, z))
	{
		if (y <= item->pos.y_pos + 32 && item->pos.y_pos < *height)
		{
			*height = item->pos.y_pos;
			OnObject = 1;
			height_type = 0;
		}
	}
}

void TwoBlockPlatformCeiling(ITEM_INFO* item, long x, long y, long z, long* height)
{
	if (OnTwoBlockPlatform(item, x, z))
	{
		if (y > item->pos.y_pos + 32 && item->pos.y_pos > *height)
			*height = item->pos.y_pos + 256;
	}
}

void ControlTwoBlockPlatform(short item_number)
{
	ITEM_INFO* item;
	long height;
	short room_number;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	if (item->trigger_flags)
	{
		if (item->pos.y_pos > item->item_flags[0] - (item->trigger_flags << 4))
			item->pos.y_pos -= item->trigger_flags & 0xF;

		room_number = item->room_number;
		item->floor = GetHeight(GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number),
			item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

		if (room_number != item->room_number)
			ItemNewRoom(item_number, room_number);
	}
	else
	{
		OnObject = 0;
		height = lara_item->pos.y_pos + 1;
		TwoBlockPlatformFloor(item, lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, &height);

		if (!OnObject || lara_item->anim_number == 89)
			item->item_flags[1] = -1;
		else
			item->item_flags[1] = 1;

		if (item->item_flags[1] > 0)
		{
			if (item->pos.y_pos >= item->item_flags[0] + 128)
				item->item_flags[1] = -1;
			else
			{
				SoundEffect(SFX_RUMBLE_NEXTDOOR, &item->pos, SFX_DEFAULT);
				item->pos.y_pos += 4;
			}
		}
		else if (item->item_flags[1] < 0)
		{
			if (item->pos.y_pos <= item->item_flags[0])
				item->item_flags[1] = 1;
			else
			{
				SoundEffect(SFX_RUMBLE_NEXTDOOR, &item->pos, SFX_DEFAULT);
				item->pos.y_pos -= 4;
			}
		}
	}
}

void ControlJobySpike(short item_number)
{
	ITEM_INFO* item;
	short* frm[2];
	long rate, y, h;

	item = &items[item_number];

	if (TriggerActive(item))
	{
		SoundEffect(SFX_METAL_SCRAPE_LOOP, &item->pos, SFX_DEFAULT);
		GetFrames(lara_item, frm, &rate);
		y = lara_item->pos.y_pos + frm[0][2];
		h = item->pos.y_pos + (3328 * item->item_flags[1] >> 12);

		if (lara_item->hit_points > 0 && h > y && ABS(item->pos.x_pos - lara_item->pos.x_pos) < 512 && ABS(item->pos.z_pos - lara_item->pos.z_pos) < 512)
		{
			DoBloodSplat(lara_item->pos.x_pos + (GetRandomControl() & 0x7F) - 64, GetRandomControl() % (h - y) + y, lara_item->pos.z_pos + (GetRandomControl() & 0x7F) - 64, (GetRandomControl() & 3) + 2, (short)(2 * GetRandomControl()), item->room_number);
			lara_item->hit_points -= 8;
		}

		if (!item->item_flags[2])
		{
			if (item->item_flags[0] < 4096)
				item->item_flags[0] += (item->item_flags[0] >> 6) + 2;
		}
		else if (item->item_flags[0] > -4096)
			item->item_flags[0] += (item->item_flags[0] >> 6) - 2;

		if (item->item_flags[1] < item->item_flags[3])
			item->item_flags[1] += 3;

		item->pos.y_rot += item->item_flags[0];
	}
}

void DrawScaledSpike(ITEM_INFO* item)
{
	PHD_VECTOR scale;
	ROOM_INFO* r;
	short** meshpp;
	short* frm[2];
	long rate, clip, lp;

	if (item->object_number == TEETH_SPIKES || item->item_flags[1])
	{
		if ((item->object_number == RAISING_BLOCK1 || item->object_number == RAISING_BLOCK2) && item->trigger_flags && !item->item_flags[0])
		{
			for (lp = 1; lp < 8; lp++)
			{
				if (!LibraryTab[lp])
					break;
			}

			if (lp == 8)
			{
				item->item_flags[0] = 1;
				item->touch_bits = 0;
				AddActiveItem(item - items);
				item->flags |= IFL_CODEBITS;
				item->status = ITEM_ACTIVE;
			}
		}

		r = &room[item->room_number];
		phd_left = r->left;
		phd_right = r->right;
		phd_top = r->top;
		phd_bottom = r->bottom;
		GetFrames(item, frm, &rate);
		phd_PushMatrix();
		phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
		phd_RotX(item->pos.x_rot);
		phd_RotZ(item->pos.z_rot);
		phd_RotY(item->pos.y_rot);
		clip = S_GetObjectBounds(frm[0]);

		if (clip)
		{
			meshpp = &meshes[objects[item->object_number].mesh_index];

			if (item->object_number == EXPANDING_PLATFORM)
			{
				scale.x = 16384;
				scale.y = 16384;
				scale.z = 4 * item->item_flags[1];
			}
			else
			{
				scale.y = 4 * item->item_flags[1];

				if (item->object_number != JOBY_SPIKES)
				{
					scale.x = 16384;
					scale.z = 16384;
				}
				else
				{
					scale.x = 12288;
					scale.z = 12288;
				}
			}

			ScaleCurrentMatrix(&scale);
			CalculateObjectLighting(item, frm[0]);
			phd_PutPolygons(*meshpp, clip);
		}

		phd_left = 0;
		phd_right = phd_winwidth;
		phd_top = 0;
		phd_bottom = phd_winheight;
		phd_PopMatrix();
	}
}

void ControlSlicerDicer(short item_number)
{
	ITEM_INFO* item;
	long distance;
	short room_number;

	item = &items[item_number];
	SoundEffect(SFX_METAL_SCRAPE_LOOP, &item->pos, SFX_DEFAULT);
	SoundEffect(SFX_METAL_SCRAPE_LOOP1, &item->pos, SFX_DEFAULT);
	distance = 4608 * phd_cos(item->trigger_flags) >> 14;
	item->pos.x_pos = 256 * item->item_flags[0] + (phd_sin(item->pos.y_rot) * distance >> 14);
	item->pos.y_pos = 256 * item->item_flags[1] - (4608 * phd_sin(item->trigger_flags) >> 14);
	item->pos.z_pos = 256 * item->item_flags[2] + (phd_cos(item->pos.y_rot) * distance >> 14);
	item->trigger_flags += 170;
	room_number = item->room_number;
	GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

	if (item->room_number != room_number)
		ItemNewRoom(item_number, room_number);

	AnimateItem(item);
}

void inject_traps(bool replace)
{
	INJECT(0x004142F0, FlameEmitterControl, replace);
	INJECT(0x00415B60, OnTwoBlockPlatform, replace);
	INJECT(0x00415AD0, TwoBlockPlatformFloor, replace);
	INJECT(0x00415B20, TwoBlockPlatformCeiling, replace);
	INJECT(0x00415C30, ControlTwoBlockPlatform, replace);
	INJECT(0x00415750, ControlJobySpike, replace);
	INJECT(0x004158E0, DrawScaledSpike, replace);
	INJECT(0x00415DB0, ControlSlicerDicer, replace);
}
