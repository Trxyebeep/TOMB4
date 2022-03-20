#include "../tomb4/pch.h"
#include "laramisc.h"
#include "objects.h"
#include "laraswim.h"
#include "lara_states.h"
#ifdef GENERAL_FIXES
#include "newinv.h"
#include "../tomb4/tomb4.h"
#endif

void LaraCheatGetStuff()
{
	lara.num_flares = -1;
	lara.num_small_medipack = -1;
	lara.num_large_medipack = -1;

	if (objects[CROWBAR_ITEM].loaded)
		lara.crowbar = 1;

	lara.lasersight = 1;
	lara.uzis_type_carried = 9;
	lara.shotgun_type_carried = 9;
	lara.sixshooter_type_carried = 9;
	lara.num_uzi_ammo = -1;
	lara.num_revolver_ammo = -1;
	lara.num_shotgun_ammo1 = -1;
}

void LaraCheatyBits()
{
#ifdef GENERAL_FIXES
	if (!tomb4.cheats)
		return;

#ifdef _DEBUG
	if (keymap[DIK_F1])
	{
		lara.num_large_medipack = -1;
		lara.num_small_medipack = -1;
		lara.num_revolver_ammo = -1;
		lara.num_uzi_ammo = -1;
		lara.num_crossbow_ammo1 = -1;
		lara.num_crossbow_ammo2 = -1;
		lara.num_crossbow_ammo3 = -1;
		lara.num_grenade_ammo1 = -1;
		lara.num_grenade_ammo2 = -1;
		lara.num_grenade_ammo3 = -1;
		lara.num_flares = -1;
		lara.num_shotgun_ammo1 = -1;
		lara.num_shotgun_ammo2 = -1;

		if (objects[LASERSIGHT_ITEM].loaded)
			lara.lasersight = 1;

		if (!(gfLevelFlags & GF_YOUNGLARA))
		{
			lara.pistols_type_carried |= 1;
			lara.uzis_type_carried |= 1;
			lara.shotgun_type_carried |= 1;
			lara.crossbow_type_carried |= 1;
			lara.grenade_type_carried |= 1;
			lara.sixshooter_type_carried |= 1;
		}

		dels_give_lara_items_cheat();
		lara_item->hit_points = 1000;
	}
#endif

#ifdef _DEBUG
	if (keymap[DIK_F2])
#else
	if (keymap[DIK_D] && keymap[DIK_O] && keymap[DIK_Z] && keymap[DIK_Y])
#endif
	{
		if (lara.vehicle != NO_ITEM)
			return;

		lara_item->pos.y_pos -= 128;

		if (lara.water_status != LW_FLYCHEAT)
		{
			lara.water_status = LW_FLYCHEAT;
			lara_item->frame_number = anims[ANIM_SWIMCHEAT].frame_base;
			lara_item->anim_number = ANIM_SWIMCHEAT;
			lara_item->current_anim_state = AS_SWIM;
			lara_item->goal_anim_state = AS_SWIM;
			lara_item->gravity_status = 0;
			lara_item->pos.x_rot = 0;
			lara.air = 1800;
			lara.death_count = 0;
			lara.torso_y_rot = 0;
			lara.torso_x_rot = 0;
			lara.head_y_rot = 0;
			lara.head_x_rot = 0;
		}
	}

#ifdef _DEBUG
	if (keymap[DIK_F3])
	{
		if (gfCurrentLevel == 2 || gfCurrentLevel == 6 || gfCurrentLevel == 13 || gfCurrentLevel == 21 || gfCurrentLevel == 27)
			skipped_level = 1;

		gfRequiredStartPos = 0;
		gfLevelComplete = gfCurrentLevel + 1;
	}
#endif

#else
	if (!Gameflow->CheatEnabled)
		return;

	if (input & IN_D)
	{
		LaraCheatGetStuff();
		lara_item->hit_points = 1000;
	}

	if (input & IN_CHEAT)
	{
		lara_item->pos.y_pos -= 128;

		if (lara.water_status != LW_FLYCHEAT)
		{
			lara.water_status = LW_FLYCHEAT;
			lara_item->frame_number = anims[ANIM_SWIMCHEAT].frame_base;
			lara_item->anim_number = ANIM_SWIMCHEAT;
			lara_item->current_anim_state = AS_SWIM;
			lara_item->goal_anim_state = AS_SWIM;
			lara_item->gravity_status = 0;
			lara_item->pos.x_rot = 5460;
			lara_item->fallspeed = 30;
			lara.air = 1800;
			lara.death_count = 0;
			lara.torso_y_rot = 0;
			lara.torso_x_rot = 0;
			lara.head_y_rot = 0;
			lara.head_x_rot = 0;
			cheat_hit_points = lara_item->hit_points;
		}
	}
#endif
}

void LaraCheat(ITEM_INFO* item, COLL_INFO* coll)
{
	lara_item->hit_points = 1000;
	LaraUnderWater(item, coll);

	if (input & IN_WALK && !(input & IN_LOOK))
	{
		lara.water_status = LW_ABOVE_WATER;
		item->frame_number = anims[ANIM_STOP].frame_base;
		item->anim_number = ANIM_STOP;
		item->pos.z_rot = 0;
		item->pos.x_rot = 0;
		lara.torso_y_rot = 0;
		lara.torso_x_rot = 0;
		lara.head_y_rot = 0;
		lara.head_x_rot = 0;
		lara.gun_status = 0;
		LaraInitialiseMeshes();
		lara.mesh_effects = 0;
#ifndef GENERAL_FIXES
		lara_item->hit_points = cheat_hit_points;
#endif
	}
}

void inject_laramisc(bool replace)
{
	INJECT(0x004301F0, LaraCheatGetStuff, replace);
	INJECT(0x00430A50, LaraCheatyBits, replace);
	INJECT(0x004309B0, LaraCheat, replace);
}
