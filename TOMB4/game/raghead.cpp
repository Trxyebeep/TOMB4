#include "../tomb4/pch.h"
#include "raghead.h"
#include "box.h"
#include "objects.h"
#include "../specific/3dmath.h"

void InitialiseRaghead(short item_number)
{
	ITEM_INFO* item;
	short obj_num, flag;

	item = &items[item_number];
	InitialiseCreature(item_number);

	if (objects[SUPER_RAGHEAD].loaded)
		obj_num = SUPER_RAGHEAD;
	else
		obj_num = RAGHEAD;

	if (item->object_number == RAGHEAD)
	{
		item->meshswap_meshbits = 0x7FC010;
		item->mesh_bits = ~0x7E0000;
		item->item_flags[2] = 24;
	}
	else
	{
		item->meshswap_meshbits = 0x880;
		item->mesh_bits = -1;
		item->item_flags[2] = 0;
	}

	item->item_flags[1] = -1;
	flag = item->trigger_flags % 1000;

	if (flag > 9 && flag < 20)
	{
		item->item_flags[2] += 24;
		item->trigger_flags -= 10;
		flag -= 10;
	}

	switch (flag)
	{
	case 0:
	case 5:
	case 6:
		item->anim_number = objects[obj_num].anim_index + 18;
		item->current_anim_state = 0;
		item->goal_anim_state = 0;
		break;

	case 1:
		item->anim_number = objects[obj_num].anim_index + 47;
		item->current_anim_state = 24;
		item->goal_anim_state = 24;
		break;

	case 2:
		item->anim_number = objects[obj_num].anim_index + 24;
		item->current_anim_state = 23;
		item->goal_anim_state = 23;
		break;

	case 3:
		item->anim_number = objects[obj_num].anim_index + 29;
		item->current_anim_state = 26;
		item->goal_anim_state = 26;
		break;

	case 4:
		item->anim_number = objects[obj_num].anim_index + 62;
		item->current_anim_state = 39;
		item->goal_anim_state = 39;
		item->pos.x_pos += 256 * phd_sin(item->pos.y_rot) >> W2V_SHIFT;
		item->pos.z_pos += 256 * phd_cos(item->pos.y_rot) >> W2V_SHIFT;
		break;

	default:

		if (flag > 100)
		{
			item->anim_number = objects[obj_num].anim_index + 29;
			item->current_anim_state = 26;
			item->goal_anim_state = 26;
			item->pos.x_pos += 256 * phd_sin(item->pos.y_rot) >> W2V_SHIFT;
			item->pos.z_pos += 256 * phd_cos(item->pos.y_rot) >> W2V_SHIFT;
			item->item_flags[3] = flag;
		}

		break;
	}

	item->frame_number = anims[item->anim_number].frame_base;
}

void inject_raghead(bool replace)
{
	INJECT(0x0040B3C0, InitialiseRaghead, replace);
}
