#include "../tomb4/pch.h"
#include "larasurf.h"
#include "lara_states.h"
#include "lara.h"

void lara_as_surfswim(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_UWDEATH;
		return;
	}

	lara.dive_count = 0;

	if (input & IN_LEFT)
		item->pos.y_rot -= 728;
	else if (input & IN_RIGHT)
		item->pos.y_rot += 728;

	if (!(input & IN_FORWARD) || input & IN_JUMP)
		item->goal_anim_state = AS_SURFTREAD;

	item->fallspeed += 8;

	if (item->fallspeed > 60)
		item->fallspeed = 60;
}

void lara_as_surfback(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_UWDEATH;
		return;
	}

	lara.dive_count = 0;

	if (input & IN_LEFT)
		item->pos.y_rot -= 364;
	else if (input & IN_RIGHT)
		item->pos.y_rot += 364;

	if (!(input & IN_BACK))
		item->goal_anim_state = AS_SURFTREAD;

	item->fallspeed += 8;

	if (item->fallspeed > 60)
		item->fallspeed = 60;
}

void lara_as_surfleft(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_UWDEATH;
		return;
	}

	lara.dive_count = 0;

	if (input & IN_LEFT)
		item->pos.y_rot -= 364;
	else if (input & IN_RIGHT)
		item->pos.y_rot += 364;

	if (!(input & IN_LSTEP))
		item->goal_anim_state = AS_SURFTREAD;

	item->fallspeed += 8;

	if (item->fallspeed > 60)
		item->fallspeed = 60;
}

void lara_as_surfright(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_UWDEATH;
		return;
	}

	lara.dive_count = 0;

	if (input & IN_LEFT)
		item->pos.y_rot -= 364;
	else if (input & IN_RIGHT)
		item->pos.y_rot += 364;

	if (!(input & IN_RSTEP))
		item->goal_anim_state = AS_SURFTREAD;

	item->fallspeed += 8;

	if (item->fallspeed > 60)
		item->fallspeed = 60;
}

void lara_as_surftread(ITEM_INFO* item, COLL_INFO* coll)
{
	item->fallspeed -= 4;

	if (item->fallspeed < 0)
		item->fallspeed = 0;

	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_UWDEATH;
		return;
	}

	if (input & IN_LOOK)
	{
		LookUpDown();
		return;
	}

	if (input & IN_LEFT)
		item->pos.y_rot -= 728;
	else if (input & IN_RIGHT)
		item->pos.y_rot += 728;

	if (input & IN_FORWARD)
		item->goal_anim_state = AS_SURFSWIM;
	else if (input & IN_BACK)
		item->goal_anim_state = AS_SURFBACK;

	if (input & IN_LSTEP)
		item->goal_anim_state = AS_SURFLEFT;
	else if (input & IN_RSTEP)
		item->goal_anim_state = AS_SURFRIGHT;

	if (input & IN_JUMP)
	{
		lara.dive_count++;

		if (lara.dive_count == 10)
			item->goal_anim_state = AS_SWIM;
	}
	else
		lara.dive_count = 0;
}

void lara_col_surfswim(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->bad_neg = -384;
	lara.move_angle = item->pos.y_rot;
	LaraSurfaceCollision(item, coll);
	LaraTestWaterClimbOut(item, coll);
}

void lara_col_surfback(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot + 32768;
	LaraSurfaceCollision(item, coll);
}

void lara_col_surfleft(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot - 16384;
	LaraSurfaceCollision(item, coll);
}

void lara_col_surfright(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot + 16384;
	LaraSurfaceCollision(item, coll);
}

void lara_col_surftread(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->goal_anim_state == AS_SWIM)
	{
		item->current_anim_state = AS_DIVE;
		item->anim_number = ANIM_SURFDIVE;
		item->pos.x_rot = -8190;
		item->frame_number = anims[ANIM_SURFDIVE].frame_base;
		item->fallspeed = 80;
		lara.water_status = LW_UNDERWATER;
	}

	lara.move_angle = item->pos.y_rot;
	LaraSurfaceCollision(item, coll);
}

void inject_larasurf(bool replace)
{
	INJECT(0x00431C10, lara_as_surfswim, replace);
	INJECT(0x00431C80, lara_as_surfback, replace);
	INJECT(0x00431CE0, lara_as_surfleft, replace);
	INJECT(0x00431D40, lara_as_surfright, replace);
	INJECT(0x00431DA0, lara_as_surftread, replace);
	INJECT(0x00431E60, lara_col_surfswim, replace);
	INJECT(0x004322B0, lara_col_surfback, replace);
	INJECT(0x004322E0, lara_col_surfleft, replace);
	INJECT(0x00432310, lara_col_surfright, replace);
	INJECT(0x00432340, lara_col_surftread, replace);
}
