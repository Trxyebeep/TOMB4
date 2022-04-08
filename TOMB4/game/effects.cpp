#include "../tomb4/pch.h"
#include "effects.h"
#include "../specific/polyinsert.h"
#include "../specific/function_table.h"
#include "sound.h"
#include "deltapak.h"
#include "objects.h"
#include "items.h"
#include "lot.h"
#include "hair.h"
#include "scarab.h"
#include "tomb4fx.h"
#include "footprnt.h"
#include "effect2.h"

void(*effect_routines[47])(ITEM_INFO* item) =
{
	turn180_effect,
	floor_shake_effect,
	PoseidonSFX,
	LaraBubbles,
	finish_level_effect,
	ActivateCamera,
	ActivateKey,
	RubbleFX,
	SwapCrowbar,
	void_effect,
	SoundFlipEffect,
	ExplosionFX,
	lara_hands_free,
	void_effect,
	draw_right_gun,
	draw_left_gun,
	shoot_right_gun,
	shoot_left_gun,
	swap_meshes_with_meshswap1,
	swap_meshes_with_meshswap2,
	swap_meshes_with_meshswap3,
	invisibility_on,
	invisibility_off,
	void_effect,
	void_effect,
	void_effect,
	reset_hair,
	void_effect,
	SetFog,
	GhostTrap,
	LaraLocation,
	ClearScarabsPatch,
	AddFootPrint,
	void_effect,
	void_effect,
	void_effect,
	void_effect,
	void_effect,
	void_effect,
	void_effect,
	void_effect,
	void_effect,
	void_effect,
	MeshSwapToPour,
	MeshSwapFromPour,
	LaraLocationPad,
	KillActiveBaddies,
};

void SetFog(ITEM_INFO* item)
{
	GlobalFogOff = 0;

	if (IsVolumetric())
	{
		if (TriggerTimer == 100)
			GlobalFogOff = 1;
		else
		{
			gfFog.r = CLRR(FogTableColor[TriggerTimer]);
			gfFog.g = CLRG(FogTableColor[TriggerTimer]);
			gfFog.b = CLRB(FogTableColor[TriggerTimer]);
			savegame.fog_colour.r = gfFog.r;
			savegame.fog_colour.g = gfFog.g;
			savegame.fog_colour.b = gfFog.b;
			SetFogColor(gfFog.r, gfFog.g, gfFog.b);
		}
	}

	flipeffect = -1;
}

void finish_level_effect(ITEM_INFO* item)
{
	gfLevelComplete = gfCurrentLevel + 1;
}

void turn180_effect(ITEM_INFO* item)
{
	item->pos.y_rot += 0x8000;
	item->pos.x_rot = -item->pos.x_rot;
}

void floor_shake_effect(ITEM_INFO* item)
{
	long x, y, z, dist;

	x = item->pos.x_pos - camera.pos.x;
	y = item->pos.y_pos - camera.pos.y;
	z = item->pos.z_pos - camera.pos.z;

	if ((ABS(x) < 16384) && (ABS(y) < 16384) && (ABS(z) < 16384))
	{
		dist = (SQUARE(x) + SQUARE(y) + SQUARE(z)) / 256;
		camera.bounce = ((SQUARE(1024) - dist) * 100) / SQUARE(1024);
	}
}

void SoundFlipEffect(ITEM_INFO* item)
{
	SoundEffect(TriggerTimer, 0, SFX_DEFAULT);
	flipeffect = -1;
}

void RubbleFX(ITEM_INFO* item)
{
	ITEM_INFO* eq;

	eq = find_a_fucking_item(EARTHQUAKE);

	if (eq)
	{
		AddActiveItem(eq - items);
		eq->status = ITEM_ACTIVE;
		eq->flags |= IFL_CODEBITS;
	}
	else
		camera.bounce = -150;

	flipeffect = -1;
}

void PoseidonSFX(ITEM_INFO* item)
{
	SoundEffect(SFX_WATER_FLUSHES, 0, SFX_DEFAULT);
	flipeffect = -1;
}

void ActivateCamera(ITEM_INFO* item)
{
	KeyTriggerActive = 2;
}

void ActivateKey(ITEM_INFO* item)
{
	KeyTriggerActive = 1;
}

void SwapCrowbar(ITEM_INFO* item)
{
	short* tmp;

	tmp = meshes[objects[LARA].mesh_index + 2 * LM_RHAND];

	if (lara.mesh_ptrs[LM_RHAND] == tmp)
		lara.mesh_ptrs[LM_RHAND] = meshes[objects[CROWBAR_ANIM].mesh_index + (2 * LM_RHAND)];
	else
		lara.mesh_ptrs[LM_RHAND] = tmp;
}

void ExplosionFX(ITEM_INFO* item)
{
	SoundEffect(SFX_EXPLOSION1, 0, SFX_DEFAULT);
	camera.bounce = -75;
	flipeffect = -1;
}

void LaraLocation(ITEM_INFO* item)
{
	lara.location = TriggerTimer;

	if (TriggerTimer > lara.highest_location)
		lara.highest_location = TriggerTimer;

	flipeffect = -1;
}

void LaraLocationPad(ITEM_INFO* item)
{
	flipeffect = -1;
	lara.locationPad = TriggerTimer;
	lara.location = TriggerTimer;
}

void GhostTrap(ITEM_INFO* item)
{
	ITEM_INFO* wraith;
	short nex;

	nex = next_item_active;

	if (next_item_active != NO_ITEM)
	{
		while (nex != NO_ITEM)
		{
			wraith = &items[nex];

			if (wraith->object_number == WRAITH3 && !wraith->hit_points)
			{
				wraith->hit_points = item - items;
				break;
			}

			nex = wraith->next_active;
		}
	}

	flipeffect = -1;
}

void KillActiveBaddies(ITEM_INFO* item)
{
	ITEM_INFO* target_item;
	short item_num;

	for (item_num = next_item_active; item_num != NO_ITEM; item_num = target_item->next_active)
	{
		target_item = &items[item_num];

		if (objects[target_item->object_number].intelligent)
		{
			target_item->status = ITEM_INVISIBLE;

			if (item != ((void*)0xABCDEF))
			{
				RemoveActiveItem(item_num);
				DisableBaddieAI(item_num);
				target_item->flags |= IFL_INVISIBLE;
			}
		}
	}

	flipeffect = -1;
}

void lara_hands_free(ITEM_INFO* item)
{
	lara.gun_status = LG_NO_ARMS;
}

void draw_right_gun(ITEM_INFO* item)
{
	short* tmp;

	tmp = lara.mesh_ptrs[LM_RTHIGH];
	lara.mesh_ptrs[LM_RTHIGH] = meshes[objects[PISTOLS_ANIM].mesh_index + LM_RTHIGH * 2];
	meshes[objects[PISTOLS_ANIM].mesh_index + LM_RTHIGH * 2] = tmp;

	tmp = lara.mesh_ptrs[LM_RHAND];
	lara.mesh_ptrs[LM_RHAND] = meshes[objects[PISTOLS_ANIM].mesh_index + LM_RHAND * 2];
	meshes[objects[PISTOLS_ANIM].mesh_index + LM_RHAND * 2] = tmp;
}

void draw_left_gun(ITEM_INFO* item)
{
	short* tmp;

	tmp = lara.mesh_ptrs[LM_LTHIGH];
	lara.mesh_ptrs[LM_LTHIGH] = meshes[objects[PISTOLS_ANIM].mesh_index + LM_LTHIGH * 2];
	meshes[objects[PISTOLS_ANIM].mesh_index + LM_LTHIGH * 2] = tmp;

	tmp = lara.mesh_ptrs[LM_LHAND];
	lara.mesh_ptrs[LM_LHAND] = meshes[objects[PISTOLS_ANIM].mesh_index + LM_LHAND * 2];
	meshes[objects[PISTOLS_ANIM].mesh_index + LM_LHAND * 2] = tmp;
}

void shoot_right_gun(ITEM_INFO* item)
{
	lara.right_arm.flash_gun = 3;
}

void shoot_left_gun(ITEM_INFO* item)
{
	lara.left_arm.flash_gun = 3;
}

void swap_meshes_with_meshswap1(ITEM_INFO* item)
{
	OBJECT_INFO* obj;
	short* tmp;

	obj = &objects[item->object_number];

	for (int i = 0; i < obj->nmeshes; i++)
	{
		tmp = meshes[obj->mesh_index + i * 2];
		meshes[obj->mesh_index + i] = meshes[objects[MESHSWAP1].mesh_index + i * 2];
		meshes[objects[MESHSWAP1].mesh_index + i * 2] = tmp;
	}
}

void swap_meshes_with_meshswap2(ITEM_INFO* item)
{
	OBJECT_INFO* obj;
	short* tmp;

	obj = &objects[item->object_number];

	for (int i = 0; i < obj->nmeshes; i++)
	{
		tmp = meshes[obj->mesh_index + i * 2];
		meshes[obj->mesh_index + i] = meshes[objects[MESHSWAP2].mesh_index + i * 2];
		meshes[objects[MESHSWAP2].mesh_index + i * 2] = tmp;
	}
}

void swap_meshes_with_meshswap3(ITEM_INFO* item)
{
	OBJECT_INFO* obj;
	short* tmp;

	obj = &objects[item->object_number];

	for (int i = 0; i < obj->nmeshes; i++)
	{
		tmp = meshes[obj->mesh_index + i * 2];
		meshes[obj->mesh_index + i] = meshes[objects[MESHSWAP3].mesh_index + i * 2];

		if (item == lara_item)
			lara.mesh_ptrs[i] = meshes[objects[MESHSWAP3].mesh_index + i * 2];

		meshes[objects[MESHSWAP3].mesh_index + i * 2] = tmp;
	}
}

void invisibility_on(ITEM_INFO* item)
{
	item->status = ITEM_INVISIBLE;
}

void invisibility_off(ITEM_INFO* item)
{
	item->status = ITEM_ACTIVE;
}

void reset_hair(ITEM_INFO* item)
{
	InitialiseHair();
}

void ClearScarabsPatch(ITEM_INFO* item)
{
	ClearScarabs();
}

void MeshSwapToPour(ITEM_INFO* item)
{
	lara.mesh_ptrs[LM_LHAND] = meshes[objects[item->item_flags[2]].mesh_index + LM_LHAND * 2];
}

void MeshSwapFromPour(ITEM_INFO* item)
{
	lara.mesh_ptrs[LM_LHAND] = meshes[objects[LARA_SKIN].mesh_index + LM_LHAND * 2];
}

void void_effect(ITEM_INFO* item)
{

}

void WaterFall(short item_number)
{
	ITEM_INFO* item;
	long dx, dy, dz;

	item = &items[item_number];
	dx = item->pos.x_pos - lara_item->pos.x_pos;
	dy = item->pos.y_pos - lara_item->pos.y_pos;
	dz = item->pos.z_pos - lara_item->pos.z_pos;

	if (dx >= -0x4000 && dx <= 0x4000 && dz >= -0x4000 && dz <= 0x4000 && dy >= -0x4000 && dy <= 0x4000)
	{
		//empty func call here

		if (!(wibble & 0xC))
		{
			dx = (136 * phd_sin(item->pos.y_rot)) >> 12;
			dz = (136 * phd_cos(item->pos.y_rot)) >> 12;
			TriggerWaterfallMist(item->pos.x_pos + dx, item->pos.y_pos, item->pos.z_pos + dz, item->pos.y_rot >> 4);
		}

		SoundEffect(SFX_WATERFALL_LOOP, &item->pos, 0);
	}
}

void inject_effects(bool replace)
{
	INJECT(0x00437AB0, SetFog, replace);
	INJECT(0x00437650, finish_level_effect, replace);
	INJECT(0x00437660, turn180_effect, replace);
	INJECT(0x00437680, floor_shake_effect, replace);
	INJECT(0x00437720, SoundFlipEffect, replace);
	INJECT(0x00437740, RubbleFX, replace);
	INJECT(0x004377B0, PoseidonSFX, replace);
	INJECT(0x004377D0, ActivateCamera, replace);
	INJECT(0x004377E0, ActivateKey, replace);
	INJECT(0x004377F0, SwapCrowbar, replace);
	INJECT(0x00437820, ExplosionFX, replace);
	INJECT(0x00437850, LaraLocation, replace);
	INJECT(0x00437880, LaraLocationPad, replace);
	INJECT(0x004378A0, GhostTrap, replace);
	INJECT(0x00437930, KillActiveBaddies, replace);
	INJECT(0x004379E0, lara_hands_free, replace);
	INJECT(0x004379F0, draw_right_gun, replace);
	INJECT(0x00437A40, draw_left_gun, replace);
	INJECT(0x00437A90, shoot_right_gun, replace);
	INJECT(0x00437AA0, shoot_left_gun, replace);
	INJECT(0x00437B60, swap_meshes_with_meshswap1, replace);
	INJECT(0x00437BC0, swap_meshes_with_meshswap2, replace);
	INJECT(0x00437C20, swap_meshes_with_meshswap3, replace);
	INJECT(0x00437CB0, invisibility_on, replace);
	INJECT(0x00437CC0, invisibility_off, replace);
	INJECT(0x00437CE0, reset_hair, replace);
	INJECT(0x00437CF0, ClearScarabsPatch, replace);
	INJECT(0x00437D00, MeshSwapToPour, replace);
	INJECT(0x00437D30, MeshSwapFromPour, replace);
	INJECT(0x00437530, WaterFall, replace);
}
