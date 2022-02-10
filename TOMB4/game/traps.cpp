#include "../tomb4/pch.h"
#include "traps.h"
#include "control.h"
#include "effect2.h"
#include "../specific/function_stubs.h"
#include "sound.h"
#include "tomb4fx.h"
#include "effects.h"

void FlameEmitterControl(short item_number)
{
	ITEM_INFO* item;
	ulong distance;
	long x, z;

	item = &items[item_number];

	if (!TriggerActive(item))
	{
		if (item->trigger_flags >= 0)
			LibrarySerpents[item->trigger_flags] = 0;

		return;
	}

	if (item->trigger_flags < 0)
	{
		if ((-item->trigger_flags & 7) != 2 && (-item->trigger_flags & 7) != 7)
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
		LibrarySerpents[item->trigger_flags] = 1;
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

void inject_traps(bool replace)
{
	INJECT(0x004142F0, FlameEmitterControl, replace);
}
