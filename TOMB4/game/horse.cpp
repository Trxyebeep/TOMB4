#include "../tomb4/pch.h"
#include "horse.h"
#include "objects.h"
#include "box.h"
#include "effect2.h"
#include "../specific/function_stubs.h"
#include "../specific/3dmath.h"
#include "control.h"
#include "items.h"
#include "sound.h"
#include "sphere.h"
#include "effects.h"
#include "lara.h"

static BITE_INFO horseman_axe = { 0, 0, 0, 6 };
static BITE_INFO horseman_lfoot = { 0, 0, 0, 14 };
static BITE_INFO horseman_shield = { 0, 0, 0, 10 };
static BITE_INFO horse_lfoot = { 0, 0, 0, 13 };
static BITE_INFO horse_rfoot = { 0, 0, 0, 17 };
static BITE_INFO horse_head = { 0, 0, 0, 19 };

void InitialiseHorseman(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[HORSEMAN].anim_index + 8;
	item->frame_number = anims[item->anim_number].frame_base;
	item->goal_anim_state = 9;
	item->current_anim_state = 9;
	item->item_flags[0] = -1;
	item->dynamic_light = 0;
}

#pragma warning(push)
#pragma warning(disable : 4551)
void HorsemanControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* item2;
	CREATURE_INFO* horseman;
	AI_INFO info, larainfo;
	PHD_VECTOR v;
	long x, z, c, s, h1, h2;
	short room_number, angle, rot;

	room_number = 0;

	if (CreatureActive(item_number))
	{
		item = &items[item_number];

		if (item->item_flags[0] == -1)
		{
			for (int i = 0; i < level_items; i++)
			{
				item2 = &items[i];

				if (item2->object_number == HORSE && item->trigger_flags == item2->trigger_flags)
				{
					item->item_flags[0] = i;
					item2->flags |= IFL_TRIGGERED;
				}
			}

			if (item->item_flags[0] == -1)
				item->item_flags[0] = 0;
		}

		if (item->item_flags[0])
			item2 = &items[item->item_flags[0]];
		else
			item2 = 0;

		horseman = (CREATURE_INFO*)item->data;
		angle = 0;

		if (item2)
		{
			room_number = item->room_number;
			c = 341 * phd_cos(item2->pos.y_rot) >> W2V_SHIFT;
			s = 341 * phd_sin(item2->pos.y_rot) >> W2V_SHIFT;
			x = item2->pos.x_pos + s;
			z = item2->pos.z_pos + c;
			h1 = GetHeight(GetFloor(x, item2->pos.y_pos, z, &room_number), x, item2->pos.y_pos, z);
			x = item2->pos.x_pos - s;
			z = item2->pos.z_pos - c;
			h2 = GetHeight(GetFloor(x, item2->pos.y_pos, z, &room_number), x, item2->pos.y_pos, z);
			rot = (short)phd_atan(682, h2 - h1);
		}

		if (item->hit_points <= 0)
		{
			item->hit_points = 0;

			if (!item->item_flags[1])
			{
				if (item->current_anim_state != 16)
				{
					item->anim_number = objects[HORSEMAN].anim_index + 21;
					item->frame_number = anims[item->anim_number].frame_base;
					item->current_anim_state = 16;

					if (item->item_flags[0])
						items[item->item_flags[0]].after_death = 1;
				}
			}
			else
			{
				item->hit_points = 100;
				item->ai_bits = 0;
				item->item_flags[1] = 0;
				horseman->enemy = 0;
				item->anim_number = objects[HORSEMAN].anim_index + 3;
				item->frame_number = anims[item->anim_number].frame_base;
				item->current_anim_state = 8;
				item2->goal_anim_state = 1;
				item->dynamic_light = 1;	//Flag for enemy bar to use the new health. bite me.
			}
		}
		else
		{
			if (item->ai_bits)
				GetAITarget(horseman);
			else if (horseman->hurt_by_lara)
				horseman->enemy = lara_item;

			CreatureAIInfo(item, &info);

			if (horseman->enemy == lara_item)
			{
				larainfo.angle = info.angle;
				larainfo.distance = info.distance;
			}
			else
			{
				z = horseman->enemy->pos.z_pos - item->pos.z_pos;
				x = horseman->enemy->pos.x_pos - item->pos.x_pos;
				larainfo.angle = short(phd_atan(z, x) - item->pos.y_rot);
				larainfo.distance = SQUARE(x) + SQUARE(z);
			}

			if (item->hit_status)
			{
				item->hit_status = 0;

				if (larainfo.angle < 12288 && larainfo.angle > -12288 && larainfo.distance < 0x400000)
				{
					if (item->current_anim_state == 15 || larainfo.angle > 0 || !item->item_flags[1])
					{
						if (item->current_anim_state != 15 && (larainfo.angle > 0 || item->mesh_bits & 0x400))
						{
							if (lara.gun_type == WEAPON_SHOTGUN)
							{
								item->hit_points -= 10;
								item->hit_status = 1;
							}
							else if (lara.gun_type == WEAPON_REVOLVER)
							{
								item->hit_points -= 20;
								item->hit_status = 1;
							}
							else
								item->hit_points--;

							SoundEffect(SFX_HORSEMAN_TAKEHIT, &item->pos, SFX_DEFAULT);
							SoundEffect(SFX_HORSE_RICOCHETS, &item->pos, SFX_DEFAULT);
							v.x = 0;
							v.y = -128;
							v.z = 80;
							GetJointAbsPosition(item, &v, 1);
							TriggerHorsemanRicochets(&v, item->pos.y_rot, 7);
						}
						else if (!(GetRandomControl() & 7))
						{
							if (item->current_anim_state == 15)
								item->goal_anim_state = 9;

							ExplodeItemNode(item, 10, 1, -24);
						}
					}

					if (!item->item_flags[1] && item->mesh_bits & 0x400)
						item->required_anim_state = 15;
				}
			}

			GetCreatureMood(item, &info, 1);
			CreatureMood(item, &info, 1);
			angle = CreatureTurn(item, horseman->maximum_turn);

			switch (item->current_anim_state)
			{
			case 1:
				horseman->maximum_turn = 546;
				item2->goal_anim_state = 2;

				if (item->required_anim_state)
				{
					item->goal_anim_state = 17;
					item2->goal_anim_state = 5;
				}
				else if (horseman->flags || horseman->reached_goal || item->hit_status && !GetRandomControl)
				{
					if (larainfo.distance <= 0x1000000 && !horseman->reached_goal)
					{
						item->ai_bits = FOLLOW;

						if (item->item_flags[3] == 1)
							item->item_flags[3] = 2;
						else
							item->item_flags[3] = 1;
					}
					else
					{
						horseman->flags = 0;
						horseman->enemy = lara_item;

						if (larainfo.angle > -8192 && larainfo.angle < 8192)
						{
							item->goal_anim_state = 3;
							item2->goal_anim_state = 1;
						}
					}
				}

				if (info.bite)
				{
					if (info.distance < 0x100000 && info.angle < 1820 && info.angle > -1820)
					{
						item->goal_anim_state = 3;
						item2->goal_anim_state = 1;
					}
					else if (info.angle < -1820 && (info.distance < 0x100000 || info.distance < 0x1C6E39 && info.angle > -3640))
					{
						horseman->maximum_turn = 0;
						item->goal_anim_state = 7;
					}
					else if (info.angle > 1820 && (info.distance < 0x100000 || info.distance < 0x1C6E39 && info.angle < 3640))
					{
						horseman->maximum_turn = 0;
						item->goal_anim_state = 6;
					}
				}

				break;

			case 2:
				horseman->maximum_turn = 273;

				if (larainfo.distance > 0x1000000 || horseman->reached_goal || horseman->enemy == lara_item)
				{
					horseman->reached_goal = 0;
					horseman->flags = 0;
					item->goal_anim_state = 1;
					item2->goal_anim_state = 2;
					horseman->enemy = lara_item;
				}

				break;

			case 3:
				horseman->maximum_turn = 0;
				item2->goal_anim_state = 1;

				if (horseman->flags)
				{
					item->ai_bits = FOLLOW;

					if (item->item_flags[3] == 1)
						item->item_flags[3] = 2;
					else
						item->item_flags[3] = 1;
				}
				else
					horseman->flags = 0;

				if (item->required_anim_state)
				{
					item->goal_anim_state = 1;
					item2->goal_anim_state = 2;
					item2->flags = 0;
				}
				else if (!horseman->reached_goal && (item2->flags || info.distance >= 0x100000 || !info.bite || info.angle >= 1820 || info.angle <= -1820))
				{
					item->goal_anim_state = 1;
					item2->goal_anim_state = 2;
					item2->flags = 0;
				}
				else
				{
					item->goal_anim_state = 4;

					if (horseman->reached_goal)
						item->required_anim_state = 17;

					item2->flags = 0;
				}

				break;

			case 4:
				horseman->maximum_turn = 0;

				if (item->frame_number == anims[item->anim_number].frame_base)
				{
					item2->anim_number = objects[HORSE].anim_index + 1;
					item2->frame_number = anims[item->anim_number].frame_base;
					item2->current_anim_state = 4;
				}

				if (!item2->flags && item2->touch_bits & 0x22000)
				{
					lara_item->hit_points -= 150;
					lara_item->hit_status = 1;

					if (item2->touch_bits & 0x2000)
						CreatureEffectT(item2, &horse_lfoot, 10, -1, DoBloodSplat);
					else
						CreatureEffectT(item2, &horse_rfoot, 10, -1, DoBloodSplat);

					item2->flags = 1;
				}

				break;

			case 6:

				if (!horseman->flags && item->touch_bits & 0x60)
				{
					lara_item->hit_points -= 250;
					lara_item->hit_status = 1;
					CreatureEffectT(item, &horseman_axe, 10, item->pos.y_rot, DoBloodSplat);
					horseman->flags = 1;
				}

				if (item->hit_status)
					item->goal_anim_state = 9;

				break;

			case 7:

				if (!horseman->flags && item->touch_bits & 0x4000)
				{
					lara_item->hit_points -= 100;
					lara_item->hit_status = 1;
					CreatureEffectT(item, &horseman_lfoot, 3, item->pos.y_rot, DoBloodSplat);
					horseman->flags = 1;
				}

				break;

			case 9:
				horseman->maximum_turn = 0;
				horseman->flags = 0;

				if (item->ai_bits && !item->item_flags[3])
					item->goal_anim_state = 10;
				else if (item->required_anim_state)
					item->goal_anim_state = item->required_anim_state;
				else if (info.bite && info.distance < 0x718E4)
					item->goal_anim_state = 14;
				else if (info.distance < 0x2400000 && info.distance > 0x718E4)
					item->goal_anim_state = 10;

				break;

			case 10:
				horseman->maximum_turn = 546;
				horseman->flags = 0;

				if (horseman->reached_goal)
				{
					room_number = item->room_number;
					item->item_flags[1] = 1;
					item->ai_bits = 0;
					item->pos = item2->pos;
					horseman->reached_goal = 0;
					horseman->enemy = 0;
					item->anim_number = objects[HORSEMAN].anim_index + 14;
					item->frame_number = anims[item->anim_number].frame_base;
					item->current_anim_state = 5;
					horseman->maximum_turn = 0;
				}
				else if (item->hit_status)
					item->goal_anim_state = 9;
				else if (info.bite && info.distance < 0x718E4)
				{
					if (GetRandomControl() & 1)
						item->goal_anim_state = 12;
					else if (GetRandomControl() & 1)
						item->goal_anim_state = 13;
					else
						item->goal_anim_state = 9;
				}
				else if (info.distance < 0x1900000 && info.distance > 0x1C6E39)
					item->goal_anim_state = 11;

				break;

			case 11:

				if (info.distance < 0x1C6E39)
					item->goal_anim_state = 10;

				break;

			case 12:
			case 13:
			case 14:
				horseman->maximum_turn = 0;

				if (abs(info.angle) < 546)
					item->pos.y_rot += info.angle;
				else if (info.angle < 0)
					item->pos.y_rot -= 546;
				else
					item->pos.y_rot += 546;

				break;

			case 15:

				if (lara.target != item || info.bite && info.distance < 0x718E4)
					item->goal_anim_state = 9;

				break;

			case 17:
				horseman->reached_goal = 0;
				horseman->maximum_turn = 546;

				if (!item2->flags && item2->touch_bits & 0xA2000)
				{
					lara_item->hit_points -= 150;
					lara_item->hit_status = 1;

					if (item2->touch_bits & 0x2000)
						CreatureEffectT(item2, &horse_lfoot, 10, -1, DoBloodSplat);

					if (item2->touch_bits & 0x20000)
						CreatureEffectT(item2, &horse_rfoot, 10, -1, DoBloodSplat);

					if (item2->touch_bits & 0x80000)
						CreatureEffectT(item2, &horse_head, 10, -1, DoBloodSplat);

					item2->flags = 1;
				}

				if (!horseman->flags && item->touch_bits & 0x460)
				{
					lara_item->hit_status = 1;

					if (item->touch_bits & 0x60)
					{
						CreatureEffectT(item2, &horseman_axe, 20, -1, DoBloodSplat);
						lara_item->hit_points -= 250;
					}
					else if (item->touch_bits & 0x400)
					{
						CreatureEffectT(item2, &horseman_shield, 10, -1, DoBloodSplat);
						lara_item->hit_points -= 150;
					}

					horseman->flags = 1;
				}

				if (item->anim_number == objects[HORSEMAN].anim_index + 29 && item->frame_number == anims[item->anim_number].frame_base)
				{
					item2->anim_number = objects[HORSE].anim_index + 10;
					item2->frame_number = anims[item2->anim_number].frame_base;
				}

				if (larainfo.distance <= 0x1000000 && !horseman->reached_goal)
				{
					if (!info.ahead)
					{
						item->goal_anim_state = 3;
						item2->goal_anim_state = 1;
					}
				}
				else
				{
					horseman->reached_goal = 0;
					horseman->flags = 0;
					horseman->enemy = lara_item;
				}

				break;
			}
		}

		if (item2 && item->item_flags[1])
		{
			if (abs(rot - item->pos.x_rot) < 256)
				item->pos.x_rot = 0;
			else if (rot > item->pos.x_rot)
				item->pos.x_rot += 256;
			else if (rot < item->pos.x_rot)
				item->pos.x_rot -= 256;

			item2->pos = item->pos;

			if (item2->room_number != item->room_number)
				ItemNewRoom(item->item_flags[0], item->room_number);

			AnimateItem(item2);
		}

		if (item->item_flags[1])
			objects[HORSEMAN].radius = 409;
		else
			objects[HORSEMAN].radius = 170;

		CreatureAnimation(item_number, angle, 0);
	}
}
#pragma warning(pop)

void TriggerHorsemanRicochets(PHD_VECTOR* pos, long yrot, long num)
{
	SPARKS* sptr;
	long random, rot;

	for (int i = 0; i < num; i++)
	{
		sptr = &spark[GetFreeSpark()];
		random = GetRandomControl();
		sptr->On = 1;
		sptr->sR = 0;
		sptr->sG = 128;
		sptr->sB = (random & 0xF) + 16;
		sptr->dR = 0;
		sptr->dG = 96;
		sptr->dB = (random >> 4 & 0x1F) + 48;
		sptr->ColFadeSpeed = 2;
		sptr->FadeToBlack = 4;
		sptr->Life = 9;
		sptr->sLife = 9;
		sptr->TransType = 2;
		sptr->x = pos->x;
		sptr->y = pos->y;
		sptr->z = pos->z;
		sptr->Gravity = random >> 7 & 0x1F;
		rot = 2 * ((random >> 3 & 0x7FF) + yrot - 1024 & 0xFFF);
		sptr->Xvel = -rcossin_tbl[rot] >> 2;
		sptr->Yvel = (random & 0xFFF) - 2048;
		sptr->Zvel = rcossin_tbl[rot + 1] >> 2;
		sptr->Friction = 34;
		sptr->Flags = 0;
		sptr->MaxYvel = 0;
	}

	for (int i = 0; i < num; i++)
	{
		sptr = &spark[GetFreeSpark()];
		random = GetRandomControl();
		sptr->On = 1;
		sptr->sR = 0;
		sptr->sG = 128;
		sptr->sB = (random & 0xF) + 16;
		sptr->dR = 0;
		sptr->dG = 96;
		sptr->dB = (random >> 4 & 0x1F) + 48;
		sptr->ColFadeSpeed = 2;
		sptr->FadeToBlack = 4;
		sptr->Life = 9;
		sptr->sLife = 9;
		sptr->TransType = 2;
		sptr->x = pos->x;
		sptr->y = pos->y;
		sptr->z = pos->z;
		rot = 2 * ((random >> 3 & 0x7FF) + yrot - 1024 & 0xFFF);
		sptr->Xvel = -rcossin_tbl[rot] >> 2;
		sptr->Yvel = (random & 0xFFF) - 2048;
		sptr->Zvel = rcossin_tbl[rot + 1] >> 2;
		sptr->Gravity = random >> 7 & 0x1F;
		sptr->RotAng = short(random >> 3);

		if (random & 1)
			sptr->RotAdd = 240 - (random & 0xF);
		else
			sptr->RotAdd = (random & 0xF) + 16;

		sptr->Scalar = 3;
		sptr->sSize = (random >> 5 & 0x7) + 4;
		sptr->Size = sptr->sSize;
		sptr->dSize = sptr->sSize >> 1;
		sptr->Friction = 34;
		sptr->Flags = 26;
		sptr->MaxYvel = 0;
	}
}

void InitialiseHorse(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->anim_number = objects[HORSE].anim_index + 2;
	item->frame_number = anims[item->anim_number].frame_base;
	item->goal_anim_state = 1;
	item->current_anim_state = 1;
}
