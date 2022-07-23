#include "../tomb4/pch.h"
#include "larafire.h"
#include "lara_states.h"
#include "objects.h"
#include "lara2gun.h"
#include "lara1gun.h"
#include "laraflar.h"

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

void inject_larafire(bool replace)
{
	INJECT(0x0042DDC0, CheckForHoldingState, replace);
	INJECT(0x0042DDF0, InitialiseNewWeapon, replace);
}
