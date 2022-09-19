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
#include "box.h"
#include "../specific/input.h"
#include "lara.h"
#include "switch.h"
#include "lot.h"

static PHD_VECTOR CrowbarDoorPos = { -412, 0, 140 };
static PHD_VECTOR PullDoorPos = { -201, 0, 322 };
static PHD_VECTOR PushDoorPos = { 201, 0, -702 };
static PHD_VECTOR KickDoorPos = { 0, 0, -917 };
static PHD_VECTOR DoubleDoorPos = { 0, 0, 220 };
static PHD_VECTOR UnderwaterDoorPos = { -251, -760, -46 };
static short CrowbarDoorBounds[12] = { -512, 512, -1024, 0, 0, 512, -14560, 14560, -14560, 14560, -14560, 14560 };
static short PushPullKickDoorBounds[12] = { -384, 384, 0, 0, -1024, 512, -1820, 1820, -5460, 5460, -1820, 1820 };
static short UnderwaterDoorBounds[12] = { -256, 256, -1024, 0, -1024, 0, -14560, 14560, -14560, 14560, -14560, 14560 };

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
			boxes[d->block].overlap_index &= ~0x4000;

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
					item->pos.y_rot ^= 0x8000;
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

void PushPullKickDoorControl(short item_number)
{
	ITEM_INFO* item;
	DOOR_DATA* door;

	item = &items[item_number];
	door = (DOOR_DATA*)item->data;

	if (!door->Opened)
	{
		OpenThatDoor(&door->d1);
		OpenThatDoor(&door->d2);
		OpenThatDoor(&door->d1flip);
		OpenThatDoor(&door->d2flip);
		door->Opened = 1;
	}

	AnimateItem(item);
}

void PushPullKickDoorCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	long pull, goin;

	item = &items[item_num];

	if (input & IN_ACTION && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH && item->status != ITEM_ACTIVE &&
		!l->gravity_status && lara.gun_status == LG_NO_ARMS || lara.IsMoving && lara.GeneralPtr == (void*)item_num)
	{
		pull = 0;

		if (l->room_number == item->room_number)
		{
			item->pos.y_rot ^= 0x8000;
			pull = 1;
		}

		if (TestLaraPosition(PushPullKickDoorBounds, item, l))
		{
			goin = 0;

			if (pull)
			{
				if (MoveLaraPosition(&PullDoorPos, item, l))
				{
					l->anim_number = ANIM_PULLDOOR;
					l->frame_number = anims[ANIM_PULLDOOR].frame_base;
					item->goal_anim_state = 3;
					goin = 1;
				}
				else
					lara.GeneralPtr = (void*)item_num;
			}
			else
			{
				if (item->object_number < KICK_DOOR1)
				{
					if (MoveLaraPosition(&PushDoorPos, item, l))
					{
						l->anim_number = ANIM_PUSHDOOR;
						l->frame_number = anims[ANIM_PUSHDOOR].frame_base;
						item->goal_anim_state = 2;
						goin = 1;
					}
					else
						lara.GeneralPtr = (void*)item_num;
				}
				else
				{
					if (MoveLaraPosition(&KickDoorPos, item, l))
					{
						l->anim_number = ANIM_KICKDOOR;
						l->frame_number = anims[ANIM_KICKDOOR].frame_base;
						item->goal_anim_state = 2;
						goin = 1;
					}
					else
						lara.GeneralPtr = (void*)item_num;
				}
			}

			if (goin)
			{
				AddActiveItem(item_num);
				item->status = ITEM_ACTIVE;
				l->current_anim_state = AS_CONTROLLED;
				l->goal_anim_state = AS_STOP;
				lara.IsMoving = 0;
				lara.gun_status = LG_HANDS_BUSY;
			}
		}
		else if (lara.IsMoving && lara.GeneralPtr == (void*)item_num)
		{
			lara.IsMoving = 0;
			lara.gun_status = LG_NO_ARMS;
		}

		if (pull)
			item->pos.y_rot ^= 0x8000;
	}
	else if (!item->current_anim_state)
		DoorCollision(item_num, l, coll);
}

void DoubleDoorCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;

	item = &items[item_num];

	if (input & IN_ACTION && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH && item->status != ITEM_ACTIVE &&
		!l->gravity_status && lara.gun_status == LG_NO_ARMS || lara.IsMoving && lara.GeneralPtr == (void*)item_num)
	{
		item->pos.y_rot ^= 0x8000;

		if (TestLaraPosition(PushPullKickDoorBounds, item, l))
		{
			if (MoveLaraPosition(&DoubleDoorPos, item, l))
			{
				l->anim_number = ANIM_TWODOOR;
				l->frame_number = anims[ANIM_TWODOOR].frame_base;
				l->current_anim_state = AS_TWODOOR;
				AddActiveItem(item_num);
				item->status = ITEM_ACTIVE;
				lara.IsMoving = 0;
				lara.gun_status = LG_HANDS_BUSY;
				lara.head_x_rot = 0;
				lara.head_y_rot = 0;
				lara.torso_x_rot = 0;
				lara.torso_y_rot = 0;
			}
			else
				lara.GeneralPtr = (void*)item_num;
		}
		else if (lara.IsMoving && lara.GeneralPtr == (void*)item_num)
		{
			lara.IsMoving = 0;
			lara.gun_status = LG_NO_ARMS;
		}

		item->pos.y_rot ^= 0x8000;
	}
}

void UnderwaterDoorCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;

	item = &items[item_num];

	if (input & IN_ACTION && item->status != ITEM_ACTIVE && l->current_anim_state == AS_TREAD && lara.water_status == LW_UNDERWATER &&
		lara.gun_status == LG_NO_ARMS || lara.IsMoving && lara.GeneralPtr == (void*)item_num)
	{
		l->pos.y_rot ^= 0x8000;

		if (TestLaraPosition(UnderwaterDoorBounds, item, l))
		{
			if (MoveLaraPosition(&UnderwaterDoorPos, item, l))
			{
				l->anim_number = ANIM_WATERDOOR;
				l->frame_number = anims[ANIM_WATERDOOR].frame_base;
				l->current_anim_state = AS_CONTROLLED;
				l->fallspeed = 0;
				item->status = ITEM_ACTIVE;
				AddActiveItem(item_num);
				item->goal_anim_state = 1;
				AnimateItem(item);
				lara.IsMoving = 0;
				lara.gun_status = LG_HANDS_BUSY;
			}
			else
				lara.GeneralPtr = (void*)item_num;
		}
		else if (lara.IsMoving && lara.GeneralPtr == (void*)item_num)
		{
			lara.IsMoving = 0;
			lara.gun_status = LG_NO_ARMS;
		}

		l->pos.y_rot ^= 0x8000;
	}
	else if (item->status == ITEM_ACTIVE)
		ObjectCollision(item_num, l, coll);
}

void SequenceDoorControl(short item_number)
{
	ITEM_INFO* item;
	DOOR_DATA* door;

	item = &items[item_number];
	door = (DOOR_DATA*)item->data;

	if (item->item_flags[0])
	{
		if (TriggerActive(item))
		{
			if (!item->current_anim_state)
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

				if (CurrentSequence == 3)
				{
					if (SequenceResults[Sequences[0]][Sequences[1]][Sequences[2]] == item->trigger_flags &&
						!Sequences[0] && Sequences[1] == 1 && Sequences[2] == 2)
					{
						CurrentSequence = 4;
						SequenceUsed[item->trigger_flags] = Sequences[1];
					}
				}
				else if ((CurrentSequence == 1 || CurrentSequence == 2) && item->trigger_flags == 2)
				{
					item->flags &= ~(IFL_INVISIBLE | IFL_ANTITRIGGER_ONESHOT);
					item->goal_anim_state = 0;
					item->item_flags[0] = 0;
				}
			}
		}
		else
		{
			if (item->current_anim_state == 1)
				item->goal_anim_state = 0;
			else
			{
				if (CurrentSequence == 3 && SequenceResults[Sequences[0]][Sequences[1]][Sequences[2]] == item->trigger_flags)
				{
					CurrentSequence = 4;

					if (item->trigger_flags != 2)
						SequenceUsed[item->trigger_flags] = 1;
				}

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
	else if (!item->current_anim_state && CurrentSequence == 3 && SequenceResults[Sequences[0]][Sequences[1]][Sequences[2]] == item->trigger_flags)
	{
		if (item->trigger_flags && item->trigger_flags != 2 && !SequenceUsed[0])
		{
			Sequences[1] = 0;
			Sequences[0] = 1;
			Sequences[2] = 2;
			return;
		}

		item->goal_anim_state = 1;
		item->item_flags[0] = 1;
	}

	AnimateItem(item);
}
