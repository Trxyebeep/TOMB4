#include "../tomb4/pch.h"
#include "raghead.h"
#include "box.h"
#include "objects.h"
#include "../specific/3dmath.h"
#include "control.h"
#include "sphere.h"
#include "effect2.h"
#include "../specific/function_stubs.h"
#include "lot.h"
#include "items.h"
#include "people.h"
#include "lara_states.h"
#include "effects.h"
#include "lara.h"
#include "gameflow.h"

static BITE_INFO raghead_fire = { 0, -16, 200, 11 };
static BITE_INFO raghead_blade = { 0, 0, 0, 15 };

void InitialiseRaghead(short item_number)
{
	ITEM_INFO* item;
	short obj_num, flag;

	item = &items[item_number];
	InitialiseCreature(item_number);

	if (objects[SUPER_RAGHEAD].loaded)
		obj_num = SUPER_RAGHEAD;
	else
		obj_num = RAGHEAD;

	if (item->object_number == RAGHEAD)
	{
		item->meshswap_meshbits = 0x7FC010;
		item->mesh_bits = ~0x7E0000;
		item->item_flags[2] = 24;
	}
	else
	{
		item->meshswap_meshbits = 0x880;
		item->mesh_bits = -1;
		item->item_flags[2] = 0;
	}

	item->item_flags[1] = -1;
	flag = item->trigger_flags % 1000;

	if (flag > 9 && flag < 20)
	{
		item->item_flags[2] += 24;
		item->trigger_flags -= 10;
		flag -= 10;
	}

	if (!flag || (flag > 4 && flag < 7))
	{
		item->anim_number = objects[obj_num].anim_index + 18;
		item->current_anim_state = 0;
		item->goal_anim_state = 0;
	}
	else if (flag == 1)
	{
		item->anim_number = objects[obj_num].anim_index + 47;
		item->current_anim_state = 24;
		item->goal_anim_state = 24;
	}
	else if (flag == 2)
	{
		item->anim_number = objects[obj_num].anim_index + 24;
		item->current_anim_state = 23;
		item->goal_anim_state = 23;
	}
	else if (flag == 3)
	{
		item->anim_number = objects[obj_num].anim_index + 29;
		item->current_anim_state = 26;
		item->goal_anim_state = 26;
	}
	else if (flag == 4)
	{
		item->anim_number = objects[obj_num].anim_index + 62;
		item->current_anim_state = 39;
		item->goal_anim_state = 39;
		item->pos.x_pos += 256 * phd_sin(item->pos.y_rot) >> W2V_SHIFT;
		item->pos.z_pos += 256 * phd_cos(item->pos.y_rot) >> W2V_SHIFT;
	}
	else if (flag > 100)
	{
		item->anim_number = objects[obj_num].anim_index + 29;
		item->current_anim_state = 26;
		item->goal_anim_state = 26;
		item->pos.x_pos += 256 * phd_sin(item->pos.y_rot) >> W2V_SHIFT;
		item->pos.z_pos += 256 * phd_cos(item->pos.y_rot) >> W2V_SHIFT;
		item->item_flags[3] = flag;
	}

	item->frame_number = anims[item->anim_number].frame_base;
}

void RagheadControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* target;
	ITEM_INFO* enemy;
	CREATURE_INFO* raghead;
	FLOOR_INFO* floor;
	PHD_VECTOR pos;
	AI_INFO info;
	AI_INFO larainfo;
	long x, y, z, Xoffset, Zoffset, nearheight, midheight, farheight, jump_ahead, long_jump_ahead;
	long dx, dz, h1, h2, can_jump, can_roll, h, c;
	short obj_num, angle, tilt, head, torso_x, torso_y, room_number, target_num, state;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	raghead = (CREATURE_INFO*)item->data;

	if (objects[SUPER_RAGHEAD].loaded)
		obj_num = SUPER_RAGHEAD;
	else
		obj_num = RAGHEAD;

	angle = 0;
	tilt = 0;
	head = 0;
	torso_x = 0;
	torso_y = 0;

	if (item->trigger_flags % 1000)
	{
		raghead->LOT.is_jumping = 1;
		raghead->maximum_turn = 0;

		if (item->trigger_flags % 1000 > 100)
		{
			item->item_flags[0] = -80;
			FindAITargetObject(raghead, AI_X1);
		}

		item->trigger_flags = 1000 * (item->trigger_flags / 1000);
	}

	Xoffset = 942 * phd_sin(item->pos.y_rot) >> W2V_SHIFT;
	Zoffset = 942 * phd_cos(item->pos.y_rot) >> W2V_SHIFT;

	room_number = item->room_number;
	x = item->pos.x_pos + Xoffset;
	y = item->pos.y_pos;
	z = item->pos.z_pos + Zoffset;
	floor = GetFloor(x, y, z, &room_number);
	nearheight = GetHeight(floor, x, y, z);

	room_number = item->room_number;
	x += Xoffset;
	z += Zoffset;
	floor = GetFloor(x, y, z, &room_number);
	midheight = GetHeight(floor, x, y, z);

	room_number = item->room_number;
	x += Xoffset;
	z += Zoffset;
	floor = GetFloor(x, y, z, &room_number);
	farheight = GetHeight(floor, x, y, z);

	if ((raghead->enemy && item->box_number == raghead->enemy->box_number) ||
		y >= nearheight - 384 || y >= midheight + 256 || y <= midheight - 256)
		jump_ahead = 0;
	else
		jump_ahead = 1;

	if ((raghead->enemy && item->box_number == raghead->enemy->box_number) ||
		y >= nearheight - 384 || y >= midheight - 384 || y >= farheight + 256 || y <= farheight - 256)
		long_jump_ahead = 0;
	else
		long_jump_ahead = 1;

	if (item->item_flags[1] != item->room_number && gfCurrentLevel != 5)
	{
		for (target_num = room[item->room_number].item_number; target_num != NO_ITEM; target_num = target->next_item)
		{
			target = &items[target_num];

			if (target->object_number == SMALLMEDI_ITEM || target->object_number == UZI_AMMO_ITEM)
			{
				if (SameZone(raghead, target) && target->status != ITEM_INVISIBLE)
				{
					raghead->enemy = target;
					break;
				}
			}
		}
	}

	item->item_flags[1] = item->room_number;

	if (item->fired_weapon)
	{
		pos.x = raghead_fire.x;
		pos.y = raghead_fire.y;
		pos.z = raghead_fire.z;
		GetJointAbsPosition(item, &pos, raghead_fire.mesh_num);
		TriggerDynamic(pos.x, pos.y, pos.z, 4 * item->fired_weapon + 8, 24, 16, 4);
		item->fired_weapon--;
	}

	if (item->hit_points <= 0)
	{
		item->hit_points = 0;
		raghead->LOT.is_jumping = 0;
		room_number = item->room_number;
		floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
		item->floor = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

		switch (item->current_anim_state)
		{
		case 18:
		case 19:
		case 20:
			item->anim_number = objects[obj_num].anim_index + 59;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 35;
			item->speed = 0;
			break;

		case 32:
			item->gravity_status = 1;
			raghead->LOT.is_jumping = 1;

			if (item->pos.y_pos >= item->floor)
			{
				item->pos.y_pos = item->floor;
				item->fallspeed = 0;
				item->gravity_status = 0;
			}

			break;

		case 35:
			item->goal_anim_state = 36;
			item->gravity_status = 0;
			break;

		case 36:
			item->gravity_status = 1;

			if (item->pos.y_pos >= item->floor)
			{
				item->pos.y_pos = item->floor;
				item->fallspeed = 0;
				item->gravity_status = 0;
				item->goal_anim_state = 37;
			}

			break;

		case 37:
			item->pos.y_pos = item->floor;
			break;

		default:
			item->anim_number = objects[obj_num].anim_index + 45;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 32;
			raghead->LOT.is_jumping = 1;

			if (item->trigger_flags > 999)
			{
				for (target_num = room[2].item_number; target_num != NO_ITEM; target_num = target->next_item)
				{
					target = &items[target_num];

					if (target->trigger_flags / 1000 == item->trigger_flags / 1000 + 1)
					{
						target->touch_bits = 0;

						if (EnableBaddieAI(target_num, 0))
							item->status = ITEM_ACTIVE;
						else
							item->status = ITEM_INVISIBLE;

						AddActiveItem(target_num);
						break;
					}
				}
			}

			break;
		}
	}
	else
	{
		if (item->ai_bits)
			GetAITarget(raghead);
		else if (!raghead->enemy)
			raghead->enemy = lara_item;

		CreatureAIInfo(item, &info);

		if (raghead->enemy == lara_item)
		{
			larainfo.angle = info.angle;
			larainfo.ahead = info.ahead;
			larainfo.distance = info.distance;
		}
		else
		{
			dx = lara_item->pos.x_pos - item->pos.x_pos;
			dz = lara_item->pos.z_pos - item->pos.z_pos;
			larainfo.angle = short(phd_atan(dz, dx) - item->pos.y_rot);

			if (larainfo.angle > -0x4000 && larainfo.angle < 0x4000)
				larainfo.ahead = 1;
			else
				larainfo.ahead = 0;

			larainfo.distance = SQUARE(dx) + SQUARE(dz);
		}

		GetCreatureMood(item, &info, 1);

		if (lara.vehicle != NO_ITEM && info.bite)
			raghead->mood = ESCAPE_MOOD;

		CreatureMood(item, &info, 1);
		angle = CreatureTurn(item, raghead->maximum_turn);
		enemy = raghead->enemy;

		if (item->hit_status || (larainfo.distance < 0x100000 || TargetVisible(item, &larainfo)) && abs(lara_item->pos.y_pos - item->pos.y_pos) < 1024)
			raghead->alerted = 1;

		raghead->enemy = enemy;

		if (item == lara.target && larainfo.distance > 0x3AE && larainfo.angle > -0x2800 && larainfo.angle < 0x2800)
		{
			room_number = item->room_number;
			x = item->pos.x_pos + (942 * phd_sin(item->pos.y_rot + 0x2000) >> W2V_SHIFT);
			z = item->pos.z_pos + (942 * phd_cos(item->pos.y_rot + 0x2000) >> W2V_SHIFT);
			floor = GetFloor(x, y, z, &room_number);
			h1 = GetHeight(floor, x, y, z);

			room_number = item->room_number;
			x = item->pos.x_pos + (942 * phd_sin(item->pos.y_rot + 0x3800) >> W2V_SHIFT);
			z = item->pos.z_pos + (942 * phd_cos(item->pos.y_rot + 0x3800) >> W2V_SHIFT);
			floor = GetFloor(x, y, z, &room_number);
			h2 = GetHeight(floor, x, y, z);

			if (abs(h2 - y) > 256 || h1 + 512 >= y)
				can_jump = 0;
			else
				can_jump = 1;

			room_number = item->room_number;
			x = item->pos.x_pos + (942 * phd_sin(item->pos.y_rot - 0x2000) >> W2V_SHIFT);
			z = item->pos.z_pos + (942 * phd_cos(item->pos.y_rot - 0x2000) >> W2V_SHIFT);
			floor = GetFloor(x, y, z, &room_number);
			h1 = GetHeight(floor, x, y, z);

			room_number = item->room_number;
			x = item->pos.x_pos + (942 * phd_sin(item->pos.y_rot - 0x3800) >> W2V_SHIFT);
			z = item->pos.z_pos + (942 * phd_cos(item->pos.y_rot - 0x3800) >> W2V_SHIFT);
			floor = GetFloor(x, y, z, &room_number);
			h2 = GetHeight(floor, x, y, z);

			if (abs(h2 - y) > 256 || h1 + 512 >= y)
				can_roll = 0;
			else
				can_roll = 1;
		}
		else
		{
			can_roll = 0;
			can_jump = 0;
		}

		switch (item->current_anim_state)
		{
		case 0:
			raghead->LOT.is_jumping = 0;
			raghead->LOT.is_monkeying = 0;
			raghead->flags = 0;
			raghead->maximum_turn = 0;
			head = info.angle >> 1;

			if (info.ahead && item->ai_bits != GUARD)
			{
				torso_y = info.angle >> 1;
				torso_x = info.x_angle;
			}

			if (item->ai_bits & GUARD)
			{
				head = AIGuard(raghead);
				item->goal_anim_state = 0;
			}
			else if (item->meshswap_meshbits == 0x880 && item == lara.target && larainfo.ahead && larainfo.distance > 0x718E4)
				item->goal_anim_state = 4;
			else if (Targetable(item, &info) && item->item_flags[2] > 0)
			{
				if (item->meshswap_meshbits == 0x7FC010)
					item->goal_anim_state = 31;
				else if (item->meshswap_meshbits == 0x7E0880 || item->meshswap_meshbits == 0x880)
					item->goal_anim_state = 13;
				else
					item->goal_anim_state = 10;
			}
			else if (item->ai_bits == MODIFY)
			{
				item->goal_anim_state = 0;

				if (item->floor > item->pos.y_pos + 768)
					item->ai_bits &= ~MODIFY;
			}
			else if (jump_ahead || long_jump_ahead)
			{
				raghead->maximum_turn = 0;
				item->anim_number = objects[obj_num].anim_index + 55;
				item->frame_number = anims[item->anim_number].frame_base;
				item->current_anim_state = 33;

				if (long_jump_ahead)
					item->goal_anim_state = 38;
				else
					item->goal_anim_state = 33;

				raghead->LOT.is_jumping = 1;
			}
			else if (enemy && (enemy->object_number == SMALLMEDI_ITEM || enemy->object_number == UZI_AMMO_ITEM) && info.distance < 0x40000)
			{
				item->goal_anim_state = 25;
				item->required_anim_state = 27;
			}
			else if (item->meshswap_meshbits == 0x7FC010 && item->item_flags[2] < 1)
					item->goal_anim_state = 11;
			else if (raghead->monkey_ahead)
			{
				floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
				h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
				c = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

				if (c == h - 1536)
				{
					if (item->meshswap_meshbits == 0x7FC800)
						item->goal_anim_state = 18;
					else if (item->meshswap_meshbits == 0x7FC010)
						item->goal_anim_state = 11;
					else
						item->goal_anim_state = 13;
				}
				else
					item->goal_anim_state = 1;
			}
			else if (can_roll)
			{
				raghead->maximum_turn = 0;
				item->goal_anim_state = 23;
			}
			else if (can_jump)
			{
				raghead->maximum_turn = 0;
				item->goal_anim_state = 24;
			}
			else if (item->meshswap_meshbits == 0x7FC800)
				item->goal_anim_state = 12;
			else if (enemy && enemy->hit_points > 0 && info.distance < 0x718E4)
			{
				if (item->meshswap_meshbits == 0x7FC010)
					item->goal_anim_state = 11;
				else if (info.distance >= 0x40000)
					item->goal_anim_state = 15;
				else if (GetRandomControl() & 1)
					item->goal_anim_state = 17;
				else
					item->goal_anim_state = 16;
			}
			else
				item->goal_anim_state = 1;

			break;

		case 1:
			raghead->LOT.is_jumping = 0;
			raghead->LOT.is_monkeying = 0;
			raghead->maximum_turn = 1274;
			raghead->flags = 0;

			if (larainfo.ahead)
				head = larainfo.angle;
			else if (info.ahead)
				head = info.angle;

			if (Targetable(item, &info) && item->item_flags[2] > 0)
				item->goal_anim_state = 0;
			else if (jump_ahead || long_jump_ahead)
			{
				raghead->maximum_turn = 0;
				item->goal_anim_state = 0;
			}
			else if (raghead->reached_goal || raghead->monkey_ahead)
				item->goal_anim_state = 0;
			else if (item->item_flags[2] >= 1 || item->meshswap_meshbits == 0x7E0880 || item->meshswap_meshbits == 0x880)
			{
				if (info.ahead && info.distance < 0x40000)
					item->goal_anim_state = 0;
				else if (info.bite && info.distance < 0x718E4)
					item->goal_anim_state = 0;
				else if (info.bite && info.distance < 0x100000)
					item->goal_anim_state = 29;
				else if (can_roll || can_jump)
					item->goal_anim_state = 0;
				else if (raghead->mood == ATTACK_MOOD && !raghead->jump_ahead && info.distance > 0x100000)
					item->goal_anim_state = 2;
			}
			else
				item->goal_anim_state = 0;

			break;

		case 2:

			if (info.ahead)
				head = info.angle;

			raghead->maximum_turn = 2002;
			tilt = angle / 2;

			if (item->object_number == SUPER_RAGHEAD && item->frame_number == anims[item->anim_number].frame_base + 11 && farheight == nearheight &&
				abs(nearheight - y) < 384 && (info.angle > -4096 && info.angle < 4096 && info.distance < 0x900000 || midheight >= nearheight + 512))
			{
				item->goal_anim_state = 30;
				raghead->maximum_turn = 0;
			}
			else if (Targetable(item, &info) && item->item_flags[2] > 0)
				item->goal_anim_state = 0;
			else if (jump_ahead || long_jump_ahead || raghead->monkey_ahead || item->ai_bits == GUARD)
				item->goal_anim_state = 0;
			else if (info.distance < 0x5C0A4 || raghead->jump_ahead)
				item->goal_anim_state = 0;
			else if (info.distance < 0x100000)
				item->goal_anim_state = 1;

			break;

		case 8:
			raghead->maximum_turn = 0;
			CreatureYRot(&item->pos, info.angle, 2002);

			if (larainfo.distance < 0x718E4 || item != lara.target)
				item->goal_anim_state = 9;

			break;

		case 10:

			if (item->frame_number == anims[item->anim_number].frame_base + 21)
				item->meshswap_meshbits = 0x7FC010;

			break;

		case 11:

			if (item->frame_number == anims[item->anim_number].frame_base + 20)
				item->meshswap_meshbits = 0x7FC800;

			break;

		case 12:

			if (item->frame_number == anims[item->anim_number].frame_base + 12)
			{
				if (item->object_number == RAGHEAD)
					item->meshswap_meshbits = 0x7E0880;
				else
					item->meshswap_meshbits = 0x880;
			}

			break;

		case 13:

			if (item->frame_number == anims[item->anim_number].frame_base + 22)
				item->meshswap_meshbits = 0x7FC800;

			break;

		case 14:

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;
			}

			CreatureYRot(&item->pos, info.angle, 1274);

			if (item->frame_number < anims[item->anim_number].frame_base + 13 && !((item->frame_number - anims[item->anim_number].frame_base) & 1))
			{
				item->fired_weapon = 1;

				if (!(item->ai_bits & MODIFY))
					item->item_flags[2]--;

				if (!ShotLara(item, &info, &raghead_fire, torso_y, 15))
					item->goal_anim_state = 0;
			}

			break;

		case 16:

			if (info.distance < 0x40000)
				item->goal_anim_state = 17;

		case 15:
		case 17:
		case 29:

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;
			}

			raghead->maximum_turn = 0;

			if (item->current_anim_state != 15 || item->frame_number < anims[item->anim_number].frame_base + 12)
			{
				if (abs(info.angle) < 1274)
					item->pos.y_rot += info.angle;
				else if (info.angle < 0)
					item->pos.y_rot -= 1274;
				else
					item->pos.y_rot += 1274;
			}

			if (!raghead->flags && item->touch_bits & 0x1C000)
			{
				if (item->frame_number > anims[item->anim_number].frame_base + 13 && item->frame_number < anims[item->anim_number].frame_base + 21)
				{
					lara_item->hit_points -= 120;
					lara_item->hit_status = 1;
					CreatureEffectT(item, &raghead_blade, 10, item->pos.y_rot, DoBloodSplat);
					raghead->flags = 1;
				}
			}

			if (item->frame_number == anims[item->anim_number].frame_end - 1)
				raghead->flags = 0;

			break;

		case 19:
			torso_x = 0;
			torso_y = 0;
			raghead->maximum_turn = 0;
			raghead->flags = 0;
			state = lara_item->current_anim_state;

			if (larainfo.ahead && larainfo.distance < 0x718E4
				&& (state > AS_DASHDIVE && state < AS_ALL4S || state == AS_HANGTURNL || state == AS_HANGTURNR))
				item->goal_anim_state = 21;
			else if (item->box_number == raghead->LOT.target_box || !raghead->monkey_ahead)
			{
				floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
				h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
				c = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

				if (c == h - 1536)
				{
					item->goal_anim_state = 22;
					raghead->LOT.is_jumping = 0;
					raghead->LOT.is_monkeying = 0;
				}
				else
					item->goal_anim_state = 20;
			}
			else
				item->goal_anim_state = 20;

			break;

		case 20:
			torso_x = 0;
			torso_y = 0;
			raghead->LOT.is_jumping = 1;
			raghead->LOT.is_monkeying = 1;
			raghead->flags = 0;
			raghead->maximum_turn = 1274;

			if (item->box_number == raghead->LOT.target_box || !raghead->monkey_ahead)
			{
				floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
				h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
				c = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

				if (c == h - 1536)
					item->goal_anim_state = 19;
			}

			state = lara_item->current_anim_state;

			if (larainfo.ahead && larainfo.distance < 0x718E4 &&
				(state > AS_DASHDIVE && state < AS_ALL4S || state == AS_HANGTURNL || state == AS_HANGTURNR))
				item->goal_anim_state = 19;

			break;

		case 21:
			raghead->maximum_turn = 1274;

			if (!raghead->flags && item->touch_bits)
			{
				lara_item->anim_number = ANIM_STOPHANG;
				lara_item->frame_number = anims[ANIM_STOPHANG].frame_base + 9;
				lara_item->current_anim_state = AS_UPJUMP;
				lara_item->goal_anim_state = AS_UPJUMP;
				lara_item->gravity_status = 1;
				lara_item->speed = 2;
				lara_item->fallspeed = 1;
				lara_item->pos.y_pos += 192;
				lara.gun_status = LG_NO_ARMS;
				raghead->flags = 1;
			}

			break;

		case 23:
		case 24:
			raghead->alerted = 0;
			raghead->maximum_turn = 0;
			item->ai_bits |= GUARD;
			break;

		case 26:

			if (item->item_flags[0])
			{
				if (info.distance < 0x718E4)
				{
					item->goal_anim_state = 28;
					raghead->enemy = 0;
				}
			}
			else if (enemy && (enemy->object_number == SMALLMEDI_ITEM || enemy->object_number == UZI_AMMO_ITEM) && info.distance < 0x40000)
				item->goal_anim_state = 27;
			else if (raghead->alerted)
				item->goal_anim_state = 28;

			break;

		case 27:
			CreatureYRot(&item->pos, info.angle, 2002);

			if (item->frame_number == anims[item->anim_number].frame_base + 9 && raghead->enemy)
			{
				if (raghead->enemy->object_number != SMALLMEDI_ITEM && raghead->enemy->object_number != UZI_AMMO_ITEM)
					break;

				if (raghead->enemy->room_number == 255 || raghead->enemy->status == ITEM_INVISIBLE || raghead->enemy->flags & IFL_CLEARBODY)
				{
					raghead->enemy = 0;
					break;
				}

				if (raghead->enemy->object_number == SMALLMEDI_ITEM)
				{
					item->hit_points += objects[item->object_number].hit_points >> 1;

					if (item->hit_points > objects[item->object_number].hit_points)
						item->hit_points = objects[item->object_number].hit_points;
				}
				else
					item->item_flags[2] += 24;

				KillItem(raghead->enemy - items);

				for (int i = 0; i < 5; i++)
				{
					if (baddie_slots[i].item_num != -1 && baddie_slots[i].item_num != item_number && baddie_slots[i].enemy == raghead->enemy)
						baddie_slots[i].enemy = 0;
				}

				raghead->enemy = 0;
			}

			break;

		case 30:

			if (item->anim_number == objects[obj_num].anim_index + 4)
				CreatureYRot(&item->pos, info.angle, 1274);
			else if (item->anim_number == objects[obj_num].anim_index + 18)
				raghead->LOT.is_jumping = 1;

			break;

		case 31:
			raghead->maximum_turn = 0;

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;
			}

			CreatureYRot(&item->pos, info.angle, 1274);

			if (Targetable(item, &info) && item->item_flags[2] >= 1)
				item->goal_anim_state = 14;
			else
				item->goal_anim_state = 0;

			break;

		case 33:
		case 38:

			if (item->item_flags[0] < 0 && item->anim_number != objects[obj_num].anim_index + 55)
				item->item_flags[0] += 2;

			break;

		case 44:

			if (!lara.blindTimer && !(GetRandomControl() & 0x7F))
				item->goal_anim_state = 0;

			break;
		}
	}

	CreatureTilt(item, tilt);
	CreatureJoint(item, 0, torso_y);
	CreatureJoint(item, 1, torso_x);
	CreatureJoint(item, 2, head);

	if (gfLevelFlags & GF_TRAIN && item->pos.y_pos > -256)
	{
		item->item_flags[0] = 0;
		item->hit_points = 0;
		item->pos.x_pos += gfUVRotate << 5;
	}
	else if (item->item_flags[0] < 0)
		item->pos.x_pos += item->item_flags[0];

	state = item->current_anim_state;

	if (state >= 38 || state == 33 || state == 20 || state == 32 || state == 30)
		CreatureAnimation(item_number, angle, 0);
	else if (lara.blindTimer > 100)
	{
		raghead->maximum_turn = 0;
		item->anim_number = objects[obj_num].anim_index + 68;
		item->frame_number = anims[item->anim_number].frame_base + (GetRandomControl() & 7);
		item->current_anim_state = 44;
	}
	else
	{
		switch (CreatureVault(item_number, angle, 2, 260))
		{
		case -4:
			raghead->maximum_turn = 0;
			item->anim_number = objects[obj_num].anim_index + 65;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 42;
			break;

		case -3:
			raghead->maximum_turn = 0;
			item->anim_number = objects[obj_num].anim_index + 66;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 43;
			break;

		case 2:
			raghead->maximum_turn = 0;
			item->anim_number = objects[obj_num].anim_index + 64;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 41;
			break;

		case 3:
			raghead->maximum_turn = 0;
			item->anim_number = objects[obj_num].anim_index + 63;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 40;
			break;

		case 4:
			raghead->maximum_turn = 0;
			item->anim_number = objects[obj_num].anim_index + 62;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 39;
			break;
		}
	}
}
