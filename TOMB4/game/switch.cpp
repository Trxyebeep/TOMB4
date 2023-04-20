#include "../tomb4/pch.h"
#include "switch.h"
#include "lara_states.h"
#include "collide.h"
#include "items.h"
#include "control.h"
#include "objects.h"
#include "draw.h"
#include "laramisc.h"
#include "sound.h"
#include "../specific/3dmath.h"
#include "camera.h"
#include "../specific/input.h"
#include "lara.h"
#include "newinv.h"

static PHD_VECTOR FullBlockSwitchPos = { 0, 256, 0 };
static PHD_VECTOR SwitchPos = { 0, 0, 0 };
static PHD_VECTOR Switch2Pos = { 0, 0, 108 };
static PHD_VECTOR UnderwaterSwitchPos = { 0, -736, -416 };
static PHD_VECTOR UnderwaterSwitchPos2 = { 0, -736, 416 };
static PHD_VECTOR PulleyPos = { 0, 0, -148 };
static PHD_VECTOR TurnSwitchPos = { 650, 0, 138 };
static PHD_VECTOR TurnSwitchPosA = { 650, 0, -138 };
static PHD_VECTOR RailSwitchPos = { 0, 0, -550 };
static PHD_VECTOR RailSwitchPos2 = { 0, 0, 550 };
static PHD_VECTOR JumpSwitchPos = { 0, -208, 256 };
static PHD_VECTOR CrowbarPos = { -89, 0, -328 };
static PHD_VECTOR CrowbarPos2 = { 89, 0, 328 };
static PHD_VECTOR CogSwitchPos = { 0, 0, -856 };

static short FullBlockSwitchBounds[12] = { -384, 384, 0, 256, 0, 512, -1820, 1820, -5460, 5460, -1820, 1820 };
static short SwitchBounds[12] = { 0, 0, 0, 0, 0, 0, -1820, 1820, -5460, 5460, -1820, 1820 };
static short Switch2Bounds[12] = { -1024, 1024, -1024, 1024, -1024, 512, -14560, 14560, -14560, 14560, -14560, 14560 };
static short UnderwaterSwitchBounds[12] = { -256, 256, -1280, -512, -512, 0, -14560, 14560, -14560, 14560, -14560, 14560 };
static short UnderwaterSwitchBounds2[12] = { -256, 256, -1280, -512, 0, 512, -14560, 14560, -14560, 14560, -14560, 14560 };
static short PulleyBounds[12] = { -256, 256, 0, 0, -512, 512, -1820, 1820, -5460, 5460, -1820, 1820 };
static short TurnSwitchBoundsA[12] = { 512, 896, 0, 0, -512, 0, -1820, 1820, -5460, 5460, -1820, 1820 };
static short TurnSwitchBoundsC[12] = { 512, 896, 0, 0, 0, 512, -1820, 1820, -5460, 5460, -1820, 1820 };
static short RailSwitchBounds[12] = { -256, 256, 0, 0, -768, -224, -1820, 1820, -5460, 5460, -1820, 1820 };
static short RailSwitchBounds2[12] = { -256, 256, 0, 0, 224, 768, -1820, 1820, -5460, 5460, -1820, 1820 };
static short JumpSwitchBounds[12] = { -128, 128, -256, 256, 384, 512, -1820, 1820, -5460, 5460, -1820, 1820 };
static short CrowbarBounds[12] = { -256, 256, 0, 0, -512, -256, -1820, 1820, -5460, 5460, -1820, 1820 };
static short CrowbarBounds2[12] = { -256, 256, 0, 0, 256, 512, -1820, 1820, -5460, 5460, -1820, 1820 };
static short CogSwitchBounds[12] = { -512, 512, 0, 0, -1536, -512, -1820, 1820, -5460, 5460, -1820, 1820 };

PHD_VECTOR OldPickupPos;
uchar CurrentSequence;
uchar Sequences[3];
uchar SequenceUsed[6];
uchar SequenceResults[3][3][3];

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

void SwitchType78Collision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{

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

void PulleyCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	short roty;

	item = &items[item_number];

	if (!(item->flags & IFL_INVISIBLE) && (input & IN_ACTION && lara.gun_status == LG_NO_ARMS && l->current_anim_state == AS_STOP &&
		l->anim_number == ANIM_BREATH && !l->gravity_status || lara.IsMoving && lara.GeneralPtr == (void*)item_number))
	{
		roty = item->pos.y_rot;
		item->pos.y_rot = l->pos.y_rot;

		if (TestLaraPosition(PulleyBounds, item, l))
		{
			if (item->item_flags[1])
			{
				if (OldPickupPos.x != l->pos.x_pos || OldPickupPos.y != l->pos.y_pos || OldPickupPos.z != l->pos.z_pos)
				{
					OldPickupPos.x = l->pos.x_pos;
					OldPickupPos.y = l->pos.y_pos;
					OldPickupPos.z = l->pos.z_pos;
					SayNo();
				}
			}
			else
			{
				if (MoveLaraPosition(&PulleyPos, item, l))
				{
					l->anim_number = ANIM_STAT2PULLEY;
					l->frame_number = anims[ANIM_STAT2PULLEY].frame_base;
					l->current_anim_state = AS_PULLEY;
					AddActiveItem(item_number);
					item->status = ITEM_ACTIVE;
					item->pos.y_rot = roty;
					lara.IsMoving = 0;
					lara.head_x_rot = 0;
					lara.head_y_rot = 0;
					lara.torso_x_rot = 0;
					lara.torso_y_rot = 0;
					lara.gun_status = LG_HANDS_BUSY;
					lara.GeneralPtr = item;
				}
				else
					lara.GeneralPtr = (void*)item_number;
			}
		}
		else if (lara.IsMoving && lara.GeneralPtr == (void*)item_number)
		{
			lara.IsMoving = 0;
			lara.gun_status = 0;
		}

		item->pos.y_rot = roty;
	}
	else if (l->current_anim_state != AS_PULLEY)
		ObjectCollision(item_number, l, coll);
}

void TurnSwitchControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* l;

	item = &items[item_number];
	l = lara_item;

	if (item->item_flags[0] == 2)
	{
		if (item->anim_number == objects[TURN_SWITCH].anim_index + 2)
		{
			item->pos.y_rot += 0x4000;

			if (input & IN_ACTION)
			{
				l->anim_number = ANIM_TURNSWITCHCB;
				l->frame_number = anims[ANIM_TURNSWITCHCB].frame_base;
				item->anim_number = objects[item->object_number].anim_index + 1;
				item->frame_number = anims[item->anim_number].frame_base;
			}
		}

		if (l->anim_number == ANIM_TURNSWITCHCD && l->frame_number == anims[ANIM_TURNSWITCHCD].frame_end && !item->item_flags[1])
			item->item_flags[1] = 1;

		if (l->frame_number >= anims[ANIM_TURNSWITCHCB].frame_base && l->frame_number <= anims[ANIM_TURNSWITCHCB].frame_base + 43 ||
			l->frame_number >= anims[ANIM_TURNSWITCHCB].frame_base + 58 && l->frame_number <= anims[ANIM_TURNSWITCHCB].frame_base + 115)
			SoundEffect(SFX_PUSHABLE_SOUND, &item->pos, SFX_ALWAYS);
	}
	else
	{
		if (item->anim_number == objects[TURN_SWITCH].anim_index + 6)
		{
			item->pos.y_rot -= 0x4000;

			if (input & IN_ACTION)
			{
				l->anim_number = ANIM_TURNSWITCHAB;
				l->frame_number = anims[ANIM_TURNSWITCHAB].frame_base;
				item->anim_number = objects[item->object_number].anim_index + 5;
				item->frame_number = anims[item->anim_number].frame_base;
			}
		}

		if (l->anim_number == ANIM_TURNSWITCHAD && l->frame_number == anims[ANIM_TURNSWITCHAD].frame_end && !item->item_flags[1])
			item->item_flags[1] = 1;

		if (l->frame_number >= anims[ANIM_TURNSWITCHAB].frame_base && l->frame_number <= anims[ANIM_TURNSWITCHAB].frame_base + 43 ||
			l->frame_number >= anims[ANIM_TURNSWITCHAB].frame_base + 58 && l->frame_number <= anims[ANIM_TURNSWITCHAB].frame_base + 115)
			SoundEffect(SFX_PUSHABLE_SOUND, &item->pos, SFX_ALWAYS);
	}

	AnimateItem(item);

	if (item->item_flags[1] == 1)
	{
		l->anim_number = ANIM_BREATH;
		l->frame_number = anims[ANIM_BREATH].frame_base;
		l->current_anim_state = AS_STOP;
		item->status = ITEM_INACTIVE;
		item->anim_number = objects[item->object_number].anim_index;
		item->frame_number = anims[item->anim_number].frame_base;
		RemoveActiveItem(item_number);
		lara.gun_status = LG_NO_ARMS;
		UseForcedFixedCamera = 0;
		item->item_flags[1] = 2;
	}
}

void TurnSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	long flag;
	short Triggers[8];

	flag = 0;
	item = &items[item_number];

	if (!item->current_anim_state && input & IN_ACTION && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH
		&& !l->gravity_status && lara.gun_status == LG_NO_ARMS || lara.IsMoving && lara.GeneralPtr == (void*)item_number)
	{
		if (TestLaraPosition(TurnSwitchBoundsA, item, l))
		{
			if (MoveLaraPosition(&TurnSwitchPosA, item, l))
			{
				l->anim_number = ANIM_TURNSWITCHA;
				l->frame_number = anims[ANIM_TURNSWITCHA].frame_base;
				item->anim_number = objects[item->object_number].anim_index + 4;
				item->frame_number = anims[item->anim_number].frame_base;
				item->item_flags[0] = 1;
				flag = -1;
				ForcedFixedCamera.x = item->pos.x_pos - ((1024 * phd_sin(item->pos.y_rot)) >> 14);
				ForcedFixedCamera.z = item->pos.z_pos - ((1024 * phd_cos(item->pos.y_rot)) >> 14);
			}
			else
				lara.GeneralPtr = (void*)item_number;
		}
		else
		{
			l->pos.y_rot ^= 0x8000;

			if (TestLaraPosition(TurnSwitchBoundsC, item, l))
			{
				if (MoveLaraPosition(&TurnSwitchPos, item, l))
				{
					flag = 1;
					l->anim_number = ANIM_TURNSWITCHC;
					l->frame_number = anims[ANIM_TURNSWITCHC].frame_base;
					item->item_flags[0] = 2;
					ForcedFixedCamera.x = item->pos.x_pos + ((1024 * phd_sin(item->pos.y_rot)) >> 14);
					ForcedFixedCamera.z = item->pos.z_pos + ((1024 * phd_cos(item->pos.y_rot)) >> 14);
				}
				else
					lara.GeneralPtr = (void*)item_number;
			}
			else if (lara.IsMoving && lara.GeneralPtr == (void*)item_number)
			{
				lara.IsMoving = 0;
				lara.gun_status = LG_NO_ARMS;
			}

			l->pos.y_rot ^= 0x8000;
		}
	}

	if (!flag)
	{
		GlobalCollisionBounds[0] = -512;
		GlobalCollisionBounds[1] = 512;
		GlobalCollisionBounds[2] = -512;
		GlobalCollisionBounds[3] = 0;
		GlobalCollisionBounds[4] = -512;
		GlobalCollisionBounds[5] = 512;
		ItemPushLara(item, l, coll, 0, 2);
		GlobalCollisionBounds[0] = 256;
		GlobalCollisionBounds[1] = 1024;
		GlobalCollisionBounds[4] = -128;
		GlobalCollisionBounds[5] = 128;
		ItemPushLara(item, l, coll, 0, 2);
	}
	else
	{
		lara.IsMoving = 0;
		lara.head_y_rot = 0;
		lara.head_x_rot = 0;
		lara.torso_y_rot = 0;
		lara.torso_x_rot = 0;
		lara.gun_status = LG_HANDS_BUSY;
		l->current_anim_state = AS_TURNSWITCH;
		UseForcedFixedCamera = 1;
		ForcedFixedCamera.y = item->pos.y_pos - 2048;
		ForcedFixedCamera.room_number = item->room_number;
		AddActiveItem(item_number);
		item->status = ITEM_ACTIVE;
		item->item_flags[1] = 0;

		if (GetSwitchTrigger(item, Triggers, 0))
		{
			item = &items[*Triggers];

			if (!TriggerActive(item))
			{
				if (flag >= 0)
					item->anim_number = objects[item->object_number].anim_index + 4;
				else
					item->anim_number = objects[item->object_number].anim_index;

				item->frame_number = anims[item->anim_number].frame_base;
			}
		}
	}
}

void RailSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	long flag;

	flag = 0;
	item = &items[item_number];

	if (input & IN_ACTION && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH &&
		lara.gun_status == LG_NO_ARMS || lara.IsMoving && lara.GeneralPtr == (void*)item_number)
	{
		if (item->current_anim_state == 1)
		{
			l->pos.y_rot ^= 0x8000;

			if (TestLaraPosition(RailSwitchBounds2, item, l))
			{
				if (MoveLaraPosition(&RailSwitchPos2, item, l))
				{
					item->goal_anim_state = 0;
					flag = 1;
				}
				else
					lara.GeneralPtr = (void*)item_number;
			}
			else if (lara.IsMoving && lara.GeneralPtr == (void*)item_number)
			{
				lara.IsMoving = 0;
				lara.gun_status = LG_NO_ARMS;
			}

			l->pos.y_rot ^= 0x8000;
		}
		else if (item->current_anim_state == 0)
		{
			if (TestLaraPosition(RailSwitchBounds, item, l))
			{
				if (MoveLaraPosition(&RailSwitchPos, item, l))
				{
					item->goal_anim_state = 1;
					flag = 1;
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

	if (flag)
	{
		l->anim_number = ANIM_RAILSWITCH;
		l->frame_number = anims[ANIM_RAILSWITCH].frame_base;
		l->current_anim_state = AS_RAILSWITCH;
		l->goal_anim_state = AS_RAILSWITCH;
		lara.IsMoving = 0;
		lara.head_x_rot = 0;
		lara.head_y_rot = 0;
		lara.torso_x_rot = 0;
		lara.torso_y_rot = 0;
		lara.gun_status = LG_HANDS_BUSY;
		item->status = ITEM_ACTIVE;
		AddActiveItem(item_number);
		AnimateItem(item);
		return;
	}

	if (!lara.IsMoving)
		ObjectCollision(item_number, l, coll);
}

void JumpSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (input & IN_ACTION && lara.gun_status == LG_NO_ARMS && (l->current_anim_state == AS_REACH || l->current_anim_state == AS_UPJUMP) &&
		l->gravity_status && l->fallspeed > 0 && !item->current_anim_state)
	{
		if (TestLaraPosition(JumpSwitchBounds, item, l))
		{
			AlignLaraPosition(&JumpSwitchPos, item, l);
			l->anim_number = ANIM_LEAPSWITCH;
			l->frame_number = anims[ANIM_LEAPSWITCH].frame_base;
			l->current_anim_state = AS_SWITCHON;
			l->fallspeed = 0;
			l->gravity_status = 0;
			lara.gun_status = LG_HANDS_BUSY;
			item->status = ITEM_ACTIVE;
			item->goal_anim_state = 1;
			AddActiveItem(item_number);
		}
	}
}

void CrowbarSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	long flag;

	flag = 0;
	item = &items[item_number];

	if (input & IN_ACTION || GLOBAL_inventoryitemchosen == CROWBAR_ITEM && l->current_anim_state == AS_STOP &&
		l->anim_number == ANIM_BREATH && lara.gun_status == LG_NO_ARMS && !item->item_flags[0] ||
		lara.IsMoving && lara.GeneralPtr == (void*)item_number)
	{
		if (item->current_anim_state == 1)
		{
			l->pos.y_rot ^= 0x8000;

			if (TestLaraPosition(CrowbarBounds2, item, l))
			{
				if (lara.IsMoving || GLOBAL_inventoryitemchosen == CROWBAR_ITEM)
				{
					if (MoveLaraPosition(&CrowbarPos2, item, l))
					{
						flag = 1;
						l->anim_number = ANIM_CROWSWITCH;
						l->frame_number = anims[ANIM_CROWSWITCH].frame_base;
						item->goal_anim_state = 0;
					}
					else
						lara.GeneralPtr = (void*)item_number;

					GLOBAL_inventoryitemchosen = NO_ITEM;
				}
				else
					flag = -1;
			}
			else if (lara.IsMoving && lara.GeneralPtr == (void*)item_number)
			{
				lara.IsMoving = 0;
				lara.gun_status = LG_NO_ARMS;
			}

			l->pos.y_rot ^= 0x8000;
		}
		else if (!item->current_anim_state)
		{
			if (TestLaraPosition(CrowbarBounds, item, l))
			{
				if (lara.IsMoving || GLOBAL_inventoryitemchosen == CROWBAR_ITEM)
				{
					if (MoveLaraPosition(&CrowbarPos, item, l))
					{
						flag = 1;
						l->anim_number = ANIM_CROWSWITCH;
						l->frame_number = anims[ANIM_CROWSWITCH].frame_base;
						item->goal_anim_state = 1;
					}
					else
						lara.GeneralPtr = (void*)item_number;

					GLOBAL_inventoryitemchosen = NO_ITEM;
				}
				else
					flag = -1;
			}
			else if (lara.IsMoving && lara.GeneralPtr == (void*)item_number)
			{
				lara.IsMoving = 0;
				lara.gun_status = LG_NO_ARMS;
			}
		}
	}

	if (!flag)
		ObjectCollision(item_number, l, coll);
	else
	{
		if (flag != -1)
		{
			l->current_anim_state = AS_SWITCHON;
			l->goal_anim_state = AS_SWITCHON;
			lara.IsMoving = 0;
			lara.head_x_rot = 0;
			lara.head_y_rot = 0;
			lara.torso_x_rot = 0;
			lara.torso_y_rot = 0;
			lara.gun_status = LG_HANDS_BUSY;
			item->status = ITEM_ACTIVE;
			AddActiveItem(item_number);
			AnimateItem(item);
		}
		else
		{
			if (lara.crowbar)
				GLOBAL_enterinventory = CROWBAR_ITEM;
			else if (OldPickupPos.x != l->pos.x_pos || OldPickupPos.y != l->pos.y_pos || OldPickupPos.z != l->pos.z_pos)
			{
				OldPickupPos.x = l->pos.x_pos;
				OldPickupPos.y = l->pos.y_pos;
				OldPickupPos.z = l->pos.z_pos;
				SayNo();
			}
		}
	}
}

void FullBlockSwitchControl(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->anim_number != objects[item->object_number].anim_index + 2 || CurrentSequence >= 3 || item->item_flags[0])
	{
		if (CurrentSequence >= 4)
		{
			item->item_flags[0] = 0;
			item->goal_anim_state = 1;
			item->status = ITEM_INACTIVE;
			CurrentSequence++;

			if (CurrentSequence >= 7)
				CurrentSequence = 0;
		}
	}
	else
	{
		item->item_flags[0] = 1;
		Sequences[CurrentSequence] = (uchar)item->trigger_flags;
		CurrentSequence++;

		if (CurrentSequence == 3 && SequenceUsed[SequenceResults[Sequences[0]][Sequences[1]][Sequences[2]]])
			CurrentSequence++;
	}

	AnimateItem(item);
}

void CogSwitchControl(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	AnimateItem(item);

	if (item->current_anim_state == 1)
	{
		if (item->goal_anim_state == 1 && !(input & IN_ACTION))
		{
			lara_item->goal_anim_state = AS_STOP;
			item->goal_anim_state = 0;
		}

		if (lara_item->anim_number == ANIM_COGSWITCHL && lara_item->frame_number == anims[ANIM_COGSWITCHL].frame_base + 10)
		{
			item = (ITEM_INFO*)lara.GeneralPtr;
			item->item_flags[0] = 40;
		}
	}
	else if (item->frame_number == anims[item->anim_number].frame_end)
	{
		item->current_anim_state = 0;
		item->status = ITEM_INACTIVE;
		RemoveActiveItem(item_number);
		lara_item->anim_number = ANIM_STOP;
		lara_item->frame_number = anims[ANIM_STOP].frame_base;
		lara_item->current_anim_state = AS_STOP;
		lara_item->goal_anim_state = AS_STOP;
		lara.gun_status = LG_NO_ARMS;
	}
}

void CogSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	ITEM_INFO* door_item;
	DOOR_DATA* door;
	short* data;

	item = &items[item_number];
	GetHeight(GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &item->room_number),
		item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	data = trigger_index;

	while ((*data & 0x1F) != TRIGGER_TYPE && !(*data & 0x8000)) data++;

	door_item = &items[data[3] & 0x3FF];
	door = (DOOR_DATA*)door_item->data;

	if (item->status != ITEM_INACTIVE)
		return;

	if (!(item->flags & IFL_INVISIBLE) && (input & IN_ACTION && lara.gun_status == LG_NO_ARMS && !l->gravity_status &&
		l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH || lara.IsMoving && lara.GeneralPtr == (void*)item_number))
	{
		if (TestLaraPosition(CogSwitchBounds, item, l))
		{
			if (MoveLaraPosition(&CogSwitchPos, item, l))
			{
				lara.IsMoving = 0;
				lara.head_x_rot = 0;
				lara.head_y_rot = 0;
				lara.torso_x_rot = 0;
				lara.torso_y_rot = 0;
				lara.gun_status = LG_HANDS_BUSY;
				lara.GeneralPtr = door_item;
				l->anim_number = ANIM_COGSWITCHS;
				l->frame_number = anims[ANIM_COGSWITCHS].frame_base;
				l->current_anim_state = AS_COGSWITCH;
				l->goal_anim_state = AS_COGSWITCH;
				AddActiveItem(item_number);
				item->status = ITEM_ACTIVE;
				item->goal_anim_state = 1;

				if (!door->Opened)
				{
					AddActiveItem(door_item - items);
					door_item->status = ITEM_ACTIVE;
					*(long*)&door_item->item_flags[2] = door_item->pos.y_pos;
				}
			}
			else
				lara.GeneralPtr = (void*)item_number;

			return;
		}

		if (lara.IsMoving && lara.GeneralPtr == (void*)item_number)
		{
			lara.IsMoving = 0;
			lara.gun_status = LG_NO_ARMS;
		}
	}

	ObjectCollision(item_number, l, coll);
}
