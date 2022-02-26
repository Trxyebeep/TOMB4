#include "../tomb4/pch.h"
#include "elements.h"
#include "objects.h"
#include "lara_states.h"
#include "collide.h"
#include "draw.h"
#include "switch.h"
#include "items.h"

short ElementPuzzleBounds[12] = { 0, 0, -64, 0, 0, 0, -1820, 1820, -5460, 5460, -1820, 1820 };

void ElementPuzzleCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	short* bounds;
	long y;
	short mesh, rotY;

	item = &items[item_number];

	if (!item->trigger_flags)
		mesh = LARA_WATER_MESH;
	else if (item->trigger_flags == 1)
		mesh = LARA_PETROL_MESH;
	else if (item->trigger_flags == 2)
		mesh = LARA_DIRT_MESH;
	else
		return;

	if ((l->anim_number == ANIM_POURWATERSKIN || l->anim_number == ANIM_FILLSCALE) && !item->item_flags[0])
	{
		bounds = GetBoundsAccurate(item);
		ElementPuzzleBounds[0] = bounds[0];
		ElementPuzzleBounds[1] = bounds[1];
		ElementPuzzleBounds[4] = bounds[4] - 200;
		ElementPuzzleBounds[5] = bounds[4] + 200;
		rotY = item->pos.y_rot;
		item->pos.y_rot = l->pos.y_rot;

		if (TestLaraPosition(ElementPuzzleBounds, item, l))
		{
			if (l->anim_number == ANIM_POURWATERSKIN && lara_item->item_flags[2] == mesh)
			{
				l->anim_number = ANIM_FILLSCALE;
				l->frame_number = anims[ANIM_FILLSCALE].frame_base;
			}

			if (l->frame_number == anims[ANIM_FILLSCALE].frame_base + 74 && lara_item->item_flags[2] == mesh)
			{
				if (!item->trigger_flags)
				{
					item->mesh_bits = 48;
					TestTriggersAtXYZ(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 1, item->flags & IFL_CODEBITS);
					item->item_flags[0] = 1;
				}
				else if (item->trigger_flags == 1)
				{
					item->mesh_bits = 3;
					lara.pickupitems &= ~2u;
					item->item_flags[0] = 1;
				}
				else
				{
					item->mesh_bits = 12;
					TestTriggersAtXYZ(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 1, item->flags & IFL_CODEBITS);
					lara.pickupitems &= ~1u;
					item->item_flags[0] = 1;
				}
			}
		}

		item->pos.y_rot = rotY;
	}
	else if (lara.gun_type == WEAPON_TORCH && lara.gun_status == LG_READY && !lara.left_arm.lock && input & IN_ACTION &&
		item->trigger_flags == 1 && item->item_flags[0] == 1 && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH &&
		lara.LitTorch && !l->gravity_status)
	{
		bounds = GetBoundsAccurate(item);
		ElementPuzzleBounds[0] = bounds[0];
		ElementPuzzleBounds[1] = bounds[1];
		ElementPuzzleBounds[4] = bounds[4] - 200;
		ElementPuzzleBounds[5] = bounds[4] + 200;
		rotY = item->pos.y_rot;
		item->pos.y_rot = l->pos.y_rot;

		if (TestLaraPosition(ElementPuzzleBounds, item, l))
		{
			y = ABS(item->pos.y_pos - l->pos.y_pos);	//literally 0
			l->anim_number = short((y >> 8) + ANIM_LIGHT_TORCH3);
			l->frame_number = anims[l->anim_number].frame_base;
			l->current_anim_state = AS_CONTROLLED;
			lara.flare_control_left = 0;	//uhhhh
			lara.left_arm.lock = 3;			//UHHHH
			item->item_flags[0] = 2;
		}

		item->pos.y_rot = rotY;
	}
	else if (l->anim_number == ANIM_LIGHT_TORCH3 && l->frame_number == anims[ANIM_LIGHT_TORCH3].frame_base + 16 && item->item_flags[0] == 2)
	{
		TestTriggersAtXYZ(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 1, item->flags & IFL_CODEBITS);
		AddActiveItem(item_number);
		item->item_flags[0] = 3;
		item->flags |= IFL_CODEBITS;
		item->status = ITEM_ACTIVE;
	}
	else
		ObjectCollisionNoBigPush(item_number, l, coll);
}

void inject_elements(bool replace)
{
	INJECT(0x00454B60, ElementPuzzleCollision, replace);
}
