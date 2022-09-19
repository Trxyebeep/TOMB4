#include "../tomb4/pch.h"
#include "elements.h"
#include "objects.h"
#include "lara_states.h"
#include "collide.h"
#include "draw.h"
#include "switch.h"
#include "items.h"
#include "control.h"
#include "sound.h"
#include "../specific/function_stubs.h"
#include "tomb4fx.h"
#include "effect2.h"
#include "../specific/input.h"
#include "lara.h"

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
			y = abs(item->pos.y_pos - l->pos.y_pos);
			l->anim_number = short((y >> 8) + ANIM_LIGHT_TORCH3);
			l->frame_number = anims[l->anim_number].frame_base;
			l->current_anim_state = AS_CONTROLLED;
			lara.flare_control_left = 0;
			lara.left_arm.lock = 3;
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

void InitialiseElementPuzzle(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (!item->trigger_flags)
		item->mesh_bits = 80;
	else if (item->trigger_flags == 1)
		item->mesh_bits = 65;
	else if (item->trigger_flags == 2)
		item->mesh_bits = 68;
	else
		item->mesh_bits = 0;
}

void ControlElementPuzzle(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* item2;
	long r, g, numTriggers;
	short itemNos[8];
	short fade, itemNum;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	if (item->trigger_flags == 1)
	{
		SoundEffect(SFX_LOOP_FOR_SMALL_FIRES, &item->pos, SFX_DEFAULT);
		r = (GetRandomControl() & 0x3F) + 192;
		g = (GetRandomControl() & 0x1F) + 96;

		if (!item->item_flags[3])
			fade = 0;
		else
		{
			item->item_flags[3]--;
			fade = 255 - GetRandomControl() % ((91 - item->item_flags[3]) << 2);

			if (fade < 1)
				fade = 1;

			if (fade <= 255)
			{
				r = (r * fade) >> 8;
				g = (g * fade) >> 8;
			}
		}

		AddFire(item->pos.x_pos, item->pos.y_pos - 620, item->pos.z_pos, 1, item->room_number, fade);
		TriggerDynamic(item->pos.x_pos, item->pos.y_pos - 768, item->pos.z_pos, 12, r, g, 0);
	}
	else if (item->trigger_flags == 3)
	{
		if (item->item_flags[1] > 90)
			SoundEffect(SFX_JOBY_WIND, &item->pos, SFX_DEFAULT);

		if (item->item_flags[1] < 60)
		{
			item->item_flags[1]++;
			return;
		}

		item->item_flags[0]++;

		if (item->item_flags[0] == 90)
		{
			for (numTriggers = GetSwitchTrigger(item, itemNos, 0); numTriggers > 0; numTriggers--)
			{
				itemNum = itemNos[numTriggers - 1];
				item2 = &items[itemNum];
				AddActiveItem(itemNum);
				item2->status = ITEM_ACTIVE;
				item2->flags |= IFL_CODEBITS;
			}

			KillItem(item_number);
		}
		else
		{
			for (itemNum = room[item->room_number].item_number; itemNum != NO_ITEM; itemNum = item2->next_item)
			{
				item2 = &items[itemNum];

				if (item2->object_number == FLAME_EMITTER2)
				{
					if (item->item_flags[0] != 89)
					{
						item2->item_flags[3] = 255 - GetRandomControl() % (item->item_flags[0] << 2);

						if (item2->item_flags[3] < 2)
							item2->item_flags[3] = 2;
						else
							continue;
					}

					RemoveActiveItem(itemNum);
					item2->status = ITEM_INACTIVE;
				}
				else if (item2->object_number == ELEMENT_PUZZLE && item2->trigger_flags == 1 && !item2->item_flags[3])
					item2->item_flags[3] = 90;
			}
		}
	}
}
