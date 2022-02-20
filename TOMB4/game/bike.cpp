#include "../tomb4/pch.h"
#include "bike.h"
#include "../specific/function_stubs.h"
#include "../specific/specificfx.h"
#include "sphere.h"
#include "camera.h"
#include "objects.h"
#include "control.h"
#include "../specific/3dmath.h"
#include "draw.h"
#include "../specific/output.h"
#include "effect2.h"
#include "lara1gun.h"
#include "tomb4fx.h"
#include "items.h"
#include "sound.h"
#include "lara_states.h"
#include "delstuff.h"
#include "laraflar.h"
#include "collide.h"

void InitialiseBike(short item_number)
{
	ITEM_INFO* item;
	BIKEINFO* bike;

	item = &items[item_number];
	bike = (BIKEINFO*)game_malloc(sizeof(BIKEINFO));
	item->data = bike;
	bike->velocity = 0;
	bike->bike_turn = 0;
	bike->pitch2 = 0;
	bike->move_angle = item->pos.y_rot;
	bike->rot_thing = 0;
	bike->extra_rotation = 0;
	bike->flags &= 0xFE00;
	bike->light_intensity = 0;
	bike->right_front_wheelrot = 0;
	bike->right_back_wheelrot = 0;
	bike->left_wheelrot = 0;
	item->mesh_bits = 1015;
	GlobalBikeItem = item;
}

void DrawBikeExtras(ITEM_INFO* item)
{
	if (lara.vehicle != NO_ITEM)
		DrawBikeSpeedo(phd_winwidth - 64, phd_winheight - 16, ((BIKEINFO*)item->data)->velocity, 0x8000, 0xC000, 32, 0);

	DrawBikeBeam(GlobalBikeItem);
}

void TriggerBikeBeam(ITEM_INFO* item)
{
	BIKEINFO* bike;
	PHD_VECTOR s;
	PHD_VECTOR d;
	long intensity;

	bike = (BIKEINFO*)item->data;
	s.x = 0;
	s.y = -470;
	s.z = 1836;
	GetJointAbsPosition(item, &s, 0);
	d.x = 0;
	d.y = -470;
	d.z = 20780;
	GetJointAbsPosition(item, &d, 0);
	intensity = bike->light_intensity << 1;

	if (intensity>0)
		LaraTorch(&s, &d, item->pos.y_rot, intensity);
	else
		bLaraTorch = 0;
}

long GetOnBike(short item_number, COLL_INFO* coll)
{
	ITEM_INFO* item;
	long dx, dz;
	ushort rot;
	short room_number;

	item = &items[item_number];

	if (item->flags & IFL_INVISIBLE || lara.gun_status != LG_NO_ARMS || lara_item->gravity_status)
		return 0;

	if (ABS(item->pos.y_pos - lara_item->pos.y_pos) > 256 || !(input & IN_ACTION) && GLOBAL_inventoryitemchosen != PUZZLE_ITEM1)
		return 0;

	dx = lara_item->pos.x_pos - item->pos.x_pos;
	dz = lara_item->pos.z_pos - item->pos.z_pos;

	if (SQUARE(dx) + SQUARE(dz) > 170000)
		return 0;

	room_number = item->room_number;

	if (GetHeight(GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number),
		item->pos.x_pos, item->pos.y_pos, item->pos.z_pos) < -32000)
		return 0;

	rot = (ushort)phd_atan(item->pos.z_pos - lara_item->pos.z_pos, item->pos.x_pos - lara_item->pos.x_pos) - item->pos.y_rot;

	if (rot > -8190 && rot < 24570)
		return 0;

	rot = lara_item->pos.y_rot - item->pos.y_rot;

	if (rot <= 40950 || rot >= 57330)
		return 0;

	return 1;
}

void DrawBikeBeam(ITEM_INFO* item)
{
	BIKEINFO* bike;
	OBJECT_INFO* obj;
	short** meshpp;
	long* bone;
	short* frames[2];
	short* rot;
	long rate, bounds, r, g, b;
#ifdef GENERAL_FIXES
	short* rot2;
	long frac;
#endif

	bike = (BIKEINFO*)item->data;

	if (!bike->light_intensity)
		return;

#ifdef GENERAL_FIXES
	frac = 
#endif
		GetFrames(item, frames, &rate);
	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	bounds = S_GetObjectBounds(frames[0]);

	if (bounds)
	{
		CalculateObjectLighting(item, frames[0]);
		bounds = 1;
		obj = &objects[item->object_number];
		meshpp = &meshes[obj->mesh_index];
		meshpp += 2;
		bone = &bones[obj->bone_index];

#ifdef GENERAL_FIXES
		if (frac)
		{
			InitInterpolate(frac, rate);
			phd_TranslateRel_ID(frames[0][6], frames[0][7], frames[0][8], frames[1][6], frames[1][7], frames[1][8]);

			rot = frames[0] + 9;
			rot2 = frames[1] + 9;
			gar_RotYXZsuperpack_I(&rot, &rot2, 0);

			for (int i = 0; i < obj->nmeshes - 1; i++)
			{
				if (bone[0] & 1)
					phd_PopMatrix_I();

				if (bone[0] & 2)
					phd_PushMatrix_I();

				phd_TranslateRel_I(bone[1], bone[2], bone[3]);
				gar_RotYXZsuperpack_I(&rot, &rot2, 0);

				if (i == 2)
				{
					r = bike->light_intensity;
					g = bike->light_intensity;
					b = bike->light_intensity;

					if (bike->light_intensity >= 127)
					{
						r = bike->light_intensity + (GetRandomDraw() & 0xF) - 32;
						g = r;
						b = r;
					}

					GlobalAmbient = RGBA(r, g, b, 255);
					phd_PutPolygons_I(meshpp[0], -1);
				}

				bone += 4;
				meshpp += 2;
			}
		}
		else
#endif
		{
			phd_TranslateRel(frames[0][6], frames[0][7], frames[0][8]);
			rot = frames[0] + 9;
			gar_RotYXZsuperpack(&rot, 0);

			for (int i = 0; i < obj->nmeshes - 1; i++)
			{
				if (bone[0] & 1)
					phd_mxptr -= 12;

				if (bone[0] & 2)
					phd_PushMatrix();

				phd_TranslateRel(bone[1], bone[2], bone[3]);
				gar_RotYXZsuperpack(&rot, 0);

				if (i == 2)
				{
					r = bike->light_intensity;
					g = bike->light_intensity;
					b = bike->light_intensity;

					if (bike->light_intensity >= 127)
					{
						r = bike->light_intensity + (GetRandomDraw() & 0xF) - 32;
						g = r;
						b = r;
					}

					GlobalAmbient = RGBA(r, g, b, 255);
					phd_PutPolygons(meshpp[0], -1);
				}

				bone += 4;
				meshpp += 2;
			}
		}
	}

	phd_PopMatrix();
}

static void TriggerExhaustSmoke(long x, long y, long z, short angle, long velocity, long thing)
{
	SPARKS* sptr;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->dR = 96;
	sptr->dG = 96;
	sptr->dB = 128;
	sptr->sR = 0;
	sptr->sG = 0;
	sptr->sB = 0;

	if (thing)
	{
		sptr->dR = (uchar)((96 * velocity) >> 5);
		sptr->dG = (uchar)((96 * velocity) >> 5);
		sptr->dB = (uchar)((128 * velocity) >> 5);
	}

	sptr->ColFadeSpeed = 4;
	sptr->FadeToBlack = 4;
	sptr->Life = (uchar)((GetRandomControl() & 3) - (velocity >> 12) + 20);
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
	phd_sin(angle);
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
	sptr->Def = (uchar)objects[DEFAULT_SPRITES].mesh_index;
	sptr->Gravity = -4 - (GetRandomControl() & 3);
	sptr->MaxYvel = -8 - (GetRandomControl() & 7);
	sptr->dSize = (uchar)((GetRandomControl() & 7) + (velocity >> 7) + 32);
	sptr->sSize = sptr->dSize >> 1;
	sptr->Size = sptr->dSize >> 1;
}

static long CanGetOff(short num)	//always called with num = 1
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	long x, y, z, h, c;
	short yrot, room_number;

	item = &items[lara.vehicle];
	yrot = item->pos.y_rot + 16384;	//right side
	x = item->pos.x_pos + (512 * phd_sin(yrot) >> 14);
	y = item->pos.y_pos;
	z = item->pos.z_pos + (512 * phd_cos(yrot) >> 14);
	room_number = item->room_number;
	floor = GetFloor(x, y, z, &room_number);
	h = GetHeight(floor, x, y, z);

	if (height_type == BIG_SLOPE || height_type == DIAGONAL || h == NO_HEIGHT)
		return 0;

	if (ABS(h - item->pos.y_pos) > 272)
		return 0;

	c = GetCeiling(floor, x, y, z);

	if (c - item->pos.y_pos > -762 || h - c < 762)
		return 0;

	x = item->pos.x_pos + (128 * phd_sin(yrot) >> 14);
	y = item->pos.y_pos;
	z = item->pos.z_pos + (128 * phd_sin(yrot) >> 14);
	floor = GetFloor(x, y, z, &room_number);
	h = GetHeight(floor, x, y, z);

	if (height_type == BIG_SLOPE || height_type == DIAGONAL || h == NO_HEIGHT)
		return 0;

	if (ABS(h - item->pos.y_pos) > 80)
		return 0;

	c = GetCeiling(floor, x, y, z);

	if (c - item->pos.y_pos > -762 || h - c < 762)
		return 0;

	return 1;
}

void BikeExplode(ITEM_INFO* item)
{
	if (room[item->room_number].flags & ROOM_UNDERWATER)
		TriggerUnderwaterExplosion(item, 1);
	else
	{
		TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 3, -2, 0, item->room_number);

		for (int i = 0; i < 3; i++)
			TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 3, -1, 0, item->room_number);
	}

	ExplodingDeath2(lara.vehicle, -2, 256);
	KillItem(lara.vehicle);
	item->status = ITEM_DEACTIVATED;
	SoundEffect(SFX_EXPLOSION1, 0, SFX_DEFAULT);
	SoundEffect(SFX_EXPLOSION1, 0, SFX_DEFAULT);
	lara.vehicle = NO_ITEM;
}

void AnimateBike(ITEM_INFO* item, long hitWall, long killed)
{
	BIKEINFO* bike;
	short state, dmg;

	bike = (BIKEINFO*)item->data;
	state = lara_item->current_anim_state;

	if (item->pos.y_pos != item->floor && state != 8 && state != 17 && state != 20 && !killed)
	{
		if (bike->velocity < 0)
			lara_item->anim_number = objects[VEHICLE_EXTRA].anim_index + 6;
		else
			lara_item->anim_number = objects[VEHICLE_EXTRA].anim_index + 22;

		lara_item->goal_anim_state = 8;
		lara_item->current_anim_state = 8;
		lara_item->frame_number = anims[lara_item->anim_number].frame_base;
	}
	else if (hitWall)
	{
		if (state != 12 && state != 11 && state != 13 && state != 14 && state != 20 && bike->velocity > 10922 && !killed)
		{
			switch (hitWall)
			{
			case 13:
				lara_item->goal_anim_state = 12;
				lara_item->current_anim_state = 12;
				lara_item->anim_number = objects[VEHICLE_EXTRA].anim_index + 12;
				break;

			case 14:
				lara_item->goal_anim_state = 11;
				lara_item->current_anim_state = 11;
				lara_item->anim_number = objects[VEHICLE_EXTRA].anim_index + 11;
				break;

			case 11:
				lara_item->goal_anim_state = 13;
				lara_item->current_anim_state = VEHICLE_EXTRA;
				lara_item->anim_number = objects[33].anim_index + 14;
				break;

			default:
				lara_item->goal_anim_state = 14;
				lara_item->current_anim_state = 14;
				lara_item->anim_number = objects[VEHICLE_EXTRA].anim_index + 13;
				break;
			}

			lara_item->frame_number = anims[lara_item->anim_number].frame_base;
		}
	}
	else
	{
		switch (lara_item->current_anim_state)
		{
		case 1:

			if (killed)
			{
				if (bike->velocity > 0x4000)
					lara_item->goal_anim_state = 19;
				else
					lara_item->goal_anim_state = 7;
			}
			else
			{
				if (bike->velocity & 0xFFFFFF00 || input & (IN_ACTION | IN_JUMP))
				{
					if (input & IN_LEFT)
						lara_item->goal_anim_state = 2;
					else if (input & IN_RIGHT)
						lara_item->goal_anim_state = 22;
					else if (input & IN_JUMP)
					{
						if (bike->velocity > 21844)
							lara_item->goal_anim_state = 6;
						else
							lara_item->goal_anim_state = 5;
					}
					else if (input & IN_BACK && bike->velocity <= 2048)
						lara_item->goal_anim_state = 3;
				}
				else
					lara_item->goal_anim_state = 15;
			}

			break;

		case 2:

			if (bike->velocity & 0xFFFFFF00)
			{
				if (input & IN_RIGHT || !(input & IN_LEFT))
					lara_item->goal_anim_state = 1;
			}
			else
				lara_item->goal_anim_state = 15;

			break;

		case 3:

			if (input & IN_BACK)
				lara_item->goal_anim_state = 4;
			else
				lara_item->goal_anim_state = 15;

			break;

		case 5:
		case 6:
		case 18:

			if (bike->velocity & 0xFFFFFF00)
			{
				if (input & IN_LEFT)
					lara_item->goal_anim_state = 2;
				else if (input & IN_RIGHT)
					lara_item->goal_anim_state = 22;
			}
			else
				lara_item->goal_anim_state = 15;

			break;

		case 8:

			if (item->pos.y_pos == item->floor)
			{
				lara_item->goal_anim_state = 17;
				dmg = (short)(bikefspeed - 140);

				if (dmg > 0)
				{
					if (dmg > 100)
						lara_item->hit_points = -1;
					else
						lara_item->hit_points += -1000 * dmg * dmg / 10000;
				}
			}
			else if (item->fallspeed > 220)
				bike->flags |= 0x40;

			break;

		case 11:
		case 12:
		case 13:
		case 14:

			if (input & (IN_ACTION | IN_JUMP))
				lara_item->goal_anim_state = 1;

			break;

		case 15:

			if (killed)
				lara_item->goal_anim_state = 7;
			else if (((input & (IN_JUMP | IN_RIGHT)) != (IN_JUMP | IN_RIGHT)) || bike->velocity || dont_exit_bike)
			{
				if (input & IN_ACTION && !(input & IN_JUMP))
					lara_item->goal_anim_state = 1;
				else if (input & IN_BACK)
					lara_item->goal_anim_state = 3;
			}
			else if (CanGetOff(1))
				lara_item->goal_anim_state = 10;

			break;

		case 22:

			if (bike->velocity & 0xFFFFFF00)
			{
				if (input & IN_LEFT || !(input & IN_RIGHT))
					lara_item->goal_anim_state = 1;
			}
			else
				lara_item->goal_anim_state = 15;

			break;
		}
	}

	if (room[item->room_number].flags & ROOM_UNDERWATER)
	{
		lara_item->goal_anim_state = 20;
		lara_item->hit_points = 0;
		BikeExplode(item);
	}
}

void BikeStart(ITEM_INFO* item, ITEM_INFO* l)
{
	BIKEINFO* bike;

	bike = (BIKEINFO*)item->data;
	lara.gun_status = LG_HANDS_BUSY;
	lara.hit_direction = -1;
	l->goal_anim_state = 15;
	l->current_anim_state = 15;
	l->anim_number = objects[VEHICLE_EXTRA].anim_index + 18;
	l->frame_number = anims[l->anim_number].frame_base;
	l->gravity_status = 0;
	item->anim_number = l->anim_number + objects[MOTORBIKE].anim_index - objects[VEHICLE_EXTRA].anim_index;
	item->frame_number= l->frame_number + anims[item->anim_number].frame_base - anims[l->anim_number].frame_base;
	item->flags |= IFL_TRIGGERED;
	item->hit_points = 1;
	bike->unused1 = 0;
}

static long TestHeight(ITEM_INFO* item, long z, long x, PHD_VECTOR* pos)
{
	FLOOR_INFO* floor;
	long sx, sz, sy, cy, c, h;
	short room_number;

	sz = z * phd_sin(item->pos.x_rot) >> 14;
	sx = x * phd_sin(item->pos.z_rot) >> 14;
	pos->y = item->pos.y_pos + sx - sz;
	sy = phd_sin(item->pos.y_rot);
	cy = phd_cos(item->pos.y_rot);
	pos->z = item->pos.z_pos + ((z * cy - x * sy) >> 14);
	pos->x = item->pos.x_pos + ((x * cy + z * sy) >> 14);
	room_number = item->room_number;
	floor = GetFloor(pos->x, pos->y, pos->z, &room_number);
	c = GetCeiling(floor, pos->x, pos->y, pos->z);

	if (pos->y < c || c == NO_HEIGHT)
		return NO_HEIGHT;

	h = GetHeight(floor, pos->x, pos->y, pos->z);

	if (pos->y > h)
		pos->y = h;

	return h;
}

long BikeCheckGetOff()
{
	ITEM_INFO* item;
	BIKEINFO* bike;
	PHD_VECTOR pos;
	short state;

	item = &items[lara.vehicle];
	bike = (BIKEINFO*)item->data;
	state = lara_item->current_anim_state;

	if (state == 10 && lara_item->frame_number == anims[lara_item->anim_number].frame_end)
	{
		lara_item->pos.y_rot -= 0x4000;
		lara_item->anim_number = ANIM_STOP;
		lara_item->frame_number = anims[ANIM_STOP].frame_base;
		lara_item->goal_anim_state = AS_STOP;
		lara_item->current_anim_state = AS_STOP;
		lara_item->pos.x_pos -= 512 * phd_sin(lara_item->pos.y_rot) >> 14;
		lara_item->pos.z_pos -= 512 * phd_cos(lara_item->pos.y_rot) >> 14;
		lara_item->pos.z_rot = 0;
		lara_item->pos.x_rot = 0;
		lara.vehicle = NO_ITEM;
		lara.gun_status = 0;
		DashTimer = 120;
	}
	else if (lara_item->frame_number == anims[lara_item->anim_number].frame_end)
	{
		if (state == 20)
		{
			lara_item->anim_number = ANIM_FASTFALL;
			lara_item->frame_number = anims[ANIM_FASTFALL].frame_base;
			lara_item->current_anim_state = AS_FASTFALL;
			pos.x = 0;
			pos.y = 0;
			pos.z = 0;
			GetLaraJointPos(&pos, LM_HIPS);
			lara_item->pos.x_pos = pos.x;
			lara_item->pos.y_pos = pos.y;
			lara_item->pos.z_pos = pos.z;
			lara_item->pos.x_rot = 0;
			lara_item->pos.z_rot = 0;
			lara_item->fallspeed = item->fallspeed;
			lara_item->gravity_status = 1;
			lara_item->hit_points = 0;
			lara.gun_status = LG_NO_ARMS;
			item->flags |= IFL_INVISIBLE;
			return 0;
		}
		
		if (state == 19)
		{
			lara_item->goal_anim_state = AS_DEATH;
			lara_item->fallspeed = 240;
			lara_item->speed = 0;
			bike->flags |= 0x80;
			return 0;
		}
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
				fallspeed += zero;
			else
				fallspeed += 6;
		}
		else
		{
			*ypos = height;
			fallspeed = 0;
		}
	}

	return fallspeed;
}

void BikeCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	BIKEINFO* bike;
	short angle;

	if (l->hit_points < 0 || lara.vehicle != NO_ITEM)
		return;

	item = &items[item_number];
	bike = (BIKEINFO*)item->data;

	if (bike->light_intensity)
	{
		bike->light_intensity = bike->light_intensity - (bike->light_intensity >> 3) - 1;
		TriggerBikeBeam(item);
	}

	if (GetOnBike(item_number, coll))
	{
		lara.vehicle = item_number;

		if (lara.gun_type == WEAPON_FLARE)
		{
			CreateFlare(FLARE_ITEM, 0);
			undraw_flare_meshes();
			lara.flare_control_left = 0;
			lara.gun_type = LG_NO_ARMS;
			lara.request_gun_type = WEAPON_NONE;
			lara.flare_age = 0;
		}

		lara.gun_status = LG_HANDS_BUSY;
		angle = short(phd_atan(item->pos.z_pos - l->pos.z_pos, item->pos.x_pos - l->pos.x_pos) - item->pos.y_rot);

		if (angle <= -8190 || angle >= 24570)
		{
			if (GLOBAL_inventoryitemchosen == PUZZLE_ITEM1)
			{
				l->anim_number = objects[VEHICLE_EXTRA].anim_index + 28;
				GLOBAL_inventoryitemchosen = NO_ITEM;
				savegame.HaveBikeBooster = 1;
			}
			else
				l->anim_number = objects[VEHICLE_EXTRA].anim_index + 9;

			l->goal_anim_state = 9;
			l->current_anim_state = 9;
		}

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
		bike->unused1 = 0;
		item->flags |= IFL_TRIGGERED;
	}
	else
		ObjectCollision(item_number, l, coll);
}

void inject_bike(bool replace)
{
	INJECT(0x00464610, InitialiseBike, replace);
	INJECT(0x00464C20, DrawBikeExtras, replace);
	INJECT(0x00464850, TriggerBikeBeam, replace);
	INJECT(0x004648F0, GetOnBike, replace);
	INJECT(0x00464A30, DrawBikeBeam, replace);
	INJECT(0x00465220, TriggerExhaustSmoke, replace);
	INJECT(0x004668E0, CanGetOff, replace);
	INJECT(0x004653D0, BikeExplode, replace);
	INJECT(0x004664B0, AnimateBike, replace);	//has bug where laura sometimes doesn't die after falling with the bike????
	INJECT(0x00466E90, BikeStart, replace);
	INJECT(0x00465660, TestHeight, replace);
	INJECT(0x004654A0, BikeCheckGetOff, replace);
	INJECT(0x00465770, DoDynamics, replace);
	INJECT(0x00464680, BikeCollision, replace);
}
