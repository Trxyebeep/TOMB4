#include "../tomb4/pch.h"
#include "switch.h"
#include "lara_states.h"
#include "collide.h"
#include "items.h"
#include "control.h"
#include "objects.h"
#include "draw.h"
#include "laramisc.h"

#ifdef GENERAL_FIXES
static PHD_VECTOR FullBlockSwitchPos = { 0, 256, 0 };
#else
static PHD_VECTOR FullBlockSwitchPos = { 0, 256, 100 };
#endif

static PHD_VECTOR SwitchPos = { 0, 0, 0 };
static PHD_VECTOR Switch2Pos = { 0, 0, 108 };
static PHD_VECTOR UnderwaterSwitchPos = { 0, -736, -416 };
static PHD_VECTOR UnderwaterSwitchPos2 = { 0, -736, 416 };

static short FullBlockSwitchBounds[12] = { -384, 384, 0, 256, 0, 512, -1820, 1820, -5460, 5460, -1820, 1820 };
static short SwitchBounds[12] = { 0, 0, 0, 0, 0, 0, -1820, 1820, -5460, 5460, -1820, 1820 };
static short Switch2Bounds[12] = { -1024, 1024, -1024, 1024, -1024, 512, -14560, 14560, -14560, 14560, -14560, 14560 };
static short UnderwaterSwitchBounds[12] = { -256, 256, -1280, -512, -512, 0, -14560, 14560, -14560, 14560, -14560, 14560 };
static short UnderwaterSwitchBounds2[12] = { -256, 256, -1280, -512, 0, 512, -14560, 14560, -14560, 14560, -14560, 14560 };

void FullBlockSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (input & IN_ACTION && item->status == ITEM_INACTIVE && !(item->flags & IFL_INVISIBLE) && CurrentSequence < 3 && lara.gun_status == LG_NO_ARMS &&
		l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH || lara.IsMoving && lara.GeneralPtr == (void*)item_number)
	{
		if (TestLaraPosition(FullBlockSwitchBounds, item, l))
		{
			if (MoveLaraPosition(&FullBlockSwitchPos, item, l))
			{
				if (item->current_anim_state == 1)
				{
					l->current_anim_state = AS_SWITCHON;
					l->anim_number = ANIM_BLOCKSWITCH;
					item->goal_anim_state = 0;
				}

				l->goal_anim_state = AS_STOP;
				l->frame_number = anims[l->anim_number].frame_base;
				item->status = ITEM_ACTIVE;
				AddActiveItem(item_number);
				AnimateItem(item);
				lara.IsMoving = 0;
				lara.head_y_rot = 0;
				lara.head_x_rot = 0;
				lara.torso_y_rot = 0;
				lara.torso_x_rot = 0;
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
}

long SwitchTrigger(short item_number, short timer)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->status == ITEM_DEACTIVATED)
	{
		if ((item->current_anim_state || item->object_number == JUMP_SWITCH) &&
			(item->current_anim_state != 1 || item->object_number != JUMP_SWITCH) || timer <= 0)
		{
			RemoveActiveItem(item_number);
			item->status = ITEM_INACTIVE;

			if (item->item_flags[0])
				item->flags |= IFL_INVISIBLE;
		}
		else
		{
			item->timer = timer;
			item->status = ITEM_ACTIVE;

			if (timer != 1)
				item->timer *= 30;
		}

		return 1;
	}

	if (item->status != ITEM_INACTIVE)
	{
		if (item->flags & IFL_INVISIBLE)
			return 1;
	}

	return 0;
}

long GetSwitchTrigger(ITEM_INFO* item, short* ItemNos, long AttatchedToSwitch)
{
	FLOOR_INFO* floor;
	short* data;
	long num;

	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &item->room_number);
	GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	if (!trigger_index)
		return 0;

	data = trigger_index;

	while ((*data & 0x1F) != TRIGGER_TYPE && !(*data & 0x8000)) data++;	//get to trigger

	if (!(*data & TRIGGER_TYPE))	//no triggers, bye
		return 0;

	data += 2;
	num = 0;

	while (1)
	{
		if ((*data & 0x3C00) == TO_OBJECT && item != &items[*data & 0x3FF])
		{
			*ItemNos++ = *data & 0x3FF;
			num++;
		}

		if (*data & 0x8000)	//gottem all
			break;

		data++;
	}

	return num;
}

void TestTriggersAtXYZ(long x, long y, long z, short room_number, short heavy, short flags)
{
	GetHeight(GetFloor(x, y, z, &room_number), x, y, z);
	TestTriggers(trigger_index, heavy, flags);
}

void SwitchControl(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->flags |= IFL_CODEBITS;

	if (!TriggerActive(item) && !(item->flags & IFL_INVISIBLE))
	{
		if (item->object_number == JUMP_SWITCH)
			item->goal_anim_state = 0;
		else
			item->goal_anim_state = 1;

		item->timer = 0;
	}

	AnimateItem(item);
}

void SwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	short* bounds;

	item = &items[item_number];

	if (input & IN_ACTION && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH && lara.gun_status == LG_NO_ARMS
		&& item->status == ITEM_INACTIVE && !(item->flags & IFL_INVISIBLE) && item->trigger_flags >= 0
		|| lara.IsMoving && lara.GeneralPtr == (void*)item_number)
	{
		bounds = GetBoundsAccurate(item);

		if (item->trigger_flags == 3 && item->current_anim_state == 1)
			return;

		SwitchBounds[0] = bounds[0] - 256;
		SwitchBounds[1] = bounds[1] + 256;

		if (item->trigger_flags)
		{
			SwitchBounds[4] = bounds[4] - 512;
			SwitchBounds[5] = bounds[5] + 512;

			if (item->trigger_flags == 3)
				SwitchPos.z = bounds[4] - 256;
			else
				SwitchPos.z = bounds[4] - 128;
		}
		else
		{
			SwitchBounds[4] = bounds[4] - 200;
			SwitchBounds[5] = bounds[5] + 200;
			SwitchPos.z = bounds[4] - 64;
		}

		if (TestLaraPosition(SwitchBounds, item, l))
		{
			if (MoveLaraPosition(&SwitchPos, item, l))
			{
				if (item->current_anim_state == 1)
				{
					if (item->trigger_flags)
					{
						l->anim_number = ANIM_HIDDENPICKUP;
						l->current_anim_state = AS_HIDDENPICKUP;
					}
					else
					{
						l->anim_number = ANIM_SWITCHOFF;
						l->current_anim_state = AS_SWITCHOFF;
					}

					item->goal_anim_state = 0;
				}
				else
				{
					if (!item->trigger_flags)
					{
						l->anim_number = ANIM_SWITCHON;
						l->current_anim_state = AS_SWITCHON;
					}
					else if (item->trigger_flags == 3)
						l->anim_number = ANIM_SMALLSWITCH;
					else
					{
						l->anim_number = ANIM_HIDDENPICKUP;
						l->current_anim_state = AS_HIDDENPICKUP;
					}

					item->goal_anim_state = 1;
				}

				l->frame_number = anims[l->anim_number].frame_base;
				lara.IsMoving = 0;
				lara.gun_status = LG_HANDS_BUSY;
				lara.head_x_rot = 0;
				lara.head_y_rot = 0;
				lara.torso_x_rot = 0;
				lara.torso_y_rot = 0;
				AddActiveItem(item_number);
				item->status = ITEM_ACTIVE;
				AnimateItem(item);
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
	else if (l->current_anim_state != AS_SWITCHON && l->current_anim_state != AS_SWITCHOFF)
		ObjectCollision(item_number, l, coll);
}

void SwitchCollision2(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (input & IN_ACTION && item->status == ITEM_INACTIVE && lara.water_status == LW_UNDERWATER &&
		lara.gun_status == LG_NO_ARMS && l->current_anim_state == AS_TREAD)
	{
		if (TestLaraPosition(Switch2Bounds, item, l))
		{
			if (!item->current_anim_state || item->current_anim_state == 1)
			{
				if (MoveLaraPosition(&Switch2Pos, item, l))
				{
					l->fallspeed = 0;
					l->goal_anim_state = AS_SWITCHON;

					do AnimateLara(l); while (l->current_anim_state != AS_SWITCHON);

					l->goal_anim_state = AS_TREAD;
					lara.gun_status = LG_HANDS_BUSY;
					item->goal_anim_state = item->current_anim_state != 1;
					item->status = ITEM_ACTIVE;
					AddActiveItem(item_number);
					AnimateItem(item);
				}
			}
		}
	}
}

void UnderwaterSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	long flag;

	item = &items[item_number];

	if (input & IN_ACTION && lara.water_status == LW_UNDERWATER && l->current_anim_state == AS_TREAD && l->anim_number == ANIM_TREAD &&
		lara.gun_status == LG_NO_ARMS && item->current_anim_state == 0 || lara.IsMoving && lara.GeneralPtr == (void*)item_number)
	{
		flag = 0;

		if (TestLaraPosition(UnderwaterSwitchBounds, item, l))
		{
			if (!MoveLaraPosition(&UnderwaterSwitchPos, item, l))
				lara.GeneralPtr = (void*)item_number;
			else
				flag = 1;
		}
		else
		{
			l->pos.y_rot ^= 0x8000;

			if (TestLaraPosition(UnderwaterSwitchBounds2, item, l))
			{
				if (MoveLaraPosition(&UnderwaterSwitchPos2, item, l))
					flag = 1;
				else
					lara.GeneralPtr = (void*)item_number;
			}

			l->pos.y_rot ^= 0x8000;
		}

		if (flag)
		{
			l->anim_number = ANIM_WATERSWITCH;
			l->frame_number = anims[ANIM_WATERSWITCH].frame_base;
			l->current_anim_state = AS_SWITCHON;
			l->fallspeed = 0;
			lara.IsMoving = 0;
			lara.gun_status = LG_HANDS_BUSY;
			item->goal_anim_state = 1;
			item->status = ITEM_ACTIVE;
			AddActiveItem(item_number);
			ForcedFixedCamera.x = item->pos.x_pos - ((1024 * phd_sin(item->pos.y_rot + 16380)) >> W2V_SHIFT);
			ForcedFixedCamera.y = item->pos.y_pos - 1024;
			ForcedFixedCamera.z = item->pos.z_pos - ((1024 * phd_cos(item->pos.y_rot + 16380)) >> W2V_SHIFT);
			ForcedFixedCamera.room_number = item->room_number;
		}
	}
}

void inject_switch(bool replace)
{
	INJECT(0x00463180, FullBlockSwitchCollision, replace);
	INJECT(0x00461B10, SwitchTrigger, replace);
	INJECT(0x00461BD0, GetSwitchTrigger, replace);
	INJECT(0x00461CA0, TestTriggersAtXYZ, replace);
	INJECT(0x00461CF0, SwitchControl, replace);
	INJECT(0x00461D60, SwitchCollision, replace);
	INJECT(0x00461FF0, SwitchCollision2, replace);
	INJECT(0x00462100, UnderwaterSwitchCollision, replace);
}
