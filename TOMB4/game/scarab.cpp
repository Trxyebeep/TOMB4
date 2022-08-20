#include "../tomb4/pch.h"
#include "scarab.h"
#include "box.h"
#include "objects.h"
#include "../specific/function_stubs.h"
#include "effects.h"
#include "../specific/3dmath.h"
#include "control.h"
#include "../specific/output.h"
#include "effect2.h"
#include "lara.h"

static BITE_INFO beetle_bite = { 0, 0, 0, 12 };

SCARAB_STRUCT Scarabs[128];

static long next_scarab = 0;

void InitialiseScarab(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[BIG_BEETLE].anim_index + 3;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = 1;
	item->goal_anim_state = 1;
}

void ScarabControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* beetle;
	AI_INFO info;
	short angle;

	if (!CreatureActive(item_number))
		return;

	angle = 0;
	item = &items[item_number];
	beetle = (CREATURE_INFO*)item->data;

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state != 6)
		{
			if (item->current_anim_state == 7)
			{
				if (item->pos.y_pos >= item->floor)
				{
					item->pos.y_pos = item->floor;
					item->gravity_status = 0;
					item->fallspeed = 0;
					item->goal_anim_state = 8;
				}
			}
			else if (item->current_anim_state == 8)
			{
				item->pos.x_rot = 0;
				item->pos.y_pos = item->floor;
			}
			else
			{
				item->anim_number = objects[BIG_BEETLE].anim_index + 5;
				item->frame_number = anims[item->anim_number].frame_base;
				item->gravity_status = 1;
				item->current_anim_state = 6;
				item->speed = 0;
				item->pos.x_rot = 0;
			}
		}

		item->pos.x_rot = 0;
	}
	else
	{
		CreatureAIInfo(item, &info);
		GetCreatureMood(item, &info, 1);

		if (beetle->flags)
			beetle->mood = ESCAPE_MOOD;

		CreatureMood(item, &info, 1);
		angle = CreatureTurn(item, beetle->maximum_turn);

		if (info.distance > 0x900000 || !(GetRandomControl() & 0x7F) || item->hit_status)
			beetle->flags = 0;

		switch (item->current_anim_state)
		{
		case 1:
			item->pos.y_pos = item->floor;
			beetle->maximum_turn = 182;

			if (item->hit_status || info.distance < 0x900000 || beetle->hurt_by_lara || item->ai_bits == MODIFY)
				item->goal_anim_state = 2;

			break;

		case 3:
			beetle->maximum_turn = 1274;

			if (item->required_anim_state)
				item->goal_anim_state = item->required_anim_state;
			else if (info.ahead)
			{
				if (info.distance < 0x10000)
					item->goal_anim_state = 9;
			}

			break;

		case 4:
			beetle->maximum_turn = 1274;

			if (info.ahead)
			{
				if (info.distance < 0x10000)
					item->goal_anim_state = 4;
				else
				{
					item->goal_anim_state = 9;
					item->required_anim_state = 3;
				}
			}
			else if (info.distance < 0x10000)
				item->goal_anim_state = 9;
			else
			{
				item->goal_anim_state = 9;
				item->required_anim_state = 3;
			}

			if (!beetle->flags && item->touch_bits & 0x60)
			{
				lara_item->hit_points -= 50;
				lara_item->hit_status = 1;
				CreatureEffectT(item, &beetle_bite, 5, -1, DoBloodSplat);
				beetle->flags = 1;
			}

			break;

		case 5:
			beetle->flags = 0;
			item->pos.y_pos += 51;

			if (item->pos.y_pos > item->floor)
				item->pos.y_pos = item->floor;

			break;

		case 9:
			beetle->maximum_turn = 1274;

			if (item->required_anim_state)
				item->goal_anim_state = item->required_anim_state;
			else if (item->hit_status || GetRandomControl() < 384 || item->ai_bits == MODIFY ||
				(beetle->mood == BORED_MOOD || GetRandomControl() < 128 && !beetle->hurt_by_lara && item->ai_bits != MODIFY))
				item->goal_anim_state = 3;
			else if (info.ahead && info.distance < 0x10000 && !beetle->flags)
				item->goal_anim_state = 4;

			break;
		}
	}

	CreatureTilt(item, angle * 2);
	CreatureAnimation(item_number, angle, angle);
}

long GetFreeScarab()
{
	SCARAB_STRUCT* fx;
	long lp, free;

	free = next_scarab;
	lp = 0;
	fx = &Scarabs[free];

	while (fx->On)
	{
		if (free == 127)
		{
			free = 0;
			fx = Scarabs;
		}
		else
		{
			free++;
			fx++;
		}

		lp++;

		if (lp >= 128)
			return -1;
	}

	next_scarab = (free + 1) & 0x7F;
	return free;
}

void ClearScarabs()
{
	for (int i = 0; i < 128; i++)
		Scarabs[i].On = 0;

	next_scarab = 0;
	flipeffect = -1;
}

void TriggerScarab(short item_number)
{
	SCARAB_STRUCT* fx;
	ITEM_INFO* item;
	short fx_num;

	item = &items[item_number];

	if (item->trigger_flags && (!item->item_flags[2] || !(GetRandomControl() & 0xF)))
	{
		item->trigger_flags--;

		if (item->item_flags[2] && GetRandomControl() & 1)
			item->item_flags[2]--;

		fx_num = (short)GetFreeScarab();

		if (fx_num != -1)
		{
			fx = &Scarabs[fx_num];
			fx->pos.x_pos = item->pos.x_pos;
			fx->pos.y_pos = item->pos.y_pos;
			fx->pos.z_pos = item->pos.z_pos;
			fx->room_number = item->room_number;

			if (item->item_flags[0])
			{
				fx->pos.y_rot = short(GetRandomControl() << 1);
				fx->fallspeed = -16 - (GetRandomControl() & 0x1F);
			}
			else
			{
				fx->fallspeed = 0;
				fx->pos.y_rot = (GetRandomControl() & 0x3FFF) + item->pos.y_rot - 8192;
			}

			fx->pos.x_rot = 0;
			fx->pos.z_rot = 0;
			fx->On = 1;
			fx->flags = 0;
			fx->speed = (GetRandomControl() & 0x1F) + 1;
		}
	}
}

void UpdateScarabs()
{
	SCARAB_STRUCT* fx;
	FLOOR_INFO* floor;
	long h, dx, dy, dz, oldx, oldy, oldz;
	short angle;

	for (int i = 0; i < 128; i++)
	{
		fx = &Scarabs[i];

		if (fx->On)
		{
			oldx = fx->pos.x_pos;
			oldy = fx->pos.y_pos;
			oldz = fx->pos.z_pos;
			fx->pos.x_pos += fx->speed * phd_sin(fx->pos.y_rot) >> 14;
			fx->pos.y_pos += fx->fallspeed;
			fx->pos.z_pos += fx->speed * phd_cos(fx->pos.y_rot) >> 14;
			fx->fallspeed += 6;
			dx = lara_item->pos.x_pos - fx->pos.x_pos;
			dy = lara_item->pos.y_pos - fx->pos.y_pos;
			dz = lara_item->pos.z_pos - fx->pos.z_pos;
			angle = (short)phd_atan(dz, dx) - fx->pos.y_rot;

			if (abs(dz) < 85 && abs(dy) < 85 && abs(dx) < 85)
			{
				lara_item->hit_points--;
				lara_item->hit_status = 1;
			}

			if (fx->flags)
			{
				if (abs(dx) + abs(dz) > 1024)
				{
					if (fx->speed < (i & 0x1F) + 24)
						fx->speed++;

					if (abs(angle) < 4096)
						fx->pos.y_rot += short((wibble - i) << 3);
					else if (angle < 0)
						fx->pos.y_rot -= 1024;
					else
						fx->pos.y_rot += 1024;
				}
				else
				{
					fx->pos.y_rot += fx->speed & 1 ? 512 : -512;
					fx->speed = 48 - (lara.LitTorch << 6) - (abs(angle) >> 7);

					if (fx->speed < -16)
						fx->speed = i & 0xF;
				}
			}

			floor = GetFloor(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, &fx->room_number);
			h = GetHeight(floor, fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);

			if (h < fx->pos.y_pos - 1280 || h == NO_HEIGHT)
			{
				fx->pos.y_rot += angle > 0 ? 0x4000 : -0x4000;
				fx->pos.x_pos = oldx;
				fx->pos.y_pos = oldy;
				fx->pos.z_pos = oldz;
				fx->fallspeed = 0;
			}
			else if (fx->pos.y_pos > h)
			{
				fx->pos.y_pos = h;
				fx->fallspeed = 0;
				fx->flags = 1;
			}
			
			if (fx->fallspeed < 500)
				fx->pos.x_rot = -64 * fx->fallspeed;
			else
			{
				fx->On = 0;
				next_scarab = 0;
			}
		}
	}
}

void DrawScarabs()
{
	SCARAB_STRUCT* fx;
	short** meshpp;

	meshpp = &meshes[objects[LITTLE_BEETLE].mesh_index + (wibble >> 2 & 2)];

	for (int i = 0; i < 128; i++)
	{
		fx = &Scarabs[i];

		if (fx->On)
		{
			phd_PushMatrix();
			phd_TranslateAbs(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);
			phd_RotYXZ(fx->pos.y_rot, fx->pos.x_rot, fx->pos.z_rot);
			phd_PutPolygons_train(*meshpp, 0);
			phd_PopMatrix();
		}
	}
}

void InitialiseScarabGenerator(short item_number)
{
	ITEM_INFO* item;
	short flag;

	item = &items[item_number];
	flag = item->trigger_flags / 1000;
	item->pos.x_rot = 0x2000;
	item->item_flags[0] = flag & 1;
	item->item_flags[1] = flag & 2;
	item->item_flags[2] = flag & 4;
	item->trigger_flags %= 1000;

	if (!item->item_flags[0])
	{
		if (item->pos.y_rot > 4096 && item->pos.y_rot < 28672)
			item->pos.x_pos -= 512;
		else if (item->pos.y_rot < -4096 && item->pos.y_rot > -28672)
			item->pos.x_pos += 512;

		if (item->pos.y_rot > -8192 && item->pos.y_rot < 8192)
			item->pos.z_pos -= 512;
		else if (item->pos.y_rot < -20480 || item->pos.y_rot > 20480)
			item->pos.z_pos += 512;
	}
}
