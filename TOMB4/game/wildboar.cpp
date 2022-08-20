#include "../tomb4/pch.h"
#include "wildboar.h"
#include "box.h"
#include "objects.h"
#include "effects.h"
#include "../specific/function_stubs.h"
#include "lara.h"
#include "control.h"
#include "lot.h"

static BITE_INFO wildboar_bite = { 0, 0, 0, 14 };

void InitialiseWildboar(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[WILD_BOAR].anim_index + 6;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = 1;
	item->goal_anim_state = 1;
}

void WildboarControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* target;
	CREATURE_INFO* boar;
	CREATURE_INFO* baddie;
	AI_INFO info;
	long dx, dz, ldist, dist, max_dist;
	short angle, neckX, neckY, headX, headY;

	if (!CreatureActive(item_number))
		return;

	angle = 0;
	neckX = 0;
	neckY = 0;
	headX = 0;
	headY = 0;
	item = &items[item_number];
	boar = (CREATURE_INFO*)item->data;

	if (item->hit_points <= 0)
	{
		item->hit_points = 0;

		if (item->current_anim_state != 5)
		{
			item->anim_number = objects[WILD_BOAR].anim_index + 5;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 5;
		}
	}
	else
	{
		dx = lara_item->pos.x_pos - item->pos.x_pos;
		dz = lara_item->pos.z_pos - item->pos.z_pos;
		ldist = SQUARE(dx) + SQUARE(dz);

		if (item->ai_bits)
			GetAITarget(boar);
		else
		{
			boar->enemy = lara_item;
			max_dist = 0x7FFFFFFF;

			for (int i = 0; i < 5; i++)
			{
				baddie = &baddie_slots[i];

				if (baddie->item_num != NO_ITEM && baddie->item_num != item_number)
				{
					target = &items[baddie->item_num];

					if (target->object_number != WILD_BOAR)
					{
						dx = target->pos.x_pos - item->pos.x_pos;
						dz = target->pos.z_pos - item->pos.z_pos;
						dist = SQUARE(dx) + SQUARE(dz);

						if (dist < max_dist && dist < ldist)
						{
							boar->enemy = target;
							max_dist = dist;
						}

						neckY = 0;
					}
				}
			}
		}

		CreatureAIInfo(item, &info);
		GetCreatureMood(item, &info, 1);

		if (item->flags)
			boar->mood = ESCAPE_MOOD;

		CreatureMood(item, &info, 1);
		angle = CreatureTurn(item, boar->maximum_turn);
		dx = abs(item->item_flags[2] - item->pos.x_pos);
		dz = abs(item->item_flags[3] - item->pos.z_pos);

		if (info.ahead)
		{
			neckY = info.angle >> 1;
			headY = info.angle >> 1;
		}

		switch (item->current_anim_state)
		{
		case 1:
			boar->maximum_turn = 0;

			if (info.ahead && info.distance || item->flags)
				item->goal_anim_state = 2;
			else if ((GetRandomControl() & 0x7F) != 0)
			{
				neckY = AIGuard(boar) >> 1;
				headY = neckY;
			}
			else
				item->goal_anim_state = 3;

			break;

		case 2:

			if (info.distance >= 0x400000)
			{
				boar->maximum_turn = 1092;
				item->flags = 0;
			}
			else
			{
				boar->maximum_turn = 546;
				neckX = (short)-info.distance;
				headX = (short)-info.distance;
			}

			if (!item->flags && (dx < 50 && dz < 50 || info.distance < 0x10000 && info.bite))
			{
				item->goal_anim_state = 4;

				if (boar->enemy == lara_item)
				{
					lara_item->hit_points -= 30;
					lara_item->hit_status = 1;
				}

				CreatureEffectT(item, &wildboar_bite, 3, item->pos.y_rot, DoBloodSplat);
				item->flags = 1;
			}

			break;

		case 3:
			boar->maximum_turn = 0;

			if (info.ahead && info.distance)
				item->goal_anim_state = 1;
			else if (!(GetRandomControl() & 0x7F))
				item->goal_anim_state = 1;

			break;

		case 4:
			boar->maximum_turn = 0;
			break;
		}
	}

	CreatureJoint(item, 0, neckX);
	CreatureJoint(item, 1, neckY);
	CreatureJoint(item, 2, headX);
	CreatureJoint(item, 3, headY);
	CreatureAnimation(item_number, angle, 0);
}
