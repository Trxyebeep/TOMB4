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
#include "../specific/function_stubs.h"
#include "sphere.h"

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

static PHD_3DPOS bum_view;
static GAME_VECTOR bum_vdest;
static GAME_VECTOR bum_vsrc;
static ITEM_INFO* TargetList[8];
static ITEM_INFO* LastTargets[8];

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

long FireWeapon(long weapon_type, ITEM_INFO* target, ITEM_INFO* src, short* angles)
{
	WEAPON_INFO* winfo;
	SPHERE* sptr;
	short* ammo;
	long r, nSpheres, bestdist, best;
	short room_number;

	bum_view.x_pos = 0;
	bum_view.y_pos = 0;
	bum_view.z_pos = 0;
	GetLaraJointPos((PHD_VECTOR*)&bum_view, 11);
	ammo = get_current_ammo_pointer(weapon_type);

	if (!*ammo)
		return 0;
	
	if (*ammo != -1)
		--*ammo;

	winfo = &weapons[weapon_type];

	bum_view.x_pos = src->pos.x_pos;
	bum_view.z_pos = src->pos.z_pos;
	bum_view.x_rot = short(winfo->shot_accuracy * (GetRandomControl() - 0x4000) / 0x10000 + angles[1]);
	bum_view.y_rot = short(winfo->shot_accuracy * (GetRandomControl() - 0x4000) / 0x10000 + angles[0]);
	bum_view.z_rot = 0;
	phd_GenerateW2V(&bum_view);

	nSpheres = GetSpheres(target, Slist, 0);
	best = -1;
	bestdist = 0x7FFFFFFF;

	for (int i = 0; i < nSpheres; i++)
	{
		sptr = &Slist[i];
		r = sptr->r;

		if (ABS(sptr->x) < r && ABS(sptr->y) < r && sptr->z > r && SQUARE(sptr->x) + SQUARE(sptr->y) <= SQUARE(r))
		{
			if (sptr->z - r < bestdist)
			{
				bestdist = sptr->z - r;
				best = i;
			}
		}
	}

	lara.has_fired = 1;
	bum_vsrc.x = bum_view.x_pos;
	bum_vsrc.y = bum_view.y_pos;
	bum_vsrc.z = bum_view.z_pos;
	room_number = src->room_number;
	GetFloor(bum_view.x_pos, bum_view.y_pos, bum_view.z_pos, &room_number);
	bum_vsrc.room_number = room_number;

	if (best < 0)
	{
		bum_vdest.x = bum_vsrc.x + (0x5000 * phd_mxptr[M20] >> W2V_SHIFT);
		bum_vdest.y = bum_vsrc.y + (0x5000 * phd_mxptr[M21] >> W2V_SHIFT);
		bum_vdest.z = bum_vsrc.z + (0x5000 * phd_mxptr[M22] >> W2V_SHIFT);
		GetTargetOnLOS(&bum_vsrc, &bum_vdest, 0, 1);
		return -1;
	}
	else
	{
		savegame.Game.AmmoHits++;
		bum_vdest.x = bum_vsrc.x + (bestdist * phd_mxptr[M20] >> W2V_SHIFT);
		bum_vdest.y = bum_vsrc.y + (bestdist * phd_mxptr[M21] >> W2V_SHIFT);
		bum_vdest.z = bum_vsrc.z + (bestdist * phd_mxptr[M22] >> W2V_SHIFT);

		if (!GetTargetOnLOS(&bum_vsrc, &bum_vdest, 0, 1))
			HitTarget(target, &bum_vdest, winfo->damage, 0);

		return 1;
	}
}

void AimWeapon(WEAPON_INFO* winfo, LARA_ARM* arm)
{
	short speed, x, y;

	speed = winfo->aim_speed;

	if (arm->lock)
	{
		y = lara.target_angles[0];
		x = lara.target_angles[1];
	}
	else
	{
		x = 0;
		y = 0;
	}

	if (arm->y_rot >= y - speed && arm->y_rot <= speed + y)
		arm->y_rot = y;
	else if (arm->y_rot < y)
		arm->y_rot += speed;
	else
		arm->y_rot -= speed;

	if (arm->x_rot >= x - speed && arm->x_rot <= speed + x)
		arm->x_rot = x;
	else if (arm->x_rot < x)
		arm->x_rot += speed;
	else
		arm->x_rot -= speed;

	arm->z_rot = 0;
}

void LaraGetNewTarget(WEAPON_INFO* winfo)
{
	ITEM_INFO* item;
	ITEM_INFO* bestitem;
	CREATURE_INFO* creature;
	GAME_VECTOR src, target;
	long x, y, z, slot, dist, maxdist, maxdist2, bestdist;
	short ang[2];
	short bestyrot, targets, match;

	if (BinocularRange)
	{
		lara.target = 0;
		return;
	}

	bestitem = 0;
	src.x = lara_item->pos.x_pos;
	src.y = lara_item->pos.y_pos - 650;
	src.z = lara_item->pos.z_pos;
	src.room_number = lara_item->room_number;
	bestyrot = 0x7FFF;
	bestdist = 0x7FFFFFFF;
	maxdist = winfo->target_dist;
	maxdist2 = SQUARE(maxdist);
	creature = baddie_slots;
	targets = 0;

	for (slot = 0; slot < 5; slot++, creature++)
	{
		if (creature->item_num != NO_ITEM)
		{
			item = &items[creature->item_num];

			if (item->hit_points > 0)
			{
				x = item->pos.x_pos - src.x;
				y = item->pos.y_pos - src.y;
				z = item->pos.z_pos - src.z;

				if (ABS(x) <= maxdist && ABS(y) <= maxdist && ABS(z) <= maxdist)
				{
					dist = SQUARE(x) + SQUARE(y) + SQUARE(z);

					if (dist < maxdist2)
					{
						find_target_point(item, &target);

						if (LOS(&src, &target))
						{
							phd_GetVectorAngles(target.x - src.x, target.y - src.y, target.z - src.z, ang);
							ang[0] -= (lara.torso_y_rot + lara_item->pos.y_rot);
							ang[1] -= (lara.torso_x_rot + lara_item->pos.x_rot);

							if (ang[0] >= winfo->lock_angles[0] && ang[0] <= winfo->lock_angles[1] &&
								ang[1] >= winfo->lock_angles[2] && ang[1] <= winfo->lock_angles[3])
							{
								TargetList[targets] = item;
								targets++;

								if (ABS(ang[0]) < bestyrot + 2730 && dist < bestdist)
								{
									bestdist = dist;
									bestyrot = ABS(ang[0]);
									bestitem = item;
								}
							}
						}
					}
				}
			}
		}
	}

	TargetList[targets] = 0;

	if (TargetList[0])
	{
		for (slot = 0; slot < 8; slot++)
		{
			if (!TargetList[slot])
				lara.target = 0;

			if (TargetList[slot] == lara.target)
				break;
		}

		if (savegame.AutoTarget || input & IN_TARGET)
		{
			if (!lara.target)
			{
				lara.target = bestitem;
				LastTargets[0] = 0;
			}
			else if (input & IN_TARGET)
			{
				lara.target = 0;

				for (match = 0; match < 8; match++)
				{
					if (!TargetList[match])
						break;

					for (slot = 0; slot < 8; slot++)
					{
						if (!LastTargets[slot])
						{
							slot = 8;
							break;
						}

						if (LastTargets[slot] == TargetList[match])
							break;
					}

					if (slot == 8)
					{
						lara.target = TargetList[match];
						break;
					}
				}

				if (!lara.target)
				{
					lara.target = bestitem;
					LastTargets[0] = 0;
				}
			}
		}
	}
	else
		lara.target = 0;

	if (lara.target != LastTargets[0])
	{
		for (slot = 7; slot > 0; slot--)
			LastTargets[slot] = LastTargets[slot - 1];

		LastTargets[0] = lara.target;
	}

	LaraTargetInfo(winfo);
}

void inject_larafire(bool replace)
{
	INJECT(0x0042DDC0, CheckForHoldingState, replace);
	INJECT(0x0042DDF0, InitialiseNewWeapon, replace);
	INJECT(0x0042E4A0, find_target_point, replace);
	INJECT(0x0042DF30, LaraTargetInfo, replace);
	INJECT(0x0042F480, get_current_ammo_pointer, replace);
	INJECT(0x0042E630, FireWeapon, replace);
	INJECT(0x0042E560, AimWeapon, replace);
	INJECT(0x0042E0D0, LaraGetNewTarget, replace);
}
