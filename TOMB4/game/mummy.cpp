#include "../tomb4/pch.h"
#include "mummy.h"
#include "box.h"
#include "objects.h"
#include "../specific/function_stubs.h"
#include "../specific/3dmath.h"
#include "effects.h"
#include "lara.h"
#include "control.h"

static BITE_INFO left_hand = { 0, 0, 0, 11 };
static BITE_INFO right_hand = { 0, 0, 0, 14 };

void InitialiseMummy(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);

	if (item->trigger_flags == 2)
	{
		item->anim_number = objects[MUMMY].anim_index + 12;
		item->current_anim_state = 8;
		item->goal_anim_state = 8;
		item->status = ITEM_INACTIVE;
	}
	else
	{
		item->anim_number = objects[MUMMY].anim_index + 19;
		item->current_anim_state = 0;
		item->goal_anim_state = 0;
	}

	item->frame_number = anims[item->anim_number].frame_base;
}

void MummyControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* mummy;
	AI_INFO info;
	long stop;
	short angle, head, torso_x, torso_y;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	mummy = (CREATURE_INFO*)item->data;
	angle = 0;
	head = 0;
	torso_x = 0;
	torso_y = 0;

	if (item->ai_bits)
		GetAITarget(mummy);
	else if (mummy->hurt_by_lara)
		mummy->enemy = lara_item;

	CreatureAIInfo(item, &info);
	stop = 0;

	if (item->hit_status && info.distance < 0x900000)
	{
		if (item->current_anim_state != 7 && item->current_anim_state != 5 && item->current_anim_state != 8)
		{
			if (!(GetRandomControl() & 3) && (lara.gun_type == WEAPON_SHOTGUN || lara.gun_type == WEAPON_GRENADE || lara.gun_type == WEAPON_REVOLVER))
			{
				item->anim_number = objects[MUMMY].anim_index + 10;
				item->frame_number = anims[item->anim_number].frame_base;
				item->current_anim_state = 7;
				item->pos.y_rot += info.angle;
				mummy->maximum_turn = 0;
			}
			else if (!(GetRandomControl() & 7) || lara.gun_type == WEAPON_SHOTGUN || lara.gun_type == WEAPON_GRENADE || lara.gun_type == WEAPON_REVOLVER)
			{
				if (item->current_anim_state == 3 || item->current_anim_state == 4)
				{
					item->anim_number = objects[MUMMY].anim_index + 20;
					item->current_anim_state = 6;
				}
				else
				{
					item->anim_number = objects[MUMMY].anim_index + 3;
					item->current_anim_state = 5;
				}

				item->frame_number = anims[item->anim_number].frame_base;
				item->pos.y_rot += info.angle;
			}

			stop = 1;
		}
	}

	if (!stop)
	{
		if (mummy->enemy != lara_item)
			phd_atan(lara_item->pos.z_pos - item->pos.z_pos, lara_item->pos.x_pos - item->pos.x_pos);

		GetCreatureMood(item, &info, 1);
		CreatureMood(item, &info, 1);
		angle = CreatureTurn(item, mummy->maximum_turn);

		if (info.ahead)
		{
			head = info.angle >> 1;
			torso_y = info.angle >> 1;
			torso_x = info.x_angle;
		}

		switch (item->current_anim_state)
		{
		case 0:
			mummy->maximum_turn = 0;

			if (info.distance < 0x100000 || item->trigger_flags > -1)
				item->goal_anim_state = 2;

			break;

		case 1:
			mummy->flags = 0;
			mummy->maximum_turn = 0;

			if (info.distance > 0x40000 && info.distance < 0x3100000)
				item->goal_anim_state = 2;
			else if (info.distance < 0x40000)
				item->goal_anim_state = 10;
			else
			{
				head = 0;
				torso_x = 0;
				torso_y = 0;
				item->goal_anim_state = 1;
				
				if (item->trigger_flags > -100 && item->trigger_flags < 0)
					item->trigger_flags++;
			}

			break;

		case 2:

			if (item->trigger_flags == 1)
			{
				mummy->maximum_turn = 0;

				if (item->frame_number == anims[item->anim_number].frame_end)
					item->trigger_flags = 0;
			}
			else
			{
				mummy->maximum_turn = 1274;

				if (info.distance < 0x900000)
					item->goal_anim_state = 3;
				else if (info.distance > 0x3100000)
					item->goal_anim_state = 1;
			}

			break;

		case 3:
			mummy->flags = 0;
			mummy->maximum_turn = 1274;

			if (info.distance < 0x40000)
				item->goal_anim_state = 1;
			else if (info.distance > 0x900000)
			{
				if (info.distance < 0x3100000)
				{
					item->goal_anim_state = 2;
					break;
				}
			}
			else if (info.distance > 0x3100000)
				item->goal_anim_state = 1;
			else if (info.distance < 0x718E4)
				item->goal_anim_state = 4;

			break;

		case 4:
		case 10:
			mummy->maximum_turn = 0;

			if (abs(info.angle) < 1274)
				item->pos.y_rot += info.angle;
			else if (info.angle < 0)
				item->pos.y_rot -= 1274;
			else
				item->pos.y_rot += 1274;

			if (!mummy->flags && item->touch_bits & 0x4800)
			{
				if (item->frame_number > anims[item->anim_number].frame_base + 13 &&
					item->frame_number < anims[item->anim_number].frame_base + 22)
				{
					lara_item->hit_points -= 100;
					lara_item->hit_status = 1;

					if (item->anim_number == objects[MUMMY].anim_index + 15)
						CreatureEffectT(item, &left_hand, 5, -1, DoBloodSplat);
					else
						CreatureEffectT(item, &right_hand, 5, -1, DoBloodSplat);

					mummy->flags = 1;
				}
			}

			break;

		case 8:
			head = 0;
			torso_x = 0;
			torso_y = 0;
			mummy->maximum_turn = 0;
			item->hit_points = 0;
			
			if (info.distance < 0x100000 || !(GetRandomControl() & 0x7F))
			{
				item->goal_anim_state = 9;
				item->hit_points = objects[MUMMY].hit_points;
			}

			break;
		}
	}

	CreatureTilt(item, 0);
	CreatureJoint(item, 0, torso_y);
	CreatureJoint(item, 1, torso_x);
	CreatureJoint(item, 2, head);
	CreatureAnimation(item_number, angle, 0);
}
