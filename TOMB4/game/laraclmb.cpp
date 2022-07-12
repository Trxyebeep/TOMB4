#include "../tomb4/pch.h"
#include "laraclmb.h"
#include "lara.h"
#include "lara_states.h"
#include "laramisc.h"
#include "control.h"

void lara_as_climbstnc(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.IsClimbing = 1;
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;
	camera.target_elevation = -3640;

	if (input & IN_LOOK)
		LookUpDown();

	if (input & IN_LEFT || input & IN_LSTEP)
	{
		item->goal_anim_state = AS_CLIMBLEFT;
		lara.move_angle = item->pos.y_rot - 16384;
	}
	else if (input & IN_RIGHT || input & IN_RSTEP)
	{
		item->goal_anim_state = AS_CLIMBRIGHT;
		lara.move_angle = item->pos.y_rot + 16384;
	}
	else if (input & IN_JUMP)
	{
		if (item->anim_number == ANIM_CLIMBSTNC)
		{
			item->goal_anim_state = AS_BACKJUMP;
			lara.gun_status = LG_NO_ARMS;
			lara.move_angle = item->pos.y_rot + 32768;
		}
	}
}

void lara_as_climbleft(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	camera.target_angle = -5460;
	camera.target_elevation = -2730;

	if (!(input & (IN_LEFT | IN_LSTEP)))
		item->goal_anim_state = AS_CLIMBSTNC;
}

void lara_col_climbleft(ITEM_INFO* item, COLL_INFO* coll)
{
	long shift, res;

	if (!LaraCheckForLetGo(item, coll))
	{
		lara.move_angle = item->pos.y_rot - 16384;
		res = LaraTestClimbPos(item, coll->radius, -(coll->radius + 120), -512, 512, &shift);
		LaraDoClimbLeftRight(item, coll, res, shift);
	}
}

void lara_as_climbright(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	camera.target_angle = 5460;
	camera.target_elevation = -2730;

	if (!(input & (IN_RIGHT | IN_RSTEP)))
		item->goal_anim_state = AS_CLIMBSTNC;
}

void lara_col_climbright(ITEM_INFO* item, COLL_INFO* coll)
{
	long shift, res;

	if (!LaraCheckForLetGo(item, coll))
	{
		lara.move_angle = item->pos.y_rot + 16384;
		res = LaraTestClimbPos(item, coll->radius, coll->radius + 120, -512, 512, &shift);
		LaraDoClimbLeftRight(item, coll, res, shift);
	}
}

void lara_as_climbing(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	camera.target_elevation = 5460;
}

void lara_as_climbdown(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	camera.target_elevation = -8190;
}

void lara_as_climbend(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_baddie_push = 0;
	coll->enable_spaz = 0;
	camera.flags = 1;
	camera.target_angle = -8190;
}

void lara_col_climbstnc(ITEM_INFO* item, COLL_INFO* coll)
{
	long result_r, result_l, shift_r, shift_l, ledge_r, ledge_l;

	if (LaraCheckForLetGo(item, coll) || item->anim_number != ANIM_CLIMBSTNC)
		return;

	if (input & IN_FORWARD)
	{
		if (item->goal_anim_state == AS_NULL)
			return;

		item->goal_anim_state = AS_CLIMBSTNC;
		result_r = LaraTestClimbUpPos(item, coll->radius, coll->radius + 120, &shift_r, &ledge_r);
		result_l = LaraTestClimbUpPos(item, coll->radius, coll->radius - 120, &shift_l, &ledge_l);

		if (!result_r || !result_l)
			return;

		if (result_r < 0 || result_l < 0)
		{
			if (ABS(ledge_l - ledge_r) <= 120)
			{
				item->goal_anim_state = AS_NULL;
				item->pos.y_pos += (ledge_l + ledge_r) / 2 - 256;
				return;
			}
		}

		if (shift_r)
		{
			if (shift_l)
			{
				if (shift_r < 0 != shift_l < 0)
					return;

				if (shift_r < 0 && shift_r < shift_l)
					shift_l = shift_r;
				else if (shift_r > 0 && shift_r > shift_l)
					shift_l = shift_r;
			}
			else
				shift_l = shift_r;
		}

		item->goal_anim_state = AS_CLIMBING;
		item->pos.y_pos += shift_l;
	}
	else if (input & IN_BACK)
	{
		if (item->goal_anim_state == AS_HANG)
			return;

		item->goal_anim_state = AS_CLIMBSTNC;
		item->pos.y_pos += 256;
		result_r = LaraTestClimbPos(item, coll->radius, coll->radius + 120, -512, 512, &shift_r);
		result_l = LaraTestClimbPos(item, coll->radius, coll->radius - 120, -512, 512, &shift_l);
		item->pos.y_pos -= 256;

		if (!result_r || !result_l)
			return;

		if (shift_r && shift_l)
		{
			if (shift_r < 0 != shift_l < 0)
				return;

			if ((shift_r > 0 && shift_r > shift_l) || (shift_r < 0 && shift_r < shift_l))
				shift_l = shift_r;
		}

		if (result_l == 1 && result_r == 1)
		{
			item->goal_anim_state = AS_CLIMBDOWN;
			item->pos.y_pos += shift_l;
		}
		else
			item->goal_anim_state = AS_HANG;
	}
}

void lara_col_climbing(ITEM_INFO* item, COLL_INFO* coll)
{
	long frame, yshift, result_r, result_l, shift_r, shift_l, ledge_r, ledge_l;

	if (LaraCheckForLetGo(item, coll))
		return;

	if (item->anim_number != ANIM_CLIMBING)
		return;

	frame = item->frame_number - anims[ANIM_CLIMBING].frame_base;

	if (!frame)
		yshift = 0;
	else if (frame == 28 || frame == 29)
		yshift = -256;
	else if (frame == 57)
		yshift = -512;
	else
		return;

	item->pos.y_pos += yshift - 256;
	result_r = LaraTestClimbUpPos(item, coll->radius, coll->radius + 120, &shift_r, &ledge_r);
	result_l = LaraTestClimbUpPos(item, coll->radius, -(coll->radius + 120), &shift_l, &ledge_l);
	item->pos.y_pos += 256;

	if (result_r && result_l && input & IN_FORWARD)
	{
		if (result_r < 0 || result_l < 0)
		{
			item->goal_anim_state = AS_CLIMBSTNC;
			AnimateLara(item);

			if (ABS(ledge_r - ledge_l) <= 120)
			{
				item->goal_anim_state = AS_NULL;
				item->pos.y_pos += (ledge_r + ledge_l) / 2 - 256;
			}
		}
		else
		{
			item->goal_anim_state = AS_CLIMBING;
			item->pos.y_pos -= yshift;
		}
	}
	else
	{
		item->goal_anim_state = AS_CLIMBSTNC;

		if (yshift)
			AnimateLara(item);
	}
}

void lara_col_climbdown(ITEM_INFO* item, COLL_INFO* coll)
{
	long frame, yshift, result_r, result_l, shift_r, shift_l;

	if (LaraCheckForLetGo(item, coll))
		return;

	if (item->anim_number != ANIM_CLIMBDOWN)
		return;

	frame = item->frame_number - anims[ANIM_CLIMBDOWN].frame_base;

	if (!frame)
		yshift = 0;
	else if (frame == 28 || frame == 29)
		yshift = 256;
	else if (frame == 57)
		yshift = 512;
	else
		return;

	item->pos.y_pos += yshift + 256;
	result_r = LaraTestClimbPos(item, coll->radius, coll->radius + 120, -512, 512, &shift_r);
	result_l = LaraTestClimbPos(item, coll->radius, -(coll->radius + 120), -512, 512, &shift_l);
	item->pos.y_pos -= 256;

	if (result_r && result_l && input & IN_BACK)
	{
		if (shift_r && shift_l && shift_r < 0 != shift_l < 0)
		{
			item->goal_anim_state = AS_CLIMBSTNC;
			AnimateLara(item);
		}
		else if (result_r == -1 || result_l == -1)
		{
			item->anim_number = ANIM_CLIMBSTNC;
			item->frame_number = anims[ANIM_CLIMBSTNC].frame_base;
			item->current_anim_state = AS_CLIMBSTNC;
			item->goal_anim_state = AS_HANG;
			AnimateLara(item);
		}
		else
		{
			item->goal_anim_state = AS_CLIMBDOWN;
			item->pos.y_pos -= yshift;
		}
	}
	else
	{
		item->goal_anim_state = AS_CLIMBSTNC;

		if (yshift)
			AnimateLara(item);
	}
}

short GetClimbTrigger(long x, long y, long z, short room_number)
{
	FLOOR_INFO* floor;
	short* data;

	floor = GetFloor(x, y, z, &room_number);
	GetHeight(floor, x, y, z);

	if (!trigger_index)
		return 0;

	data = trigger_index;

	if ((*data & 0x1F) == LAVA_TYPE)
	{
		if (*data & 0x8000)
			return 0;

		data++;
	}

	if ((*data & 0x1F) == CLIMB_TYPE)
		return *data;

	return 0;
}

void inject_laraclmb(bool replace)
{
	INJECT(0x0042C6C0, lara_as_climbstnc, replace);
	INJECT(0x0042CC80, lara_as_climbleft, replace);
	INJECT(0x0042CCC0, lara_col_climbleft, replace);
	INJECT(0x0042D3D0, lara_as_climbright, replace);
	INJECT(0x0042D410, lara_col_climbright, replace);
	INJECT(0x0042D470, lara_as_climbing, replace);
	INJECT(0x0042D5E0, lara_as_climbdown, replace);
	INJECT(0x0042D7A0, lara_as_climbend, replace);
	INJECT(0x0042C770, lara_col_climbstnc, replace);
	INJECT(0x0042D490, lara_col_climbing, replace);
	INJECT(0x0042D600, lara_col_climbdown, replace);
	INJECT(0x0042D7D0, GetClimbTrigger, replace);
}
