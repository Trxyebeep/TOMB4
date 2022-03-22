#include "../tomb4/pch.h"
#include "jeep.h"
#include "../specific/function_stubs.h"
#include "objects.h"
#include "lara_states.h"
#include "collide.h"
#include "control.h"
#include "../specific/3dmath.h"
#include "newinv.h"
#include "../specific/specificfx.h"
#include "effect2.h"
#include "lara1gun.h"
#include "tomb4fx.h"
#include "items.h"
#include "sound.h"
#include "../specific/audio.h"
#include "laraflar.h"
#include "lot.h"

void InitialiseJeep(short item_number)
{
	ITEM_INFO* item;
	JEEPINFO* jeep;

	item = &items[item_number];
	jeep = (JEEPINFO*)game_malloc(sizeof(JEEPINFO));
	item->data = jeep;
	jeep->velocity = 0;
	jeep->unused1 = 0;
	jeep->turn_rate = 0;
	jeep->camera_angle = 0;
	jeep->extra_rotation = 0;
	jeep->move_angle = item->pos.y_rot;
	jeep->pitch2 = 0;
	jeep->flags = 0;
	jeep->gear = 0;
	jeep->right_front_wheelrot = 0;
	jeep->left_front_wheelrot = 0;
	jeep->left_back_wheelrot = 0;
	jeep->right_back_wheelrot = 0;
	item->mesh_bits = 0x1BFFF;
}

static long GetOnJeep(short item_number, COLL_INFO* coll)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	long h;
	short room_number, ang;

	item = &items[item_number];

	if (input & IN_ACTION || GLOBAL_inventoryitemchosen == PUZZLE_ITEM1)
	{
		if (!(item->flags & IFL_INVISIBLE) && lara.gun_status == LG_NO_ARMS && lara_item->current_anim_state == AS_STOP &&
			lara_item->anim_number == ANIM_BREATH && !lara_item->gravity_status)
		{
			if (ABS(item->pos.y_pos - lara_item->pos.y_pos) < 256 && TestBoundsCollide(item, lara_item, 100))
			{
				room_number = item->room_number;
				floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
				h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
				
				if (h >= NO_HEIGHT + 512)	//mmmm
				{
					ang = short(phd_atan(item->pos.z_pos - lara_item->pos.z_pos, item->pos.x_pos - lara_item->pos.x_pos) - item->pos.y_rot);

					if (ang <= -8190 || ang >= 24570)
					{
						ang = lara_item->pos.y_rot - item->pos.y_rot;

						if (ang > -24586 && ang < -8206)
						{
							if (GLOBAL_inventoryitemchosen == PUZZLE_ITEM1)
							{
								GLOBAL_inventoryitemchosen = NO_ITEM;
								return 1;
							}
							
							if (have_i_got_object(PUZZLE_ITEM1))
								GLOBAL_enterinventory = PUZZLE_ITEM1;
						}
					}
					else
					{
						ang = lara_item->pos.y_rot - item->pos.y_rot;

						if (ang > 8190 && ang < 24570)
						{
							if (GLOBAL_inventoryitemchosen == PUZZLE_ITEM1)
							{
								GLOBAL_inventoryitemchosen = NO_ITEM;
								return 1;
							}

							if (have_i_got_object(PUZZLE_ITEM1))
								GLOBAL_enterinventory = PUZZLE_ITEM1;
						}
					}
				}
			}
		}
	}

	return 0;
}

void DrawJeepExtras(ITEM_INFO* item)
{
	JEEPINFO* jeep;

	if (lara.vehicle == NO_ITEM)
		return;

	jeep = (JEEPINFO*)item->data;
	DrawJeepSpeedo(phd_winwidth - 64, phd_winheight - 16, jeep->velocity, 0x6000, 0x8000, 32, jeep->gear);
}

static void TriggerExhaustSmoke(long x, long y, long z, short angle, long velocity, long thing)
{
	SPARKS* sptr;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->dR = 16;
	sptr->dG = 16;
	sptr->dB = 32;
	sptr->sR = 0;
	sptr->sG = 0;
	sptr->sB = 0;

	if (thing)
	{
		sptr->dR = uchar((16 * velocity) >> 5);
		sptr->dG = uchar((16 * velocity) >> 5);
		sptr->dB = uchar((32 * velocity) >> 5);
	}

	sptr->ColFadeSpeed = 4;
	sptr->FadeToBlack = 4;
	sptr->Life = uchar((GetRandomControl() & 3) - (velocity >> 12) + 20);
	sptr->sLife = sptr->Life;

	if (sptr->Life < 9)
	{
		sptr->Life = 9;
		sptr->sLife = 9;
	}

	sptr->TransType = 2;
	sptr->x = (GetRandomControl() & 0xF) + x - 8;
	sptr->y = (GetRandomControl() & 0xF) + y - 8;
	sptr->z = (GetRandomControl() & 0xF) + z - 8;
	sptr->Xvel = velocity * phd_sin(angle) >> 16;
	sptr->Yvel = -8 - (GetRandomControl() & 7);
	sptr->Zvel = velocity * phd_cos(angle) >> 16;
	sptr->Friction = 4;

	if (GetRandomControl() & 1)
	{
		sptr->Flags = 538;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -24 - (GetRandomControl() & 7);
		else
			sptr->RotAdd = (GetRandomControl() & 7) + 24;
	}
	else
		sptr->Flags = 522;

	sptr->Scalar = 1;
	sptr->Gravity = -4 - (GetRandomControl() & 3);
	sptr->MaxYvel = -8 - (GetRandomControl() & 7);
	sptr->dSize = uchar((GetRandomControl() & 7) + (velocity >> 7) + 32);
	sptr->sSize = sptr->dSize >> 1;
	sptr->Size = sptr->dSize >> 1;
}

void JeepExplode(ITEM_INFO* item)
{
	if (room[item->room_number].flags & ROOM_UNDERWATER)
		TriggerUnderwaterExplosion(item, 1);
	else
	{
		TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 3, -2, 0, item->room_number);

		for (int i = 0; i < 3; i++)
			TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 3, -1, 0, item->room_number);
	}

	ExplodingDeath2(lara.vehicle, -1, 256);
	KillItem(lara.vehicle);
	item->status = ITEM_DEACTIVATED;
	SoundEffect(SFX_EXPLOSION1, 0, SFX_DEFAULT);
	SoundEffect(SFX_EXPLOSION2, 0, SFX_DEFAULT);
	lara.vehicle = NO_ITEM;
}

static long JeepCheckGetOut()
{
	if (lara_item->current_anim_state == 10 && lara_item->frame_number == anims[lara_item->anim_number].frame_end)
	{
		lara_item->pos.y_rot += 0x4000;
		lara_item->anim_number = ANIM_STOP;
		lara_item->frame_number = anims[ANIM_STOP].frame_base;
		lara_item->current_anim_state = AS_STOP;
		lara_item->goal_anim_state = AS_STOP;
		lara_item->pos.x_pos -= 512 * phd_sin(lara_item->pos.y_rot) >> 14;
		lara_item->pos.z_pos -= 512 * phd_cos(lara_item->pos.y_rot) >> 14;
		lara_item->pos.x_rot = 0;
		lara_item->pos.z_rot = 0;
		lara.vehicle = NO_ITEM;
		lara.gun_status = LG_NO_ARMS;
		CurrentAtmosphere = 110;
		IsAtmospherePlaying = 1;
		S_CDPlay(110, 1);
	}

	return 1;
}

static long DoDynamics(long height, long fallspeed, long* ypos, long zero)
{
	long bounce;

	if (height <= *ypos)
	{
		if (zero)
			return fallspeed;
		else
		{
			bounce = height - *ypos;

			if (height - *ypos < -80)
				bounce = -80;

			fallspeed += ((bounce - fallspeed) >> 4);

			if (*ypos > height)
				*ypos = height;
		}
	}
	else
	{
		*ypos += fallspeed;

		if (*ypos <= height - 32)
		{
			if (zero)
				fallspeed += zero + (zero >> 1);
			else
				fallspeed += 9;
		}
		else
		{
			*ypos = height;

			if (fallspeed > 150)
				lara_item->hit_points += short(150 - fallspeed);

			fallspeed = 0;
		}
	}

	return fallspeed;
}

static long CanGetOff(short num)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	long x, y, z, h, c;
	short yrot, room_number;

	item = &items[lara.vehicle];
	yrot = item->pos.y_rot + 0x4000;
	x = item->pos.x_pos - (512 * phd_sin(yrot) >> 14);
	y = item->pos.y_pos;
	z = item->pos.z_pos - (512 * phd_cos(yrot) >> 14);
	room_number = item->room_number;
	floor = GetFloor(x, y, z, &room_number);
	h = GetHeight(floor, x, y, z);

	if (height_type != BIG_SLOPE && height_type != DIAGONAL && h != NO_HEIGHT)
	{
		if (ABS(h - item->pos.y_pos) <= 512)
		{
			c = GetCeiling(floor, x, y, z);

			if (c - item->pos.y_pos <= -762 && h - c >= 762)
				return 1;
		}
	}

	return 0;
}

void JeepCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	ITEM_INFO* item2;
	JEEPINFO* jeep;
	short ang;

	if (l->hit_points <= 0 || lara.vehicle != NO_ITEM)
		return;

	item = &items[item_number];

	if (GetOnJeep(item_number, coll))
	{
		lara.vehicle = item_number;

		if (lara.gun_type == WEAPON_FLARE)
		{
			CreateFlare(FLARE_ITEM, 0);
			undraw_flare_meshes();
			lara.flare_control_left = 0;
			lara.gun_type = WEAPON_NONE;
			lara.request_gun_type = WEAPON_NONE;
			lara.flare_age = 0;
		}

		lara.gun_status = LG_HANDS_BUSY;

		for (short item_num = room[item->room_number].item_number; item_num != NO_ITEM; item_num = item2->next_item)
		{
			item2 = &items[item_num];

			if (item2->object_number == ENEMY_JEEP)
			{
				EnableBaddieAI(item_num, 1);
				item2->status = ITEM_ACTIVE;
				AddActiveItem(item_num);
			}
		}

		ang = short(phd_atan(item->pos.z_pos - l->pos.z_pos, item->pos.x_pos - l->pos.x_pos) - item->pos.y_rot);

		if (ang <= -8190 || ang >= 24570)
			l->anim_number = objects[VEHICLE_EXTRA].anim_index + 9;
		else
			l->anim_number = objects[VEHICLE_EXTRA].anim_index + 18;

		l->current_anim_state = 9;
		l->goal_anim_state = 9;
		l->frame_number = anims[l->anim_number].frame_base;
		item->hit_points = 1;
		l->pos.x_pos = item->pos.x_pos;
		l->pos.y_pos = item->pos.y_pos;
		l->pos.z_pos = item->pos.z_pos;
		l->pos.y_rot = item->pos.y_rot;
		lara.head_x_rot = 0;
		lara.head_y_rot = 0;
		lara.torso_x_rot = 0;
		lara.torso_y_rot = 0;
		lara.hit_direction = -1;
		AnimateItem(l);
		jeep = (JEEPINFO*)item->data;
		jeep->unused1 = 0;
		jeep->gear = 0;
		item->flags |= IFL_TRIGGERED;
		CurrentAtmosphere = 98;
		IsAtmospherePlaying = 1;
		S_CDPlay(98, 1);
	}
	else
		ObjectCollision(item_number, l, coll);
}

void inject_jeep(bool replace)
{
	INJECT(0x00466F40, InitialiseJeep, replace);
	INJECT(0x004671B0, GetOnJeep, replace);
	INJECT(0x00467330, DrawJeepExtras, replace);
	INJECT(0x00467920, TriggerExhaustSmoke, replace);
	INJECT(0x00467AC0, JeepExplode, replace);
	INJECT(0x00467B90, JeepCheckGetOut, replace);
	INJECT(0x00467C60, DoDynamics, replace);
	INJECT(0x00469770, CanGetOff, replace);
	INJECT(0x00466FA0, JeepCollision, replace);
}
