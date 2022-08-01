#include "../tomb4/pch.h"
#include "larafire.h"
#include "lara_states.h"
#include "objects.h"
#include "lara2gun.h"
#include "lara1gun.h"
#include "laraflar.h"
#include "delstuff.h"
#include "draw.h"
#include "../specific/3dmath.h"
#include "control.h"

static short HoldStates[] =
{
	AS_WALK,
#ifndef GENERAL_FIXES
	AS_RUN,
#endif
	AS_STOP,
	AS_POSE,
	AS_TURN_R,
	AS_TURN_L,
	AS_BACK,
	AS_FASTTURN,
	AS_STEPLEFT,
	AS_STEPRIGHT,
	AS_WADE,
#ifndef GENERAL_FIXES
	AS_PICKUP,
	AS_SWITCHON,
	AS_SWITCHOFF,
#endif
	AS_DUCK,
	AS_DUCKROTL,
	AS_DUCKROTR,
	-1
};

static long CheckForHoldingState(long state)
{
	short* holds;

	holds = HoldStates;

	while (*holds >= 0)
	{
		if (state == *holds)
			return 1;

		holds++;
	}

	return 0;
}

void InitialiseNewWeapon()
{
	lara.right_arm.frame_number = 0;
	lara.left_arm.frame_number = 0;
	lara.left_arm.z_rot = 0;
	lara.left_arm.y_rot = 0;
	lara.left_arm.x_rot = 0;
	lara.right_arm.z_rot = 0;
	lara.right_arm.y_rot = 0;
	lara.right_arm.x_rot = 0;
	lara.target = 0;
	lara.right_arm.lock = 0;
	lara.left_arm.lock = 0;
	lara.right_arm.flash_gun = 0;
	lara.left_arm.flash_gun = 0;

	switch (lara.gun_type)
	{
	case WEAPON_PISTOLS:
	case WEAPON_UZI:
		lara.left_arm.frame_base = objects[PISTOLS_ANIM].frame_base;
		lara.right_arm.frame_base = objects[PISTOLS_ANIM].frame_base;

		if (lara.gun_status != LG_NO_ARMS)
			draw_pistol_meshes(lara.gun_type);

		break;

	case WEAPON_REVOLVER:
	case WEAPON_SHOTGUN:
	case WEAPON_GRENADE:
		lara.left_arm.frame_base = objects[WeaponObject(lara.gun_type)].frame_base;
		lara.right_arm.frame_base = objects[WeaponObject(lara.gun_type)].frame_base;

		if (lara.gun_status != LG_NO_ARMS)
			draw_shotgun_meshes(lara.gun_type);

		break;

	case WEAPON_FLARE:
		lara.left_arm.frame_base = objects[FLARE_ANIM].frame_base;
		lara.right_arm.frame_base = objects[FLARE_ANIM].frame_base;

		if (lara.gun_status != LG_NO_ARMS)
			draw_flare_meshes();

		break;

	default:
		lara.left_arm.frame_base = anims[lara_item->anim_number].frame_ptr;
		lara.right_arm.frame_base = anims[lara_item->anim_number].frame_ptr;
		break;
	}
}

static void find_target_point(ITEM_INFO* item, GAME_VECTOR* target)
{
	long x, y, z, c, s;
	short* bounds;

	bounds = GetBestFrame(item);
	x = (bounds[0] + bounds[1]) >> 1;
	y = (bounds[2] + (bounds[3] - bounds[2]) / 3);
	z = (bounds[4] + bounds[5]) >> 1;
	s = phd_sin(item->pos.y_rot);
	c = phd_cos(item->pos.y_rot);
	target->x = item->pos.x_pos + ((x * c + z * s) >> W2V_SHIFT);
	target->y = item->pos.y_pos + y;
	target->z = item->pos.z_pos + ((z * c - x * s) >> W2V_SHIFT);
	target->room_number = item->room_number;
}

void LaraTargetInfo(WEAPON_INFO* winfo)
{
	GAME_VECTOR src, target;
	short ang[2];

	if (!lara.target)
	{
		lara.right_arm.lock = 0;
		lara.left_arm.lock = 0;
		lara.target_angles[1] = 0;
		lara.target_angles[0] = 0;
		return;
	}

	src.x = 0;
	src.y = 0;
	src.z = 0;
	GetLaraJointPos((PHD_VECTOR*)&src, 11);
	src.x = lara_item->pos.x_pos;
	src.z = lara_item->pos.z_pos;
	src.room_number = lara_item->room_number;
	find_target_point(lara.target, &target);
	phd_GetVectorAngles(target.x - src.x, target.y - src.y, target.z - src.z, ang);
	ang[0] -= lara_item->pos.y_rot;
	ang[1] -= lara_item->pos.x_rot;

	if (LOS(&src, &target))
	{
		if (ang[0] >= winfo->lock_angles[0] && ang[0] <= winfo->lock_angles[1] &&
			ang[1] >= winfo->lock_angles[2] && ang[1] <= winfo->lock_angles[3])
		{
			lara.left_arm.lock = 1;
			lara.right_arm.lock = 1;
			lara.target_angles[0] = ang[0];
			lara.target_angles[1] = ang[1];
			return;
		}

		if (lara.left_arm.lock)
		{
			if (ang[0] < winfo->left_angles[0] || ang[0] > winfo->left_angles[1] ||
				ang[1] < winfo->left_angles[2] || ang[1] > winfo->left_angles[3])
				lara.left_arm.lock = 0;
		}

		if (lara.right_arm.lock)
		{
			if (ang[0] < winfo->right_angles[0] || ang[0] > winfo->right_angles[1] ||
				ang[1] < winfo->left_angles[2] || ang[1] > winfo->left_angles[3])
			{
				lara.right_arm.lock = 0;
				lara.target_angles[0] = ang[0];
				lara.target_angles[1] = ang[1];
				return;
			}
		}
	}
	else
	{
		lara.right_arm.lock = 0;
		lara.left_arm.lock = 0;
	}

	lara.target_angles[0] = ang[0];
	lara.target_angles[1] = ang[1];
}

short* get_current_ammo_pointer(long weapon_type)
{
	short* ammo;

	switch (weapon_type)
	{
	case WEAPON_REVOLVER:
		ammo = &lara.num_revolver_ammo;
		break;

	case WEAPON_UZI:
		ammo = &lara.num_uzi_ammo;
		break;

	case WEAPON_SHOTGUN:

		if (lara.shotgun_type_carried & 8)
			ammo = &lara.num_shotgun_ammo1;
		else
			ammo = &lara.num_shotgun_ammo2;

		break;

	case WEAPON_GRENADE:

		if (lara.grenade_type_carried & 8)
			ammo = &lara.num_grenade_ammo1;
		else if (lara.grenade_type_carried & 0x10)
			ammo = &lara.num_grenade_ammo2;
		else
			ammo = &lara.num_grenade_ammo3;

		break;

	case WEAPON_CROSSBOW:

		if (lara.crossbow_type_carried & 8)
			ammo = &lara.num_crossbow_ammo1;
		else if (lara.crossbow_type_carried & 0x10)
			ammo = &lara.num_crossbow_ammo2;
		else
			ammo = &lara.num_crossbow_ammo3;

		break;

	default:
		ammo = &lara.num_pistols_ammo;
		break;
	}

	return ammo;
}

void inject_larafire(bool replace)
{
	INJECT(0x0042DDC0, CheckForHoldingState, replace);
	INJECT(0x0042DDF0, InitialiseNewWeapon, replace);
	INJECT(0x0042E4A0, find_target_point, replace);
	INJECT(0x0042DF30, LaraTargetInfo, replace);
	INJECT(0x0042F480, get_current_ammo_pointer, replace);
}
