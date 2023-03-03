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
#include "lara.h"
#include "sphere.h"
#include "effects.h"
#include "traps.h"
#include "debris.h"
#include "bike.h"
#include "box.h"
#include "switch.h"
#include "camera.h"
#include "draw.h"
#include "../specific/input.h"
#include "laramisc.h"
#include "../specific/file.h"

static short jroomies[22];
static char dont_exit_jeep = 0;

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
			if (abs(item->pos.y_pos - lara_item->pos.y_pos) < 256 && TestBoundsCollide(item, lara_item, 100))
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
		lara_item->pos.x_pos -= 512 * phd_sin(lara_item->pos.y_rot) >> W2V_SHIFT;
		lara_item->pos.z_pos -= 512 * phd_cos(lara_item->pos.y_rot) >> W2V_SHIFT;
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
	x = item->pos.x_pos - (512 * phd_sin(yrot) >> W2V_SHIFT);
	y = item->pos.y_pos;
	z = item->pos.z_pos - (512 * phd_cos(yrot) >> W2V_SHIFT);
	room_number = item->room_number;
	floor = GetFloor(x, y, z, &room_number);
	h = GetHeight(floor, x, y, z);

	if (height_type != BIG_SLOPE && height_type != DIAGONAL && h != NO_HEIGHT)
	{
		if (abs(h - item->pos.y_pos) <= 512)
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

long GetCollisionAnim(ITEM_INFO* item, PHD_VECTOR* pos, BIKEINFO* vehicle)
{
	long sin, cos, fb, lr;
	pos->x = item->pos.x_pos - pos->x;
	pos->z = item->pos.z_pos - pos->z;

	if (!pos->x && !pos->z)
		return 0;

	sin = phd_sin(item->pos.y_rot);
	cos = phd_cos(item->pos.y_rot);
	fb = (sin * pos->x + cos * pos->z) >> W2V_SHIFT;
	lr = (cos * pos->x - sin * pos->z) >> W2V_SHIFT;

	if (abs(fb) <= abs(lr))
	{
		if (lr > 0)
			return 11;
		else
			return 12;
	}
	else
	{
		if (fb > 0)
			return 14;
		else
			return 13;
	}
}

long DoShift(ITEM_INFO* item, PHD_VECTOR* newPos, PHD_VECTOR* oldPos)
{
	FLOOR_INFO* floor;
	long x, z, nX, nZ, oX, oZ, sX, sZ, h;
	short room_number;

	nX = newPos->x >> 10;
	nZ = newPos->z >> 10;
	oX = oldPos->x >> 10;
	oZ = oldPos->z >> 10;
	sX = newPos->x & 0x3FF;
	sZ = newPos->z & 0x3FF;

	if (nX == oX)
	{
		if (nZ == oZ)
		{
			item->pos.z_pos += (oldPos->z - newPos->z);
			item->pos.x_pos += (oldPos->x - newPos->x);
			return 0;
		}
		else if (nZ <= oZ)
		{
			item->pos.z_pos += 1025 - sZ;
			return item->pos.x_pos - newPos->x;
		}
		else
		{
			item->pos.z_pos -= 1 + sZ;
			return newPos->x - item->pos.x_pos;
		}
	}

	if (nZ == oZ)
	{
		if (nX <= oX)
		{
			item->pos.x_pos += 1025 - sX;
			return newPos->z - item->pos.z_pos;
		}
		else
		{
			item->pos.x_pos -= 1 + sX;
			return item->pos.z_pos - newPos->z;
		}
	}

	x = 0;
	z = 0;
	room_number = item->room_number;
	floor = GetFloor(oldPos->x, newPos->y, newPos->z, &room_number);
	h = GetHeight(floor, oldPos->x, newPos->y, newPos->z);

	if (h < oldPos->y - 256)
	{
		if (newPos->z > oldPos->z)
			z = -1 - sZ;
		else
			z = 1025 - sZ;
	}

	room_number = item->room_number;
	floor = GetFloor(newPos->x, newPos->y, oldPos->z, &room_number);
	h = GetHeight(floor, newPos->x, newPos->y, oldPos->z);

	if (h < oldPos->y - 256)
	{
		if (newPos->x > oldPos->x)
			x = -1 - sX;
		else
			x = 1025 - sX;
	}

	if (x && z)
	{
		item->pos.x_pos += x;
		item->pos.z_pos += z;
		return 0;
	}

	if (z)
	{
		item->pos.z_pos += z;

		if (z > 0)
			return item->pos.x_pos - newPos->x;
		else
			return newPos->x - item->pos.x_pos;
	}

	if (x)
	{
		item->pos.x_pos += x;

		if (x > 0)
			return newPos->z - item->pos.z_pos;
		else
			return item->pos.z_pos - newPos->z;
	}

	item->pos.x_pos += oldPos->x - newPos->x;
	item->pos.z_pos += oldPos->z - newPos->z;
	return 0;
}

static void AnimateJeep(ITEM_INFO* item, long hitWall, long killed)
{
	JEEPINFO* jeep;
	short state;

	jeep = (JEEPINFO*)item->data;
	state = lara_item->current_anim_state;

	if (item->pos.y_pos != item->floor && state != 11 && state != 12 && !killed)
	{
		if (jeep->gear == 1)
			lara_item->anim_number = objects[VEHICLE_EXTRA].anim_index + 20;
		else
			lara_item->anim_number = objects[VEHICLE_EXTRA].anim_index + 6;

		lara_item->frame_number = anims[lara_item->anim_number].frame_base;
		lara_item->current_anim_state = 11;
		lara_item->goal_anim_state = 11;
	}
	else if (hitWall && state != 4 && state != 5 && state != 2 && state != 3 && state != 11 && jeep->velocity > 10922 && !killed)
	{
		switch (hitWall)
		{
		case 13:
			lara_item->current_anim_state = 4;
			lara_item->goal_anim_state = 4;
			lara_item->anim_number = objects[VEHICLE_EXTRA].anim_index + 11;
			break;

		case 14:
			lara_item->current_anim_state = 5;
			lara_item->goal_anim_state = 5;
			lara_item->anim_number = objects[VEHICLE_EXTRA].anim_index + 10;
			break;

		case 11:
			lara_item->current_anim_state = 2;
			lara_item->goal_anim_state = 2;
			lara_item->anim_number = objects[VEHICLE_EXTRA].anim_index + 12;
			break;

		default:
			lara_item->current_anim_state = 3;
			lara_item->goal_anim_state = 3;
			lara_item->anim_number = objects[VEHICLE_EXTRA].anim_index + 13;
			break;
		}

		lara_item->frame_number = anims[lara_item->anim_number].frame_base;
	}
	else
	{
		switch (lara_item->current_anim_state)
		{
		case 0:

			if (killed)
				lara_item->goal_anim_state = 16;
			else if (((input & (IN_JUMP | IN_LEFT)) == (IN_JUMP | IN_LEFT)) && !jeep->velocity && !dont_exit_jeep)
			{
				if (CanGetOff(0))
					lara_item->goal_anim_state = 10;
			}
			else if (dbinput & IN_WALK)
			{
				if (jeep->gear)
					jeep->gear--;
			}
			else if (dbinput & IN_SPRINT)
			{
				if (jeep->gear < 1)
					jeep->gear++;

				if (jeep->gear == 1)
					lara_item->goal_anim_state = 17;
			}
			else if (input & IN_ACTION && !(input & IN_JUMP))
				lara_item->goal_anim_state = 1;
			else if (input & (IN_LSTEP | IN_LEFT))
				lara_item->goal_anim_state = 7;
			else if (input & (IN_RSTEP | IN_RIGHT))
				lara_item->goal_anim_state = 8;

			break;

		case 1:

			if (killed)
				lara_item->goal_anim_state = 0;
			else if (jeep->velocity & 0xFFFFFF00 || input & (IN_ACTION | IN_JUMP))
			{
				if (input & IN_JUMP)
				{
					if (jeep->velocity > 21844)
						lara_item->goal_anim_state = 6;
					else
						lara_item->goal_anim_state = 0;
				}
				else if (input & (IN_LSTEP | IN_LEFT))
					lara_item->goal_anim_state = 7;
				else if (input & (IN_RSTEP | IN_RIGHT))
					lara_item->goal_anim_state = 8;
			}
			else
				lara_item->goal_anim_state = 0;

			break;

		case 2:
		case 3:
		case 4:
		case 5:

			if (killed)
				lara_item->goal_anim_state = 0;
			else if (input & (IN_ACTION | IN_JUMP))
				lara_item->goal_anim_state = 1;

			break;

		case 6:

			if (killed)
				lara_item->goal_anim_state = 0;
			else if (jeep->velocity & 0xFFFFFF00)
			{
				if (input & (IN_LSTEP | IN_LEFT))
					lara_item->goal_anim_state = 7;
				else if (input & (IN_RSTEP | IN_RIGHT))
					lara_item->goal_anim_state = 8;
			}
			else
				lara_item->goal_anim_state = 0;

			break;

		case 7:

			if (killed)
				lara_item->goal_anim_state = 0;
			else if (dbinput & IN_WALK)
			{
				if (jeep->gear)
					jeep->gear--;
			}
			else if (dbinput & IN_SPRINT)
			{
				if (jeep->gear < 1)
					jeep->gear++;

				if (jeep->gear == 1)
				{
					lara_item->current_anim_state = 15;
					lara_item->goal_anim_state = 15;
					lara_item->anim_number = objects[VEHICLE_EXTRA].anim_index + 40;
					lara_item->frame_number = anims[lara_item->anim_number].frame_base;
					break;
				}
			}
			else if (input & (IN_RSTEP | IN_RIGHT))
				lara_item->goal_anim_state = 1;
			else if (input & (IN_LSTEP | IN_LEFT))
				lara_item->goal_anim_state = 7;
			else if (jeep->velocity)
				lara_item->goal_anim_state = 1;
			else
				lara_item->goal_anim_state = 0;

			if (lara_item->anim_number == objects[VEHICLE_EXTRA].anim_index + 4 && !jeep->velocity)
			{
				lara_item->anim_number = objects[VEHICLE_EXTRA].anim_index + 32;
				lara_item->frame_number = anims[lara_item->anim_number].frame_base + 14;
			}

			if (lara_item->anim_number == objects[VEHICLE_EXTRA].anim_index + 32 && jeep->velocity)
			{
				lara_item->anim_number = objects[VEHICLE_EXTRA].anim_index + 4;
				lara_item->frame_number = anims[lara_item->anim_number].frame_base;
			}

			break;

		case 8:

			if (killed)
				lara_item->goal_anim_state = 0;
			else if (dbinput & IN_WALK)
			{
				if (jeep->gear)
					jeep->gear--;
			}
			else if (dbinput & IN_SPRINT)
			{
				if (jeep->gear < 1)
					jeep->gear++;

				if (jeep->gear == 1)
				{
					lara_item->current_anim_state = 14;
					lara_item->goal_anim_state = 14;
					lara_item->anim_number = objects[VEHICLE_EXTRA].anim_index + 41;
					lara_item->frame_number = anims[lara_item->anim_number].frame_base;
					break;
				}
			}
			else if (input & (IN_LSTEP | IN_LEFT))
				lara_item->goal_anim_state = 1;
			else if (input & (IN_RSTEP | IN_RIGHT))
				lara_item->goal_anim_state = 8;
			else if (jeep->velocity)
				lara_item->goal_anim_state = 1;
			else
				lara_item->goal_anim_state = 0;

			if (lara_item->anim_number == objects[VEHICLE_EXTRA].anim_index + 16 && !jeep->velocity)
			{
				lara_item->anim_number = objects[VEHICLE_EXTRA].anim_index + 33;
				lara_item->frame_number = anims[lara_item->anim_number].frame_base + 14;
			}

			if (lara_item->anim_number == objects[VEHICLE_EXTRA].anim_index + 33 && jeep->velocity)
			{
				lara_item->anim_number = objects[VEHICLE_EXTRA].anim_index + 16;
				lara_item->frame_number = anims[lara_item->anim_number].frame_base;
			}

			break;

		case 11:

			if (item->pos.y_pos == item->floor)
				lara_item->goal_anim_state = 12;
			else if (item->fallspeed > 300)
				jeep->flags |= 0x40;

			break;

		case 13:

			if (killed)
				lara_item->goal_anim_state = 17;
			else if (abs(jeep->velocity) & 0xFFFFFF00)
			{
				if (input & (IN_LSTEP | IN_LEFT))
					lara_item->goal_anim_state = 15;
				else if (input & (IN_RSTEP | IN_RIGHT))
					lara_item->goal_anim_state = 14;
			}
			else
				lara_item->goal_anim_state = 17;

			break;

		case 14:

			if (killed)
				lara_item->goal_anim_state = 17;
			else if (dbinput & IN_WALK)
			{
				if (jeep->gear)
				{
					jeep->gear--;

					if (!jeep->gear)
					{
						lara_item->current_anim_state = 8;
						lara_item->goal_anim_state = 8;
						lara_item->anim_number = objects[VEHICLE_EXTRA].anim_index + 44;
						lara_item->frame_number = anims[lara_item->anim_number].frame_base;
						break;
					}
				}
			}
			else if (dbinput & IN_SPRINT)
			{
				if (jeep->gear < 1)
					jeep->gear++;
			}
			else if (input & (IN_RSTEP | IN_RIGHT))
				lara_item->goal_anim_state = 14;
			else
				lara_item->goal_anim_state = 13;

			if (lara_item->anim_number == objects[VEHICLE_EXTRA].anim_index + 30 && !jeep->velocity)
			{
				lara_item->anim_number = objects[VEHICLE_EXTRA].anim_index + 37;
				lara_item->frame_number = anims[lara_item->anim_number].frame_base + 14;
			}

			if (lara_item->anim_number == objects[VEHICLE_EXTRA].anim_index + 37 && jeep->velocity)
			{
				lara_item->anim_number = objects[VEHICLE_EXTRA].anim_index + 30;
				lara_item->frame_number = anims[lara_item->anim_number].frame_base;
			}

			break;

		case 15:

			if (killed)
				lara_item->goal_anim_state = 17;
			else if (dbinput & IN_WALK)
			{
				if (jeep->gear)
				{
					jeep->gear--;

					if (!jeep->gear)
					{
						lara_item->current_anim_state = 7;
						lara_item->goal_anim_state = 7;
						lara_item->anim_number = objects[VEHICLE_EXTRA].anim_index + 44;
						lara_item->frame_number = anims[lara_item->anim_number].frame_base;
						break;
					}
				}
			}
			else if (dbinput & IN_SPRINT)
			{
				if (jeep->gear < 1)
					jeep->gear++;
			}
			else if (input & (IN_LSTEP | IN_LEFT))
				lara_item->goal_anim_state = 15;
			else
				lara_item->goal_anim_state = 13;

			if (lara_item->anim_number == objects[VEHICLE_EXTRA].anim_index + 27 && !jeep->velocity)
			{
				lara_item->anim_number = objects[VEHICLE_EXTRA].anim_index + 36;
				lara_item->frame_number = anims[lara_item->anim_number].frame_base + 14;
			}

			if (lara_item->anim_number == objects[VEHICLE_EXTRA].anim_index + 36 && jeep->velocity)
			{
				lara_item->anim_number = objects[VEHICLE_EXTRA].anim_index + 27;
				lara_item->frame_number = anims[lara_item->anim_number].frame_base;
			}

			break;

		case 17:

			if (killed)
				lara_item->goal_anim_state = 0;

			if (((input & (IN_JUMP | IN_LEFT)) == (IN_JUMP | IN_LEFT)) && !jeep->velocity && !dont_exit_jeep)
			{
				if (CanGetOff(0))
					lara_item->goal_anim_state = 10;
			}
			else if (dbinput & IN_WALK)
			{
				if (jeep->gear)
				{
					jeep->gear--;

					if (!jeep->gear)
						lara_item->goal_anim_state = 0;
				}
			}
			else if (dbinput & IN_SPRINT)
			{
				if (jeep->gear < 1)
					jeep->gear++;
			}
			else if (input & IN_ACTION && !(input & IN_JUMP))
				lara_item->goal_anim_state = 13;
			else if (input & (IN_LSTEP | IN_LEFT))
				lara_item->goal_anim_state = 15;
			else if (input & (IN_RSTEP | IN_RIGHT))
				lara_item->goal_anim_state = 14;

			break;
		}
	}

	if (room[item->room_number].flags & ROOM_UNDERWATER)
	{
		lara_item->goal_anim_state = 11;
		lara_item->hit_points = 0;
		JeepExplode(item);
	}
}

static long UserControl(ITEM_INFO* item, long height, long* pitch)
{
	JEEPINFO* jeep;
	PHD_VECTOR pos;
	long turn, maxTurn, vel;

	if (lara_item->current_anim_state == 10 || lara_item->goal_anim_state == 10)
		input = 0;

	jeep = (JEEPINFO*)item->data;

	if (jeep->unused1 > 16)
	{
		jeep->velocity += jeep->unused1 >> 4;
		jeep->unused1 = jeep->unused1 - (jeep->unused1 >> 3);
	}
	else
		jeep->unused1 = 0;

	if (item->pos.y_pos >= height - 256)
	{
		if (!jeep->velocity && input & IN_LOOK)
			LookUpDown();

		vel = abs(jeep->velocity);

		if (vel > 0x4000)
		{
			maxTurn = 910;
			turn = 242;
		}
		else
		{
			maxTurn = (910 * vel) >> 14;
			turn = ((60 * vel) >> 14) + 182;
		}

		if (jeep->velocity > 0)
		{
			if (input & (IN_LSTEP | IN_LEFT))
			{
				jeep->turn_rate -= turn;

				if (jeep->turn_rate < -maxTurn)
					jeep->turn_rate = -maxTurn;
			}
			else if (input & (IN_RSTEP | IN_RIGHT))
			{
				jeep->turn_rate += turn;

				if (jeep->turn_rate > maxTurn)
					jeep->turn_rate = maxTurn;
			}
		}
		else if (jeep->velocity < 0)
		{
			if (input & (IN_LSTEP | IN_LEFT))
			{
				jeep->turn_rate += turn;

				if (jeep->turn_rate > maxTurn)
					jeep->turn_rate = maxTurn;
			}
			else if (input & (IN_RSTEP | IN_RIGHT))
			{
				jeep->turn_rate -= turn;

				if (jeep->turn_rate < -maxTurn)
					jeep->turn_rate = -maxTurn;
			}
		}

		if (input & IN_JUMP)
		{
			if (jeep->velocity > 0)
			{
				jeep->velocity -= 768;

				if (jeep->velocity < 0)
					jeep->velocity = 0;
			}
			else if (jeep->velocity < 0)
			{
				jeep->velocity += 768;

				if (jeep->velocity > 0)
					jeep->velocity = 0;
			}
		}
		else if (input & IN_ACTION)
		{
			if (!jeep->gear)
			{
				if (jeep->velocity >= 0x8000)
					jeep->velocity = 0x8000;
				else if (jeep->velocity < 0x4000)
					jeep->velocity += ((0x4800 - jeep->velocity) >> 3) + 8;
				else if (jeep->velocity >= 0x7000)
					jeep->velocity += ((0x8000 - jeep->velocity) >> 3) + 2;
				else
					jeep->velocity += ((0x7800 - jeep->velocity) >> 4) + 4;
			}
			else if (jeep->gear == 1)
			{
				if (jeep->velocity <= -0x4000)
					jeep->velocity = -0x4000;
				else
					jeep->velocity -= (abs(-0x4000 - jeep->velocity) >> 3) - 2;
			}

			jeep->velocity -= abs(item->pos.y_rot - jeep->move_angle) >> 6;
		}

		if (!(input & IN_ACTION))
		{
			if (jeep->velocity > 256)
				jeep->velocity -= 256;
			else if (jeep->velocity < -256)
				jeep->velocity += 256;
			else
				jeep->velocity = 0;
		}

		item->speed = short(jeep->velocity >> 8);

		if (jeep->pitch1 > 0xC000)
			jeep->pitch1 = (GetRandomControl() & 0x1FF) + 0xBF00;

		vel = jeep->velocity;

		if (vel < 0)
			vel >>= 1;

		jeep->pitch1 += (abs(vel) - jeep->pitch1) >> 3;
	}
	else if (jeep->pitch1 < 0xFFFF)
		jeep->pitch1 += (0xFFFF - jeep->pitch1) >> 3;

	if (input & IN_JUMP)
	{
		pos.x = 0;
		pos.y = -144;
		pos.z = -1024;
		GetJointAbsPosition(item, &pos, 11);
		TriggerDynamic(pos.x, pos.y, pos.z, 10, 64, 0, 0);
		item->mesh_bits = 0x27FFF;
	}
	else
		item->mesh_bits = 0x1BFFF;

	*pitch = jeep->pitch1;
	return 0;
}

void JeepBaddieCollision(ITEM_INFO* item)
{
	JEEPINFO* jeep;
	ITEM_INFO* collided;
	OBJECT_INFO* obj;
	short* doors;
	long j, dx, dy, dz;
	short room_count, item_number;

	jeep = (JEEPINFO*)item->data;
	room_count = 1;
	jroomies[0] = item->room_number;
	doors = room[item->room_number].door;

	for (int i = *doors++; i > 0; i--, doors += 16)
	{
		for (j = 0; j < room_count; j++)
		{
			if (jroomies[j] == *doors)
				break;
		}

		if (j == room_count)
		{
			jroomies[room_count] = *doors;
			room_count++;
		}
	}

	for (int i = 0; i < room_count; i++)
	{
		for (item_number = room[jroomies[i]].item_number; item_number != NO_ITEM; item_number = collided->next_item)
		{
			collided = &items[item_number];
			obj = &objects[collided->object_number];

			if (collided->collidable && collided->status != ITEM_INVISIBLE && collided != lara_item && collided != item)
			{
				if (collided->object_number == ENEMY_JEEP)
				{
					mycoll.coll_type = 0;
					mycoll.radius = 400;
					mycoll.enable_baddie_push = 1;
					mycoll.enable_spaz = 0;
					ObjectCollision(item_number, item, &mycoll);
				}
				else if (obj->collision && obj->intelligent || collided->object_number == ROLLINGBALL || collided->object_number == TEETH_SPIKES)
				{
					dx = item->pos.x_pos - collided->pos.x_pos;
					dy = item->pos.y_pos - collided->pos.y_pos;
					dz = item->pos.z_pos - collided->pos.z_pos;

					if (dx > -2048 && dx < 2048 && dz > -2048 && dz < 2048 && dy > -2048 && dy < 2048)
					{
						if (collided->object_number == ROLLINGBALL)
						{
							if (TestBoundsCollide(collided, lara_item, 100) && lara_item->hit_points > 0)
							{
								DoLotsOfBlood(lara_item->pos.x_pos, lara_item->pos.y_pos - 512, lara_item->pos.z_pos,
									(GetRandomControl() & 3) + 8, lara_item->pos.y_rot, lara_item->room_number, 5);
								lara_item->hit_points -= 8;
								lara_item->hit_status = 1;
							}
						}
						else if (collided->object_number == TEETH_SPIKES)
						{
							if (TestBoundsCollideTeethSpikes(collided) && collided->object_number == TEETH_SPIKES)
								jeep->flags |= 0x40;
						}
						else if (TestBoundsCollide(collided, item, 550))
						{
							DoLotsOfBlood(collided->pos.x_pos, item->pos.y_pos - 256, collided->pos.z_pos,
								(GetRandomControl() & 3) + 8, item->pos.y_rot, collided->room_number, 3);
							collided->hit_points = 0;
						}
					}
				}
			}
		}
	}
}

void JeepCollideStaticObjects(long x, long y, long z, short room_number, long height)
{
	MESH_INFO* mesh;
	STATIC_INFO* sinfo;
	ROOM_INFO* r;
	PHD_VECTOR pos;
	short* doors;
	long j;
	static long JeepBounds[6] = { 0, 0, 0, 0, 0, 0 };
	static long CollidedStaticBounds[6] = { 0, 0, 0, 0, 0, 0 };
	short room_count, rn;

	pos.x = x;
	pos.y = y;
	pos.z = z;
	JeepBounds[0] = x + 256;
	JeepBounds[1] = x - 256;
	JeepBounds[2] = y;
	JeepBounds[3] = y - height;
	JeepBounds[4] = z + 256;
	JeepBounds[5] = z - 256;
	room_count = 1;
	jroomies[0] = room_number;
	doors = room[room_number].door;

	for (int i = *doors++; i > 0; i--, doors += 16)
	{
		for (j = 0; j < room_count; j++)
		{
			if (jroomies[j] == *doors)
				break;
		}

		if (j == room_count)
		{
			jroomies[room_count] = *doors;
			room_count++;
		}
	}

	for (int i = 0; i < room_count; i++)
	{
		rn = jroomies[i];
		r = &room[rn];
		mesh = r->mesh;

		for (j = r->num_meshes; j > 0; j--, mesh++)
		{
			sinfo = &static_objects[mesh->static_number];

			if (mesh->Flags & 1)
			{
				if (mesh->static_number >= SHATTER0 && mesh->static_number <= SHATTER9)
				{
					CollidedStaticBounds[2] = mesh->y + sinfo->y_maxc;
					CollidedStaticBounds[3] = mesh->y + sinfo->y_minc;

					if (mesh->y_rot == -0x8000)
					{
						CollidedStaticBounds[0] = mesh->x - sinfo->x_minc;
						CollidedStaticBounds[1] = mesh->x - sinfo->x_maxc;
						CollidedStaticBounds[4] = mesh->z - sinfo->z_minc;
						CollidedStaticBounds[5] = mesh->z - sinfo->z_maxc;
					}
					else if (mesh->y_rot == -0x4000)
					{
						CollidedStaticBounds[0] = mesh->x - sinfo->z_minc;
						CollidedStaticBounds[1] = mesh->x - sinfo->z_maxc;
						CollidedStaticBounds[4] = mesh->z + sinfo->x_maxc;
						CollidedStaticBounds[5] = mesh->z + sinfo->x_minc;
					}
					else if (mesh->y_rot == 0x4000)
					{
						CollidedStaticBounds[0] = mesh->x + sinfo->z_maxc;
						CollidedStaticBounds[1] = mesh->x + sinfo->z_minc;
						CollidedStaticBounds[4] = mesh->z - sinfo->x_minc;
						CollidedStaticBounds[5] = mesh->z - sinfo->x_maxc;
					}
					else
					{
						CollidedStaticBounds[0] = mesh->x + sinfo->x_maxc;
						CollidedStaticBounds[1] = mesh->x + sinfo->x_minc;
						CollidedStaticBounds[4] = mesh->z + sinfo->z_maxc;
						CollidedStaticBounds[5] = mesh->z + sinfo->z_minc;
					}

					if (JeepBounds[0] > CollidedStaticBounds[1] &&
						JeepBounds[1] < CollidedStaticBounds[0] &&
						JeepBounds[2] > CollidedStaticBounds[3] &&
						JeepBounds[3] < CollidedStaticBounds[2] &&
						JeepBounds[4] > CollidedStaticBounds[5] &&
						JeepBounds[5] < CollidedStaticBounds[4])
					{
						ShatterObject(0, mesh, -128, rn, 0);
						SoundEffect(SFX_HIT_ROCK, (PHD_3DPOS*)&pos, SFX_DEFAULT);
						SmashedMeshRoom[SmashedMeshCount] = rn;
						SmashedMesh[SmashedMeshCount] = mesh;
						SmashedMeshCount++;
						mesh->Flags &= ~1;
					}
				}
			}
		}
	}
}

long JeepDynamics(ITEM_INFO* item)
{
	JEEPINFO* jeep;
	FLOOR_INFO* floor;
	PHD_VECTOR pos, newPos;
	PHD_VECTOR flPos, frPos, blPos, brPos, fmPos, flPos2, frPos2, blPos2, brPos2, fmPos2;
	long front_left, front_right, back_left, back_right, front_mid, front_left2, front_right2, back_left2, back_right2, front_mid2;
	long h, speed, shift, shift2, anim, dx, dz;
	short ang, ang2, vel, room_number;

	dont_exit_jeep = 0;
	jeep = (JEEPINFO*)item->data;
	front_left = TestHeight(item, 550, -256, &flPos);
	front_right = TestHeight(item, 550, 256, &frPos);
	back_left = TestHeight(item, -600, -256, &blPos);
	back_right = TestHeight(item, -600, 256, &brPos);
	front_mid = TestHeight(item, -600, 0, &fmPos);
	pos.x = item->pos.x_pos;
	pos.y = item->pos.y_pos;
	pos.z = item->pos.z_pos;

	if (blPos.y > back_left)
		blPos.y = back_left;

	if (brPos.y > back_right)
		brPos.y = back_right;

	if (flPos.y > front_left)
		flPos.y = front_left;

	if (frPos.y > front_right)
		frPos.y = front_right;

	if (fmPos.y > front_mid)
		fmPos.y = front_mid;

	if (item->pos.y_pos <= item->floor - 8)
	{
		if (jeep->turn_rate < -91)
			jeep->turn_rate += 91;
		else if (jeep->turn_rate > 91)
			jeep->turn_rate -= 91;
		else
			jeep->turn_rate = 0;

		item->pos.y_rot += short(jeep->turn_rate + jeep->extra_rotation);
		jeep->move_angle += short(item->pos.y_rot - jeep->move_angle) >> 5;
	}
	else
	{
		if (jeep->turn_rate < -182)
			jeep->turn_rate += 182;
		else if (jeep->turn_rate > 182)
			jeep->turn_rate -= 182;
		else
			jeep->turn_rate = 0;

		item->pos.y_rot += short(jeep->turn_rate + jeep->extra_rotation);
		ang = item->pos.y_rot - jeep->move_angle;
		vel = short(728 - ((3 * jeep->velocity) >> 11));

		if (!(input & IN_ACTION) && jeep->velocity > 0)
			vel -= vel >> 2;

		if (ang < -273)
		{
			if (ang < -13650)
			{
				item->pos.y_pos -= 41;
				item->fallspeed = -6 - (GetRandomControl() & 3);
				jeep->turn_rate = 0;
				jeep->velocity -= jeep->velocity >> 3;
			}

			if (ang < -16380)
				jeep->move_angle = item->pos.y_rot + 0x3FFC;
			else
				jeep->move_angle -= vel;
		}
		else if (ang > 273)
		{
			if (ang > 13650)
			{
				item->pos.y_pos -= 41;
				item->fallspeed = -6 - (GetRandomControl() & 3);
				jeep->turn_rate = 0;
				jeep->velocity -= jeep->velocity >> 3;
			}

			if (ang > 16380)
				jeep->move_angle = item->pos.y_rot - 0x3FFC;
			else
				jeep->move_angle += vel;
		}
		else
			jeep->move_angle = item->pos.y_rot;
	}

	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	if (item->pos.y_pos < h)
		speed = item->speed;
	else
		speed = (item->speed * phd_cos(item->pos.x_rot)) >> W2V_SHIFT;

	item->pos.x_pos += (speed * phd_sin(jeep->move_angle)) >> W2V_SHIFT;
	item->pos.z_pos += (speed * phd_cos(jeep->move_angle)) >> W2V_SHIFT;

	if (item->pos.y_pos >= h)
	{
		ang = (100 * phd_sin(item->pos.x_rot)) >> W2V_SHIFT;

		if (abs(ang) > 16)
		{
			dont_exit_jeep = 1;

			if (ang < 0)
				jeep->velocity += SQUARE(ang + 16) >> 1;
			else
				jeep->velocity -= SQUARE(ang - 16) >> 1;
		}

		ang = (128 * phd_sin(item->pos.z_rot)) >> W2V_SHIFT;

		if (abs(ang) > 32)
		{
			dont_exit_jeep = 1;

			if (ang < 0)
				ang2 = item->pos.y_rot - 0x4000;
			else
				ang2 = item->pos.y_rot + 0x4000;

			item->pos.x_pos += ((abs(ang) - 24) * phd_sin(ang2)) >> W2V_SHIFT;
			item->pos.z_pos += ((abs(ang) - 24) * phd_cos(ang2)) >> 14;
		}
	}

	if (jeep->velocity > 0x8000)
		jeep->velocity = 0x8000;
	else if (jeep->velocity < -0x4000)
		jeep->velocity = -0x4000;

	newPos.x = item->pos.x_pos;
	newPos.z = item->pos.z_pos;

	if (!(item->flags & IFL_INVISIBLE))
	{
		JeepBaddieCollision(item);
		JeepCollideStaticObjects(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 512);
	}

	shift = 0;
	shift2 = 0;
	front_left2 = TestHeight(item, 550, -256, &flPos2);

	if (front_left2 < flPos.y - 256)
		shift = abs(DoShift(item, &flPos2, &flPos) << 2);

	back_left2 = TestHeight(item, -600, -256, &blPos2);

	if (back_left2 < blPos.y - 256)
	{
		if (shift)
			shift += abs(DoShift(item, &blPos2, &blPos) << 2);
		else
			shift = -abs(DoShift(item, &blPos2, &blPos) << 2);
	}

	front_right2 = TestHeight(item, 550, 256, &frPos2);

	if (front_right2 < frPos.y - 256)
		shift2 = -abs(DoShift(item, &frPos2, &frPos) << 2);

	front_mid2 = TestHeight(item, -600, 0, &fmPos2);

	if (front_mid2 < fmPos.y - 256)
		DoShift(item, &fmPos2, &fmPos);

	back_right2 = TestHeight(item, -600, 256, &brPos2);

	if (back_right2 < brPos.y - 256)
	{
		if (shift2)
			shift2 -= abs(DoShift(item, &brPos2, &brPos) << 2);
		else
			shift2 = abs(DoShift(item, &brPos2, &brPos) << 2);
	}

	if (!shift)
		shift = shift2;

	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	if (h < item->pos.y_pos - 256)
		DoShift(item, (PHD_VECTOR*)&item->pos, &pos);

	if (!jeep->velocity)
		shift = 0;

	jeep->rot_thing = short((jeep->rot_thing + shift) >> 1);

	if (abs(jeep->rot_thing) < 2)
		jeep->rot_thing = 0;

	if (abs(jeep->rot_thing - jeep->extra_rotation) < 4)
		jeep->extra_rotation = jeep->rot_thing;
	else
		jeep->extra_rotation += (jeep->rot_thing - jeep->extra_rotation) >> 2;

	anim = GetCollisionAnim(item, &newPos, 0);

	if (anim)
	{
		dx = item->pos.x_pos - pos.x;
		dz = item->pos.z_pos - pos.z;
		speed = (dx * phd_sin(jeep->move_angle) + dz * phd_cos(jeep->move_angle)) >> W2V_SHIFT;
		speed <<= 8;

		if (&items[lara.vehicle] == item && jeep->velocity == 0x8000 && speed < 0x7FF6)
		{
			lara_item->hit_points -= short((0x8000 - speed) >> 7);
			lara_item->hit_status = 1;
		}

		if (jeep->velocity > 0 && speed < jeep->velocity)
			jeep->velocity = speed < 0 ? 0 : speed;
		else if (jeep->velocity < 0 && speed > jeep->velocity)
			jeep->velocity = speed > 0 ? 0 : speed;

		if (jeep->velocity < -0x4000)
			jeep->velocity = -0x4000;
	}

	return anim;
}

void JeepControl(short item_number)
{
	ITEM_INFO* item;
	JEEPINFO* jeep;
	FLOOR_INFO* floor;
	PHD_VECTOR flPos, frPos, fmPos;
	PHD_VECTOR pos;
	long front_left, front_right, front_mid;
	long hitWall, h, driving, killed, pitch, oldY, hdiff, smokeVel;
	short room_number, wheelRot, xRot, zRot;
	static uchar ExhaustSmokeVel;

	driving = -1;
	killed = 0;
	pitch = 0;
	item = &items[item_number];
	jeep = (JEEPINFO*)item->data;
	hitWall = JeepDynamics(item);
	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	front_left = TestHeight(item, 550, -256, &flPos);
	front_right = TestHeight(item, 550, 256, &frPos);
	front_mid = TestHeight(item, -600, 0, &fmPos);
	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	TestTriggers(trigger_index, 1, 0);
	TestTriggers(trigger_index, 0, 0);

	if (lara_item->hit_points <= 0)
	{
		killed = 1;
		input &= ~(IN_FORWARD | IN_BACK | IN_LEFT | IN_RIGHT | IN_LSTEP | IN_RSTEP);
	}

	if (jeep->flags)
		hitWall = 0;
	else
	{
		if (lara_item->current_anim_state != 9)
			driving = UserControl(item, h, &pitch);
		else
		{
			driving = -1;
			hitWall = 0;
		}
	}

	if (jeep->velocity || jeep->unused1)
	{
		jeep->pitch2 = pitch;

		if (jeep->pitch2 < -0x8000)
			jeep->pitch2 = -0x8000;
		else if (jeep->pitch2 > 0xA000)
			jeep->pitch2 = 0xA000;

		SoundEffect(SFX_JEEP_MOVE, &item->pos, (jeep->pitch2 << 8) + (SFX_SETPITCH | 0x1000000));
	}
	else
	{
		if (driving != -1)
			SoundEffect(SFX_JEEP_IDLE, &item->pos, SFX_DEFAULT);

		jeep->pitch2 = 0;
	}

	item->floor = h;
	wheelRot = short(jeep->velocity >> 2);
	jeep->right_front_wheelrot -= wheelRot;
	jeep->left_front_wheelrot -= wheelRot;
	jeep->left_back_wheelrot -= wheelRot;
	jeep->right_back_wheelrot -= wheelRot;
	oldY = item->pos.y_pos;
	item->fallspeed = (short)DoDynamics(h, item->fallspeed, &item->pos.y_pos, 0);
	hdiff = (frPos.y + flPos.y) >> 1;

	if (fmPos.y < front_mid)
	{
		if (hdiff < (front_left + front_right) >> 1)
		{
			xRot = (short)phd_atan(137, oldY - item->pos.y_pos);

			if (jeep->velocity < 0)
				xRot = -xRot;
		}
		else
			xRot = (short)phd_atan(550, item->pos.y_pos - hdiff);
	}
	else
	{
		if (hdiff < (front_left + front_right) >> 1)
			xRot = (short)phd_atan(550, front_mid - item->pos.y_pos);
		else
			xRot = (short)phd_atan(1100, front_mid - hdiff);
	}

	zRot = (short)phd_atan(350, hdiff - flPos.y);
	item->pos.x_rot += (xRot - item->pos.x_rot) >> 2;
	item->pos.z_rot += (zRot - item->pos.z_rot) >> 2;

	if (!(jeep->flags & 0x80))
	{
		if (room_number != item->room_number)
		{
			ItemNewRoom(lara.vehicle, room_number);
			ItemNewRoom(lara.item_number, room_number);
		}

		lara_item->pos.x_pos = item->pos.x_pos;
		lara_item->pos.y_pos = item->pos.y_pos;
		lara_item->pos.z_pos = item->pos.z_pos;
		lara_item->pos.x_rot = item->pos.x_rot;
		lara_item->pos.y_rot = item->pos.y_rot;
		lara_item->pos.z_rot = item->pos.z_rot;
		AnimateJeep(item, hitWall, killed);
		AnimateItem(lara_item);
		item->anim_number = objects[JEEP].anim_index + lara_item->anim_number - objects[VEHICLE_EXTRA].anim_index;
		item->frame_number = lara_item->frame_number + anims[item->anim_number].frame_base - anims[lara_item->anim_number].frame_base;
		camera.target_elevation = -5460;
		camera.target_distance = 2048;

		if (!jeep->gear)
			jeep->camera_angle -= jeep->camera_angle >> 3;
		else if (jeep->gear == 1)
			jeep->camera_angle += (0x7F42 - jeep->camera_angle) >> 3;

		camera.target_angle = (short)jeep->camera_angle;

		if (jeep->flags & 0x40 && item->pos.y_pos == item->floor)
		{
			lara_item->mesh_bits = 0;
			lara_item->hit_points = 0;
			lara_item->flags |= IFL_INVISIBLE;
			JeepExplode(item);
			return;
		}
	}

	if (lara_item->current_anim_state == 9 || lara_item->current_anim_state == 10)
		ExhaustSmokeVel = 0;
	else
	{
		pos.x = 80;
		pos.y = 0;
		pos.z = -500;
		GetJointAbsPosition(item, &pos, 11);

		if (item->speed > 32)
		{
			if (item->speed < 64)
				TriggerExhaustSmoke(pos.x, pos.y, pos.z, item->pos.y_rot + 0x8000, 64 - item->speed, 1);
		}
		else
		{
			if (ExhaustSmokeVel < 16)
			{
				smokeVel = ((GetRandomControl() & 7) + (GetRandomControl() & 0x10) + 2 * ExhaustSmokeVel) << 6;
				ExhaustSmokeVel++;
			}
			else if (GetRandomControl() & 3)
				smokeVel = 0;
			else
				smokeVel = ((GetRandomControl() & 0xF) + (GetRandomControl() & 0x10)) << 6;

			TriggerExhaustSmoke(pos.x, pos.y, pos.z, item->pos.y_rot + 0x8000, smokeVel, 0);
		}
	}

	JeepCheckGetOut();
}

void JeepStart(ITEM_INFO* item, ITEM_INFO* l)
{
	JEEPINFO* jeep;

	jeep = (JEEPINFO*)item->data;
	lara.gun_status = LG_HANDS_BUSY;
	lara.hit_direction = -1;
	l->current_anim_state = 0;
	l->goal_anim_state = 0;
	l->anim_number = objects[VEHICLE_EXTRA].anim_index + 14;
	l->frame_number = anims[l->anim_number].frame_base;
	item->anim_number = l->anim_number + objects[JEEP].anim_index - objects[VEHICLE_EXTRA].anim_index;
	item->frame_number = l->frame_number + anims[item->anim_number].frame_base - anims[l->anim_number].frame_base;
	item->flags |= IFL_TRIGGERED;
	item->hit_points = 1;
	jeep->unused1 = 0;
	jeep->gear = 0;
}

void JeepFireGrenade(ITEM_INFO* item)
{
	ITEM_INFO* grenade;
	short item_number;

	item_number = CreateItem();

	if (item_number != NO_ITEM)
	{
		grenade = &items[item_number];
		grenade->shade = -0x3DF0;
		grenade->object_number = GRENADE;
		grenade->room_number = item->room_number;
		InitialiseItem(item_number);
		grenade->pos.x_rot = item->pos.x_rot;
		grenade->pos.z_rot = 0;
		grenade->pos.y_rot= item->pos.y_rot + 0x8000;
		grenade->pos.x_pos = item->pos.x_pos + (1024 * phd_sin(grenade->pos.y_rot) >> W2V_SHIFT);
		grenade->pos.y_pos = item->pos.y_pos - 768;
		grenade->pos.z_pos = item->pos.z_pos + (1024 * phd_cos(grenade->pos.y_rot) >> W2V_SHIFT);
		SmokeCountL = 32;
		SmokeWeapon = 5;

		for (int i = 0; i < 5; i++)
			TriggerGunSmoke(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 0, 0, 0, 1, 5, 32);

		if (GetRandomControl() & 3)
			grenade->item_flags[0] = 1;
		else
			grenade->item_flags[0] = 2;

		grenade->speed = 32;
		grenade->fallspeed = -32 * phd_sin(grenade->pos.x_rot) >> W2V_SHIFT;
		grenade->current_anim_state = grenade->pos.x_rot;
		grenade->goal_anim_state = grenade->pos.y_rot;
		grenade->required_anim_state = 0;
		grenade->hit_points = 120;
		AddActiveItem(item_number);
	}
}

void InitialiseEnemyJeep(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[item->object_number].anim_index + 14;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = 0;
	item->goal_anim_state = 0;
	item->mesh_bits = 0xFFFDBFFF;
	item->status -= ITEM_INVISIBLE;
}

void EnemyJeepControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* jeep;
	FLOOR_INFO* floor;
	AIOBJECT* aiobj;
	AI_INFO info;
	PHD_VECTOR pos;
	long Xoffset, Zoffset, x, y, z, h1, h2, _h1, _h2, iAngle, iDist;
	short room_number, xrot, zrot;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	jeep = (CREATURE_INFO*)item->data;
	Xoffset = 682 * phd_sin(item->pos.y_rot) >> W2V_SHIFT;
	Zoffset = 682 * phd_cos(item->pos.y_rot) >> W2V_SHIFT;
	x = item->pos.x_pos - Zoffset;
	y = item->pos.y_pos;
	z = item->pos.z_pos + Xoffset;
	room_number = item->room_number;
	floor = GetFloor(x, y, z, &room_number);
	h1 = GetHeight(floor, x, y, z);
	
	if (abs(y - h1) > 768)
	{
		item->pos.x_pos += Zoffset >> 6;
		item->pos.z_pos -= Xoffset >> 6;
		item->pos.y_rot += 364;
		h1 = y;
	}

	x = item->pos.x_pos + Zoffset;
	z = item->pos.z_pos - Xoffset;
	room_number = item->room_number;
	floor = GetFloor(x, y, z, &room_number);
	h2 = GetHeight(floor, x, y, z);

	if (abs(y - h2) > 768)
	{
		item->pos.x_pos -= Zoffset >> 6;
		item->pos.z_pos += Xoffset >> 6;
		item->pos.y_rot -= 364;
		h2 = y;
	}

	zrot = (short)phd_atan(1364, h2 - h1);

	x = item->pos.x_pos + Xoffset;
	z = item->pos.z_pos + Zoffset;
	room_number = item->room_number;
	floor = GetFloor(x, y, z, &room_number);
	h1 = GetHeight(floor, x, y, z);
	_h1 = h1;

	if (abs(y - h1) > 768)
		h1 = y;

	x = item->pos.x_pos - Xoffset;
	z = item->pos.z_pos - Zoffset;
	room_number = item->room_number;
	floor = GetFloor(x, y, z, &room_number);
	h2 = GetHeight(floor, x, y, z);
	_h2 = h2;

	if (abs(y - h2) > 768)
		h2 = y;

	xrot = (short)phd_atan(1364, h2 - h1);
	CreatureAIInfo(item, &info);
	jeep->enemy = &jeep->ai_target;

	if (jeep->enemy == lara_item)
	{
		iAngle = info.angle;
		iDist = info.distance;
	}
	else
	{
		x = lara_item->pos.x_pos - item->pos.x_pos;
		z = lara_item->pos.z_pos - item->pos.z_pos;

		iAngle = phd_atan(z, x) - item->pos.y_rot;

		if (x > 32000 || x < -32000 || z > 32000 || z < -32000)
			iDist = 0x7FFFFFFF;
		else
			iDist = SQUARE(x) + SQUARE(z);
	}

	switch (item->current_anim_state)
	{
	case 0:
	case 2:
		item->item_flags[0] -= 128;
		item->mesh_bits = 0xFFFE7FFF;

		if (item->item_flags[0] < 0)
			item->item_flags[0] = 0;

		pos.x = 0;
		pos.y = -144;
		pos.z = -1024;
		GetJointAbsPosition(item, &pos, 11);
		TriggerDynamic(pos.x, pos.y, pos.z, 10, 64, 0, 0);

		if (item->required_anim_state)
			item->goal_anim_state = item->required_anim_state;
		else if (info.distance > 0x100000 || lara.location >= item->item_flags[3])
			item->goal_anim_state = 1;

		break;
		
	case 1:
		jeep->maximum_turn = item->item_flags[0] >> 4;
		item->item_flags[0] += 37;		//34 in debug exe
		item->mesh_bits = 0xFFFDBFFF;

		if (item->item_flags[0] > 0x2200)
			item->item_flags[0] = 0x2200;

		if (info.angle > 256)
			item->goal_anim_state = 4;
		else if (info.angle < -256)
			item->goal_anim_state = 3;

		break;

	case 3:
	case 4:
		item->item_flags[0] += 18;		//17 in debug exe

		if (item->item_flags[0] > 0x2200)
			item->item_flags[0] = 0x2200;

		item->goal_anim_state = 1;
		break;

	case 5:

		if (item->item_flags[0] < 0x4A0)
			item->item_flags[0] = 0x4A0;

		break;
	}

	if (_h1 > item->floor + 512)
	{
		jeep->LOT.is_jumping = 1;

		if (item->item_flags[1] > 0)
		{
			xrot = item->item_flags[1];
			item->item_flags[1] -= 8;

			if (item->item_flags[1] < 0)
				jeep->LOT.is_jumping = 0;

			item->pos.y_pos += item->item_flags[1] >> 6;
		}
		else
		{
			item->item_flags[1] = xrot << 1;
			jeep->LOT.is_jumping = 1;
		}

		if (jeep->LOT.is_jumping)
		{
			jeep->maximum_turn = 0;
			item->goal_anim_state = 1;
		}
	}
	else if (_h2 > item->floor + 512 && item->current_anim_state != 5)
	{
		item->item_flags[1] = 0;
		item->anim_number = objects[item->object_number].anim_index + 8;
		item->frame_number = anims[item->anim_number].frame_base;
		item->current_anim_state = 5;
		item->goal_anim_state = 1;
	}

	if (info.distance < 0x240000 || item->item_flags[3] == -2)
		jeep->reached_goal = 1;

	if (jeep->reached_goal)
	{
		TestTriggersAtXYZ(jeep->enemy->pos.x_pos, jeep->enemy->pos.y_pos, jeep->enemy->pos.z_pos, jeep->enemy->room_number, 1, 0);

		if (lara.location < item->item_flags[3] && item->current_anim_state != 2 && item->goal_anim_state != 2)
		{
			item->anim_number = objects[item->object_number].anim_index + 1;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 2;
			item->goal_anim_state = 2;

			if (jeep->enemy->flags & 4)
			{
				item->pos.x_pos = jeep->enemy->pos.x_pos;
				item->pos.y_pos = jeep->enemy->pos.y_pos;
				item->pos.z_pos = jeep->enemy->pos.z_pos;
				item->pos.x_rot = jeep->enemy->pos.x_rot;
				item->pos.y_rot = jeep->enemy->pos.y_rot;
				item->pos.z_rot = jeep->enemy->pos.z_rot;

				if (item->room_number != jeep->enemy->room_number)
					ItemNewRoom(item_number, jeep->enemy->room_number);
			}
		}

		if (iDist > 0x400000 && iDist < 0x6400000 && !item->item_flags[2] && (iAngle < -20480 || iAngle > 20480))
		{
			JeepFireGrenade(item);
			item->item_flags[2] = 150;
		}

		if (jeep->enemy->flags == 62)
		{
			item->status = ITEM_INVISIBLE;
			RemoveActiveItem(item_number);
			DisableBaddieAI(item_number);
		}

		if (lara.location >= item->item_flags[3] || !(jeep->enemy->flags & 4))
		{
			jeep->reached_goal = 0;
			item->item_flags[3]++;
			jeep->enemy = 0;

			for (int i = 0; i < nAIObjects; i++)
			{
				aiobj = &AIObjects[i];

				if (aiobj->trigger_flags == item->item_flags[3] && aiobj->room_number != 255)
				{
					jeep->enemy = &jeep->ai_target;
					jeep->enemy->object_number = aiobj->object_number;
					jeep->enemy->room_number = aiobj->room_number;
					jeep->enemy->pos.x_pos = aiobj->x;
					jeep->enemy->pos.y_pos = aiobj->y;
					jeep->enemy->pos.z_pos = aiobj->z;
					jeep->enemy->pos.y_rot = aiobj->y_rot;
					jeep->enemy->flags = aiobj->flags;
					jeep->enemy->trigger_flags = aiobj->trigger_flags;
					jeep->enemy->box_number = aiobj->box_number;

					if (!(jeep->enemy->flags & 0x20))
					{
						jeep->enemy->pos.x_pos += 256 * phd_sin(jeep->enemy->pos.y_rot) >> W2V_SHIFT;
						jeep->enemy->pos.z_pos += 256 * phd_cos(jeep->enemy->pos.y_rot) >> W2V_SHIFT;
					}

					break;
				}
			}
		}
	}

	item->item_flags[2]--;

	if (item->item_flags[2] < 0)
		item->item_flags[2] = 0;

	if (abs(xrot - item->pos.x_rot) < 256)
		item->pos.x_rot = xrot;
	else if (xrot > item->pos.x_rot)
		item->pos.x_rot += 256;
	else if (xrot < item->pos.x_rot)
		item->pos.x_rot -= 256;

	if (abs(zrot - item->pos.z_rot) < 256)
		item->pos.z_rot = zrot;
	else if (zrot > item->pos.z_rot)
		item->pos.z_rot += 256;
	else if (zrot < item->pos.z_rot)
		item->pos.z_rot -= 256;

	item->item_flags[0] -= xrot >> 9;
	item->item_flags[0] -= 2;

	if (item->item_flags[0] < 0)
		item->item_flags[0] = 0;

	x = item->item_flags[0] * phd_sin(item->pos.y_rot) >> W2V_SHIFT;
	z = item->item_flags[0] * phd_cos(item->pos.y_rot) >> W2V_SHIFT;

	for (int i = 0; i < 4; i++)
		jeep->joint_rotation[i] -= item->item_flags[0];

	item->pos.x_pos += x >> 6;
	item->pos.z_pos += z >> 6;

	if (!jeep->reached_goal)
		CreatureYRot(&item->pos, info.angle, item->item_flags[0] >> 4);

	jeep->maximum_turn = 0;
	AnimateItem(item);
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	item->floor = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	if (item->room_number != room_number)
		ItemNewRoom(item_number, room_number);

	if (item->pos.y_pos < item->floor)
		item->gravity_status = 1;
	else
	{
		item->fallspeed = 0;
		item->pos.y_pos = item->floor;
		item->gravity_status = 0;
	}

	SoundEffect(SFX_JEEP_MOVE, &item->pos, (item->item_flags[0] << 10) + (SFX_SETPITCH | 0x1000000));
}
