#include "../tomb4/pch.h"
#include "pickup.h"
#include "lara_states.h"
#include "collide.h"
#include "items.h"
#include "health.h"
#include "objects.h"
#include "newinv.h"
#include "control.h"

static short SarcophagusBounds[12] = { -512, 512, -100, 100, -512, 0, -1820, 1820, -5460, 5460, 0, 0 };
static short KeyHoleBounds[12] = { -256, 256, 0, 0, 0, 412, -1820, 1820, -5460, 5460, -1820, 1820 };
static PHD_VECTOR  SarcophagusPos = { 0, 0, -300 };
static PHD_VECTOR KeyHolePosition = { 0, 0, 362 };

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

void KeyHoleCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	short key, hole;

	item = &items[item_number];

	if ((input & IN_ACTION || GLOBAL_inventoryitemchosen != NO_ITEM) && !BinocularRange && lara.gun_status == LG_NO_ARMS &&
		l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH || lara.IsMoving && lara.GeneralPtr == (void*)item_number)
	{
		key = short(GLOBAL_inventoryitemchosen - KEY_ITEM1);
		hole = item->object_number - KEY_HOLE1;

		if (TestLaraPosition(KeyHoleBounds, item, l))
		{
			if (!lara.IsMoving)
			{
				if (item->status == ITEM_INVISIBLE)
					return;

				if (GLOBAL_inventoryitemchosen == NO_ITEM)
				{
					if (have_i_got_object(hole + KEY_ITEM1))
						GLOBAL_enterinventory = hole + KEY_ITEM1;

					return;
				}

				if (key != hole)
					return;
			}

			if (MoveLaraPosition(&KeyHolePosition, item, l))
			{
				remove_inventory_item(hole + KEY_ITEM1);
				l->anim_number = ANIM_USEKEY;
				l->frame_number = anims[l->anim_number].frame_base;
				l->current_anim_state = AS_USEKEY;
				lara.IsMoving = 0;
				lara.head_x_rot = 0;
				lara.head_y_rot = 0;
				lara.torso_x_rot = 0;
				lara.torso_y_rot = 0;
				lara.gun_status = LG_HANDS_BUSY;
				item->status = ITEM_ACTIVE;
			}
			else
				lara.GeneralPtr = (void*)item_number;

			GLOBAL_inventoryitemchosen = NO_ITEM;
		}
		else if (lara.IsMoving && lara.GeneralPtr == (void*)item_number)
		{
			lara.IsMoving = 0;
			lara.gun_status = LG_NO_ARMS;
		}
	}
	else if (item->object_number <= KEY_HOLE6)
		ObjectCollision(item_number, l, coll);
}

void PuzzleDoneCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll)
{
	if (items[item_num].trigger_flags != 999)
		ObjectCollision(item_num, l, coll);
}

void PuzzleDone(ITEM_INFO* item, short item_number)
{
	item->object_number += 12;
	item->anim_number = objects[item->object_number].anim_index;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = anims[item->anim_number].current_anim_state;
	item->goal_anim_state = item->current_anim_state;
	item->required_anim_state = 0;
	AddActiveItem(item_number);
	item->flags |= IFL_CODEBITS;
	item->status = ITEM_ACTIVE;
}

void AnimatingPickUp(short item_number)
{
	if ((items[item_number].trigger_flags & 0x3F) == 2)
		AnimateItem(&items[item_number]);
}

void inject_pickup(bool replace)
{
	INJECT(0x004587E0, SarcophagusCollision, replace);
	INJECT(0x00458090, KeyHoleCollision, replace);
	INJECT(0x00458260, PuzzleDoneCollision, replace);
	INJECT(0x00458690, PuzzleDone, replace);
	INJECT(0x00457610, AnimatingPickUp, replace);
}
