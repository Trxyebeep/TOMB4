#include "../tomb4/pch.h"
#include "lara2gun.h"
#include "objects.h"
#include "larafire.h"
#include "sound.h"

static PISTOL_DEF PistolTable[4] =
{
	{ LARA, 0, 0, 0, 0 },
	{ PISTOLS_ANIM, 4, 5, 13, 24 },
	{ SIXSHOOTER_ANIM , 7, 8, 15, 29 },
	{ UZI_ANIM, 4, 5, 13, 24 }
};

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

static void set_arm_info(LARA_ARM* arm, long frame)
{
	PISTOL_DEF* p;
	long anim_base;

	p = &PistolTable[lara.gun_type];
	anim_base = objects[p->ObjectNum].anim_index;

	if (frame >= p->Draw1Anim)
	{
		if (frame >= p->Draw2Anim)
		{
			if (frame < p->RecoilAnim)
				anim_base += 2;
			else
				anim_base += 3;
		}
		else
			anim_base++;
	}

	arm->anim_number = (short)anim_base;
	arm->frame_number = (short)frame;
	arm->frame_base = anims[anim_base].frame_ptr;
}

void ready_pistols(long weapon_type)
{
	lara.gun_status = LG_READY;
	lara.left_arm.x_rot = 0;
	lara.left_arm.y_rot = 0;
	lara.left_arm.z_rot = 0;
	lara.right_arm.x_rot = 0;
	lara.right_arm.y_rot = 0;
	lara.right_arm.z_rot = 0;
	lara.right_arm.frame_number = 0;
	lara.left_arm.frame_number = 0;
	lara.target = 0;
	lara.right_arm.lock = 0;
	lara.left_arm.lock = 0;
	lara.right_arm.frame_base = objects[WeaponObject(weapon_type)].frame_base;
	lara.left_arm.frame_base = lara.right_arm.frame_base;
}

void draw_pistol_meshes(long weapon_type)
{
	long mesh_index;

	mesh_index = objects[WeaponObjectMesh(weapon_type)].mesh_index;
	lara.holster = LARA_HOLSTERS;
	lara.mesh_ptrs[LM_RHAND] = meshes[mesh_index + LM_RHAND * 2];

	if (weapon_type != WEAPON_REVOLVER)
		lara.mesh_ptrs[LM_LHAND] = meshes[mesh_index + LM_LHAND * 2];
}

void draw_pistols(long weapon_type)
{
	PISTOL_DEF* p;
	short ani;

	ani = lara.left_arm.frame_number + 1;
	p = &PistolTable[lara.gun_type];

	if (ani < p->Draw1Anim || ani > p->RecoilAnim - 1)
		ani = p->Draw1Anim;
	else if (ani == p->Draw2Anim)
	{
		draw_pistol_meshes(weapon_type);
		SoundEffect(SFX_LARA_DRAW, &lara_item->pos, SFX_DEFAULT);
	}
	else if (ani == p->RecoilAnim - 1)
	{
		ready_pistols(weapon_type);
		ani = 0;
	}

	set_arm_info(&lara.right_arm, ani);
	set_arm_info(&lara.left_arm, ani);
}

void undraw_pistols(long weapon_type)
{
	PISTOL_DEF* p;
	short anil, anir;

	p = &PistolTable[lara.gun_type];
	anil = lara.left_arm.frame_number;

	if (lara.left_arm.frame_number >= p->RecoilAnim)
		anil = p->Draw1Anim2;
	else if (lara.left_arm.frame_number > 0 && lara.left_arm.frame_number < p->Draw1Anim)
	{
		lara.left_arm.x_rot -= lara.left_arm.x_rot / anil;
		lara.left_arm.y_rot -= lara.left_arm.y_rot / anil;
		anil--;
	}
	else if (!lara.left_arm.frame_number)
	{
		lara.left_arm.x_rot = 0;
		lara.left_arm.y_rot = 0;
		lara.left_arm.z_rot = 0;
		anil = p->RecoilAnim - 1;
	}
	else if (lara.left_arm.frame_number > p->Draw1Anim)
	{
		anil--;

		if (anil == p->Draw2Anim - 1)
		{
			undraw_pistol_mesh_left(weapon_type);
			SoundEffect(SFX_LARA_HOLSTER, &lara_item->pos, SFX_DEFAULT);
		}
	}

	set_arm_info(&lara.left_arm, anil);
	anir = lara.right_arm.frame_number;

	if (lara.right_arm.frame_number >= p->RecoilAnim)
		anir = p->Draw1Anim2;
	else if (lara.right_arm.frame_number > 0 && lara.right_arm.frame_number < p->Draw1Anim)
	{
		lara.right_arm.x_rot -= lara.right_arm.x_rot / anir;
		lara.right_arm.y_rot -= lara.right_arm.y_rot / anir;
		anir--;
	}
	else if (!lara.right_arm.frame_number)
	{
		lara.right_arm.z_rot = 0;
		lara.right_arm.y_rot = 0;
		lara.right_arm.x_rot = 0;
		anir = p->RecoilAnim - 1;
	}
	else if (lara.right_arm.frame_number > p->Draw1Anim)
	{
		anir--;

		if (anir == p->Draw2Anim - 1)
		{
			undraw_pistol_mesh_right(weapon_type);
			SoundEffect(SFX_LARA_HOLSTER, &lara_item->pos, SFX_DEFAULT);
		}
	}

	set_arm_info(&lara.right_arm, anir);

	if (anil == p->Draw1Anim && anir == p->Draw1Anim)
	{
		lara.gun_status = LG_NO_ARMS;
		lara.left_arm.frame_number = 0;
		lara.right_arm.frame_number = 0;
		lara.target = 0;
		lara.right_arm.lock = 0;
		lara.left_arm.lock = 0;
	}

	if (!(input & IN_LOOK))
	{
		lara.head_x_rot = (lara.left_arm.x_rot + lara.right_arm.x_rot) >> 2;
		lara.torso_x_rot = lara.head_x_rot;
		lara.head_y_rot = (lara.left_arm.y_rot + lara.right_arm.y_rot) >> 2;
		lara.torso_y_rot = lara.head_y_rot;
	}
}

void inject_lara2gun(bool replace)
{
	INJECT(0x0042BB70, undraw_pistol_mesh_left, replace);
	INJECT(0x0042BBB0, undraw_pistol_mesh_right, replace);
	INJECT(0x0042B7C0, set_arm_info, replace);
	INJECT(0x0042BAB0, ready_pistols, replace);
	INJECT(0x0042BB20, draw_pistol_meshes, replace);
	INJECT(0x0042B720, draw_pistols, replace);
	INJECT(0x0042B840, undraw_pistols, replace);
}
