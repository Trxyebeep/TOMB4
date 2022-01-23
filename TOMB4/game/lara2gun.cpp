#include "../tomb4/pch.h"
#include "lara2gun.h"
#include "objects.h"
#include "larafire.h"

void undraw_pistol_mesh_left(long weapon_type)
{
	if (weapon_type != WEAPON_REVOLVER)
	{
		WeaponObject(weapon_type);
		lara.mesh_ptrs[LM_LHAND] = meshes[objects[LARA].mesh_index + LM_LHAND * 2];

		if (weapon_type == WEAPON_PISTOLS)
			lara.holster = LARA_HOLSTERS_PISTOLS;
#ifdef GENERAL_FIXES
		else if (weapon_type == WEAPON_UZI)
			lara.holster = LARA_HOLSTERS_UZIS;
#endif
	}
}

void undraw_pistol_mesh_right(long weapon_type)
{
	WeaponObject(weapon_type);
	lara.mesh_ptrs[LM_RHAND] = meshes[objects[LARA].mesh_index + LM_RHAND * 2];

	if (weapon_type == WEAPON_PISTOLS)
		lara.holster = LARA_HOLSTERS_PISTOLS;
#ifdef GENERAL_FIXES
	else if (weapon_type == WEAPON_UZI)
		lara.holster = LARA_HOLSTERS_UZIS;
	else if (weapon_type == WEAPON_REVOLVER)
		lara.holster = LARA_HOLSTERS_SIXSHOOTER;
#endif
}

void inject_lara2gun(bool replace)
{
	INJECT(0x0042BB70, undraw_pistol_mesh_left, replace);
	INJECT(0x0042BBB0, undraw_pistol_mesh_right, replace);
}
