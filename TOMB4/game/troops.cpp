#include "../tomb4/pch.h"
#include "troops.h"
#include "box.h"
#include "objects.h"
#include "sphere.h"
#include "effect2.h"
#include "../specific/3dmath.h"
#include "../specific/function_stubs.h"
#include "people.h"
#include "lara.h"
#include "control.h"
#include "lot.h"

static BITE_INFO troop_gun = { 0, 300, 64, 7 };

void InitialiseTroop(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);

	if (item->trigger_flags == 1)
	{
		item->anim_number = objects[TROOPS].anim_index + 27;
		item->current_anim_state = 16;
		item->goal_anim_state = 16;
	}
	else
	{
		item->anim_number = objects[TROOPS].anim_index + 12;
		item->current_anim_state = 1;
		item->goal_anim_state = 1;
	}

	item->frame_number = anims[item->anim_number].frame_base;
}

void TroopControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* enemy;
	CREATURE_INFO* troop;
	CREATURE_INFO* baddie;
	PHD_VECTOR pos;
	AI_INFO info;
	long dist, max_dist, dx, dz, iDistance;
	short angle, tilt, torso_x, torso_y, head, iAngle;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	troop = (CREATURE_INFO*)item->data;
	angle = 0;
	tilt = 0;
	torso_x = 0;
	torso_y = 0;
	head = 0;

	if (item->fired_weapon)
	{
		pos.x = troop_gun.x;
		pos.y = troop_gun.y;
		pos.z = troop_gun.z;
		GetJointAbsPosition(item, &pos, troop_gun.mesh_num);
		TriggerDynamic(pos.x, pos.y, pos.z, 2 * item->fired_weapon + 8, 24, 16, 4);
		item->fired_weapon--;
	}

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state != 7 && item->current_anim_state != 15)
		{
			enemy = troop->enemy;

			if (enemy && enemy->object_number == SCORPION && item->item_flags[0] < 80)
			{
				if (enemy->anim_number == objects[SCORPION].anim_index + 6)
				{
					item->anim_number = objects[item->object_number].anim_index + 23;

					if (item->current_anim_state == 16)
						item->frame_number = anims[item->anim_number].frame_base + 37;
					else
						item->frame_number = anims[item->anim_number].frame_base;

					item->current_anim_state = 15;
					item->goal_anim_state = 15;
					item->pos = enemy->pos;
					enemy->trigger_flags = 99;
				}
				else
					item->item_flags[0]++;
			}
			else
			{
				item->anim_number = objects[item->object_number].anim_index + 19;
				item->frame_number = anims[item->anim_number].frame_base;
				item->current_anim_state = 7;
			}
		}
	}
	else
	{
		if (item->ai_bits)
			GetAITarget(troop);
		else
		{
			troop->enemy = 0;
			max_dist = 0x7FFFFFFF;

			for (int i = 0; i < 5; i++)
			{
				baddie = &baddie_slots[i];

				if (baddie->item_num != NO_ITEM && baddie->item_num != item_number)
				{
					enemy = &items[baddie->item_num];

					if (enemy->object_number != LARA && enemy->object_number != TROOPS && (enemy != lara_item || troop->hurt_by_lara))
					{
						dx = enemy->pos.x_pos - item->pos.x_pos;
						dz = enemy->pos.z_pos - item->pos.z_pos;
						dist = SQUARE(dx) + SQUARE(dz);

						if (dist < max_dist)
						{
							troop->enemy = enemy;
							max_dist = dist;
						}
					}
				}
			}
		}

		if (troop->hurt_by_lara && item->current_anim_state != 16)
			troop->enemy = lara_item;

		CreatureAIInfo(item, &info);

		if (troop->enemy == lara_item)
		{
			iDistance = info.distance;
			iAngle = info.angle;
		}
		else
		{
			dx = lara_item->pos.x_pos - item->pos.x_pos;
			dz = lara_item->pos.z_pos - item->pos.z_pos;
			iAngle = short(phd_atan(dz, dx) - item->pos.y_rot);
			iDistance = SQUARE(dx) + SQUARE(dz);
		}

		if (!troop->hurt_by_lara && troop->enemy == lara_item)
			troop->enemy = 0;

		GetCreatureMood(item, &info, 0);
		CreatureMood(item, &info, 0);
		angle = CreatureTurn(item, troop->maximum_turn);

		if (item->hit_status)
			AlertAllGuards(item_number);

		switch (item->current_anim_state)
		{
		case 1:
			head = iAngle;
			troop->flags = 0;
			troop->maximum_turn = 0;

			if (item->anim_number == objects[item->object_number].anim_index + 17)
			{
				if (abs(info.angle) < 1820)
					item->pos.y_rot += info.angle;
				else if (info.angle < 0)
					item->pos.y_rot -= 1820;
				else
					item->pos.y_rot += 1820;
			}

			if (item->ai_bits & GUARD)
			{
				head = AIGuard(troop);

				if (!(GetRandomControl() & 0xFF))
				{
					if (item->current_anim_state == 1)
						item->goal_anim_state = 4;
					else
						item->goal_anim_state = 1;
				}
			}
			else if (item->ai_bits & PATROL1)
			{
				item->goal_anim_state = 2;
				head = 0;
			}
			else if (troop->mood == ESCAPE_MOOD)
				item->goal_anim_state = 3;
			else if (Targetable(item, &info))
			{
				if (info.distance >= 0x900000 && info.zone_number == info.enemy_zone)
					item->goal_anim_state = 2;
				else if (GetRandomControl() >= 0x4000)
					item->goal_anim_state = 10;
				else
					item->goal_anim_state = 8;
			}
			else
			{
				if ((troop->alerted || troop->mood != BORED_MOOD) && (!(item->ai_bits & FOLLOW) || !troop->reached_goal && iDistance <= 0x400000))
				{
					if (troop->mood == BORED_MOOD || info.distance <= 0x400000)
						item->goal_anim_state = 2;
					else
						item->goal_anim_state = 3;
				}
				else
					item->goal_anim_state = 1;
			}

			break;

		case 2:
			head = iAngle;
			troop->flags = 0;
			troop->maximum_turn = 910;

			if (item->ai_bits & PATROL1)
				item->goal_anim_state = 2;
			else if (troop->mood == ESCAPE_MOOD)
				item->goal_anim_state = 3;
			else if (item->ai_bits & GUARD || item->ai_bits & FOLLOW && (troop->reached_goal || iDistance > 0x400000))
				item->goal_anim_state = 1;
			else if (Targetable(item, &info))
			{
				if (info.distance < 0x900000 || info.zone_number != info.enemy_zone)
					item->goal_anim_state = 1;
				else
					item->goal_anim_state = 9;
			}
			else if (troop->mood != BORED_MOOD)
			{
				if (info.distance > 0x400000)
					item->goal_anim_state = 3;
			}
			else if (info.ahead)
				item->goal_anim_state = 1;

			break;

		case 3:

			if (info.ahead)
				head = info.angle;

			troop->maximum_turn = 1820;
			tilt = angle >> 1;

			if (item->ai_bits & GUARD || item->ai_bits & FOLLOW && (troop->reached_goal || iDistance > 0x400000))
				item->goal_anim_state = 2;
			else if (troop->mood != ESCAPE_MOOD)
			{
				if (Targetable(item, &info))
					item->goal_anim_state = 2;
				else if (troop->mood == BORED_MOOD || troop->mood == STALK_MOOD && !(item->ai_bits & FOLLOW) && info.distance < 0x400000)
					item->goal_anim_state = 2;
			}

			break;

		case 4:
			head = iAngle;
			troop->flags = 0;
			troop->maximum_turn = 0;

			if (item->ai_bits & GUARD)
			{
				head = AIGuard(troop);

				if (!(GetRandomControl() & 0xFF))
					item->goal_anim_state = 1;
			}
			else if (Targetable(item, &info))
				item->goal_anim_state = 5;
			else if (troop->mood != BORED_MOOD || !info.ahead)
				item->goal_anim_state = 1;

			break;

		case 11:

			if (item->goal_anim_state != 1 && (troop->mood == ESCAPE_MOOD || info.distance > 0x900000 || !Targetable(item, &info)))
				item->goal_anim_state = 1;

		case 5:
		case 6:

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;
			}

			if (troop->flags)
				troop->flags--;
			else
			{
				ShotLara(item, &info, &troop_gun, torso_y, 23);
				troop->flags = 5;
			}

			break;

		case 8:
		case 10:
			troop->flags = 0;

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;

				if (Targetable(item, &info))
					item->goal_anim_state = item->current_anim_state != 8 ? 11 : 5;
				else
					item->goal_anim_state = 1;
			}

			break;

		case 9:
			troop->flags = 0;

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;

				if (Targetable(item, &info))
					item->goal_anim_state = 6;
				else
					item->goal_anim_state = 2;
			}

			break;

		case 16:
			troop->maximum_turn = 0;
			break;

		case 17:

			if (!lara.blindTimer && !(GetRandomControl() & 0x7F))
				item->goal_anim_state = 4;

			break;
		}

		if (lara.blindTimer > 100)
		{
			if (item->current_anim_state != 17 && item->current_anim_state != 16)
			{
				troop->maximum_turn = 0;
				item->anim_number = objects[TROOPS].anim_index + 28;
				item->frame_number = anims[item->anim_number].frame_base + (GetRandomControl() & 7);
				item->current_anim_state = 17;
			}
		}
	}

	CreatureTilt(item, tilt);
	CreatureJoint(item, 0, torso_y);
	CreatureJoint(item, 1, torso_x);
	CreatureJoint(item, 2, head);
	CreatureAnimation(item_number, angle, 0);
}
