#include "../tomb4/pch.h"
#include "jeep.h"
#include "../specific/function_stubs.h"
#include "objects.h"
#include "lara_states.h"
#include "collide.h"
#include "control.h"
#include "../specific/3dmath.h"
#include "newinv.h"

void InitialiseJeep(short item_number)
{
	ITEM_INFO* item;
	JEEPINFO* jeep;

	item = &items[item_number];
	jeep = (JEEPINFO*)game_malloc(sizeof(JEEPINFO));
	item->data = jeep;
	jeep->velocity = 0;
	jeep->unused1 = 0;
	jeep->turn_rate = 0;
	jeep->camera_angle = 0;
	jeep->extra_rotation = 0;
	jeep->move_angle = item->pos.y_rot;
	jeep->pitch2 = 0;
	jeep->flags = 0;
	jeep->gear = 0;
	jeep->right_front_wheelrot = 0;
	jeep->left_front_wheelrot = 0;
	jeep->left_back_wheelrot = 0;
	jeep->right_back_wheelrot = 0;
	item->mesh_bits = 0x1BFFF;
}

static long GetOnJeep(short item_number, COLL_INFO* coll)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	long h;
	short room_number, ang;

	item = &items[item_number];

	if (input & IN_ACTION || GLOBAL_inventoryitemchosen == PUZZLE_ITEM1)
	{
		if (!(item->flags & IFL_INVISIBLE) && lara.gun_status == LG_NO_ARMS && lara_item->current_anim_state == AS_STOP &&
			lara_item->anim_number == ANIM_BREATH && !lara_item->gravity_status)
		{
			if (ABS(item->pos.y_pos - lara_item->pos.y_pos) < 256 && TestBoundsCollide(item, lara_item, 100))
			{
				room_number = item->room_number;
				floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
				h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
				
				if (h >= NO_HEIGHT + 512)	//mmmm
				{
					ang = short(phd_atan(item->pos.z_pos - lara_item->pos.z_pos, item->pos.x_pos - lara_item->pos.x_pos) - item->pos.y_rot);

					if (ang <= -8190 || ang >= 24570)
					{
						ang = lara_item->pos.y_rot - item->pos.y_rot;

						if (ang > -24586 && ang < -8206)
						{
							if (GLOBAL_inventoryitemchosen == PUZZLE_ITEM1)
							{
								GLOBAL_inventoryitemchosen = NO_ITEM;
								return 1;
							}
							
							if (have_i_got_object(PUZZLE_ITEM1))
								GLOBAL_enterinventory = PUZZLE_ITEM1;
						}
					}
					else
					{
						ang = lara_item->pos.y_rot - item->pos.y_rot;

						if (ang > 8190 && ang < 24570)
						{
							if (GLOBAL_inventoryitemchosen == PUZZLE_ITEM1)
							{
								GLOBAL_inventoryitemchosen = NO_ITEM;
								return 1;
							}

							if (have_i_got_object(PUZZLE_ITEM1))
								GLOBAL_enterinventory = PUZZLE_ITEM1;
						}
					}
				}
			}
		}
	}

	return 0;
}

void inject_jeep(bool replace)
{
	INJECT(0x00466F40, InitialiseJeep, replace);
	INJECT(0x004671B0, GetOnJeep, replace);
}
