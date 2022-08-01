#include "../tomb4/pch.h"
#include "lara1gun.h"
#include "traps.h"
#include "objects.h"
#include "larafire.h"
#include "box.h"
#include "items.h"
#include "delstuff.h"
#include "control.h"
#include "sound.h"
#include "../specific/function_stubs.h"
#include "tomb4fx.h"
#include "lara2gun.h"
#include "effect2.h"
#include "sphere.h"
#include "switch.h"

void DoGrenadeDamageOnBaddie(ITEM_INFO* baddie, ITEM_INFO* item)
{
	if (baddie->flags & 0x8000)
		return;

	if (baddie == lara_item && lara_item->hit_points > 0)
	{
		lara_item->hit_points -= 50;

		if (!(room[item->room_number].flags & ROOM_UNDERWATER) && lara_item->hit_points <= 50)
			LaraBurn();
	}
	else if (!item->item_flags[2])
	{
		baddie->hit_status = 1;

		if ((!objects[baddie->object_number].undead || baddie->object_number == SKELETON || baddie->object_number == MUMMY) &&
			baddie->object_number != AHMET)
		{
			HitTarget(baddie, 0, 30, 1);

			if (baddie != lara_item)
			{
				savegame.Game.AmmoHits++;

				if (baddie->hit_points <= 0)
				{
					savegame.Level.Kills++;

					if (baddie->object_number != BABOON_NORMAL && baddie->object_number != BABOON_INV && baddie->object_number != BABOON_SILENT)
						CreatureDie(baddie - items, 1);
				}
			}
		}
	}
}

void FireCrossbow(PHD_3DPOS* pos)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	PHD_VECTOR vec;
	short* ammo;
	long h;
	short item_number;

	ammo = get_current_ammo_pointer(WEAPON_CROSSBOW);

	if (!*ammo)
		return;

	lara.has_fired = 1;
	item_number = CreateItem();

	if (item_number != NO_ITEM)
	{
		item = &items[item_number];
		item->object_number = CROSSBOW_BOLT;
		item->shade = -0x3DF0;

		if (pos)
		{
			item->room_number = lara_item->room_number;
			item->pos.x_pos = pos->x_pos;
			item->pos.y_pos = pos->y_pos;
			item->pos.z_pos = pos->z_pos;
			InitialiseItem(item_number);
			item->pos.x_rot = pos->x_rot;
			item->pos.y_rot = pos->y_rot;
			item->pos.z_rot = pos->z_rot;
		}
		else
		{
			if (*ammo != -1)
				--*ammo;

			vec.x = 0;
			vec.y = 228;
			vec.z = 32;
			GetLaraJointPos(&vec, 11);
			item->room_number = lara_item->room_number;
			floor = GetFloor(vec.x, vec.y, vec.z, &item->room_number);
			h = GetHeight(floor, vec.x, vec.y, vec.z);

			if (h >= vec.y)
			{
				item->pos.x_pos = vec.x;
				item->pos.y_pos = vec.y;
				item->pos.z_pos = vec.z;
			}
			else
			{
				item->pos.x_pos = lara_item->pos.x_pos;
				item->pos.y_pos = vec.y;
				item->pos.z_pos = lara_item->pos.z_pos;
				item->room_number = lara_item->room_number;
			}

			InitialiseItem(item_number);
			item->pos.x_rot = lara.left_arm.x_rot + lara_item->pos.x_rot;
			item->pos.y_rot = lara.left_arm.y_rot + lara_item->pos.y_rot;
			item->pos.z_rot = 0;

			if (!lara.left_arm.lock)
			{
				item->pos.x_rot += lara.torso_x_rot;
				item->pos.y_rot += lara.torso_y_rot;
			}
		}

		item->speed = 512;
		AddActiveItem(item_number);

		if (lara.crossbow_type_carried & 8)
			item->item_flags[0] = 1;
		else if (lara.crossbow_type_carried & 0x10)
			item->item_flags[0] = 2;
		else
			item->item_flags[0] = 3;

		SoundEffect(SFX_LARA_CROSSBOW, 0, SFX_DEFAULT);
		savegame.Game.AmmoUsed++;
	}
}

void draw_shotgun_meshes(long weapon_type)
{
	lara.back_gun = WEAPON_NONE;
	lara.mesh_ptrs[LM_RHAND] = meshes[objects[WeaponObjectMesh(weapon_type)].mesh_index + 2 * LM_RHAND];
}

void undraw_shotgun_meshes(long weapon_type)
{
	lara.back_gun = (short)WeaponObject(weapon_type);
	lara.mesh_ptrs[LM_RHAND] = meshes[objects[LARA].mesh_index + 2 * LM_RHAND];
}

void ready_shotgun(long weapon_type)
{
	lara.gun_status = LG_READY;
	lara.target = 0;

	lara.left_arm.x_rot = 0;
	lara.left_arm.y_rot = 0;
	lara.left_arm.z_rot = 0;
	lara.left_arm.frame_number = 0;
	lara.left_arm.lock = 0;
	lara.left_arm.frame_base = objects[WeaponObject(weapon_type)].frame_base;

	lara.right_arm.x_rot = 0;
	lara.right_arm.y_rot = 0;
	lara.right_arm.z_rot = 0;
	lara.right_arm.frame_number = 0;
	lara.right_arm.lock = 0;
	lara.right_arm.frame_base = lara.left_arm.frame_base;
}

void FireShotgun()
{
	PHD_VECTOR pos;
	PHD_VECTOR pos2;
	long fired, scatter;
	short angles[2];
	short dangles[2];

	angles[0] = lara.left_arm.y_rot + lara_item->pos.y_rot;
	angles[1] = lara.left_arm.x_rot;

	if (lara.left_arm.lock)
	{
		angles[0] += lara.torso_y_rot;
		angles[1] += lara.torso_x_rot;
	}

	fired = 0;

	if (lara.shotgun_type_carried & 8)
		scatter = 1820;
	else
		scatter = 5460;

	for (int i = 0; i < 6; i++)
	{
		dangles[0] = short(angles[0] + scatter * (GetRandomControl() - 0x4000) / 0x10000);
		dangles[1] = short(angles[1] + scatter * (GetRandomControl() - 0x4000) / 0x10000);

		if (FireWeapon(WEAPON_SHOTGUN, lara.target, lara_item, dangles))
			fired = 1;
	}

	if (fired)
	{
		pos.x = 0;
		pos.y = 228;
		pos.z = 32;
		GetLaraJointPos(&pos, 11);

		pos2.x = 0;
		pos2.y = 1508;
		pos2.z = 32;
		GetLaraJointPos(&pos2, 11);

		SmokeCountL = 32;
		SmokeWeapon = WEAPON_SHOTGUN;

		if (lara_item->mesh_bits)
		{
			for (int i = 0; i < 7; i++)
				TriggerGunSmoke(pos.x, pos.y, pos.z, pos2.x - pos.x, pos2.y - pos.y, pos2.z - pos.z, 1, SmokeWeapon, 32);

		//	for (int i = 0; i < 12; i++)
				//empty func call here
		}

		lara.right_arm.flash_gun = weapons[WEAPON_SHOTGUN].flash_time;
		SoundEffect(SFX_EXPLOSION1, &lara_item->pos, 0x1400000 | SFX_SETPITCH);
		SoundEffect(weapons[WEAPON_SHOTGUN].sample_num, &lara_item->pos, SFX_DEFAULT);
		savegame.Game.AmmoUsed++;
	}
}

void FireGrenade()
{
	ITEM_INFO* item;
	PHD_VECTOR pos;
	PHD_VECTOR pos2;
	short* ammo;
	long h;
	short item_number;

	ammo = get_current_ammo_pointer(WEAPON_GRENADE);

	if (!*ammo)
		return;

	lara.has_fired = 1;
	item_number = CreateItem();

	if (item_number == NO_ITEM)
		return;

	item = &items[item_number];
	item->shade = -0x3DF0;
	item->object_number = GRENADE;
	item->room_number = lara_item->room_number;

	pos.x = 0;
	pos.y = 276;
	pos.z = 80;
	GetLaraJointPos(&pos, 11);
	item->pos.x_pos = pos.x;
	item->pos.y_pos = pos.y;
	item->pos.z_pos = pos.z;
	h = GetHeight(GetFloor(pos.x, pos.y, pos.z, &item->room_number), pos.x, pos.y, pos.z);

	if (h < pos.y)
	{
		item->pos.x_pos = lara_item->pos.x_pos;
		item->pos.y_pos = pos.y;
		item->pos.z_pos = lara_item->pos.z_pos;
		item->room_number = lara_item->room_number;
	}

	pos2.x = 0;
	pos2.y = 1204;
	pos2.z = 80;
	GetLaraJointPos(&pos2, 11);

	SmokeCountL = 32;
	SmokeWeapon = 5;

	for (int i = 0; i < 5; i++)
		TriggerGunSmoke(pos.x, pos.y, pos.z, pos2.x - pos.x, pos2.y - pos.y, pos2.z - pos.z, 1, SmokeWeapon, SmokeCountL);

	InitialiseItem(item_number);
	item->pos.x_rot = lara.left_arm.x_rot + lara_item->pos.x_rot;
	item->pos.y_rot = lara.left_arm.y_rot + lara_item->pos.y_rot;
	item->pos.z_rot = 0;

	if (!lara.left_arm.lock)
	{
		item->pos.x_rot += lara.torso_x_rot;
		item->pos.y_rot += lara.torso_y_rot;
	}

	item->speed = 128;
	item->current_anim_state = item->pos.x_rot;
	item->fallspeed = (-128 * phd_sin(item->pos.x_rot)) >> W2V_SHIFT;
	item->goal_anim_state = item->pos.y_rot;
	item->required_anim_state = 0;
	item->hit_points = 120;
	AddActiveItem(item_number);

	if (*ammo != -1)
		--*ammo;

	if (lara.grenade_type_carried & 8)
		item->item_flags[0] = 1;
	else if (lara.grenade_type_carried & 0x10)
		item->item_flags[0] = 2;
	else
		item->item_flags[0] = 3;

	savegame.Game.AmmoUsed++;
}

void AnimateShotgun(long weapon_type)
{
	ITEM_INFO* item;
	PHD_VECTOR pos;
	static long m16_firing = 0;
	static long harpoon_fired = 0;

	item = &items[lara.weapon_item];

	if (SmokeCountL)
	{
		if (SmokeWeapon == WEAPON_GRENADE)
		{
			pos.x = 0;
			pos.y = 180;
			pos.z = 80;
		}
		else if (SmokeWeapon == WEAPON_SHOTGUN)
		{
			pos.x = -16;
			pos.y = 228;
			pos.z = 32;
		}

		GetLaraJointPos(&pos, 11);

		if (lara_item->mesh_bits)
			TriggerGunSmoke(pos.x, pos.y, pos.z, 0, 0, 0, 0, SmokeWeapon, SmokeCountL);
	}

	switch (item->current_anim_state)
	{
	case 0:
		m16_firing = 0;

		if (harpoon_fired)
		{
			item->goal_anim_state = 5;
			harpoon_fired = 0;
		}

		else if (lara.water_status == LW_UNDERWATER)
			item->goal_anim_state = 6;
		else if (input & IN_ACTION && !lara.target || lara.left_arm.lock)
			item->goal_anim_state = 2;
		else
			item->goal_anim_state = 4;

		break;

	case 2:

		if (item->frame_number == anims[item->anim_number].frame_base)
		{
			item->goal_anim_state = 4;

			if (lara.water_status != 1 && !harpoon_fired)
			{
				if (input & IN_ACTION)
				{
					if (!lara.target || lara.left_arm.lock)
					{
						if (weapon_type == WEAPON_GRENADE)
							FireGrenade();
						else if (weapon_type == WEAPON_CROSSBOW)
							FireCrossbow(0);
						else
							FireShotgun();

						item->goal_anim_state = 2;
					}
				}
				else if (lara.left_arm.lock)
					item->goal_anim_state = 0;
			}

			if (item->goal_anim_state != 2)
			{
				if (m16_firing)
				{
					SoundEffect(SFX_EXPLOSION1, &lara_item->pos, 0x5000000 | SFX_SETPITCH);
					m16_firing = 0;
				}
			}
		}
		else if (m16_firing)
		{
			SoundEffect(SFX_EXPLOSION1, &lara_item->pos, 0x5000000 | SFX_SETPITCH);
			SoundEffect(SFX_MP5_FIRE, &lara_item->pos, SFX_DEFAULT);
		}
		else if (weapon_type == 4 && !(input & IN_ACTION) && !lara.left_arm.lock)
			item->goal_anim_state = 4;
		if (item->frame_number - anims[item->anim_number].frame_base == 12 && weapon_type == WEAPON_SHOTGUN)
			TriggerGunShell(1, SHOTGUNSHELL, 4);

		break;

	case 6:
		m16_firing = 0;

		if (harpoon_fired)
		{
			item->goal_anim_state = 5;
			harpoon_fired = 0;
		}
		else if (lara.water_status != LW_UNDERWATER)
			item->goal_anim_state = 0;
		else if (input & IN_ACTION && !lara.target || lara.left_arm.lock)
			item->goal_anim_state = 8;
		else
			item->goal_anim_state = 7;

		break;
	}

	AnimateItem(item);
	lara.right_arm.frame_base = anims[item->anim_number].frame_ptr;
	lara.right_arm.frame_number = item->frame_number - anims[item->anim_number].frame_base;
	lara.right_arm.anim_number = item->anim_number;
	lara.left_arm.frame_base = lara.right_arm.frame_base;
	lara.left_arm.frame_number = lara.right_arm.frame_number;
	lara.left_arm.anim_number = lara.right_arm.anim_number;
}

void RifleHandler(long weapon_type)
{
	WEAPON_INFO* winfo;
	PHD_VECTOR pos;
	long x, y, z, r, g, b;

	winfo = &weapons[weapon_type];
	LaraGetNewTarget(winfo);

	if (input & IN_ACTION)
		LaraTargetInfo(winfo);

	AimWeapon(winfo, &lara.left_arm);

	if (lara.left_arm.lock)
	{
		lara.torso_x_rot = lara.left_arm.x_rot;
		lara.torso_y_rot = lara.left_arm.y_rot;

		if (camera.old_type != LOOK_CAMERA && !BinocularRange)
		{
			lara.head_y_rot = 0;
			lara.head_x_rot = 0;
		}
	}

	if (weapon_type == WEAPON_REVOLVER)
		AnimatePistols(weapon_type);
	else
		AnimateShotgun(weapon_type);

	if (lara.right_arm.flash_gun)
	{
		r = (GetRandomControl() & 0x3F) + 192;
		g = (GetRandomControl() & 0x1F) + 128;
		b = GetRandomControl() & 63;

		if (weapon_type == WEAPON_SHOTGUN)
		{
			x = (GetRandomControl() & 0xFF) + (phd_sin(lara_item->pos.y_rot) >> 4) + lara_item->pos.x_pos;
			y = ((GetRandomControl() & 0x7F) - 575) + lara_item->pos.y_pos;
			z = (GetRandomControl() & 0xFF) + (phd_cos(lara_item->pos.y_rot) >> 4) + lara_item->pos.z_pos;

			if (gfLevelFlags & GF_MIRROR && lara_item->room_number == gfMirrorRoom)
				TriggerDynamic_MIRROR(x, y, z, 12, r, g, b);
			else
				TriggerDynamic(x, y, z, 12, r, g, b);
		}
		else if (weapon_type == WEAPON_REVOLVER)
		{
			pos.x = (GetRandomControl() & 0xFF) - 128;
			pos.y = (GetRandomControl() & 0x7F) - 63;
			pos.z = (GetRandomControl() & 0xFF) - 128;
			GetLaraJointPos(&pos, 11);

			if (gfLevelFlags & GF_MIRROR && lara_item->room_number == gfMirrorRoom)
				TriggerDynamic_MIRROR(pos.x, pos.y, pos.z, 12, r, g, b);
			else
				TriggerDynamic(pos.x, pos.y, pos.z, 12, r, g, b);
		}
	}
}

void CrossbowHitSwitchType78(ITEM_INFO* item, ITEM_INFO* target, long MustHitLastNode)
{
	SPHERE* ptr1;
	long dx, dy, dz, num1, cs, cd, speed;
	short TriggerItems[8];
	short NumTrigs, room_number;

	if (target->flags & IFL_SWITCH_ONESHOT && target->object_number != SKELETON)
		return;

	if (!MustHitLastNode)
	{
		num1 = objects[target->object_number].nmeshes;
		cs = num1 - 1;
	}
	else
	{
		num1 = GetSpheres(target, Slist, 1);
		cs = -1;
		cd = 0x7FFFFFFF;

		if (target->object_number == SKELETON)
		{
			for (int i = 0; i < 8; i++)
			{
				speed = item->speed * phd_cos(item->pos.x_rot) >> W2V_SHIFT;
				item->pos.x_pos += speed * phd_sin(item->pos.y_rot) >> 17;
				item->pos.y_pos += item->speed * phd_sin(-item->pos.x_rot) >> 17;
				item->pos.z_pos += speed * phd_cos(item->pos.y_rot) >> 17;
				ptr1 = Slist;

				for (int j = 0; j < num1; j++)
				{
					dx = ptr1->x - item->pos.x_pos;
					dy = ptr1->y - item->pos.y_pos;
					dz = ptr1->z - item->pos.z_pos;
					dy = SQUARE(dx) + SQUARE(dy) + SQUARE(dz);

					if (dy < SQUARE(ptr1->r))
					{
						cs = j;
						break;
					}

					ptr1++;
				}

				if (cs != -1)
					break;
			}
		}
		else
		{
			ptr1 = Slist;

			for (int i = 0; i < num1; i++)
			{
				dx = ptr1->x - item->pos.x_pos;
				dy = ptr1->y - item->pos.y_pos;
				dz = ptr1->z - item->pos.z_pos;
				dy = SQUARE(dx) + SQUARE(dy) + SQUARE(dz) - SQUARE(ptr1->r);

				if (dy < cd)
				{
					cd = dy;
					cs = i;
				}

				ptr1++;
			}
		}
	}

	if (target->object_number == SKELETON)
	{
		if (cs != -1 && objects[target->object_number].explodable_meshbits & 1 << cs)
		{
			ExplodeItemNode(target, cs, 0, 64);
			target->mesh_bits &= ~(1 << cs);
		}
	}
	else
	{
		if (cs == num1 - 1)
		{
			if (target->flags & IFL_CODEBITS && (target->flags & IFL_CODEBITS) != IFL_CODEBITS)
			{
				room_number = target->room_number;
				GetHeight(GetFloor(target->pos.x_pos, target->pos.y_pos - 256, target->pos.z_pos, &room_number),
					target->pos.x_pos, target->pos.y_pos - 256, target->pos.z_pos);
				TestTriggers(trigger_index, 1, target->flags & IFL_CODEBITS);
			}
			else
			{
				NumTrigs = (short)GetSwitchTrigger(target, TriggerItems, 1);

				for (int i = 0; i < NumTrigs; i++)
				{
					AddActiveItem(TriggerItems[i]);
					items[TriggerItems[i]].status = ITEM_ACTIVE;
					items[TriggerItems[i]].flags |= IFL_CODEBITS;
				}
			}

			if (target->object_number == SWITCH_TYPE7)
				ExplodeItemNode(target, objects[SWITCH_TYPE7].nmeshes - 1, 0, 64);

			AddActiveItem(target - items);
			target->flags |= IFL_CODEBITS | IFL_SWITCH_ONESHOT;
			target->status = ITEM_ACTIVE;
		}
	}
}

void TriggerUnderwaterExplosion(ITEM_INFO* item, long vehicle)
{
	long x, y, z, wh;

	if (vehicle)
	{
		x = (GetRandomControl() & 0x1FF) + item->pos.x_pos - 256;
		z = (GetRandomControl() & 0x1FF) + item->pos.z_pos - 256;
		TriggerExplosionBubble(x, item->pos.y_pos, z, item->room_number);
		TriggerExplosionSparks(x, item->pos.y_pos, z, 2, -1, 1, item->room_number);
		wh = GetWaterHeight(x, item->pos.y_pos, z, item->room_number);

		if (wh != NO_HEIGHT)
			TriggerSmallSplash(x, wh, z, 8);
	}
	else
	{
		TriggerExplosionBubble(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number);
		TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 2, -2, 1, item->room_number);

		for (int i = 0; i < 3; i++)
			TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 2, -1, 1, item->room_number);

		wh = GetWaterHeight(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number);

		if (wh != NO_HEIGHT)
		{
			y = item->pos.y_pos - wh;

			if (y < 2048)
			{
				splash_setup.x = item->pos.x_pos;
				splash_setup.y = wh;
				splash_setup.z = item->pos.z_pos;
				wh = 2048 - y;
				splash_setup.InnerRadVel = 160;
				splash_setup.MiddleSize = 224;
				splash_setup.OuterRad = 272;
				splash_setup.InnerRad = short((wh >> 6) + 16);
				splash_setup.InnerSize = short((wh >> 6) + 12);
				splash_setup.InnerYVel = short((-512 - wh) << 3);
				splash_setup.pad1 = short((wh >> 6) + 24);
				splash_setup.MiddleRad = short((wh >> 6) + 24);
				splash_setup.MiddleRadVel = short((-768 - wh) << 2);
				splash_setup.MiddleYVel = short((wh >> 6) + 32);
				splash_setup.pad2 = short((wh >> 6) + 32);
				SetupSplash(&splash_setup);
			}
		}
	}
}

void draw_shotgun(long weapon_type)
{
	ITEM_INFO* item;

	if (lara.weapon_item == NO_ITEM)
	{
		lara.weapon_item = CreateItem();
		item = &items[lara.weapon_item];
		item->object_number = (short)WeaponObject(weapon_type);

		if (weapon_type == WEAPON_GRENADE)
			item->anim_number = objects[GRENADE_GUN_ANIM].anim_index;
		else
			item->anim_number = objects[item->object_number].anim_index + 1;

		item->frame_number = anims[item->anim_number].frame_base;
		item->status = ITEM_ACTIVE;
		item->current_anim_state = 1;
		item->goal_anim_state = 1;
		item->room_number = 255;
		lara.left_arm.frame_base = objects[item->object_number].frame_base;
		lara.right_arm.frame_base = objects[item->object_number].frame_base;
	}
	else
		item = &items[lara.weapon_item];

	AnimateItem(item);

	if (!item->current_anim_state || item->current_anim_state == 6)
		ready_shotgun(weapon_type);
	else if (item->frame_number - anims[item->anim_number].frame_base == weapons[weapon_type].draw_frame)
		draw_shotgun_meshes(weapon_type);
	else if (lara.water_status == LW_UNDERWATER)
		item->goal_anim_state = 6;

	lara.left_arm.frame_base = anims[item->anim_number].frame_ptr;
	lara.right_arm.frame_base = anims[item->anim_number].frame_ptr;
	lara.left_arm.frame_number = item->frame_number - anims[item->anim_number].frame_base;
	lara.right_arm.frame_number = item->frame_number - anims[item->anim_number].frame_base;
	lara.left_arm.anim_number = item->anim_number;
	lara.right_arm.anim_number = item->anim_number;
}

void inject_lara1gun(bool replace)
{
	INJECT(0x0042B600, DoGrenadeDamageOnBaddie, replace);
	INJECT(0x0042A270, FireCrossbow, replace);
	INJECT(0x00428E40, draw_shotgun_meshes, replace);
	INJECT(0x00428E70, undraw_shotgun_meshes, replace);
	INJECT(0x00428EA0, ready_shotgun, replace);
	INJECT(0x00429260, FireShotgun, replace);
	INJECT(0x00429480, FireGrenade, replace);
	INJECT(0x0042B100, AnimateShotgun, replace);
	INJECT(0x00428F10, RifleHandler, replace);
	INJECT(0x0042A490, CrossbowHitSwitchType78, replace);
	INJECT(0x0042B430, TriggerUnderwaterExplosion, replace);
	INJECT(0x0042AE50, draw_shotgun, replace);
}
