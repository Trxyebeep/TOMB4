#include "../tomb4/pch.h"
#include "clockworkbeetle.h"
#include "lara_states.h"
#include "items.h"
#include "objects.h"

void TriggerClockworkBeetle(long flag)
{
	ITEM_INFO* item;
	ITEM_INFO* item2;
	long dx, dy, dz;
	short item_number;

	if (flag || lara_item->current_anim_state == AS_STOP && lara_item->anim_number == ANIM_BREATH &&
		!lara_item->gravity_status && lara.gun_status == LG_NO_ARMS)
	{
		item_number = CreateItem();

		if (item_number != NO_ITEM)
		{
			lara.mechanical_scarab &= ~1;
			item = &items[item_number];
			item->shade = -0x3DF0;
			item->object_number = CLOCKWORK_BEETLE;
			item->room_number = lara_item->room_number;
			item->pos.x_pos = lara_item->pos.x_pos;
			item->pos.y_pos = lara_item->pos.y_pos;
			item->pos.z_pos = lara_item->pos.z_pos;
			InitialiseItem(item_number);
			item->pos.z_rot = 0;
			item->pos.x_rot = 0;
			item->pos.y_rot = lara_item->pos.y_rot;

			if (lara.beetle_uses)
				item->item_flags[0] = lara.OnBeetleFloor;
			else
				item->item_flags[0] = 0;

			item->speed = 0;
			AddActiveItem(item_number);

			if (item->item_flags[0])
			{
				for (item_number = room[item->room_number].item_number; item_number != NO_ITEM; item_number = item2->next_item)
				{
					item2 = &items[item_number];

					if (item2->object_number == MAPPER)
					{
						dx = item->pos.x_pos - item2->pos.x_pos;
						dy = item->pos.y_pos - item2->pos.y_pos;
						dz = item->pos.z_pos - item2->pos.z_pos;

						if (dx > -1024 && dx < 1024 && dz > -1024 && dz < 1024 && dy > -1024 && dy < 1024)
						{
							item->item_flags[1] = item2->pos.y_rot + 0x8000;

							if (item2->item_flags[0])
								item->item_flags[0] = 0;
							else
								item2->item_flags[0] = 1;
						}
					}
				}
			}

			if (!item->item_flags[0])
				item->item_flags[3] = 150;
		}
	}
}

void inject_clockwork(bool replace)
{
	INJECT(0x004458F0, TriggerClockworkBeetle, replace);
}
