#include "../tomb4/pch.h"
#include "pickup.h"
#include "lara_states.h"
#include "collide.h"
#include "items.h"
#include "health.h"
#include "objects.h"
#include "newinv.h"
#include "control.h"
#include "draw.h"

static short SarcophagusBounds[12] = { -512, 512, -100, 100, -512, 0, -1820, 1820, -5460, 5460, 0, 0 };
static short KeyHoleBounds[12] = { -256, 256, 0, 0, 0, 412, -1820, 1820, -5460, 5460, -1820, 1820 };
static PHD_VECTOR  SarcophagusPos = { 0, 0, -300 };
static PHD_VECTOR KeyHolePosition = { 0, 0, 362 };

void SarcophagusCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	ITEM_INFO* pickup;
	short pickup_num;

	item = &items[item_number];

	if (input & IN_ACTION && item->status != ITEM_ACTIVE && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH &&
		lara.gun_status == LG_NO_ARMS || lara.IsMoving && lara.GeneralPtr == (void*)item_number)
	{
		if (TestLaraPosition(SarcophagusBounds, item, l))
		{
			if (MoveLaraPosition(&SarcophagusPos, item, l))
			{
				l->anim_number = ANIM_SARCOPHAGUS;
				l->frame_number = anims[ANIM_SARCOPHAGUS].frame_base;
				l->current_anim_state = AS_CONTROLLED;
				item->flags |= IFL_CODEBITS;
				item->status = ITEM_ACTIVE;
				AddActiveItem(item_number);
				lara.IsMoving = 0;
				lara.head_x_rot = 0;
				lara.head_y_rot = 0;
				lara.torso_x_rot = 0;
				lara.torso_y_rot = 0;
				lara.gun_status = LG_HANDS_BUSY;
			}
			else
				lara.GeneralPtr = (void*)item_number;
		}
		else if (lara.IsMoving && lara.GeneralPtr == (void*)item_number)
		{
			lara.IsMoving = 0;
			lara.gun_status = LG_NO_ARMS;
		}
	}
	else if (l->anim_number == ANIM_SARCOPHAGUS && l->frame_number == anims[ANIM_SARCOPHAGUS].frame_base + 113)
	{
		for (pickup_num = room[item->room_number].item_number; pickup_num != NO_ITEM; pickup_num = pickup->next_item)
		{
			pickup = &items[pickup_num];

			if (item != pickup && item->pos.x_pos == pickup->pos.x_pos && item->pos.z_pos == pickup->pos.z_pos)
			{
				if (objects[pickup->object_number].collision == PickUpCollision)
				{
					AddDisplayPickup(pickup->object_number);
					pickup->item_flags[3] = 1;
					pickup->status = ITEM_INVISIBLE;
				}
			}
		}
	}
	else
		ObjectCollision(item_number, l, coll);
}

void KeyHoleCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	short key, hole;

	item = &items[item_number];

	if ((input & IN_ACTION || GLOBAL_inventoryitemchosen != NO_ITEM) && !BinocularRange && lara.gun_status == LG_NO_ARMS &&
		l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH || lara.IsMoving && lara.GeneralPtr == (void*)item_number)
	{
		key = short(GLOBAL_inventoryitemchosen - KEY_ITEM1);
		hole = item->object_number - KEY_HOLE1;

		if (TestLaraPosition(KeyHoleBounds, item, l))
		{
			if (!lara.IsMoving)
			{
				if (item->status == ITEM_INVISIBLE)
					return;

				if (GLOBAL_inventoryitemchosen == NO_ITEM)
				{
					if (have_i_got_object(hole + KEY_ITEM1))
						GLOBAL_enterinventory = hole + KEY_ITEM1;

					return;
				}

				if (key != hole)
					return;
			}

			if (MoveLaraPosition(&KeyHolePosition, item, l))
			{
				remove_inventory_item(hole + KEY_ITEM1);
				l->anim_number = ANIM_USEKEY;
				l->frame_number = anims[l->anim_number].frame_base;
				l->current_anim_state = AS_USEKEY;
				lara.IsMoving = 0;
				lara.head_x_rot = 0;
				lara.head_y_rot = 0;
				lara.torso_x_rot = 0;
				lara.torso_y_rot = 0;
				lara.gun_status = LG_HANDS_BUSY;
				item->status = ITEM_ACTIVE;
			}
			else
				lara.GeneralPtr = (void*)item_number;

			GLOBAL_inventoryitemchosen = NO_ITEM;
		}
		else if (lara.IsMoving && lara.GeneralPtr == (void*)item_number)
		{
			lara.IsMoving = 0;
			lara.gun_status = LG_NO_ARMS;
		}
	}
	else if (item->object_number <= KEY_HOLE6)
		ObjectCollision(item_number, l, coll);
}

void PuzzleDoneCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll)
{
	if (items[item_num].trigger_flags != 999)
		ObjectCollision(item_num, l, coll);
}

void PuzzleDone(ITEM_INFO* item, short item_number)
{
	item->object_number += 12;
	item->anim_number = objects[item->object_number].anim_index;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = anims[item->anim_number].current_anim_state;
	item->goal_anim_state = item->current_anim_state;
	item->required_anim_state = 0;
	AddActiveItem(item_number);
	item->flags |= IFL_CODEBITS;
	item->status = ITEM_ACTIVE;
}

void AnimatingPickUp(short item_number)
{
	if ((items[item_number].trigger_flags & 0x3F) == 2)
		AnimateItem(&items[item_number]);
}

short* FindPlinth(ITEM_INFO* item)
{
	ITEM_INFO* plinth;
	ROOM_INFO* r;
	MESH_INFO* mesh;
	short* p;
	short* o;
	long i;
	short item_num;

	o = 0;
	r = &room[item->room_number];
	mesh = r->mesh;

	for (i = r->num_meshes; i > 0; i--)
	{
		if (mesh->Flags & 1 && item->pos.x_pos == mesh->x && item->pos.z_pos == mesh->z)
		{
			p = GetBestFrame(item);
			o = &static_objects[mesh->static_number].x_minc;

			if (p[0] <= o[1] && p[1] >= o[0] && p[4] <= o[5] && p[5] >= o[4] && (o[0] || o[1]))
				break;
		}

		mesh++;
	}

	if (i)
		return o;

	item_num = r->item_number;

	while (1)
	{
		plinth = &items[item_num];

		if (item != plinth && objects[plinth->object_number].collision != PickUpCollision &&
			item->pos.x_pos == plinth->pos.x_pos && item->pos.y_pos <= plinth->pos.y_pos && item->pos.z_pos == plinth->pos.z_pos)
			break;

		item_num = plinth->next_item;

		if (item_num == NO_ITEM)
			return 0;
	}

	return GetBestFrame(&items[item_num]);
}

long KeyTrigger(short item_num)
{
	ITEM_INFO* item;
	long oldkey;

	item = &items[item_num];

	if ((item->status != ITEM_ACTIVE || lara.gun_status == LG_HANDS_BUSY) && (!KeyTriggerActive || lara.gun_status != LG_HANDS_BUSY))
		return -1;

	oldkey = KeyTriggerActive;

	if (!KeyTriggerActive)
		item->status = ITEM_DEACTIVATED;

	KeyTriggerActive = 0;
	return oldkey;
}

long PickupTrigger(short item_num)
{
	ITEM_INFO* item;

	item = &items[item_num];

	if (item->flags & IFL_CLEARBODY || item->status != ITEM_INVISIBLE || item->item_flags[3] != 1 || item->trigger_flags & 128)
		return 0;

	KillItem(item_num);
	return 1;
}

void RegeneratePickups()
{
	ITEM_INFO* item;
	short* ammo;
	short objnum;

	for (int i = 0; i < NumRPickups; i++)
	{
		item = &items[RPickups[i]];

		if (item->status == ITEM_INVISIBLE)
		{
			objnum = item->object_number;

			if (objnum >= CROSSBOW_AMMO1_ITEM && objnum <= CROSSBOW_AMMO3_ITEM)
			{
				ammo = &lara.num_crossbow_ammo1;

				if (!ammo[objnum - CROSSBOW_AMMO1_ITEM])
					item->status = ITEM_INACTIVE;
			}
			else if (objnum >= GRENADE_GUN_AMMO1_ITEM && objnum <= GRENADE_GUN_AMMO3_ITEM)
			{
				ammo = &lara.num_grenade_ammo1;

				if (!ammo[objnum - GRENADE_GUN_AMMO1_ITEM])
					item->status = ITEM_INACTIVE;
			}
			else if (objnum >= SHOTGUN_AMMO1_ITEM && objnum <= SHOTGUN_AMMO2_ITEM)
			{
				ammo = &lara.num_shotgun_ammo1;

				if (!ammo[objnum - SHOTGUN_AMMO1_ITEM])
					item->status = ITEM_INACTIVE;
			}
			else if (objnum == SIXSHOOTER_AMMO_ITEM)
			{
				ammo = &lara.num_revolver_ammo;

				if (!*ammo)
					item->status = ITEM_INACTIVE;
			}
		}
	}
}

void inject_pickup(bool replace)
{
	INJECT(0x004587E0, SarcophagusCollision, replace);
	INJECT(0x00458090, KeyHoleCollision, replace);
	INJECT(0x00458260, PuzzleDoneCollision, replace);
	INJECT(0x00458690, PuzzleDone, replace);
	INJECT(0x00457610, AnimatingPickUp, replace);
	INJECT(0x00457F30, FindPlinth, replace);
	INJECT(0x00458710, KeyTrigger, replace);
	INJECT(0x00458780, PickupTrigger, replace);
	INJECT(0x00457650, RegeneratePickups, replace);
}
