#include "../tomb4/pch.h"
#include "traps.h"
#include "control.h"
#include "effect2.h"
#include "../specific/function_stubs.h"
#include "sound.h"
#include "tomb4fx.h"
#include "effects.h"
#include "items.h"

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

void inject_traps(bool replace)
{
	INJECT(0x004142F0, FlameEmitterControl, replace);
	INJECT(0x00415B60, OnTwoBlockPlatform, replace);
	INJECT(0x00415AD0, TwoBlockPlatformFloor, replace);
	INJECT(0x00415B20, TwoBlockPlatformCeiling, replace);
	INJECT(0x00415C30, ControlTwoBlockPlatform, replace);
}
