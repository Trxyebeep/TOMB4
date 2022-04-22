#include "../tomb4/pch.h"
#include "door.h"
#include "draw.h"
#include "control.h"
#include "objects.h"
#include "lara_states.h"
#include "collide.h"
#include "newinv.h"
#include "sound.h"
#include "items.h"
#include "sphere.h"

#ifdef GENERAL_FIXES
static PHD_VECTOR CrowbarDoorPos = { -412, 0, 140 };
#else
static PHD_VECTOR CrowbarDoorPos = { -412, 0, 256 };
#endif

static short CrowbarDoorBounds[12] = { -512, 512, -1024, 0, 0, 512, -14560, 14560, -14560, 14560, -14560, 14560 };

void ShutThatDoor(DOORPOS_DATA* d)
{
	CREATURE_INFO* cinfo;

	if (d->floor)
	{
		d->floor->box = -1;
		d->floor->ceiling = -127;
		d->floor->floor = -127;
		d->floor->index = 0;
		d->floor->pit_room = -1;
		d->floor->sky_room = -1;

		if (d->block != 2047)
		{
			boxes[d->block].overlap_index |= 0x4000;

			for (short slot = 0; slot < 5; slot++)
			{
				cinfo = &baddie_slots[slot];
				cinfo->LOT.target_box = 2047;
			}
		}
	}
}

void OpenThatDoor(DOORPOS_DATA* d)
{
	CREATURE_INFO* cinfo;

	if (d->floor)
	{
		*d->floor = d->data;

		if (d->block != 2047)
		{
			boxes[d->block].overlap_index |= ~0x4000;

			for (short slot = 0; slot < 5; slot++)
			{
				cinfo = &baddie_slots[slot];
				cinfo->LOT.target_box = 2047;
			}
		}
	}
}

void DoorControl(short item_number)
{
	ITEM_INFO* item;
	DOOR_DATA* door;
	short* bounds;

	item = &items[item_number];
	door = (DOOR_DATA*)item->data;

	if (item->trigger_flags == 1)
	{
		if (item->item_flags[0])
		{
			bounds = GetBoundsAccurate(item);
			item->item_flags[0]--;
			item->pos.y_pos -= 12;

			if (item->pos.y_pos < bounds[2] + *(long*)&item->item_flags[2] - 256)
			{
				item->pos.y_pos = bounds[2] + *(long*)&item->item_flags[2] - 256;
				item->item_flags[0] = 0;
			}

			if (!door->Opened)
			{
				OpenThatDoor(&door->d1);
				OpenThatDoor(&door->d2);
				OpenThatDoor(&door->d1flip);
				OpenThatDoor(&door->d2flip);
				door->Opened = 1;
			}
		}
		else
		{
			if (item->pos.y_pos < item->item_flags[2])
				item->pos.y_pos += 4;

			if (item->pos.y_pos >= item->item_flags[2])
			{
				item->pos.y_pos = item->item_flags[2];

				if (door->Opened)
				{
					ShutThatDoor(&door->d1);
					ShutThatDoor(&door->d2);
					ShutThatDoor(&door->d1flip);
					ShutThatDoor(&door->d2flip);
					door->Opened = 0;
				}
			}
		}
	}
	else
	{
		if (TriggerActive(item))
		{
			if (item->current_anim_state == 0)
				item->goal_anim_state = 1;
			else
			{
				if (!door->Opened)
				{
					OpenThatDoor(&door->d1);
					OpenThatDoor(&door->d2);
					OpenThatDoor(&door->d1flip);
					OpenThatDoor(&door->d2flip);
					door->Opened = 1;
				}
			}
		}
		else
		{
			if (item->current_anim_state == 1)
				item->goal_anim_state = 0;
			else if (door->Opened)
			{
				ShutThatDoor(&door->d1);
				ShutThatDoor(&door->d2);
				ShutThatDoor(&door->d1flip);
				ShutThatDoor(&door->d2flip);
				door->Opened = 0;
			}
		}

		AnimateItem(item);
	}
}

void DoorCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;

	item = &items[item_num];

	if (item->trigger_flags == 2 && item->status != ITEM_ACTIVE && ((input & IN_ACTION || GLOBAL_inventoryitemchosen == CROWBAR_ITEM) &&
		l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH && !l->gravity_status && lara.gun_status == LG_NO_ARMS ||
		lara.IsMoving && lara.GeneralPtr == (void*)item_num))
	{
		item->pos.y_rot ^= 0x8000;

		if (TestLaraPosition(CrowbarDoorBounds, item, l))
		{
			if (!lara.IsMoving)
			{
				if (GLOBAL_inventoryitemchosen == NO_ITEM)
				{
					if (have_i_got_object(CROWBAR_ITEM))
						GLOBAL_enterinventory = CROWBAR_ITEM;
					else
					{
						if (OldPickupPos.x != l->pos.x_pos || OldPickupPos.y != l->pos.y_pos || OldPickupPos.z != l->pos.z_pos)
						{
							OldPickupPos.x = l->pos.x_pos;
							OldPickupPos.y = l->pos.y_pos;
							OldPickupPos.z = l->pos.z_pos;
							SayNo();
						}
					}

					item->pos.y_rot ^= 0x8000;
					return;
				}

				if (GLOBAL_inventoryitemchosen != CROWBAR_ITEM)
				{
					item->pos.y_rot ^= 32768;
					return;
				}
			}

			GLOBAL_inventoryitemchosen = NO_ITEM;

			if (MoveLaraPosition(&CrowbarDoorPos, item, l))
			{
				l->anim_number = ANIM_CROWBARDOOR;
				l->frame_number = anims[ANIM_CROWBARDOOR].frame_base;
				l->current_anim_state = AS_CONTROLLED;
				item->pos.y_rot ^= 0x8000;
				AddActiveItem(item_num);
				item->flags |= IFL_CODEBITS;
				item->status = ITEM_ACTIVE;
				item->goal_anim_state = 1;
				lara.IsMoving = 0;
				lara.gun_status = LG_HANDS_BUSY;
				return;
			}

			lara.GeneralPtr = (void*)item_num;
		}
		else if (lara.IsMoving && lara.GeneralPtr == (void*)item_num)
		{
			lara.IsMoving = 0;
			lara.gun_status = LG_NO_ARMS;
		}

		item->pos.y_rot ^= 0x8000;
	}

	if (TestBoundsCollide(item, l, coll->radius))
	{
		if (TestCollision(item, l))
		{
			if (coll->enable_baddie_push)
			{
				if (item->current_anim_state == item->goal_anim_state)
					ItemPushLara(item, l, coll, 0, 1);
				else
					ItemPushLara(item, l, coll, coll->enable_spaz, 1);
			}
		}
	}
}

void inject_door(bool replace)
{
	INJECT(0x0044DF60, ShutThatDoor, replace);
	INJECT(0x0044DFC0, OpenThatDoor, replace);
	INJECT(0x0044E010, DoorControl, replace);
	INJECT(0x0044E1C0, DoorCollision, replace);
}
