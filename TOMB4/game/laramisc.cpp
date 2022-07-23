#include "../tomb4/pch.h"
#include "laramisc.h"
#include "objects.h"
#include "laraswim.h"
#include "lara_states.h"
#include "control.h"
#include "collide.h"
#ifdef GENERAL_FIXES
#include "newinv.h"
#include "../tomb4/tomb4.h"
#endif
#include "sound.h"
#include "effects.h"
#include "rope.h"

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

void InitialiseLaraLoad(short item_number)
{
	lara.item_number = item_number;
	lara_item = &items[item_number];
}

void InitialiseLaraAnims(ITEM_INFO* item)
{
	if (room[item->room_number].flags & ROOM_UNDERWATER)
	{
		item->anim_number = ANIM_TREAD;
		item->frame_number = anims[ANIM_TREAD].frame_base;
		item->current_anim_state = AS_TREAD;
		item->goal_anim_state = AS_TREAD;
		lara.water_status = LW_UNDERWATER;
		item->fallspeed = 0;
	}
	else
	{
		item->anim_number = ANIM_STOP;
		item->frame_number = anims[ANIM_STOP].frame_base;
		item->current_anim_state = AS_STOP;
		item->goal_anim_state = AS_STOP;
		lara.water_status = LW_ABOVE_WATER;
	}
}

void LaraInitialiseMeshes()
{
	for (int i = 0; i < 15; i++)
	{
		meshes[objects[LARA].mesh_index + i * 2] = meshes[objects[LARA_SKIN].mesh_index + i * 2];
		lara.mesh_ptrs[i] = meshes[objects[LARA].mesh_index + i * 2];
	}

	if (lara.gun_type == WEAPON_GRENADE)
		lara.back_gun = WEAPON_GRENADE;
	else if (lara.shotgun_type_carried)
		lara.back_gun = WEAPON_UZI;
	else if (lara.grenade_type_carried)
		lara.back_gun = WEAPON_GRENADE;

	lara.gun_status = LG_NO_ARMS;
	lara.left_arm.frame_number = 0;
	lara.right_arm.frame_number = 0;
	lara.target = 0;
	lara.right_arm.lock = 0;
	lara.left_arm.lock = 0;
}

void AnimateLara(ITEM_INFO* item)
{
	ANIM_STRUCT* anim;
	short* cmd;
	long speed;
	short type, num;

	item->frame_number++;
	anim = &anims[item->anim_number];

	if (anim->number_changes > 0)
	{
		if (GetChange(item, anim))
		{
			anim = &anims[item->anim_number];
			item->current_anim_state = anim->current_anim_state;
		}
	}

	if (item->frame_number > anim->frame_end)
	{
		if (anim->number_commands > 0)
		{
			cmd = &commands[anim->command_index];

			for (int i = anim->number_commands; i > 0; i--)
			{
				switch (*cmd++)
				{
				case ACMD_SETPOS:
					TranslateItem(item, *cmd, cmd[1], cmd[2]);
					UpdateLaraRoom(item, -381);
					cmd += 3;
					break;

				case ACMD_JUMPVEL:
					item->fallspeed = cmd[0];
					item->speed = cmd[1];
					item->gravity_status = 1;

					if (lara.calc_fallspeed)
					{
						item->fallspeed = lara.calc_fallspeed;
						lara.calc_fallspeed = 0;
					}

					cmd += 2;
					break;

				case ACMD_FREEHANDS:

					if (lara.gun_status != LG_FLARE)
						lara.gun_status = LG_NO_ARMS;

					break;

				case ACMD_PLAYSFX:
				case ACMD_FLIPEFFECT:
					cmd += 2;
					break;
				}
			}
		}

		item->anim_number = anim->jump_anim_num;
		item->frame_number = anim->jump_frame_num;
		anim = &anims[item->anim_number];
		item->current_anim_state = anim->current_anim_state;
	}

	if (anim->number_commands > 0)
	{
		cmd = &commands[anim->command_index];

		for (int i = anim->number_commands; i > 0; i--)
		{
			switch (*cmd++)
			{
			case ACMD_SETPOS:
				cmd += 3;
				break;

			case ACMD_JUMPVEL:
				cmd += 2;
				break;

			case ACMD_PLAYSFX:
				num = cmd[1] & 0x3FFF;
				type = cmd[1] & 0xC000;

				if (item->frame_number == cmd[0])
				{
					if (type == SFX_LANDANDWATER ||
						(type == SFX_LANDONLY && (lara.water_surface_dist >= 0 || lara.water_surface_dist == NO_HEIGHT)) ||
						(type == SFX_WATERONLY && lara.water_surface_dist < 0 && lara.water_surface_dist != NO_HEIGHT))
						SoundEffect(num, &item->pos, SFX_ALWAYS);
				}

				cmd += 2;
				break;

			case ACMD_FLIPEFFECT:

				if (item->frame_number == *cmd)
				{
					FXType = cmd[1] & 0xC000;
					effect_routines[cmd[1] & 0x3FFF](item);
				}

				cmd += 2;
				break;
			}
		}
	}

	if (item->gravity_status)
	{
		speed = anim->velocity + anim->acceleration * (item->frame_number - anim->frame_base - 1);
		item->speed -= speed >> 16;
		speed += anim->acceleration;
		item->speed += speed >> 16;
		item->fallspeed += item->fallspeed < 128 ? 6 : 1;
		item->pos.y_pos += item->fallspeed;
	}
	else
	{
		speed = anim->velocity;

		if (anim->acceleration)
			speed += anim->acceleration * (item->frame_number - anim->frame_base);

		item->speed = speed >> 16;
	}

	if (lara.RopePtr != -1)
		AlignLaraToRope(item);

	if (!lara.IsMoving)
	{
		item->pos.x_pos += (item->speed * phd_sin(lara.move_angle)) >> W2V_SHIFT;
		item->pos.z_pos += (item->speed * phd_cos(lara.move_angle)) >> W2V_SHIFT;
	}
}

void inject_laramisc(bool replace)
{
	INJECT(0x004301F0, LaraCheatGetStuff, replace);
	INJECT(0x00430A50, LaraCheatyBits, replace);
	INJECT(0x004309B0, LaraCheat, replace);
	INJECT(0x00430EB0, InitialiseLaraLoad, replace);
	INJECT(0x00430EE0, InitialiseLaraAnims, replace);
	INJECT(0x00430140, LaraInitialiseMeshes, replace);
	INJECT(0x00430B60, AnimateLara, replace);
}
