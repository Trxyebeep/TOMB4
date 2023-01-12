#include "../tomb4/pch.h"
#include "baboon.h"
#include "tomb4fx.h"
#include "objects.h"
#include "box.h"
#include "control.h"
#include "items.h"
#include "lot.h"
#include "../specific/function_stubs.h"
#include "switch.h"
#include "../specific/3dmath.h"
#include "effects.h"
#include "lara.h"

static BITE_INFO baboon_hit = { 10, 10, 11, 4 };

void InitialiseBaboon(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[BABOON_NORMAL].anim_index + 2;
	item->frame_number = anims[item->anim_number].frame_base;
	item->goal_anim_state = 6;
	item->current_anim_state = 6;
	item->item_flags[0] = (short)(item->pos.z_pos >> 2 & 0xFFFFFF00 | item->pos.x_pos >> 10);
	item->item_flags[1] = (short)(item->pos.y_pos >> 8);

	if (item->object_number == BABOON_NORMAL)
		item->ai_bits = FOLLOW;
}

void BaboonControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* item2;
	CREATURE_INFO* baboon;
	AI_INFO info;
	long distance, dx, dz;
	short tilt, head, angle, room_number;

	if (CreatureActive(item_number))
	{
		tilt = 0;
		angle = 0;
		head = 0;
		item = &items[item_number];
		baboon = (CREATURE_INFO*)item->data;

		if (!item->item_flags[2])
			FindCrowbarSwitch(item, 1);

		if (item->hit_points <= 0 && item->hit_points != -16384)
		{
			if (item->current_anim_state != 11 && item->current_anim_state != 21)
			{
				item->anim_number = objects[BABOON_NORMAL].anim_index + 14;
				item->frame_number = anims[item->anim_number].frame_base;
				item->current_anim_state = 11;
			}
			else if (item->current_anim_state == 11 && item->frame_number == anims[item->anim_number].frame_end)
				ReTriggerBaboon(item_number);
		}
		else
		{
			GetAITarget(baboon);

			if (item->object_number != BABOON_NORMAL || baboon->hurt_by_lara)
				baboon->enemy = lara_item;

			CreatureAIInfo(item, &info);

			if (!baboon->hurt_by_lara && item->object_number == BABOON_NORMAL && baboon->enemy == lara_item)
				baboon->enemy = 0;

			if (baboon->enemy == lara_item)
				distance = info.distance;
			else
			{
				dx = lara_item->pos.x_pos - item->pos.x_pos;
				dz = lara_item->pos.z_pos - item->pos.z_pos;
				phd_atan(dz, dx);
				distance = SQUARE(dx) + SQUARE(dz);
			}

			GetCreatureMood(item, &info, 1);

			if (item->ai_bits & FOLLOW && distance > 1048576)
			{
				baboon->maximum_turn >>= 2;
				baboon->mood = BORED_MOOD;
			}

			CreatureMood(item, &info, 1);
			angle = CreatureTurn(item, baboon->maximum_turn);

			switch (item->current_anim_state)
			{
			case 2:
				baboon->maximum_turn = 1274;

				if (item->ai_bits & PATROL1)
					item->goal_anim_state = 2;
				else if (baboon->mood == ESCAPE_MOOD)
					item->goal_anim_state = 4;
				else if (baboon->mood == BORED_MOOD)
				{
					if (GetRandomControl() < 256)
						item->goal_anim_state = 6;
				}
				else if (info.bite && info.distance < 465124)
					item->goal_anim_state = 3;
				else
					item->goal_anim_state = 3;

				break;

			case 3:
				baboon->flags = 0;
				baboon->maximum_turn = 0;
				item2 = baboon->enemy;

				if (item->ai_bits & GUARD)
				{
					AIGuard(baboon);

					if (!(GetRandomControl() & 0xF))
					{
						if (GetRandomControl() & 0x1)
							item->goal_anim_state = 10;
						else
							item->goal_anim_state = 6;
					}
				}
				else if (item->ai_bits & PATROL1)
					item->goal_anim_state = 2;
				else if (baboon->mood == ESCAPE_MOOD)
				{
					if (lara.target != item && info.ahead)
						item->goal_anim_state = 3;
					else
						item->goal_anim_state = 4;
				}
				else if (baboon->mood == BORED_MOOD)
				{
					if (item->required_anim_state)
						item->goal_anim_state = item->required_anim_state;
					else if (!(GetRandomControl() & 0x3))
						item->goal_anim_state = 2;
					else if (GetRandomControl() & 0x1)
					{
						if (GetRandomControl() & 0x1)
							item->goal_anim_state = 10;
						else
							item->goal_anim_state = 9;
					}
				}
				else if (item->ai_bits & FOLLOW && (baboon->reached_goal || distance > 1048576))
				{
					if (item->required_anim_state)
						item->goal_anim_state = item->required_anim_state;
					else if (GetRandomControl() & 0x1)
						item->goal_anim_state = 6;
					else if (GetRandomControl() & 0x1)
						item->goal_anim_state = 10;
					else
						item->goal_anim_state = 9;
				}
				else if (info.bite)
				{
					if (info.distance < 116281)
					{
						if (lara_item->pos.y_pos < item->pos.y_pos)
							item->goal_anim_state = 13;
						else
							item->goal_anim_state = 12;
					}
					else if (info.distance >= 465124)
					{
						if (info.bite && info.distance < 1048576)
							item->goal_anim_state = 9;
						else
							item->goal_anim_state = 4;
					}
					else
						item->goal_anim_state = 14;
				}
				else if (info.distance < 465124 && item2 != lara_item && item2 && item2->object_number != AI_PATROL1 &&
					item2->object_number != AI_PATROL2 && abs(item->pos.y_pos - item2->pos.y_pos) < 256)
				{
					item->pos.x_pos = item2->pos.x_pos;
					item->pos.y_pos = item2->pos.y_pos;
					item->pos.z_pos = item2->pos.z_pos;
					item->pos.x_rot = item2->pos.x_rot;
					item->pos.y_rot = item2->pos.y_rot;
					item->pos.z_rot = item2->pos.z_rot;
					item->anim_number = objects[BABOON_NORMAL].anim_index + 31;
					item->frame_number = anims[item->anim_number].frame_base;
					item->goal_anim_state = 21;
					item->current_anim_state = 21;
					room_number = item->room_number;
					GetHeight(GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number), item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
					TestTriggers(trigger_index, 1, 0);
				}
				else if (info.bite && info.distance < 1048576)
					item->goal_anim_state = 9;
				else
					item->goal_anim_state = 4;

				break;

			case 4:
				baboon->maximum_turn = 2002;
				tilt = angle / 2;

				if (item->ai_bits & GUARD)
					item->goal_anim_state = 3;
				else if (baboon->mood == ESCAPE_MOOD)
				{
					if (lara.target != item && info.ahead)
						item->goal_anim_state = 3;
				}
				else if (item->ai_bits & FOLLOW && (baboon->reached_goal || distance > 4194304))
					item->goal_anim_state = 3;
				else if (baboon->mood == BORED_MOOD)
					item->goal_anim_state = 9;
				else if (info.distance < 465124)
					item->goal_anim_state = 3;
				else if (info.bite && info.distance < 1048576)
					item->goal_anim_state = 9;

				break;

			case 5:
				baboon->reached_goal = 1;
				item2 = baboon->enemy;

				if (item2)
				{
					if (item2->object_number == KEY_ITEM4 && item->frame_number == anims[item->anim_number].frame_base + 12)
					{
						if (item2->room_number != 255 && item2->status != ITEM_INVISIBLE && !(item2->flags & IFL_CLEARBODY))
						{
							item->carried_item = item2 - items;
							RemoveDrawnItem(item->carried_item);
							item2->room_number = 255;
							item2->carried_item = -1;

							for (int i = 0; i < 5; i++)
							{
								if (baddie_slots[i].item_num != -1 && baddie_slots[i].item_num != item_number && baddie_slots[i].enemy == baboon->enemy)
									baddie_slots[i].enemy = 0;
							}

							if (item->ai_bits != MODIFY)
								item->ai_bits |= MODIFY | AMBUSH;
						}

						baboon->enemy = 0;
					}
					else if (item2->object_number == AI_AMBUSH && item->frame_number == anims[item->anim_number].frame_base + 12)
					{
						item->ai_bits = 0;
						item2 = &items[item->carried_item];
						item2->pos.x_pos = item->pos.x_pos;
						item2->pos.y_pos = item->pos.y_pos;
						item2->pos.z_pos = item->pos.z_pos;
						ItemNewRoom(item->carried_item, item->room_number);
						item->carried_item = -1;
						item2->ai_bits = GUARD;
						baboon->enemy = 0;
					}
					else
					{
						baboon->maximum_turn = 0;

						if (abs(info.angle) < 1274)
							item->pos.y_rot += info.angle;
						else if (info.angle < 0)
							item->pos.y_rot -= 1274;
						else
							item->pos.y_rot += 1274;
					}
				}

				break;

			case 6:
				baboon->flags = 0;
				baboon->maximum_turn = 0;

				if (item->ai_bits & GUARD)
				{
					AIGuard(baboon);

					if (!(GetRandomControl() & 0xF))
					{
						if (GetRandomControl() & 0x1)
							item->goal_anim_state = 8;
						else
							item->goal_anim_state = 7;
					}
				}
				else if (item->ai_bits & PATROL1)
					item->goal_anim_state = 2;
				else if (baboon->mood == ESCAPE_MOOD)
					item->goal_anim_state = 3;
				else if (baboon->mood == BORED_MOOD)
				{
					if (item->required_anim_state)
						item->goal_anim_state = item->required_anim_state;
					else if (!(GetRandomControl() & 0x3))
						item->goal_anim_state = 2;
					else if (GetRandomControl() & 0x3)
					{
						if (GetRandomControl() & 0x1)
							item->goal_anim_state = 8;
						else
							item->goal_anim_state = 7;
					}
				}
				else if (!(item->ai_bits & FOLLOW))
				{
					if (info.bite && info.distance < 465124)
						item->goal_anim_state = 3;
					else
						item->goal_anim_state = 3;
				}
				else if (baboon->reached_goal || distance > 1048576)
				{
					if (item->required_anim_state)
						item->goal_anim_state = item->required_anim_state;
					else if (GetRandomControl() & 0x1)
						item->goal_anim_state = 3;
					else if (GetRandomControl() & 0x1)
						item->goal_anim_state = 8;
					else
						item->goal_anim_state = 7;
				}
				else if (info.bite && info.distance < 465124)
					item->goal_anim_state = 3;
				else
					item->goal_anim_state = 3;

				break;

			case 12:
			case 13:
			case 14:

				if (info.ahead)
					head = info.angle;

				baboon->maximum_turn = 0;
				
				if (abs(info.angle) < 1274)
					item->pos.y_rot += info.angle;
				else if (info.angle < 0)
					item->pos.y_rot -= 1274;
				else
					item->pos.y_rot += 1274;

				if (!baboon->flags && item->touch_bits & 0x90)
				{
					lara_item->hit_points -= 70;
					lara_item->hit_status = 1;
					CreatureEffectT(item, &baboon_hit, 10, -1, DoBloodSplat);
					baboon->flags = 1;
				}

				break;

			case 21:
				objects[item->object_number].HitEffect = 0;
				item->hit_points = -16384;

				if (item->frame_number == anims[item->anim_number].frame_base + 212)
				{
					TestTriggersAtXYZ(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos + 1024, item->room_number, 1, 0);
					item->trigger_flags = 1;
				}

				break;
			}
		}

		CreatureTilt(item, tilt);
		CreatureJoint(item, 0, head);
		CreatureAnimation(item_number, angle, 0);
	}
}

void FindCrowbarSwitch(ITEM_INFO* item, short switch_index)
{
	ITEM_INFO* item2;
	short item_num;

	for (item_num = room[item->room_number].item_number; item_num != NO_ITEM; item_num = item2->next_item)
	{
		item2 = &items[item_num];

		if (item2->object_number == COG)
			item2->item_flags[0] = switch_index;
	}

	item->item_flags[2] = switch_index;
}

void ReTriggerBaboon(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	ExplodeBaboon(item);
	item->pos.x_pos = (item->item_flags[0] & 0xFF) << 10 | 0x200;
	item->pos.y_pos = item->item_flags[1] << 8;
	item->pos.z_pos = (item->item_flags[0] & 0xFFFFFF00) << 2 | 0x200;
	IsRoomOutside(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	if (item->room_number != IsRoomOutsideNo)
		ItemNewRoom(item_number, IsRoomOutsideNo);

	item->anim_number = objects[BABOON_NORMAL].anim_index + 2;
	item->frame_number = anims[item->anim_number].frame_base;
	item->goal_anim_state = 6;
	item->current_anim_state = 6;
	item->hit_points = objects[item->object_number].hit_points;
	RemoveActiveItem(item_number);
	item->flags &= ~(IFL_INVISIBLE | IFL_CODEBITS);
	item->after_death = 0;
	item->status = ITEM_INVISIBLE;
	DisableBaddieAI(item_number);

	if (item->object_number != BABOON_NORMAL || !item->trigger_flags)
	{
		if (item->object_number == BABOON_NORMAL || !item->trigger_flags)
			item->ai_bits = FOLLOW;

		FindCrowbarSwitch(item, 0);
	}

}

void ExplodeBaboon(ITEM_INFO* item)
{
	item->pos.y_pos -= 128;
	TriggerShockwave((PHD_VECTOR*)&item->pos, 0x2000280, -48, 0x28802000, 0);
	TriggerShockwave((PHD_VECTOR*)&item->pos, 0x2000280, -48, 0x28802000, 0x2000);
	TriggerShockwave((PHD_VECTOR*)&item->pos, 0x2000280, -48, 0x28802000, 0x4000);
	TriggerShockwave((PHD_VECTOR*)&item->pos, 0x2000280, -48, 0x28802000, 0x6000);
	item->pos.y_pos += 128;
	FlashFadeR = 255;
	FlashFadeG = 64;
	FlashFadeB = 0;
	FlashFader = 32;
}
