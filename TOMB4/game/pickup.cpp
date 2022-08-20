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
#include "camera.h"
#include "../specific/input.h"
#include "larafire.h"
#include "laraflar.h"
#include "flmtorch.h"
#include "lara.h"
#include "deltapak.h"

uchar NumRPickups;
uchar RPickups[16];
char KeyTriggerActive = 0;

static short SarcophagusBounds[12] = { -512, 512, -100, 100, -512, 0, -1820, 1820, -5460, 5460, 0, 0 };
static short KeyHoleBounds[12] = { -256, 256, 0, 0, 0, 412, -1820, 1820, -5460, 5460, -1820, 1820 };
static short HiddenPickUpBounds[12] = { -256, 256, -100, 100, -800, -256, -1820, 1820, -5460, 5460, 0, 0 };
static short CrowbarPickUpBounds[12] = { -256, 256, -100, 100, 200, 512, -1820, 1820, -5460, 5460, 0, 0 };
static short PlinthPickUpBounds[12] = { -256, 256, -640, 640, -511, 0, -1820, 1820, -5460, 5460, 0, 0 };
static short PickUpBounds[12] = { -256, 256, -200, 200, -256, 256, -1820, 1820, 0, 0, 0, 0 };
static short PickUpBoundsUW[12] = { -512, 512, -512, 512, -512, 512, -8190, 8190, -8190, 8190, -8190, 8190 };
static short PuzzleBounds[12] = { 0, 0, -256, 256, 0, 0, -1820, 1820, -5460, 5460, -1820, 1820 };
static PHD_VECTOR  SarcophagusPos = { 0, 0, -300 };
static PHD_VECTOR KeyHolePosition = { 0, 0, 362 };
static PHD_VECTOR HiddenPickUpPosition = { 0, 0, -690 };
static PHD_VECTOR CrowbarPickUpPosition = { 0, 0, 215 };
static PHD_VECTOR PlinthPickUpPosition = { 0, 0, -460 };
static PHD_VECTOR PickUpPosition = { 0, 0, -100 };
static PHD_VECTOR PickUpPositionUW = { 0, -200, -350 };

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

void PickUpCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	ITEM_INFO* itemme;
	short* bounds;
	long flag;
	short rotx, roty, rotz, ocb;

	item = &items[item_number];

	if (item->status == ITEM_INVISIBLE || (item->object_number == CLOCKWORK_BEETLE && item->item_flags[0] == 1 && item->item_flags[2] != 5) ||
		(item->object_number == FLARE_ITEM && lara.gun_type == WEAPON_FLARE))
		return;

	rotx = item->pos.x_rot;
	roty = item->pos.y_rot;
	rotz = item->pos.z_rot;
	ocb = item->trigger_flags & 0x3F;
	item->pos.y_rot = l->pos.y_rot;
	item->pos.z_rot = 0;

	if (lara.water_status == LW_ABOVE_WATER || lara.water_status == LW_WADE)
	{
		if ((input & IN_ACTION || (GLOBAL_inventoryitemchosen != NO_ITEM && ocb == 2)) && !BinocularRange &&
			((l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH && lara.gun_status == LG_NO_ARMS) ||
				(l->current_anim_state == AS_DUCK && l->anim_number == ANIM_DUCKBREATHE && lara.gun_status == LG_NO_ARMS) ||
				(l->current_anim_state == AS_ALL4S && l->anim_number == ANIM_ALL4S)) || lara.IsMoving && lara.GeneralPtr == (void*)item_number)
		{
			flag = 0;
			item->pos.x_rot = 0;

			switch (ocb)
			{
			case 1:
				item->pos.y_rot = roty;

				if (!lara.IsDucked && TestLaraPosition(HiddenPickUpBounds, item, l))
				{
					if (MoveLaraPosition(&HiddenPickUpPosition, item, l))
					{
						l->anim_number = ANIM_HIDDENPICKUP;
						l->current_anim_state = AS_HIDDENPICKUP;
						flag = 1;
					}

					lara.GeneralPtr = (void*)item_number;
				}
				else if (lara.IsMoving && lara.GeneralPtr == (void*)item_number)
				{
					lara.IsMoving = 0;
					lara.gun_status = LG_NO_ARMS;
				}

				break;

			case 2:
				item->pos.y_rot = roty;

				if (!lara.IsDucked && TestLaraPosition(CrowbarPickUpBounds, item, l))
				{
					if (!lara.IsMoving)
					{
						if (GLOBAL_inventoryitemchosen == NO_ITEM)
						{
							if (have_i_got_object(CROWBAR_ITEM))
								GLOBAL_enterinventory = CROWBAR_ITEM;

							break;
						}

						if (GLOBAL_inventoryitemchosen != CROWBAR_ITEM)
							break;

						GLOBAL_inventoryitemchosen = NO_ITEM;
					}

					if (MoveLaraPosition(&CrowbarPickUpPosition, item, l))
					{
						l->anim_number = ANIM_CROWBAR;
						l->current_anim_state = AS_PICKUP;
						item->status = ITEM_ACTIVE;
						AddActiveItem(item_number);
						AnimateItem(item);
						flag = 1;
					}

					lara.GeneralPtr = (void*)item_number;
				}
				else if (lara.IsMoving && lara.GeneralPtr == (void*)item_number)
				{
					lara.IsMoving = 0;
					lara.gun_status = LG_NO_ARMS;
				}

				break;

			case 3:
			case 4:
				bounds = FindPlinth(item);

				if (bounds)
				{
					PlinthPickUpBounds[0] = bounds[0];
					PlinthPickUpBounds[1] = bounds[1];
					PlinthPickUpBounds[5] = bounds[5] + 320;
					PlinthPickUpPosition.z = -200 - bounds[5];
				}

				PlinthPickUpBounds[3] = short(l->pos.y_pos - item->pos.y_pos + 100);

				if (TestLaraPosition(PlinthPickUpBounds, item, l) && !lara.IsDucked)
				{
					if (item->pos.y_pos == l->pos.y_pos)
						PlinthPickUpPosition.y = 0;
					else
						PlinthPickUpPosition.y = l->pos.y_pos - item->pos.y_pos;

					if (MoveLaraPosition(&PlinthPickUpPosition, item, l))
					{
						if (ocb == 3)
							l->anim_number = ANIM_PLINTHHI;
						else
							l->anim_number = ANIM_PLINTHLO;

						l->current_anim_state = AS_PICKUP;
						flag = 1;
					}

					lara.GeneralPtr = (void*)item_number;
				}
				else if (lara.IsMoving && lara.GeneralPtr == (void*)item_number)
				{
					lara.IsMoving = 0;
					lara.gun_status = LG_NO_ARMS;
				}

				break;

			default:

				if (TestLaraPosition(PickUpBounds, item, l))
				{
					PickUpPosition.y = l->pos.y_pos - item->pos.y_pos;

					if (l->current_anim_state == AS_DUCK)
					{
						AlignLaraPosition(&PickUpPosition, item, l);

						if (item->object_number == FLARE_ITEM)
						{
							l->anim_number = ANIM_DUCKPICKUPF;
							l->current_anim_state = AS_FLAREPICKUP;
						}
						else
						{
							l->anim_number = ANIM_DUCKPICKUP;
							l->current_anim_state = AS_PICKUP;
						}

						flag = 1;
					}
					else if (l->current_anim_state == AS_ALL4S)
						l->goal_anim_state = AS_DUCK;
					else if (MoveLaraPosition(&PickUpPosition, item, l))
					{
						if (item->object_number == FLARE_ITEM)
						{
							l->anim_number = ANIM_PICKUPF;
							l->current_anim_state = AS_FLAREPICKUP;
						}
						else
						{
							l->anim_number = ANIM_PICKUP;
							l->current_anim_state = AS_PICKUP;
						}

						flag = 1;
					}

					lara.GeneralPtr = (void*)item_number;
				}
				else if (lara.IsMoving && lara.GeneralPtr == (void*)item_number)
				{
					lara.IsMoving = 0;
					lara.gun_status = LG_NO_ARMS;
				}

				break;
			}

			if (flag)
			{
				lara.head_x_rot = 0;
				lara.head_y_rot = 0;
				lara.torso_x_rot = 0;
				lara.torso_y_rot = 0;
				l->frame_number = anims[l->anim_number].frame_base;
				lara.IsMoving = 0;
				lara.gun_status = LG_HANDS_BUSY;
			}
		}
		else if (lara.GeneralPtr == (void*)item_number && (l->current_anim_state == AS_PICKUP || l->current_anim_state == AS_HIDDENPICKUP))
		{
			if (l->frame_number == anims[ANIM_PICKUP].frame_base + 15 ||
				l->frame_number == anims[ANIM_DUCKPICKUP].frame_base + 22 ||
				l->frame_number == anims[ANIM_DUCKPICKUP].frame_base + 20 ||
				l->frame_number == anims[ANIM_PLINTHLO].frame_base + 29 ||
				l->frame_number == anims[ANIM_PLINTHHI].frame_base + 45 ||
				l->frame_number == anims[ANIM_HIDDENPICKUP].frame_base + 42 ||
				(l->anim_number == ANIM_CROWBAR && l->frame_number == anims[ANIM_CROWBAR].frame_base + 123))
			{
				if (item->object_number == BURNING_TORCH_ITEM)
				{
					AddDisplayPickup(BURNING_TORCH_ITEM);
					GetFlameTorch();
					lara.LitTorch = item->item_flags[3] & 1;
					KillItem(item_number);
				}
				else
				{
					if (item->object_number != FLARE_ITEM)
					{
						AddDisplayPickup(item->object_number);

						if (item->trigger_flags & 0x100)
						{
							for (int i = 0; i < level_items; i++)
							{
								itemme = &items[i];

								if (itemme->object_number == item->object_number)
									KillItem(i);
							}
						}
					}

					if (!(item->trigger_flags & 0xC0))
						KillItem(item_number);
					else
					{
						item->item_flags[3] = 1;
						item->flags |= IFL_TRIGGERED;
						item->status = ITEM_INVISIBLE;
					}
				}
			}
		}
		else if (lara.GeneralPtr == (void*)item_number && l->current_anim_state == AS_FLAREPICKUP)
		{
			if ((l->anim_number == ANIM_DUCKPICKUPF && l->frame_number == anims[ANIM_DUCKPICKUPF].frame_base + 22) ||
				l->frame_number == anims[ANIM_PICKUPF].frame_base + 58)
			{
				lara.request_gun_type = WEAPON_FLARE;
				lara.gun_type = WEAPON_FLARE;
				InitialiseNewWeapon();
				lara.gun_status = LG_FLARE;
				lara.flare_age = (long)item->data & 0x7FFF;
				KillItem(item_number);
			}
		}
	}
	else if (lara.water_status == LW_UNDERWATER)
	{
		item->pos.x_rot = -4550;

		if (input & IN_ACTION && item->object_number != BURNING_TORCH_ITEM && l->current_anim_state == AS_TREAD &&
			lara.gun_status == LG_NO_ARMS && TestLaraPosition(PickUpBoundsUW, item, l) ||
			lara.IsMoving && lara.GeneralPtr == (void*)item_number)
		{
			if (TestLaraPosition(PickUpBoundsUW, item, l))
			{
				if (MoveLaraPosition(&PickUpPositionUW, item, l))
				{
					if (item->object_number == FLARE_ITEM)
					{
						l->anim_number = ANIM_PICKUPF_UW;
						l->current_anim_state = AS_FLAREPICKUP;
						l->fallspeed = 0;
					}
					else
					{
						l->anim_number = ANIM_PICKUP_UW;
						l->current_anim_state = AS_PICKUP;
					}

					l->goal_anim_state = AS_TREAD;
					l->frame_number = anims[l->anim_number].frame_base;
					lara.IsMoving = 0;
					lara.gun_status = LG_HANDS_BUSY;
				}

				lara.GeneralPtr = (void*)item_number;
			}
			else if (lara.IsMoving && lara.GeneralPtr == (void*)item_number)
			{
				lara.IsMoving = 0;
				lara.gun_status = LG_NO_ARMS;
			}
		}
		else if (lara.GeneralPtr == (void*)item_number && l->current_anim_state == AS_PICKUP &&
			l->frame_number == anims[ANIM_PICKUP_UW].frame_base + 18)
		{
			AddDisplayPickup(item->object_number);

			if (!(item->trigger_flags & 0xC0))
				KillItem(item_number);
			else
			{
				item->item_flags[3] = 1;
				item->flags |= IFL_TRIGGERED;
				item->status = ITEM_INVISIBLE;
			}
		}
		else if (lara.GeneralPtr == (void*)item_number && l->current_anim_state == AS_FLAREPICKUP &&
			l->frame_number == anims[ANIM_PICKUPF_UW].frame_base + 20)
		{
			lara.request_gun_type = WEAPON_FLARE;
			lara.gun_type = WEAPON_FLARE;
			InitialiseNewWeapon();
			lara.gun_status = LG_FLARE;
			lara.flare_age = (long)item->data & 0x7FFF;
			draw_flare_meshes();
			KillItem(item_number);
		}
	}

	item->pos.x_rot = rotx;
	item->pos.y_rot = roty;
	item->pos.z_rot = rotz;
}

void PuzzleHoleCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	PHD_VECTOR pos;
	short* bounds;
	long PuzzleType, hole, puzzle;
	short yrot;

	PuzzleType = 0;
	item = &items[item_num];

	if (item->trigger_flags < 0)
		PuzzleType = 1;
	else if (item->trigger_flags > 1024)
		PuzzleType = 2;
	else if (item->trigger_flags && item->trigger_flags != 999)
		PuzzleType = 3;

	if (((input & IN_ACTION || GLOBAL_inventoryitemchosen != NO_ITEM) &&
		(!BinocularRange && lara.gun_status == LG_NO_ARMS && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH)) ||
		(lara.IsMoving && lara.GeneralPtr == (void*)item_num))
	{
		bounds = GetBoundsAccurate(item);
		yrot = item->pos.y_rot;
		PuzzleBounds[0] = bounds[0] - 256;
		PuzzleBounds[1] = bounds[1] + 256;
		PuzzleBounds[4] = bounds[4] - 256;
		PuzzleBounds[5] = bounds[5] + 256;

		if (PuzzleType == 2)
		{
			PuzzleBounds[0] -= 1024;
			PuzzleBounds[1] += 1024;
			PuzzleBounds[4] -= 1024;
			PuzzleBounds[5] += 1024;
			item->pos.y_rot = l->pos.y_rot;
		}

		if (TestLaraPosition(PuzzleBounds, item, l))
		{
			pos.x = 0;
			pos.y = 0;
			pos.z = 0;
			hole = item->object_number - PUZZLE_HOLE1;
			puzzle = GLOBAL_inventoryitemchosen - PUZZLE_ITEM1;

			if (!lara.IsMoving)
			{
				if (GLOBAL_inventoryitemchosen == NO_ITEM)
				{
					if (have_i_got_object(short(hole + PUZZLE_ITEM1)))
						GLOBAL_enterinventory = hole + PUZZLE_ITEM1;

					item->pos.y_rot = yrot;
					return;
				}
				
				if (puzzle != hole)
				{
					item->pos.y_rot = yrot;
					return;
				}
			}

			pos.z = bounds[4] - 100;

			if ((PuzzleType == 2 && item->trigger_flags != 1036) || MoveLaraPosition(&pos, item, l))
			{
				remove_inventory_item(short(hole + PUZZLE_ITEM1));

				if (PuzzleType == 1)
				{
					l->anim_number = -item->trigger_flags;
					l->current_anim_state = AS_CONTROLLED;

					if (l->anim_number != 423)
						PuzzleDone(item, item_num);
				}
				else if (PuzzleType == 2)
				{
					cutseq_num = item->trigger_flags - 1024;
					PuzzleDone(item, item_num);
				}
				else
				{
					l->anim_number = ANIM_USEPUZZLE;
					l->current_anim_state = AS_USEPUZZLE;
					item->item_flags[0] = 1;
				}

				l->frame_number = anims[l->anim_number].frame_base;
				lara.IsMoving = 0;
				lara.head_x_rot = 0;
				lara.head_y_rot = 0;
				lara.torso_x_rot = 0;
				lara.torso_y_rot = 0;
				lara.gun_status = LG_HANDS_BUSY;
				item->flags |= IFL_TRIGGERED;
			}

			lara.GeneralPtr = (void*)item_num;
			GLOBAL_inventoryitemchosen = NO_ITEM;
		}
		else if (lara.IsMoving && lara.GeneralPtr == (void*)item_num)
		{
			lara.IsMoving = 0;
			lara.gun_status = LG_NO_ARMS;
		}

		item->pos.y_rot = yrot;
	}
	else
	{
		if (lara.GeneralPtr == (void*)item_num && l->current_anim_state == AS_USEPUZZLE &&
			l->frame_number == anims[ANIM_USEPUZZLE].frame_base + 80 && item->item_flags[0])
		{
			if (PuzzleType == 3)
				l->item_flags[0] = item->trigger_flags;
			else
				l->item_flags[0] = 0;

			PuzzleDone(item, item_num);
			item->item_flags[0] = 0;
		}
		else if (lara.GeneralPtr == (void*)item_num && l->current_anim_state == AS_CONTROLLED &&
			l->anim_number == 423 && l->frame_number == anims[423].frame_base + 180)
			PuzzleDone(item, item_num);
		else if (l->current_anim_state != AS_CONTROLLED && PuzzleType != 2)
			ObjectCollision(item_num, l, coll);
	}
}
