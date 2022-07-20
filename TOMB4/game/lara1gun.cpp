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

void inject_lara1gun(bool replace)
{
	INJECT(0x0042B600, DoGrenadeDamageOnBaddie, replace);
	INJECT(0x0042A270, FireCrossbow, replace);
}
