#include "../tomb4/pch.h"
#include "pickup.h"
#include "lara_states.h"
#include "collide.h"
#include "items.h"
#include "health.h"

static short SarcophagusBounds[14] = { -512, 512, -100, 100, -512, 0, -1820, 1820, -5460, 5460, 0, 0, 0, 0 };
static PHD_VECTOR  SarcophagusPos = { 0, 0, -300 };

void SarcophagusCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	ITEM_INFO* pickup;
	short pickup_num;

	item = &items[item_number];

	if (input & IN_ACTION && item->status != ITEM_ACTIVE && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH &&
		lara.gun_status == LG_NO_ARMS || lara.IsMoving && lara.GeneralPtr == (void*)item_number)
	{
		if (TestLaraPosition(SarcophagusBounds, item, l))
		{
			if (MoveLaraPosition(&SarcophagusPos, item, l))
			{
				l->anim_number = ANIM_SARCOPHAGUS;
				l->frame_number = anims[ANIM_SARCOPHAGUS].frame_base;
				l->current_anim_state = AS_CONTROLLED;
				item->flags |= IFL_CODEBITS;
				item->status = ITEM_ACTIVE;
				AddActiveItem(item_number);
				lara.IsMoving = 0;
				lara.head_x_rot = 0;
				lara.head_y_rot = 0;
				lara.torso_x_rot = 0;
				lara.torso_y_rot = 0;
				lara.gun_status = LG_HANDS_BUSY;
			}
			else
				lara.GeneralPtr = (void*)item_number;
		}
		else if (lara.IsMoving && lara.GeneralPtr == (void*)item_number)
		{
			lara.IsMoving = 0;
			lara.gun_status = LG_NO_ARMS;
		}
	}
	else if (l->anim_number == ANIM_SARCOPHAGUS && l->frame_number == anims[ANIM_SARCOPHAGUS].frame_base + 113)
	{
		for (pickup_num = room[item->room_number].item_number; pickup_num != NO_ITEM; pickup_num = pickup->next_item)
		{
			pickup = &items[pickup_num];

			if (item != pickup && item->pos.x_pos == pickup->pos.x_pos && item->pos.z_pos == pickup->pos.z_pos)
			{
				if (objects[pickup->object_number].collision == PickUpCollision)
				{
					AddDisplayPickup(pickup->object_number);
					pickup->item_flags[3] = 1;
					pickup->status = ITEM_INVISIBLE;
				}
			}
		}
	}
	else
		ObjectCollision(item_number, l, coll);
}

void inject_pickup(bool replace)
{
	INJECT(0x004587E0, SarcophagusCollision, replace);
}
