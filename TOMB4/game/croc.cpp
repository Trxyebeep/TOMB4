#include "../tomb4/pch.h"
#include "croc.h"
#include "box.h"
#include "objects.h"
#include "control.h"
#include "../specific/3dmath.h"
#include "people.h"
#include "effects.h"
#include "../specific/function_stubs.h"
#include "sphere.h"
#include "items.h"
#include "../specific/output.h"
#include "draw.h"
#include "sound.h"
#include "tomb4fx.h"

static BITE_INFO croc_bite = { 0, -100, 500, 9 };

void InitialiseCroc(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);

	if (room[item->room_number].flags & ROOM_UNDERWATER)
	{
		item->anim_number = objects[CROCODILE].anim_index + 12;
		item->frame_number = anims[item->anim_number].frame_base;
		item->current_anim_state = 8;
		item->goal_anim_state = 8;
	}
	else
	{
		item->anim_number = objects[CROCODILE].anim_index;
		item->frame_number = anims[item->anim_number].frame_base;
		item->current_anim_state = 1;
		item->goal_anim_state = 1;
	}
}

void CrocControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* croc;
	FLOOR_INFO* floor;
	AI_INFO info;
	long s, c, x, z, h, h2;
	short room_number, angle, rot, roll;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	croc = (CREATURE_INFO*)item->data;
	angle = 0;
	rot = 0;
	s = (1024 * phd_sin(item->pos.y_rot)) >> 14;
	c = (1024 * phd_cos(item->pos.y_rot)) >> 14;
	x = item->pos.x_pos + s;
	z = item->pos.z_pos + c;
	room_number = item->room_number;
	floor = GetFloor(x, item->pos.y_pos, z, &room_number);
	h = GetHeight(floor, x, item->pos.y_pos, z);

	if (ABS(item->pos.y_pos - h) > 512)
		h = item->pos.y_pos;

	x = item->pos.x_pos - s;
	z = item->pos.z_pos - c;
	room_number = item->room_number;
	floor = GetFloor(x, item->pos.y_pos, z, &room_number);
	h2 = GetHeight(floor, x, item->pos.y_pos, z);

	if (ABS(item->pos.y_pos - h2) > 512)
		h2 = item->pos.y_pos;

	roll = (short)phd_atan(2048, h2 - h);

	if (item->hit_points <= 0)
	{
		item->hit_points = 0;

		if (item->current_anim_state != 7 && item->current_anim_state != 10)
		{
			if (room[item->room_number].flags & ROOM_UNDERWATER)
			{
				item->anim_number = objects[CROCODILE].anim_index + 16;
				item->frame_number = anims[item->anim_number].frame_base;
				item->current_anim_state = 10;
				item->goal_anim_state = 10;
				item->hit_points = -16384;
			}
			else
			{
				item->anim_number = objects[CROCODILE].anim_index + 11;
				item->frame_number = anims[item->anim_number].frame_base;
				item->current_anim_state = 7;
				item->goal_anim_state = 7;
			}
		}

		if (room[item->room_number].flags & ROOM_UNDERWATER)
			CreatureFloat(item_number);
	}
	else
	{
		if (item->ai_bits)
			GetAITarget(croc);
		else if (croc->hurt_by_lara)
			croc->enemy = lara_item;

		CreatureAIInfo(item, &info);
		GetCreatureMood(item, &info, 1);
		CreatureMood(item, &info, 1);
		angle = CreatureTurn(item, croc->maximum_turn);

		if (item->hit_status || info.distance < 0x240000 || (TargetVisible(item, &info) && info.distance < 0x1900000))
		{
			if (!croc->alerted)
				croc->alerted = 1;

			AlertAllGuards(item_number);
		}

		rot = angle << 2;

		switch (item->current_anim_state)
		{
		case 1:
			croc->maximum_turn = 0;

			if (item->ai_bits & 1)
			{
				rot = item->item_flags[0];
				item->goal_anim_state = 1;
				item->item_flags[0] += item->item_flags[1];

				if (!(GetRandomControl() & 0x1F))
				{
					if (GetRandomControl() & 1)
						item->item_flags[1] = 0;
					else
					{
						if (GetRandomControl() & 1)
							item->item_flags[1] = 12;
						else
							item->item_flags[1] = -12;
					}
				}

				if (item->item_flags[0] > 1024)
					item->item_flags[0] = 1024;
				else if (item->item_flags[0] < -1024)
					item->item_flags[0] = -1024;
			}
			else if (info.bite && info.distance < 0x90000)
				item->goal_anim_state = 5;
			else if (info.ahead && info.distance < 0x100000)
				item->goal_anim_state = 3;
			else
				item->goal_anim_state = 2;

			break;

		case 2:
			croc->maximum_turn = 546;

			if (item->required_anim_state)
				item->goal_anim_state = item->required_anim_state;
			else if (info.bite && info.distance < 0x90000)
				item->goal_anim_state = 1;
			else if (info.ahead && info.distance < 0x100000)
				item->goal_anim_state = 3;

			break;

		case 3:
			croc->maximum_turn = 546;
			croc->LOT.step = 256;
			croc->LOT.drop = -256;

			if (item->required_anim_state)
				item->goal_anim_state = item->required_anim_state;
			else if (info.bite && info.distance < 0x90000)
				item->goal_anim_state = 1;
			else if (!info.ahead || info.distance > 0x240000)
				item->goal_anim_state = 2;

			break;

		case 5:

			if (item->frame_number == anims[item->anim_number].frame_base)
				item->required_anim_state = 0;

			if (info.bite && item->touch_bits & 0x300)
			{
				if (!item->required_anim_state)
				{
					CreatureEffectT(item, &croc_bite, 10, -1, DoBloodSplat);
					lara_item->hit_points -= 120;
					lara_item->hit_status = 1;
					item->required_anim_state = 1;
				}
			}
			else
				item->goal_anim_state = 1;

			break;

		case 8:
			croc->maximum_turn = 546;
			croc->LOT.step = 20480;
			croc->LOT.drop = -20480;

			if (item->required_anim_state)
				item->goal_anim_state = item->required_anim_state;
			else if (info.bite && item->touch_bits & 0x300)
				item->goal_anim_state = 9;

			break;

		case 9:

			if (item->frame_number == anims[item->anim_number].frame_base)
				item->required_anim_state = 0;

			if (info.bite && item->touch_bits & 0x300)
			{
				if (!item->required_anim_state)
				{
					CreatureEffectT(item, &croc_bite, 10, -1, DoBloodSplat);
					lara_item->hit_points -= 120;
					lara_item->hit_status = 1;
					item->required_anim_state = 8;
				}
			}
			else
				item->goal_anim_state = 8;

			break;
		}
	}

	CreatureTilt(item, 0);
	CreatureJoint(item, 0, rot);
	CreatureJoint(item, 1, rot);
	CreatureJoint(item, 2, -rot);
	CreatureJoint(item, 3, -rot);

	if (item->current_anim_state < 8)
	{
		if (ABS(roll - item->pos.x_rot) < 256)
			item->pos.x_rot = roll;
		else if (roll > item->pos.x_rot)
			item->pos.x_rot += 256;
		else if (roll < item->pos.x_rot)
			item->pos.x_rot -= 256;
	}

	CreatureAnimation(item_number, angle, 0);
	
	if (item->current_anim_state == 8)
		s = (1024 * phd_sin(item->pos.y_rot)) >> 14;
	else
		s = (512 * phd_sin(item->pos.y_rot)) >> 14;

	c = (1024 * phd_cos(item->pos.y_rot)) >> 14;
	x = item->pos.x_pos + s;
	z = item->pos.z_pos + c;
	room_number = item->room_number;
	GetFloor(x, item->pos.y_pos, z, &room_number);

	if (room[item->room_number].flags & ROOM_UNDERWATER)
	{
		if (room[room_number].flags & 1)
		{
			if (item->current_anim_state == 2)
			{
				item->required_anim_state = 3;
				item->goal_anim_state = 3;
			}
			else if (item->current_anim_state == 3)
			{
				item->required_anim_state = 8;
				item->goal_anim_state = 8;
			}
			else if (item->anim_number != objects[CROCODILE].anim_index + 17)
			{
				croc->LOT.step = 20480;
				croc->LOT.drop = -20480;
				croc->LOT.fly = 16;
				CreatureUnderwater(item, 256);
			}
		}
		else
		{
			item->required_anim_state = 3;
			item->goal_anim_state = 3;
			croc->LOT.step = 256;
			croc->LOT.drop = -256;
			croc->LOT.fly = 0;
			CreatureUnderwater(item, 0);
		}
	}
	else
		croc->LOT.fly = 0;
}

long GetFreeLocust()
{
	LOCUST_STRUCT* fx;

	fx = &Locusts[next_locust];

	for (int free = next_locust, i = 0; i < 64; i++)
	{
		if (fx->On)
		{
			if (free == 63)
			{
				fx = Locusts;
				free = 0;
			}
			else
			{
				free++;
				fx++;
			}
		}
		else
		{
			next_locust = (free + 1) & 0x3F;
			return free;
		}
	}

	return NO_ITEM;
}

void TriggerLocust(ITEM_INFO* item)
{
	LOCUST_STRUCT* fx;
	PHD_VECTOR vec;
	PHD_VECTOR vec2;
	long fx_number;
	short angles[2];

	fx_number = GetFreeLocust();

	if (fx_number == NO_ITEM)
		return;

	fx = &Locusts[fx_number];

	if (item->object_number == FISH)
	{
		vec.x = item->pos.x_pos;
		vec.y = item->pos.y_pos;
		vec.z = item->pos.z_pos;
		*(long*)angles = item->pos.y_rot + 0x8000;
	}
	else
	{
		vec2.x = 0;
		vec2.y = -96;
		vec2.z = 144;
		GetJointAbsPosition(item, &vec2, 9);
		vec.x = 0;
		vec.y = -128;
		vec.z = 288;
		GetJointAbsPosition(item, &vec, 9);
		phd_GetVectorAngles(vec.x - vec2.x, vec.y - vec2.y, vec.z - vec2.z, angles);
	}

	fx->room_number = item->room_number;
	fx->pos.x_pos = vec.x;
	fx->pos.y_pos = vec.y;
	fx->pos.z_pos = vec.z;
	fx->pos.x_rot = (GetRandomControl() & 0x3FF) + angles[1] - 512;
	fx->pos.y_rot = (GetRandomControl() & 0x7FF) + angles[0] - 1024;
	fx->On = 1;
	fx->flags = 0;
	fx->speed = (GetRandomControl() & 0x1F) + 16;
	fx->LaraTarget = GetRandomControl() & 0x1FF;
	fx->Counter = 20 * ((GetRandomControl() & 7) + 15);
}

void InitialiseLocustEmitter(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (!item->pos.y_rot)
		item->pos.z_pos += 512;
	else if (item->pos.y_rot == 16384)
		item->pos.x_pos += 512;
	else if (item->pos.y_rot == -32768)
		item->pos.z_pos -= 512;
	else if (item->pos.y_rot == -16384)
		item->pos.x_pos -= 512;
}

void ControlLocustEmitter(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	if (item->trigger_flags)
	{
		TriggerLocust(item);
		item->trigger_flags--;
	}
	else
		KillItem(item_number);
}

void DrawLocusts()
{
	LOCUST_STRUCT* fx;
	short** meshpp;

	for (int i = 0; i < 64; i++)
	{
		fx = &Locusts[i];

		if (fx->On)
		{
			meshpp = &meshes[objects[AHMET_MIP].mesh_index + 2 * (-GlobalCounter & 3)];
			phd_PushMatrix();
			phd_TranslateAbs(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);
			phd_RotYXZ(fx->pos.y_rot, fx->pos.x_rot, fx->pos.z_rot);
			phd_PutPolygons_train(*meshpp, 0);
			phd_PopMatrix();
		}
	}
}

void UpdateLocusts()
{
	LOCUST_STRUCT* fx;
	short* lb;
	long bounds[6];
	long speed, ox, oy, oz, closestdist, closestnum;
	short angles[2];
	short max_turn;

	lb = GetBoundsAccurate(lara_item);
	bounds[0] = lb[0] - (lb[0] >> 2) + lara_item->pos.x_pos;
	bounds[1] = lb[1] - (lb[1] >> 2) + lara_item->pos.x_pos;
	bounds[2] = lb[2] - (lb[2] >> 2) + lara_item->pos.y_pos;
	bounds[3] = lb[3] - (lb[3] >> 2) + lara_item->pos.y_pos;
	bounds[4] = lb[4] - (lb[4] >> 2) + lara_item->pos.z_pos;
	bounds[5] = lb[5] - (lb[5] >> 2) + lara_item->pos.z_pos;
	closestdist = 0xFFFFFFF;
	closestnum = -1;

	for (int i = 0; i < 64; i++)
	{
		fx = &Locusts[i];

		if (fx->On)
		{
			if ((lara.burn || lara_item->hit_points <= 0) && fx->Counter > 90 && !(GetRandomControl() & 7))
				fx->Counter = 90;

			fx->Counter--;

			if (!fx->Counter)
			{
				fx->On = 0;
				continue;
			}

			if (!(GetRandomControl() & 7))
			{
				fx->LaraTarget = (GetRandomControl() % 640) + 128;
				fx->XTarget = (GetRandomControl() & 0x7F) - 64;
				fx->ZTarget = (GetRandomControl() & 0x7F) - 64;
			}

			phd_GetVectorAngles(
				lara_item->pos.x_pos + (fx->XTarget << 3) - fx->pos.x_pos,
				lara_item->pos.y_pos - fx->LaraTarget - fx->pos.y_pos,
				lara_item->pos.z_pos + (fx->ZTarget << 3) - fx->pos.z_pos,
				angles);

			ox = SQUARE(lara_item->pos.x_pos - fx->pos.x_pos);
			oz = SQUARE(lara_item->pos.z_pos - fx->pos.z_pos);

			if (ox + oz < closestdist)
			{
				closestdist = ox + oz;
				closestnum = i;
			}

			ox = phd_sqrt(ox + oz) >> 3;

			if (ox > 128)
				ox = 128;
			else if (ox < 48)
				ox = 48;

			if (fx->speed < ox)
				fx->speed++;
			else if (fx->speed > ox)
				fx->speed--;

			if (fx->Counter > 90)
			{
				max_turn = fx->speed << 7;
				oy = (ushort)angles[0] - (ushort)fx->pos.y_rot;

				if (abs(oy) > 32768)
					oy = (ushort)fx->pos.y_rot - (ushort)angles[0];

				ox = (ushort)angles[1] - (ushort)fx->pos.x_rot;

				if (ABS(ox) > 32768)
					ox = (ushort)fx->pos.x_rot - (ushort)angles[0];

				ox >>= 3;
				oy >>= 3;

				if (oy > max_turn)
					oy = max_turn;
				else if (oy < -max_turn)
					oy = -max_turn;
				if (ox > max_turn)
					ox = max_turn;
				else if (ox < -max_turn)
					ox = -max_turn;

				fx->pos.y_rot += (short)oy;
				fx->pos.x_rot += (short)ox;
			}

			ox = fx->pos.x_pos;
			oy = fx->pos.y_pos;
			oz = fx->pos.z_pos;
			speed = fx->speed * phd_cos(fx->pos.x_rot) >> 14;
			fx->pos.x_pos += speed * phd_sin(fx->pos.y_rot) >> 14;
			fx->pos.y_pos += fx->speed * phd_sin(-fx->pos.x_rot) >> 14;
			fx->pos.z_pos += speed * phd_cos(fx->pos.y_rot) >> 14;

			if (!(i & 1))
			{
				if (fx->pos.x_pos > bounds[0] && fx->pos.x_pos < bounds[1] && fx->pos.y_pos > bounds[2] &&
					fx->pos.y_pos < bounds[3] && fx->pos.z_pos > bounds[4] && fx->pos.z_pos < bounds[5])
				{
					TriggerBlood(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, GetRandomControl() << 1, 2);

					if (lara_item->hit_points > 0)
						lara_item->hit_points -= 3;
				}
			}
		}
	}

	if (closestnum != -1)
	{
		fx = &Locusts[closestnum];
		SoundEffect(SFX_LOCUSTS_LOOP, &fx->pos, SFX_DEFAULT);
	}
}

void inject_croc(bool replace)
{
	INJECT(0x00402D90, InitialiseCroc, replace);
	INJECT(0x00402E30, CrocControl, replace);
	INJECT(0x004035D0, GetFreeLocust, replace);
	INJECT(0x00403640, TriggerLocust, replace);
	INJECT(0x004037B0, InitialiseLocustEmitter, replace);
	INJECT(0x00403810, ControlLocustEmitter, replace);
	INJECT(0x00403C10, DrawLocusts, replace);
	INJECT(0x00403870, UpdateLocusts, replace);
}
