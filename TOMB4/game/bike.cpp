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
#include "effects.h"
#include "debris.h"
#include "lara.h"
#include "jeep.h"
#include "../specific/input.h"
#include "newinv.h"
#include "savegame.h"

static ITEM_INFO* GlobalBikeItem;
static long bikefspeed = 0;
static short broomies[22];
static char dont_exit_bike = 0;

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
	intensity = (bike->light_intensity << 1) - (GetRandomControl() & 0xF);

	if (intensity > 0)
		LaraTorch(&s, &d, item->pos.y_rot, intensity);
	else
		bLaraTorch = 0;
}

long GetOnBike(short item_number, COLL_INFO* coll)
{
	ITEM_INFO* item;
	long dx, dz;
	short room_number, rot;

	item = &items[item_number];

	if (item->flags & IFL_INVISIBLE || lara.gun_status != LG_NO_ARMS || lara_item->gravity_status)
		return 0;

	if (abs(item->pos.y_pos - lara_item->pos.y_pos) > 256 || !(input & IN_ACTION) && GLOBAL_inventoryitemchosen != PUZZLE_ITEM1)
		return 0;

	dx = lara_item->pos.x_pos - item->pos.x_pos;
	dz = lara_item->pos.z_pos - item->pos.z_pos;

	if (SQUARE(dx) + SQUARE(dz) > 170000)
		return 0;

	room_number = item->room_number;

	if (GetHeight(GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number),
		item->pos.x_pos, item->pos.y_pos, item->pos.z_pos) < -32000)
		return 0;

	rot = short(phd_atan(item->pos.z_pos - lara_item->pos.z_pos, item->pos.x_pos - lara_item->pos.x_pos) - item->pos.y_rot);

	if (rot > -8190 && rot < 24570)
		return 0;

	rot = lara_item->pos.y_rot - item->pos.y_rot;

	if (rot <= -24586 || rot >= -8206)
		return 0;

	return 1;
}

void DrawBikeBeam(ITEM_INFO* item)
{
	BIKEINFO* bike;
	OBJECT_INFO* obj;
	short** meshpp;
	long* bone;
	short* frm[2];
	short* rot;
	long frac, rate, bounds, r, g, b;
	short* rot2;

	bike = (BIKEINFO*)item->data;

	if (!bike->light_intensity)
		return;

	frac = GetFrames(item, frm, &rate);
	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	bounds = S_GetObjectBounds(frm[0]);

	if (bounds)
	{
		CalculateObjectLighting(item, frm[0]);
		bounds = 1;
		obj = &objects[item->object_number];
		meshpp = &meshes[obj->mesh_index];
		meshpp += 2;
		bone = &bones[obj->bone_index];

		if (frac)
		{
			InitInterpolate(frac, rate);
			phd_TranslateRel_ID(frm[0][6], frm[0][7], frm[0][8], frm[1][6], frm[1][7], frm[1][8]);

			rot = frm[0] + 9;
			rot2 = frm[1] + 9;
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
		{
			phd_TranslateRel(frm[0][6], frm[0][7], frm[0][8]);
			rot = frm[0] + 9;
			gar_RotYXZsuperpack(&rot, 0);

			for (int i = 0; i < obj->nmeshes - 1; i++)
			{
				if (bone[0] & 1)
					phd_PopMatrix();

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
		sptr->dR = uchar((96 * velocity) >> 5);
		sptr->dG = uchar((96 * velocity) >> 5);
		sptr->dB = uchar((128 * velocity) >> 5);
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
	sptr->Def = (uchar)objects[DEFAULT_SPRITES].mesh_index;
	sptr->Gravity = -4 - (GetRandomControl() & 3);
	sptr->MaxYvel = -8 - (GetRandomControl() & 7);
	sptr->dSize = uchar((GetRandomControl() & 7) + (velocity >> 7) + 32);
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
	x = item->pos.x_pos + (512 * phd_sin(yrot) >> W2V_SHIFT);
	y = item->pos.y_pos;
	z = item->pos.z_pos + (512 * phd_cos(yrot) >> W2V_SHIFT);
	room_number = item->room_number;
	floor = GetFloor(x, y, z, &room_number);
	h = GetHeight(floor, x, y, z);

	if (height_type == BIG_SLOPE || height_type == DIAGONAL || h == NO_HEIGHT)
		return 0;

	if (abs(h - item->pos.y_pos) > 272)
		return 0;

	c = GetCeiling(floor, x, y, z);

	if (c - item->pos.y_pos > -762 || h - c < 762)
		return 0;

	x = item->pos.x_pos + (128 * phd_sin(yrot) >> W2V_SHIFT);
	y = item->pos.y_pos;
	z = item->pos.z_pos + (128 * phd_sin(yrot) >> W2V_SHIFT);
	floor = GetFloor(x, y, z, &room_number);
	h = GetHeight(floor, x, y, z);

	if (height_type == BIG_SLOPE || height_type == DIAGONAL || h == NO_HEIGHT)
		return 0;

	if (abs(h - item->pos.y_pos) > 80)
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
	SoundEffect(SFX_EXPLOSION2, 0, SFX_DEFAULT);
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
	else if (hitWall && state != 12 && state != 11 && state != 13 && state != 14 && state != 20 && bike->velocity > 10922 && !killed)
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
			lara_item->current_anim_state = 13;
			lara_item->anim_number = objects[VEHICLE_EXTRA].anim_index + 14;
			break;

		default:
			lara_item->goal_anim_state = 14;
			lara_item->current_anim_state = 14;
			lara_item->anim_number = objects[VEHICLE_EXTRA].anim_index + 13;
			break;
		}

		lara_item->frame_number = anims[lara_item->anim_number].frame_base;
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
				dmg = short(bikefspeed - 140);

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

long TestHeight(ITEM_INFO* item, long z, long x, PHD_VECTOR* pos)
{
	FLOOR_INFO* floor;
	long sx, sz, sy, cy, c, h;
	short room_number;

	sz = z * phd_sin(item->pos.x_rot) >> W2V_SHIFT;
	sx = x * phd_sin(item->pos.z_rot) >> W2V_SHIFT;
	pos->y = item->pos.y_pos + sx - sz;
	sy = phd_sin(item->pos.y_rot);
	cy = phd_cos(item->pos.y_rot);
	pos->z = item->pos.z_pos + ((z * cy - x * sy) >> W2V_SHIFT);
	pos->x = item->pos.x_pos + ((x * cy + z * sy) >> W2V_SHIFT);
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

static long BikeCheckGetOff()
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
		lara_item->pos.x_pos -= 512 * phd_sin(lara_item->pos.y_rot) >> W2V_SHIFT;
		lara_item->pos.z_pos -= 512 * phd_cos(lara_item->pos.y_rot) >> W2V_SHIFT;
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

long BikeBaddieCollision(ITEM_INFO* bike)
{
	ITEM_INFO* item;
	OBJECT_INFO* obj;
	short* doors;
	long j, dx, dy, dz;
	short room_count, item_number;

	room_count = 1;
	broomies[0] = bike->room_number;
	doors = room[bike->room_number].door;

	for (int i = *doors++; i > 0; i--, doors += 16)
	{
		for (j = 0; j < room_count; j++)
		{
			if (broomies[j] == *doors)
				break;
		}

		if (j == room_count)
		{
			broomies[room_count] = *doors;
			room_count++;
		}
	}

	for (int i = 0; i < room_count; i++)
	{
		for (item_number = room[broomies[i]].item_number; item_number != NO_ITEM; item_number = item->next_item)
		{
			item = &items[item_number];

			if (item->collidable && item->status != ITEM_INVISIBLE && item != lara_item && item != bike)
			{
				obj = &objects[item->object_number];

				if (obj->collision && obj->intelligent)
				{
					dx = bike->pos.x_pos - item->pos.x_pos;
					dy = bike->pos.y_pos - item->pos.y_pos;
					dz = bike->pos.z_pos - item->pos.z_pos;

					if (dx > -2048 && dx < 2048 && dz > -2048 && dz < 2048 && dy > -2048 && dy < 2048)
					{
						if (TestBoundsCollide(item, bike, 500))
						{
							if (item->object_number == MUTANT)
								return 1;

							if (item->hit_points)
								SoundEffect(SFX_BIKE_HIT_ENEMIES, &item->pos, SFX_DEFAULT);

							DoLotsOfBlood(item->pos.x_pos, bike->pos.y_pos - 256, item->pos.z_pos, (GetRandomControl() & 3) + 8,
								bike->pos.y_rot, item->room_number, 3);
							item->hit_points = 0;
						}
					}
				}
			}
		}
	}

	return 0;
}

void BikeCollideStaticObjects(long x, long y, long z, short room_number, long height)
{
	MESH_INFO* mesh;
	STATIC_INFO* sinfo;
	ROOM_INFO* r;
	PHD_VECTOR pos;
	short* doors;
	long j;
	static long BikeBounds[6] = { 0, 0, 0, 0, 0, 0 };
	static long CollidedStaticBounds[6] = { 0, 0, 0, 0, 0, 0 };
	short room_count, rn;

	pos.x = x;
	pos.y = y;
	pos.z = z;
	BikeBounds[0] = x + 256;
	BikeBounds[1] = x - 256;
	BikeBounds[2] = y;
	BikeBounds[3] = y - height;
	BikeBounds[4] = z + 256;
	BikeBounds[5] = z - 256;
	room_count = 1;
	broomies[0] = room_number;
	doors = room[room_number].door;

	for (int i = *doors++; i > 0; i--, doors += 16)
	{
		for (j = 0; j < room_count; j++)
		{
			if (broomies[j] == *doors)
				break;
		}

		if (j == room_count)
		{
			broomies[room_count] = *doors;
			room_count++;
		}
	}

	for (int i = 0; i < room_count; i++)
	{
		rn = broomies[i];
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

					if (BikeBounds[0] > CollidedStaticBounds[1] &&
						BikeBounds[1] < CollidedStaticBounds[0] &&
						BikeBounds[2] > CollidedStaticBounds[3] &&
						BikeBounds[3] < CollidedStaticBounds[2] &&
						BikeBounds[4] > CollidedStaticBounds[5] &&
						BikeBounds[5] < CollidedStaticBounds[4])
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

static long UserControl(ITEM_INFO* item, long height, long* pitch)
{
	BIKEINFO* bike;
	PHD_VECTOR pos;
	long turn, vel;
	short frame, base;

	bike = (BIKEINFO*)item->data;

	if (bike->light_intensity < 127)
	{
		bike->light_intensity += (GetRandomControl() & 7) + 3;

		if (bike->light_intensity > 127)
			bike->light_intensity = 127;
	}

	if (bike->unused1 > 16)
	{
		bike->velocity += bike->unused1 >> 4;
		bike->unused1 -= bike->unused1 >> 3;
	}
	else
		bike->unused1 = 0;

	if (input & IN_SPRINT && input & IN_ACTION && DashTimer && savegame.HaveBikeBooster)
	{
		bike->flags |= 0x100;
		DashTimer -= 2;

		if (DashTimer < 0)
		{
			bike->flags &= ~0x100;
			DashTimer = 0;
		}
	}
	else
		bike->flags &= ~0x100;

	if (item->pos.y_pos >= height - 256)
	{
		if (bike->velocity > 0x4000)
			turn = 910;
		else
			turn = (910 * bike->velocity) >> W2V_SHIFT;

		if (!bike->velocity && input & IN_LOOK)
			LookUpDown();

		if (bike->velocity > 0)
		{
			if (input & IN_LEFT)
			{
				if (bike->velocity > 0x4000)
					bike->bike_turn -= 273;
				else
					bike->bike_turn -= 182 + ((91 * bike->velocity) >> W2V_SHIFT);

				if (bike->bike_turn < -turn)
					bike->bike_turn = -turn;
			}
			else if (input & IN_RIGHT)
			{
				if (bike->velocity > 0x4000)
					bike->bike_turn += 273;
				else
					bike->bike_turn += 182 + ((91 * bike->velocity) >> W2V_SHIFT);

				if (bike->bike_turn > turn)
					bike->bike_turn = turn;
			}
		}
		else if (bike->velocity < 0)
		{
			if (input & IN_RIGHT)
			{
				bike->bike_turn -= 91;

				if (bike->bike_turn < -910)
					bike->bike_turn = -910;
			}
			else if (input & IN_LEFT)
			{
				bike->bike_turn += 91;

				if (bike->bike_turn > 910)
					bike->bike_turn = 910;
			}
		}

		if (input & IN_JUMP)
		{
			pos.x = 0;
			pos.y = -144;
			pos.z = -1024;
			GetJointAbsPosition(item, &pos, 0);
			TriggerDynamic(pos.x, pos.y, pos.z, 10, 64, 0, 0);
			item->mesh_bits = 0x5F7;
		}
		else
			item->mesh_bits = 0x3F7;

		if (input & IN_JUMP)
		{
			if (bike->velocity > 0)
			{
				bike->velocity -= 0x300;

				if (bike->velocity < 0)
					bike->velocity = 0;
			}
			else if (bike->velocity < 0)
			{
				bike->velocity += 0x300;

				if (bike->velocity > 0)
					bike->velocity = 0;
			}
		}
		else if (input & IN_ACTION)
		{
			if (bike->velocity < 0xC000)
			{
				if (bike->velocity < 0x4000)
					bike->velocity += ((0x4800 - bike->velocity) >> 3) + 8;
				else if (bike->velocity < 0x7000)
					bike->velocity += ((0x7800 - bike->velocity) >> 4) + 4;
				else
					bike->velocity += ((0xC000 - bike->velocity) >> 4) + 2;

				if (bike->flags & 0x100)
					bike->velocity += 0x100;
			}
			else
				bike->velocity = 0xC000;

			bike->velocity -= abs(item->pos.y_rot - bike->move_angle) >> 6;
		}

		if (lara_item->current_anim_state == 3)
		{
			frame = lara_item->frame_number;
			base = anims[lara_item->anim_number].frame_base;

			if (frame >= base + 24 && frame <= base + 29)
			{
				if (bike->velocity > -0x3000)
					bike->velocity -= 0x600;
			}
		}

		if (!(input & IN_ACTION))
		{
			if (bike->velocity > 384)
				bike->velocity -= 384;
			else if (bike->velocity < -384)
				bike->velocity += 384;
			else
				bike->velocity = 0;
		}

		item->speed = short(bike->velocity >> 8);

		if (bike->pitch1 > 0xC000)
			bike->pitch1 = (GetRandomControl() & 0x1FF) + 0xBF00;

		vel = bike->velocity;

		if (vel < 0)
			vel >>= 1;

		bike->pitch1 += (abs(vel) - 0x2000 - bike->pitch1) >> 3;
	}
	else if (bike->pitch1 < 0xFFFF)
		bike->pitch1 += (0xFFFF - bike->pitch1) >> 3;

	*pitch = bike->pitch1;
	return 0;
}

long BikeDynamics(ITEM_INFO* item)
{
	BIKEINFO* bike;
	FLOOR_INFO* floor;
	PHD_VECTOR pos, newPos;
	PHD_VECTOR flPos, frPos, blPos, brPos, fmPos, flPos2, frPos2, blPos2, brPos2, fmPos2;
	long front_left, front_right, back_left, back_right, front_mid, front_left2, front_right2, back_left2, back_right2, front_mid2;
	long h, speed, shift, shift2, anim, dx, dz;
	short ang, ang2, vel, room_number;

	dont_exit_bike = 0;
	bike = (BIKEINFO*)item->data;
	front_left = TestHeight(item, 500, -350, &flPos);
	front_right = TestHeight(item, 500, 128, &frPos);
	back_left = TestHeight(item, -500, -350, &blPos);
	back_right = TestHeight(item, -500, 128, &brPos);
	front_mid = TestHeight(item, -500, 0, &fmPos);
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
		if (bike->bike_turn < -91)
			bike->bike_turn += 91;
		else if (bike->bike_turn > 91)
			bike->bike_turn -= 91;
		else
			bike->bike_turn = 0;

		item->pos.y_rot += short(bike->bike_turn + bike->extra_rotation);
		bike->move_angle += short(item->pos.y_rot - bike->move_angle) >> 5;
	}
	else
	{
		if (bike->bike_turn < -182)
			bike->bike_turn += 182;
		else if (bike->bike_turn > 182)
			bike->bike_turn -= 182;
		else
			bike->bike_turn = 0;

		item->pos.y_rot += short(bike->bike_turn + bike->extra_rotation);
		ang = item->pos.y_rot - bike->move_angle;
		vel = short(728 - ((2 * bike->velocity) >> 10));

		if (!(input & IN_ACTION) && bike->velocity > 0)
			vel += vel >> 1;

		if (ang < -273)
		{
			if (ang < -8190)
				bike->move_angle = item->pos.y_rot + 8190;
			else
				bike->move_angle -= vel;
		}
		else if (ang > 273)
		{
			if (ang > 8190)
				bike->move_angle = item->pos.y_rot - 8190;
			else
				bike->move_angle += vel;
		}
		else
			bike->move_angle = item->pos.y_rot;
	}

	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	if (item->pos.y_pos < h)
		speed = item->speed;
	else
		speed = (item->speed * phd_cos(item->pos.x_rot)) >> W2V_SHIFT;

	item->pos.x_pos += (speed * phd_sin(bike->move_angle)) >> W2V_SHIFT;
	item->pos.z_pos += (speed * phd_cos(bike->move_angle)) >> W2V_SHIFT;

	if (item->pos.y_pos >= h)
	{
		ang = (100 * phd_sin(item->pos.x_rot)) >> W2V_SHIFT;

		if (abs(ang) > 16)
		{
			ang2 = (100 * phd_sin(item->pos.x_rot)) >> W2V_SHIFT;

			if (ang < 0)
				ang2 = -ang;

			if (ang2 > 24)
				dont_exit_bike = 1;

			ang <<= 4;
			bike->velocity -= ang;
		}

		ang = (128 * phd_sin(item->pos.z_rot)) >> W2V_SHIFT;

		if (abs(ang) > 32)
		{
			dont_exit_bike = 1;

			if (ang < 0)
				ang2 = item->pos.y_rot - 0x4000;
			else
				ang2 = item->pos.y_rot + 0x4000;

			item->pos.x_pos += ((abs(ang) - 24) * phd_sin(ang2)) >> W2V_SHIFT;
			item->pos.z_pos += ((abs(ang) - 24) * phd_cos(ang2)) >> W2V_SHIFT;
		}
	}

	if (bike->velocity <= 0x8000 || bike->flags & 0x100)
	{
		if (bike->velocity > 0xC000)
			bike->velocity = 0xC000;
		else if (bike->velocity < -0x3000)
			bike->velocity = -0x3000;
	}
	else
		bike->velocity -= 0x440;

	newPos.x = item->pos.x_pos;
	newPos.z = item->pos.z_pos;

	if (!(item->flags & 0x100))
	{
		if (BikeBaddieCollision(item))	//this returns = we hit the crocgod, explode her
			return -888;

		BikeCollideStaticObjects(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 512);
	}

	shift = 0;
	shift2 = 0;
	front_left2 = TestHeight(item, 500, -350, &flPos2);

	if (front_left2 < flPos.y - 256)
		shift = abs(DoShift(item, &flPos2, &flPos) << 2);

	back_left2 = TestHeight(item, -500, -350, &blPos2);

	if (back_left2 < blPos.y - 256)
	{
		if (shift)
			shift += abs(DoShift(item, &blPos2, &blPos) << 2);
		else
			shift = -abs(DoShift(item, &blPos2, &blPos) << 2);
	}

	front_right2 = TestHeight(item, 500, 128, &frPos2);

	if (front_right2 < frPos.y - 256)
		shift2 = -abs(DoShift(item, &frPos2, &frPos) << 2);

	front_mid2 = TestHeight(item, -500, 0, &fmPos2);

	if (front_mid2 < fmPos.y - 256)
		DoShift(item, &fmPos2, &fmPos);

	back_right2 = TestHeight(item, -500, 128, &brPos2);

	if (back_right2 < brPos.y - 256)
	{
		if (shift2)
			shift2 -= abs(DoShift(item, &brPos2, &brPos) << 2);
		else
			shift2 = abs(DoShift(item, &brPos2, &brPos) << 2);
	}

	if (shift)
		shift2 = shift;

	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	if (h < item->pos.y_pos - 256)
		DoShift(item, (PHD_VECTOR*)&item->pos, &pos);

	if (!bike->velocity)
		shift2 = 0;

	bike->rot_thing = short((bike->rot_thing + shift2) >> 1);

	if (abs(bike->rot_thing) < 2)
		bike->rot_thing = 0;

	if (abs(bike->rot_thing) < 4)
		bike->extra_rotation = bike->rot_thing;
	else
		bike->extra_rotation += (bike->rot_thing - bike->extra_rotation) >> 2;

	anim = GetCollisionAnim(item, &newPos, bike);

	if (anim)
	{
		dx = item->pos.x_pos - pos.x;
		dz = item->pos.z_pos - pos.z;
		speed = (dx * phd_sin(bike->move_angle) + dz * phd_cos(bike->move_angle)) >> W2V_SHIFT;
		speed <<= 8;

		if (&items[lara.vehicle] == item && bike->velocity >= 0x8000 && speed < bike->velocity - 10)	//did we just stop randomly (i.e hit a wall)
		{
			lara_item->hit_points -= short((bike->velocity - speed) >> 7);
			lara_item->hit_status = 1;
		}

		if (bike->velocity > 0 && speed < bike->velocity)
			bike->velocity = speed < 0 ? 0 : speed;
		else if (bike->velocity < 0 && speed > bike->velocity)
			bike->velocity = speed > 0 ? 0 : speed;

		if (bike->velocity < -0x3000)
			bike->velocity = -0x3000;
	}

	return anim;
}

void BikeControl(short item_number)
{
	ITEM_INFO* item;
	BIKEINFO* bike;
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
	item = &items[lara.vehicle];
	bike = (BIKEINFO*)item->data;
	hitWall = BikeDynamics(item);

	if (hitWall == -888)	//hit the crocgod
	{
		ExplodingDeath2(lara.item_number, -1, 256);
		lara_item->hit_points = 0;
		lara_item->flags |= IFL_INVISIBLE;
		BikeExplode(item);
		return;
	}

	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	front_left = TestHeight(item, 500, -350, &flPos);
	front_right = TestHeight(item, 500, 128, &frPos);
	front_mid = TestHeight(item, -500, 0, &fmPos);
	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	TestTriggers(trigger_index, 0, 0);
	TestTriggers(trigger_index, 1, 0);

	if (lara_item->hit_points <= 0)
	{
		killed = 1;
		input &= ~(IN_FORWARD | IN_BACK | IN_LEFT | IN_RIGHT);	//should be IN_ACTION instead?
	}

	if (bike->flags & 0xFF)
		hitWall = 0;
	else
	{
		TriggerBikeBeam(item);

		if (lara_item->current_anim_state < 9 || lara_item->current_anim_state > 10)
			driving = UserControl(item, h, &pitch);
		else
		{
			driving = -1;
			hitWall = 0;
		}
	}

	if (bike->velocity > 0 || bike->unused1)
	{
		bike->pitch2 = pitch;

		if (bike->pitch2 < -0x8000)
			bike->pitch2 = -0x8000;
		else if (bike->pitch2 > 0xA000)
			bike->pitch2 = 0xA000;

		SoundEffect(SFX_BIKE_MOVING, &item->pos, (bike->pitch2 << 8) + (SFX_SETPITCH | 0x1000000));
	}
	else
	{
		if (driving != -1)
		{
			SoundEffect(SFX_BIKE_IDLE, &item->pos, SFX_DEFAULT);
			SoundEffect(SFX_BIKE_MOVING, &item->pos, (bike->pitch2 << 8) + (SFX_SETPITCH | 0x1000000));
		}

		bike->pitch2 = 0;
	}

	item->floor = h;
	wheelRot = short(bike->velocity >> 2);
	bike->right_front_wheelrot -= wheelRot;
	bike->right_back_wheelrot -= wheelRot;
	bike->left_wheelrot -= wheelRot;
	bikefspeed = item->fallspeed;
	oldY = item->pos.y_pos;
	item->fallspeed = (short)DoDynamics(h, item->fallspeed, &item->pos.y_pos, 0);
	hdiff = (frPos.y + flPos.y) >> 1;

	if (fmPos.y < front_mid)
	{
		if (hdiff < (front_left + front_right) >> 1)
			xRot = (short)phd_atan(125, oldY - item->pos.y_pos);
		else
			xRot = (short)phd_atan(500, item->pos.y_pos - hdiff);
	}
	else
	{
		if (hdiff < (front_left + front_right) >> 1)
			xRot = (short)phd_atan(500, front_mid - item->pos.y_pos);
		else
			xRot = (short)phd_atan(1000, front_mid - hdiff);
	}

	zRot = (short)phd_atan(350, hdiff - flPos.y);
	item->pos.x_rot += (xRot - item->pos.x_rot) >> 2;
	item->pos.z_rot += (zRot - item->pos.z_rot) >> 2;

	if (!(bike->flags & 0x80))
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
		AnimateBike(item, hitWall, killed);
		AnimateItem(lara_item);
		item->anim_number = objects[MOTORBIKE].anim_index + lara_item->anim_number - objects[VEHICLE_EXTRA].anim_index;
		item->frame_number = lara_item->frame_number + anims[item->anim_number].frame_base - anims[lara_item->anim_number].frame_base;
		camera.target_elevation = -5460;

		if (bike->flags & 0x40 && item->pos.y_pos == item->floor)
		{
			ExplodingDeath2(lara.item_number, -1, 256);
			lara_item->hit_points = 0;
			lara_item->flags |= IFL_INVISIBLE;
			BikeExplode(item);
			return;
		}
	}

	if (lara_item->current_anim_state == 9 || lara_item->current_anim_state == 10)
		ExhaustSmokeVel = 0;
	else
	{
		pos.x = 56;
		pos.y = -144;
		pos.z = -500;
		GetJointAbsPosition(item, &pos, 0);

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

	BikeCheckGetOff();
}
