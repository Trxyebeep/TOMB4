#include "../tomb4/pch.h"
#include "dog.h"
#include "box.h"
#include "objects.h"
#include "../specific/function_stubs.h"
#include "effects.h"
#include "../specific/3dmath.h"
#include "lara.h"
#include "control.h"

static BITE_INFO dog_bite = { 0, 0, 100, 3 };
static char DeathAnims[4] = { 20, 21, 22, 21 };

void InitialiseDog(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	item->current_anim_state = 1;

	if (item->trigger_flags)
	{
		item->anim_number = objects[item->object_number].anim_index + 1;
		item->status -= ITEM_INVISIBLE;
	}
	else
		item->anim_number = objects[item->object_number].anim_index + 8;

	item->frame_number = anims[item->anim_number].frame_base;
}

void DogControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* dog;
	AI_INFO info, lara_info;
	long lara_dx, lara_dz;
	short angle, head, head_x, torso_y, random, frame;

	if (!CreatureActive(item_number))
		return;

	angle = 0;
	head_x = 0;
	head = 0;
	torso_y = 0;
	item = &items[item_number];
	dog = (CREATURE_INFO*)item->data;

	if (item->hit_points <= 0)
	{
		if (item->anim_number == objects[item->object_number].anim_index + 1)
			item->hit_points = objects[item->object_number].hit_points;
		else if (item->current_anim_state != 11)
		{
			item->anim_number = objects[item->object_number].anim_index + DeathAnims[GetRandomControl() & 3];
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 11;
		}
	}
	else
	{
		if (item->ai_bits)
			GetAITarget(dog);
		else
			dog->enemy = lara_item;

		CreatureAIInfo(item, &info);

		if (dog->enemy == lara_item)
			lara_info.distance = info.distance;
		else
		{
			lara_dx = lara_item->pos.x_pos - item->pos.x_pos;
			lara_dz = lara_item->pos.z_pos - item->pos.z_pos;
			phd_atan(lara_dz, lara_dx);
			lara_info.distance = SQUARE(lara_dx) + SQUARE(lara_dz);
		}

		if (info.ahead)
		{
			head = info.angle;
			head_x = info.x_angle;
		}

		GetCreatureMood(item, &info, 1);
		CreatureMood(item, &info, 1);

		if (dog->mood == BORED_MOOD)
			dog->maximum_turn >>= 1;

		angle = CreatureTurn(item, dog->maximum_turn);
		torso_y = angle << 2;

		if (dog->hurt_by_lara || lara_info.distance < 0x900000 && !(item->ai_bits & MODIFY))
		{
			AlertAllGuards(item_number);
			item->ai_bits &= ~MODIFY;
		}

		random = (short)GetRandomControl();
		frame = item->frame_number - anims[item->anim_number].frame_base;

		switch (item->current_anim_state)
		{
		case 0:
		case 8:
			head = 0;
			head_x = 0;

			if (dog->mood == BORED_MOOD || item->ai_bits == MODIFY)
			{
				dog->flags++;
				dog->maximum_turn = 0;

				if (dog->flags > 300 && random < 128)
					item->goal_anim_state = 1;
			}
			else
				item->goal_anim_state = 1;

			break;

		case 2:
			dog->maximum_turn = 546;

			if (item->ai_bits & PATROL1)
				item->goal_anim_state = 2;
			else if (dog->mood == BORED_MOOD && random < 256)
				item->goal_anim_state = 1;
			else
				item->goal_anim_state = 5;

			break;

		case 3:
			dog->maximum_turn = 1092;

			if (dog->mood == ESCAPE_MOOD)
			{
				if (lara.target != item && info.ahead)
					item->goal_anim_state = 9;
			}
			else if (dog->mood == BORED_MOOD)
				item->goal_anim_state = 9;
			else if (info.bite && info.distance < 0x100000)
				item->goal_anim_state = 6;
			else if (info.distance < 0x240000)
			{
				item->required_anim_state = 5;
				item->goal_anim_state = 9;
			}

			break;

		case 5:
			dog->maximum_turn = 546;

			if (dog->mood == BORED_MOOD)
				item->goal_anim_state = 9;
			else if (dog->mood == ESCAPE_MOOD)
				item->goal_anim_state = 3;
			else if (info.bite && info.distance < 0x1C639)
			{
				item->goal_anim_state = 12;
				item->required_anim_state = 5;
			}
			else if (info.distance > 0x240000 || item->hit_status)
				item->goal_anim_state = 3;

			break;

		case 6:

			if (info.bite && item->touch_bits & 0x6648 && frame >= 4 && frame <= 14)
			{
				CreatureEffectT(item, &dog_bite, 2, -1, DoBloodSplat);
				lara_item->hit_points -= 20;
				lara_item->hit_status = 1;
			}

			item->goal_anim_state = 3;

			break;

		case 7:
			head = 0;
			head_x = 0;
			break;

		case 9:

			if (item->required_anim_state)
			{
				item->goal_anim_state = item->required_anim_state;
				break;
			}

		case 1:
			dog->maximum_turn = 0;

			if (item->ai_bits & GUARD)
			{
				head = AIGuard(dog);

				if (!(GetRandomControl() & 0xFF))
				{
					if (item->current_anim_state == 1)
						item->goal_anim_state = 9;
					else
						item->goal_anim_state = 1;
				}
			}
			else if (item->current_anim_state == 9 && random < 128)
				item->goal_anim_state = 1;
			else if (item->ai_bits & PATROL1)
			{
				if (item->current_anim_state == 1)
					item->goal_anim_state = 2;
				else
					item->goal_anim_state = 1;
			}
			else if (dog->mood == ESCAPE_MOOD)
			{
				if (lara.target != item && info.ahead && !item->hit_status)
					item->goal_anim_state = 1;
				else
				{
					item->required_anim_state = 3;
					item->goal_anim_state = 9;
				}
			}
			else if (dog->mood == BORED_MOOD)
			{
				dog->flags = 0;
				dog->maximum_turn = 182;

				if (random < 256 && item->ai_bits & MODIFY && item->current_anim_state == 1)
				{
					item->goal_anim_state = 8;
					dog->flags = 0;
				}
				else if (random < 4096)
				{
					if (item->current_anim_state == 1)
						item->goal_anim_state = 2;
					else
						item->goal_anim_state = 1;
				}
				else if (!(random & 0x1F))
					item->goal_anim_state = 7;
			}
			else
			{
				item->required_anim_state = 3;

				if (item->current_anim_state == 1)
					item->goal_anim_state = 9;
			}

			break;

		case 12:

			if (info.bite && item->touch_bits & 0x48 && (frame >= 9 && frame <= 12 || frame >= 22 && frame <= 25))
			{
				CreatureEffectT(item, &dog_bite, 2, -1, DoBloodSplat);
				lara_item->hit_points -= 10;
				lara_item->hit_status = 1;
			}

			break;
		}
	}

	CreatureTilt(item, 0);
	CreatureJoint(item, 0, torso_y);
	CreatureJoint(item, 1, head);
	CreatureJoint(item, 2, head_x);
	CreatureAnimation(item_number, angle, 0);
}
