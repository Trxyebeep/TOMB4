#include "../tomb4/pch.h"
#include "box.h"
#include "tomb4fx.h"
#include "items.h"
#include "lot.h"
#include "objects.h"
#include "control.h"
#include "draw.h"

void CreatureDie(short item_number, long explode)
{
	ITEM_INFO* item;
	ITEM_INFO* pickup;
	short pickup_number, room_number;

	item = &items[item_number];
	item->hit_points = -16384;
	item->collidable = 0;

	if (explode)
	{
		if (objects[item->object_number].HitEffect == 1)
			ExplodingDeath2(item_number, -1, 258);
		else
			ExplodingDeath2(item_number, -1, 256);

		KillItem(item_number);
	}
	else
		RemoveActiveItem(item_number);

	DisableBaddieAI(item_number);
	item->flags |= IFL_INVISIBLE | IFL_CLEARBODY;

	//inlined DropBaddyPickups..

	pickup_number = item->carried_item;

	while (pickup_number != NO_ITEM)
	{
		pickup = &items[pickup_number];

		if (item->object_number == TROOPS && item->trigger_flags == 1)
		{
			pickup->pos.x_pos = ((item->pos.x_pos + ((1024 * phd_sin(item->pos.y_rot)) >> W2V_SHIFT)) & -512) | 512;
			pickup->pos.z_pos = ((item->pos.z_pos + ((1024 * phd_cos(item->pos.y_rot)) >> W2V_SHIFT)) & -512) | 512;
		}
		else
		{
			pickup->pos.x_pos = (item->pos.x_pos & -512) | 512;
			pickup->pos.z_pos = (item->pos.z_pos & -512) | 512;
		}

		room_number = item->room_number;
		pickup->pos.y_pos = GetHeight(GetFloor(pickup->pos.x_pos, item->pos.y_pos, pickup->pos.z_pos, &room_number),
			pickup->pos.x_pos, item->pos.y_pos, pickup->pos.z_pos);
		pickup->pos.y_pos -= GetBoundsAccurate(pickup)[3];
		ItemNewRoom(pickup_number, item->room_number);
		pickup->flags |= IFL_TRIGGERED;
		pickup_number = pickup->carried_item;
	}
}

void inject_box(bool replace)
{
	INJECT(0x00441080, CreatureDie, replace);
}
