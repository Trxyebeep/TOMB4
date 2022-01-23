#include "../tomb4/pch.h"
#include "laramisc.h"
#include "objects.h"
#include "laraswim.h"
#include "lara_states.h"

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
#ifndef GENERAL_FIXES
	if (!Gameflow->CheatEnabled)
		return;
#endif
	
#ifdef GENERAL_FIXES
	if (keymap[DIK_F1])
#else
	if (input & IN_D)
#endif
	{
		LaraCheatGetStuff();
		lara_item->hit_points = 1000;
	}

#ifdef GENERAL_FIXES
	if (keymap[DIK_F2])
#else
	if (input & IN_CHEAT)
#endif
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
#ifdef GENERAL_FIXES
			lara_item->pos.x_rot = 0;
#else
			lara_item->pos.x_rot = 5460;
#endif
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

#ifdef GENERAL_FIXES
	if (keymap[DIK_F3])
	{
		if (gfCurrentLevel == 2 || gfCurrentLevel == 6 || gfCurrentLevel == 13 || gfCurrentLevel == 21 || gfCurrentLevel == 27)
			skipped_level = 1;

		gfRequiredStartPos = 0;
		gfLevelComplete = gfCurrentLevel + 1;
	}
#endif
}

void LaraCheat(ITEM_INFO* item, COLL_INFO* coll)
{
	lara_item->hit_points = 1000;
	LaraUnderWater(item, coll);

	if (input & IN_WALK && !(input & IN_LOOK))
	{
		lara.water_status = 0;
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
		lara_item->hit_points = cheat_hit_points;
	}
}

void inject_laramisc(bool replace)
{
	INJECT(0x004301F0, LaraCheatGetStuff, replace);
	INJECT(0x00430A50, LaraCheatyBits, replace);
	INJECT(0x004309B0, LaraCheat, replace);
}
