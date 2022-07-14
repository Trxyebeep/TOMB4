#include "../tomb4/pch.h"
#include "sas.h"
#include "objects.h"
#include "../specific/function_stubs.h"
#include "control.h"
#include "lara_states.h"
#include "switch.h"
#include "items.h"
#include "collide.h"
#include "sphere.h"
#include "tomb4fx.h"

static short DragSASBounds[12] = { -256, 256, -100, 100, -512, -460, -1820, 1820, -5460, 5460, 0, 0 };
static PHD_VECTOR DragSASPos = { 0, 0, -460 };
static BITE_INFO sas_fire = { 0, 300, 64, 7 };

void InitialiseInjuredSas(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->trigger_flags)
	{
		item->anim_number = objects[SAS_DYING].anim_index;
		item->current_anim_state = 1;
		item->goal_anim_state = 1;
	}
	else
	{
		item->anim_number = objects[SAS_DYING].anim_index + 3;
		item->current_anim_state = 4;
		item->goal_anim_state = 4;
	}

	item->frame_number = anims[item->anim_number].frame_base;
}

void InjuredSasControl(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->current_anim_state == 1)
	{
		if (!(GetRandomControl() & 0x7F))
			item->goal_anim_state = 2;
		else if (!(GetRandomControl() & 0xFF))
			item->goal_anim_state = 3;
	}
	else if (item->current_anim_state == 4 && !(GetRandomControl() & 0x7F))
		item->goal_anim_state = 5;

	AnimateItem(item);
}

void DragSASCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	long x, z;

	item = &items[item_number];

	if (input & IN_ACTION && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH && lara.gun_status == LG_NO_ARMS &&
		!l->gravity_status && !(item->flags & IFL_CODEBITS) || lara.IsMoving && lara.GeneralPtr == (void*)item_number)
	{
		if (TestLaraPosition(DragSASBounds, item, l))
		{
			if (MoveLaraPosition(&DragSASPos, item, l))
			{
				l->anim_number = ANIM_DRAGSAS;
				l->frame_number = anims[ANIM_DRAGSAS].frame_base;
				l->current_anim_state = AS_CONTROLLED;
				l->pos.y_rot = item->pos.y_rot;
				lara.IsMoving = 0;
				lara.head_x_rot = 0;
				lara.head_y_rot = 0;
				lara.torso_x_rot = 0;
				lara.torso_y_rot = 0;
				lara.gun_status = LG_HANDS_BUSY;
				item->flags |= IFL_CODEBITS;
				item->status = ITEM_ACTIVE;
				AddActiveItem(item_number);
			}
			else
				lara.GeneralPtr = (void*)item_number;
		}
	}
	else
	{
		if (item->status == ITEM_ACTIVE)
		{
			if (item->frame_number == anims[item->anim_number].frame_end)
			{
				x = (2048 * phd_sin(l->pos.y_rot)) >> W2V_SHIFT;
				z = (2048 * phd_cos(l->pos.y_rot)) >> W2V_SHIFT;
				TestTriggersAtXYZ(l->pos.x_pos - x, l->pos.y_pos, l->pos.z_pos - z, l->room_number, 1, 0);
				RemoveActiveItem(item_number);
				item->status = ITEM_INACTIVE;
			}
		}

		ObjectCollision(item_number, l, coll);
	}
}

static void SasFireGrenade(ITEM_INFO* sas, short xrot, short yrot)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	PHD_VECTOR pos;
	PHD_VECTOR oPos;
	long h;
	short item_number;

	item_number = CreateItem();

	if (item_number == NO_ITEM)
		return;

	item = &items[item_number];
	item->shade = -0x3DF0;
	item->object_number = GRENADE;
	item->room_number = sas->room_number;
	pos.x = sas_fire.x;
	pos.y = sas_fire.y;
	pos.z = sas_fire.z;
	GetJointAbsPosition(sas, &pos, sas_fire.mesh_num);
	item->pos.x_pos = pos.x;
	item->pos.y_pos = pos.y;
	item->pos.z_pos = pos.z;
	oPos.x = pos.x;
	oPos.y = pos.y;
	oPos.z = pos.z;
	floor = GetFloor(pos.x, pos.y, pos.z, &item->room_number);
	h = GetHeight(floor, pos.x, pos.y, pos.z);

	if (h < pos.y)
	{
		item->pos.x_pos = sas->pos.x_pos;
		item->pos.y_pos = pos.y;
		item->pos.z_pos = sas->pos.z_pos;
		item->room_number = sas->room_number;
	}

	SmokeCountL = 32;
	SmokeWeapon = WEAPON_GRENADE;

	for (int i = 0; i < 5; i++)
		TriggerGunSmoke(oPos.x, oPos.y, oPos.z, pos.x - oPos.x, pos.y - oPos.y, pos.z - oPos.z, 1, 5, 32);

	InitialiseItem(item_number);
	item->pos.x_rot = xrot + sas->pos.x_rot;
	item->pos.y_rot = yrot + sas->pos.y_rot;
	item->pos.z_rot = 0;

	if (GetRandomControl() & 3)
		item->item_flags[0] = 1;
	else
		item->item_flags[0] = 2;

	item->item_flags[2] = 1;
	item->speed = 128;
	item->fallspeed = (-128 * phd_sin(item->pos.x_rot)) >> W2V_SHIFT;
	item->current_anim_state = item->pos.x_rot;
	item->goal_anim_state = item->pos.y_rot;
	item->required_anim_state = 0;
	item->hit_points = 120;
	AddActiveItem(item_number);
}

void inject_sas(bool replace)
{
	INJECT(0x0040DA00, InitialiseInjuredSas, replace);
	INJECT(0x0040DA70, InjuredSasControl, replace);
	INJECT(0x0040DCD0, DragSASCollision, replace);
	INJECT(0x0040DAF0, SasFireGrenade, replace);
}
