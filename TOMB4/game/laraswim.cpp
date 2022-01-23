#include "../tomb4/pch.h"
#include "laraswim.h"
#include "lara_states.h"
#include "lara.h"
#include "laramisc.h"
#include "collide.h"
#include "larafire.h"
#include "control.h"
#ifdef GENERAL_FIXES
#include "effect2.h"
#endif

#ifdef GENERAL_FIXES
static void lara_as_swimcheat(ITEM_INFO* item, COLL_INFO* coll)
{
	if (input & IN_FORWARD)
		item->pos.x_rot -= 546;
	else if (input & IN_BACK)
		item->pos.x_rot += 546;

	if (input & IN_LEFT)
	{
		lara.turn_rate -= 613;

		if (lara.turn_rate < -1092)
			lara.turn_rate = -1092;
	}
	else if (input & IN_RIGHT)
	{
		lara.turn_rate += 613;

		if (lara.turn_rate > 1092)
			lara.turn_rate = 1092;
	}

	if (input & IN_ACTION)
		TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 31, 255, 255, 255);

	if (input & IN_ROLL)
		lara.turn_rate = -2184;

	if (input & IN_JUMP)
	{
		item->fallspeed += 16;

		if (item->fallspeed > 400)
			item->fallspeed = 400;
	}
	else
	{
		if (item->fallspeed >= 8)
			item->fallspeed -= item->fallspeed >> 3;
		else
			item->fallspeed = 0;
	}
}
#endif

void lara_as_swim(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_UWDEATH;
		return;
	}

	if (input & IN_ROLL)
	{
		item->current_anim_state = AS_WATERROLL;
		item->anim_number = ANIM_WATERROLL;
		item->frame_number = anims[ANIM_WATERROLL].frame_base;
	}
	else
	{
		SwimTurn(item);
		item->fallspeed += 8;

		if (item->fallspeed > 200)
			item->fallspeed = 200;

		if (!(input & IN_JUMP))
			item->goal_anim_state = AS_GLIDE;
	}
}

void LaraUnderWater(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -400;
	coll->bad_ceiling = 400;
	coll->old.x = item->pos.x_pos;
	coll->old.y = item->pos.y_pos;
	coll->old.z = item->pos.z_pos;
	coll->radius = 300;
	coll->trigger = 0;
	coll->slopes_are_walls = 0;
	coll->slopes_are_pits = 0;
	coll->lava_is_pit = 0;
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 1;

	if (input & IN_LOOK && lara.look)
		LookLeftRight();
	else
		ResetLook();

	lara.look = 1;
	lara_control_routines[item->current_anim_state](item, coll);

	if (item->pos.z_rot < -364)
		item->pos.z_rot += 364;
	else if (item->pos.z_rot > 364)
		item->pos.z_rot -= 364;
	else
		item->pos.z_rot = 0;

	if (item->pos.x_rot < -15470)
		item->pos.x_rot = -15470;
	else if (item->pos.x_rot > 15470)
		item->pos.x_rot = 15470;

	if (item->pos.z_rot < -4004)
		item->pos.z_rot = -4004;
	else if (item->pos.z_rot > 4004)
		item->pos.z_rot = 4004;

	if (lara.turn_rate < -364)
		lara.turn_rate += 364;
	else if (lara.turn_rate > 364)
		lara.turn_rate -= 364;
	else
		lara.turn_rate = 0;

	item->pos.y_rot += lara.turn_rate;

	if (lara.current_active && lara.water_status != LW_FLYCHEAT)
		LaraWaterCurrent(coll);

#ifndef GENERAL_FIXES
	if (lara.water_status == LW_FLYCHEAT)
	{
		if (input & IN_ACTION)
		{
			item->anim_number = 0;
			item->frame_number = 0;
		}
		else
		{
			item->anim_number = 103;
			item->frame_number = anims[103].frame_base;
			item->fallspeed = 0;
			item->speed = 0;
		}
	}
#endif

	AnimateLara(item);
	item->pos.x_pos += (((phd_sin(item->pos.y_rot) * item->fallspeed) >> 16) * phd_cos(item->pos.x_rot)) >> 14;
	item->pos.y_pos -= (phd_sin(item->pos.x_rot) * item->fallspeed) >> 16;
	item->pos.z_pos += (((phd_cos(item->pos.y_rot) * item->fallspeed) >> 16) * phd_cos(item->pos.x_rot)) >> 14;
	LaraBaddieCollision(item, coll);

	if (lara.vehicle == NO_ITEM)
		lara_collision_routines[item->current_anim_state](item, coll);

	UpdateLaraRoom(item, 0);
	LaraGun();
	TestTriggers(coll->trigger, 0, 0);

	if (lara.water_status == LW_FLYCHEAT)
	{
#ifdef GENERAL_FIXES
		item->anim_number = 263;
		item->frame_number = anims[263].frame_base;
#else
		item->anim_number = ANIM_FASTFALL;
		item->frame_number = anims[ANIM_FASTFALL].frame_base + 5;
#endif
	}
}

void lara_col_swim(ITEM_INFO* item, COLL_INFO* coll)
{
	LaraSwimCollision(item, coll);
}

void lara_col_glide(ITEM_INFO* item, COLL_INFO* coll)
{
#ifdef GENERAL_FIXES
	if (lara.water_status == LW_FLYCHEAT)
	{
		coll->bad_pos = -NO_HEIGHT;
		coll->bad_neg = -384;
		coll->bad_ceiling = 0;
		coll->radius = 200;
		coll->enable_baddie_push = 0;
		GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 384);
		ShiftItem(item, coll);

		if (coll->mid_floor < 0 && coll->mid_floor != NO_HEIGHT)
		{
			item->pos.x_rot += 728;
			item->pos.y_pos += coll->mid_floor;
		}
	}
	else
#endif
		LaraSwimCollision(item, coll);
}

void lara_col_tread(ITEM_INFO* item, COLL_INFO* coll)
{
	LaraSwimCollision(item, coll);
}

void lara_col_dive(ITEM_INFO* item, COLL_INFO* coll)
{
	LaraSwimCollision(item, coll);
}

void lara_col_waterroll(ITEM_INFO* item, COLL_INFO* coll)
{
	LaraSwimCollision(item, coll);
}

void lara_as_glide(ITEM_INFO* item, COLL_INFO* coll)
{
#ifdef GENERAL_FIXES
	if (lara.water_status == LW_FLYCHEAT)
		lara_as_swimcheat(item, coll);
	else
	{
#endif
		if (item->hit_points <= 0)
		{
			item->goal_anim_state = AS_UWDEATH;
			return;
		}

		if (input & IN_ROLL)
		{
			item->current_anim_state = AS_WATERROLL;
			item->anim_number = ANIM_WATERROLL;
			item->frame_number = anims[ANIM_WATERROLL].frame_base;
		}
		else
		{
			SwimTurn(item);

			if (input & IN_JUMP)
				item->goal_anim_state = AS_SWIM;

			item->fallspeed -= 6;

			if (item->fallspeed < 0)
				item->fallspeed = 0;

			if (item->fallspeed <= 133)
				item->goal_anim_state = AS_TREAD;
		}
#ifdef GENERAL_FIXES
	}
#endif
}

void lara_as_tread(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_UWDEATH;
		return;
	}

	if (input & IN_ROLL)
	{
		item->current_anim_state = AS_WATERROLL;
		item->anim_number = ANIM_WATERROLL;
		item->frame_number = anims[ANIM_WATERROLL].frame_base;
		return;
	}

	if (input & IN_LOOK)
		LookUpDown();

	SwimTurn(item);

	if (input & IN_JUMP)
		item->goal_anim_state = AS_SWIM;

	item->fallspeed -= 6;

	if (item->fallspeed < 0)
		item->fallspeed = 0;

	if (lara.gun_status == LG_HANDS_BUSY)
		lara.gun_status = LG_NO_ARMS;
}

void lara_as_dive(ITEM_INFO* item, COLL_INFO* coll)
{
	if (input & IN_FORWARD)
		item->pos.x_rot -= 182;
}

void lara_as_uwdeath(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;
	item->fallspeed -= 8;

	if (item->fallspeed <= 0)
		item->fallspeed = 0;

	if (item->pos.x_rot >= -364 && item->pos.x_pos <= 364)
		item->pos.x_rot = 0;
	else if (item->pos.x_pos < 0)
		item->pos.x_rot += 364;
	else
		item->pos.x_rot -= 364;
}

void lara_as_waterroll(ITEM_INFO* item, COLL_INFO* coll)
{
	item->fallspeed = 0;
}

void lara_col_uwdeath(ITEM_INFO* item, COLL_INFO* coll)
{
	long wh;

	item->hit_points = -1;
	lara.air = -1;
	lara.gun_status = LG_HANDS_BUSY;
	wh = GetWaterHeight(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number);

	if (wh != NO_HEIGHT && wh < item->pos.y_pos - 100)
		item->pos.y_pos -= 5;

	LaraSwimCollision(item, coll);
}

void inject_laraswim(bool replace)
{
	INJECT(0x00432620, lara_as_swim, replace);
	INJECT(0x004323A0, LaraUnderWater, replace);
	INJECT(0x004328C0, lara_col_swim, replace);
	INJECT(0x00432B10, lara_col_glide, replace);
	INJECT(0x00432B30, lara_col_tread, replace);
	INJECT(0x00432B50, lara_col_dive, replace);
	INJECT(0x00432BD0, lara_col_waterroll, replace);
	INJECT(0x00432720, lara_as_glide, replace);
	INJECT(0x004327A0, lara_as_tread, replace);
	INJECT(0x00432830, lara_as_dive, replace);
	INJECT(0x00432850, lara_as_uwdeath, replace);
	INJECT(0x004328B0, lara_as_waterroll, replace);
	INJECT(0x00432B70, lara_col_uwdeath, replace);
}
