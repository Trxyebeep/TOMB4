#include "../tomb4/pch.h"
#include "guide.h"
#include "box.h"
#include "objects.h"
#include "../specific/function_stubs.h"
#include "sphere.h"
#include "tomb4fx.h"
#include "sound.h"
#include "effect2.h"
#include "../specific/3dmath.h"
#include "switch.h"
#include "items.h"
#include "lot.h"
#include "effects.h"
#include "lara.h"
#include "control.h"

static BITE_INFO guide_hit = { 0, 20, 200, 18 };
static BITE_INFO guide_lighter = { 30, 80, 50, 15 };

void InitialiseGuide(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[GUIDE].anim_index + 4;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = 1;
	item->goal_anim_state = 1;

	if (!objects[WRAITH1].loaded)
		item->meshswap_meshbits = 0x40000;
	else
	{
		item->meshswap_meshbits = 0;
		item->item_flags[1] = 2;
	}
}

void GuideControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* target;
	ITEM_INFO* candidate;
	ITEM_INFO* enemy;
	CREATURE_INFO* guide;
	PHD_VECTOR pos;
	AI_INFO info;
	long rnd, r, g, b, x, y, z, iAhead, iDistance, dist, bestdist;
	short got_torch, tilt, head, torso_x, torso_y, iAngle, xAngle, angle, dy, candidate_num, frame;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	guide = (CREATURE_INFO*)item->data;
	got_torch = 0;		//JUST grabbed the torch
	tilt = 0;
	head = 0;
	torso_x = 0;
	torso_y = 0;

	if (item->item_flags[1] == 2)
	{
		rnd = GetRandomControl();
		r = 255 - ((rnd >> 4) & 0x1F);
		g = 192 - ((rnd >> 6) & 0x1F);
		b = rnd & 0x3F;
		pos.x = guide_hit.x;
		pos.y = guide_hit.y;
		pos.z = guide_hit.z;
		GetJointAbsPosition(item, &pos, guide_hit.mesh_num);
		AddFire(pos.x, pos.y, pos.z, 0, item->room_number, 0);
		SoundEffect(SFX_LOOP_FOR_SMALL_FIRES, &item->pos, SFX_DEFAULT);
		TriggerFireFlame(pos.x, pos.y - 40, pos.z, -1, 7);
		TriggerDynamic(pos.x, pos.y, pos.z, 15, r, g, b);

		if (item->anim_number == objects[GUIDE].anim_index + 61)
		{
			if (item->frame_number > anims[item->anim_number].frame_base + 32 && item->frame_number < anims[item->anim_number].frame_base + 42)
			{
				x = (rnd & 0x3F) + pos.x - 32;
				y = ((rnd >> 3) & 0x3F) + pos.y - 128;
				z = pos.z + ((rnd >> 6) & 0x3F) - 32;
				TriggerFireFlame(x, y, z, -1, 1);
			}
		}
	}

	item->ai_bits = FOLLOW;
	GetAITarget(guide);
	x = lara_item->pos.x_pos - item->pos.x_pos;
	z = lara_item->pos.z_pos - item->pos.z_pos;
	iAngle = short(phd_atan(z, x) - item->pos.y_rot);

	if (iAngle > -0x4000 && iAngle < 0x4000)
		iAhead = 1;
	else
		iAhead = 0;

	if (z > 32000 || z < -32000 || x > 32000 || x < -32000)
		iDistance = 0x7FFFFFFF;
	else
		iDistance = SQUARE(x) + SQUARE(z);

	x = abs(x);
	z = abs(z);

	if (x > z)
		xAngle = (short)phd_atan(x + (z >> 1), item->pos.y_pos - lara_item->pos.y_pos);
	else
		xAngle = (short)phd_atan(z + (x >> 1), item->pos.y_pos - lara_item->pos.y_pos);

	target = 0;
	bestdist = 0x7FFFFFFF;

	if (!objects[WRAITH1].loaded && (item->current_anim_state < 4 || item->current_anim_state == 31))
	{
		for (int i = 0; i < 5; i++)
		{
			if (baddie_slots[i].item_num != NO_ITEM && baddie_slots[i].item_num != item_number)
			{
				candidate = &items[baddie_slots[i].item_num];

				if (candidate->object_number != GUIDE && abs(candidate->pos.y_pos - item->pos.y_pos) <= 512)
				{
					x = candidate->pos.x_pos - item->pos.x_pos;
					z = candidate->pos.z_pos - item->pos.z_pos;

					if (z > 32000 || z < -32000 || x > 32000 || x < -32000)
						dist = 0x7FFFFFFF;
					else
						dist = SQUARE(x) + SQUARE(z);

					if (dist < bestdist && dist < 0x400000 &&
						(abs(item->pos.y_pos - candidate->pos.y_pos) < 256 || iDistance < 0x400000 || candidate->object_number == FUCKED_UP_DOG))
					{
						target = candidate;
						bestdist = dist;
					}
				}
			}
		}
	}

	enemy = guide->enemy;

	if (target)
		guide->enemy = target;

	CreatureAIInfo(item, &info);
	GetCreatureMood(item, &info, 1);
	CreatureMood(item, &info, 1);
	angle = CreatureTurn(item, guide->maximum_turn);

	if (target)
	{
		guide->enemy = enemy;
		enemy = target;
	}

	switch (item->current_anim_state)
	{
	case 1:
		guide->LOT.is_jumping = 0;
		guide->flags = 0;
		guide->maximum_turn = 0;
		head = info.angle >> 1;

		if (iAhead)
		{
			torso_x = xAngle >> 1;
			torso_y = iAngle >> 1;
			head = iAngle >> 1;
		}
		else if (info.ahead)
		{
			torso_x = info.x_angle >> 1;
			torso_y = info.angle >> 1;
			head = info.angle >> 1;
		}

		if (objects[WRAITH1].loaded)
		{
			if (item->item_flags[3] == 5 || item->item_flags[3] == 6)
			{
				if (item->item_flags[3] == 5)
					item->goal_anim_state = 2;

				break;
			}
		}

		if (item->required_anim_state)
			item->goal_anim_state = item->required_anim_state;
		else if (lara.location < item->item_flags[3] && item->item_flags[1] == 2)
			item->goal_anim_state = 1;
		else if (!guide->reached_goal || target)
		{
			if (item->meshswap_meshbits == 0x40000)
				item->goal_anim_state = 40;
			else if (target && info.distance < 0x100000)
			{
				if (info.bite)
					item->goal_anim_state = 31;
			}
			else if (enemy != lara_item || info.distance > 0x400000)
				item->goal_anim_state = 2;
		}
		else if (!enemy->flags)
		{
			guide->reached_goal = 0;
			guide->enemy = 0;
			item->ai_bits = FOLLOW;
			item->item_flags[3]++;
		}
		else if (info.distance > 0x4000)
		{
			guide->maximum_turn = 0;

			if (info.ahead)
				item->required_anim_state = 41;
			else
				item->required_anim_state = 42;
		}
		else
		{
			switch (enemy->flags)
			{
			case 2:
				item->goal_anim_state = 38;
				item->required_anim_state = 38;
				break;

			case 32:
				item->goal_anim_state = 37;
				item->required_anim_state = 37;
				break;

			case 40:

				if (iDistance < 0x400000)
				{
					item->goal_anim_state = 39;
					item->required_anim_state = 39;
				}

				break;

			case 16:

				if (iDistance < 0x400000)
				{
					item->goal_anim_state = 36;
					item->required_anim_state = 36;
				}

				break;

			case 4:

				if (iDistance < 0x400000)
				{
					item->goal_anim_state = 36;
					item->required_anim_state = 43;
				}

				break;
			case 62:
				item->status = ITEM_INVISIBLE;
				RemoveActiveItem(item_number);
				DisableBaddieAI(item_number);
				break;
			}
		}

		break;

	case 2:
		guide->LOT.is_jumping = 0;
		guide->maximum_turn = 1274;

		if (iAhead)
			head = iAngle;
		else if (info.ahead)
			head = info.angle;

		if (objects[WRAITH1].loaded && item->item_flags[3] == 5)
		{
			item->item_flags[3] = 6;
			item->goal_anim_state = 1;
		}
		else if (item->item_flags[1] == 1)
		{
			item->goal_anim_state = 1;
			item->required_anim_state = 11;
		}
		else if (guide->reached_goal)
		{
			if (!enemy->flags)
			{
				guide->reached_goal = 0;
				guide->enemy = 0;
				item->ai_bits = FOLLOW;
				item->item_flags[3]++;
			}
			else
				item->goal_anim_state = 1;
		}
		else if (lara.location < item->item_flags[3])
			item->goal_anim_state = 1;
		else if (!target || info.distance >= 0x200000 && (item->meshswap_meshbits & 0x40000 || info.distance >= 0x900000))
		{
			if (enemy == lara_item)
			{
				if (info.distance < 0x400000)
					item->goal_anim_state = 1;
				else if (info.distance > 0x1000000)
					item->goal_anim_state = 3;
			}
			else if (lara.location > item->item_flags[3] && iDistance > 0x400000)
				item->goal_anim_state = 3;
		}
		else
			item->goal_anim_state = 1;

		break;

	case 3:

		if (info.ahead)
			head = info.angle;

		guide->maximum_turn = 2002;
		tilt = angle / 2;

		if (info.distance < 0x400000 || lara.location < item->item_flags[3])
			item->goal_anim_state = 1;
		else if (guide->reached_goal)
		{
			if (!enemy->flags)
			{
				guide->reached_goal = 0;
				guide->enemy = 0;
				item->ai_bits = FOLLOW;
				item->item_flags[3]++;
			}
			else
				item->goal_anim_state = 1;
		}
		else if (target && !(item->meshswap_meshbits & 0x40000) && info.distance < 0x900000)
			item->goal_anim_state = 1;

		break;

	case 11:
		rnd = GetRandomControl();
		pos.x = guide_lighter.x;
		pos.y = guide_lighter.y;
		pos.z = guide_lighter.z;
		GetJointAbsPosition(item, &pos, guide_lighter.mesh_num);
		frame = item->frame_number - anims[item->anim_number].frame_base;

		if (frame == 32)
			item->meshswap_meshbits |= 0x8000;
		else if (frame == 216)
			item->meshswap_meshbits &= ~0x8000;
		else if (frame > 79 && frame < 84)
		{
			r = rnd & 0x1F;
			g = 96 - ((rnd >> 6) & 0x1F);
			b = 128 - ((rnd >> 4) & 0x1F);
			TriggerDynamic(pos.x, pos.y, pos.z, 10, r, g, b);
			TriggerFlareSparks(pos.x, pos.y, pos.z, -1, -1, 0, 1);
		}
		else if (frame > 83 && frame < 94)
		{
			r = 192 - ((rnd >> 4) & 0x1F);
			g = 128 - ((rnd >> 6) & 0x1F);
			b = rnd & 0x1F;
			TriggerDynamic(pos.x - 32, pos.y - 64, pos.z - 32, 10, r, g, b);

			x = (rnd & 0x3F) + pos.x - 64;
			y = ((rnd >> 5) & 0x3F) + pos.y - 96;
			z = ((rnd >> 10) & 0x3F) + pos.z - 64;
			TriggerFireFlame(x, y, z, -1, 7);
		}
		else if (frame > 159 && frame < 164)
		{
			r = rnd & 0x1F;
			g = 96 - ((rnd >> 6) & 0x1F);
			b = 128 - ((rnd >> 4) & 0x1F);
			TriggerFlareSparks(pos.x, pos.y, pos.z, -1, -1, 0, 1);
			TriggerDynamic(pos.x, pos.y, pos.z, 10, r, g, b);
		}
		else if (frame > 163 && frame < 181)
		{
			x = (rnd & 0x3F) + pos.x - 64;
			y = ((rnd >> 5) & 0x3F) + pos.y - 96;
			z = ((rnd >> 10) & 0x3F) + pos.z - 64;
			TriggerFireFlame(x, y, z, -1, 7);

			r = 192 - ((rnd >> 4) & 0x1F);
			g = 128 - ((rnd >> 6) & 0x1F);
			b = rnd & 0x1F;
			TriggerDynamic(pos.x - 32, pos.y - 64, pos.z - 32, 10, r, g, b);
			item->item_flags[1] = 2;
		}

		break;

	case 22:
		guide->maximum_turn = 0;

		if (iAngle < -256)
			item->pos.y_rot -= 399;

		break;

	case 31:

		if (info.ahead)
		{
			torso_x = info.x_angle >> 1;
			torso_y = info.angle >> 1;
			head = info.angle >> 1;
		}

		guide->maximum_turn = 0;

		if (abs(info.angle) < 1274)
			item->pos.y_rot += info.angle;
		else if (info.angle < 0)
			item->pos.y_rot -= 1274;
		else
			item->pos.y_rot += 1274;

		if (guide->flags || !enemy)
			break;

		if (item->frame_number > anims[item->anim_number].frame_base + 15 && item->frame_number < anims[item->anim_number].frame_base + 26)
		{
			x = abs(enemy->pos.x_pos - item->pos.x_pos);
			y = abs(enemy->pos.y_pos - item->pos.y_pos);
			z = abs(enemy->pos.z_pos - item->pos.z_pos);

			if (x < 512 && y <= 512 && z < 512)
			{
				enemy->hit_points -= 20;

				if (enemy->hit_points <= 0)
					item->ai_bits = FOLLOW;

				enemy->hit_status = 1;
				guide->flags = 1;
				CreatureEffectT(item, &guide_hit, 8, -1, DoBloodSplat);
			}
		}

		break;

	case 35:
		guide->maximum_turn = 0;

		if (iAngle > 256)
			item->pos.y_rot += 399;

		break;

	case 36:
	case 43:

		if (enemy)
		{
			dy = enemy->pos.y_rot - item->pos.y_rot;

			if (dy > 364)
				item->pos.y_rot += 364;
			else if (dy < -364)
				item->pos.y_rot -= 364;
		}

		if (item->required_anim_state == 43)
			item->goal_anim_state = 43;
		else if (item->anim_number != objects[GUIDE].anim_index + 57 && item->frame_number == anims[item->anim_number].frame_end - 20)
		{
			item->goal_anim_state = 1;
			TestTriggersAtXYZ(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 1, 0);
			guide->reached_goal = 0;
			guide->enemy = 0;
			item->ai_bits = FOLLOW;
			item->item_flags[3]++;
		}

		break;

	case 37:

		if (item->frame_number == anims[item->anim_number].frame_base)
		{
			got_torch = 1;
			item->pos.x_pos = enemy->pos.x_pos;
			item->pos.y_pos = enemy->pos.y_pos;
			item->pos.z_pos = enemy->pos.z_pos;
			item->pos.x_rot = enemy->pos.x_rot;
			item->pos.y_rot = enemy->pos.y_rot;
			item->pos.z_rot = enemy->pos.z_rot;
		}
		else if (item->frame_number == anims[item->anim_number].frame_base + 35)
		{
			item->meshswap_meshbits &= ~0x40000;

			for (candidate_num = room[item->room_number].item_number; candidate_num != NO_ITEM; candidate_num = candidate->next_item)
			{
				candidate = &items[candidate_num];

				if (candidate->object_number >= ANIMATING1 && candidate->object_number <= ANIMATING15 &&
					!((item->pos.z_pos ^ candidate->pos.z_pos) & ~0x3FF) && !((item->pos.x_pos ^ candidate->pos.x_pos) & ~0x3FF))
				{
					candidate->mesh_bits = 0xFFFFFFFD;
					break;
				}
			}
		}

		item->item_flags[1] = 1;

		if (got_torch)
		{
			guide->reached_goal = 0;
			guide->enemy = 0;
			item->ai_bits = FOLLOW;
			item->item_flags[3]++;
		}

		break;

	case 38:

		if (item->frame_number == anims[item->anim_number].frame_base)
		{
			item->pos.x_pos = enemy->pos.x_pos;
			item->pos.y_pos = enemy->pos.y_pos;
			item->pos.z_pos = enemy->pos.z_pos;
		}
		else if (item->frame_number == anims[item->anim_number].frame_base + 42)
		{
			TestTriggersAtXYZ(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 1, 0);
			item->pos.y_rot = enemy->pos.y_rot;
			guide->reached_goal = 0;
			guide->enemy = 0;
			item->ai_bits = FOLLOW;
			item->item_flags[3]++;
		}
		else if (item->frame_number < anims[item->anim_number].frame_base + 42)
		{
			dy = enemy->pos.y_rot - item->pos.y_rot;

			if (dy > 364)
				item->pos.y_rot += 364;
			else if (dy < -364)
				item->pos.y_rot -= 364;
		}

		break;

	case 39:

		if (item->frame_number < anims[item->anim_number].frame_base + 20)
		{
			dy = enemy->pos.y_rot - item->pos.y_rot;

			if (dy > 364)
				item->pos.y_rot += 364;
			else if (dy < -364)
				item->pos.y_rot -= 364;
		}
		else if (item->frame_number == anims[item->anim_number].frame_base + 20)
		{
			item->goal_anim_state = 1;
			TestTriggersAtXYZ(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 1, 0);
			guide->reached_goal = 0;
			guide->enemy = 0;
			item->ai_bits = FOLLOW;
			item->item_flags[3]++;
		}
		else if (item->frame_number == anims[item->anim_number].frame_base + 70 && item->room_number == 70)
		{
			item->required_anim_state = 3;
			item->meshswap_meshbits |= 0x200000;
			SoundEffect(SFX_GUIDE_SCARE, &item->pos, SFX_DEFAULT);
		}

		break;

	case 40:
		guide->LOT.is_jumping = 0;
		guide->maximum_turn = 1274;

		if (iAhead)
			head = iAngle;
		else if (info.ahead)
			head = info.angle;

		if (guide->reached_goal)
		{
			if (!enemy->flags)
			{
				guide->reached_goal = 0;
				guide->enemy = 0;
				item->ai_bits = FOLLOW;
				item->item_flags[3]++;
				break;
			}

			if (enemy->flags == 42)
			{
				TestTriggersAtXYZ(enemy->pos.x_pos, enemy->pos.y_pos, enemy->pos.z_pos, enemy->room_number, 1, 0);
				guide->reached_goal = 0;
				guide->enemy = 0;
				item->ai_bits = FOLLOW;
				item->item_flags[3]++;
			}
			else if (item->trigger_flags <= 999)
				item->goal_anim_state = 1;
			else
			{
				KillItem(item_number);
				DisableBaddieAI(item_number);
				item->flags |= IFL_INVISIBLE;
			}
		}

		break;

	case 41:
	case 42:
		guide->maximum_turn = 0;
		MoveCreature3DPos(&item->pos, &enemy->pos, 15, enemy->pos.y_rot - item->pos.y_rot, 1820);
		break;
	}

	CreatureTilt(item, tilt);
	CreatureJoint(item, 0, torso_y);
	CreatureJoint(item, 1, torso_x);
	CreatureJoint(item, 2, head);
	CreatureJoint(item, 3, torso_x);
	CreatureAnimation(item_number, angle, 0);
}
