#include "../tomb4/pch.h"
#include "sas.h"
#include "objects.h"
#include "../specific/function_stubs.h"
#include "control.h"
#include "lara_states.h"
#include "switch.h"
#include "items.h"
#include "collide.h"

static short DragSASBounds[12] = { -256, 256, -100, 100, -512, -460, -1820, 1820, -5460, 5460, 0, 0 };
static PHD_VECTOR DragSASPos = { 0, 0, -460 };

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

void inject_sas(bool replace)
{
	INJECT(0x0040DA00, InitialiseInjuredSas, replace);
	INJECT(0x0040DA70, InjuredSasControl, replace);
	INJECT(0x0040DCD0, DragSASCollision, replace);
}
