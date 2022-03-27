#include "../tomb4/pch.h"
#include "switch.h"
#include "lara_states.h"
#include "collide.h"
#include "items.h"
#include "control.h"

static short FullBlockSwitchBounds[12] = { -384, 384, 0, 256, 0, 512, -1820, 1820, -5460, 5460, -1820, 1820 };
#ifdef GENERAL_FIXES
static PHD_VECTOR FullBlockSwitchPos = { 0, 256, 0 };
#else
static PHD_VECTOR FullBlockSwitchPos = { 0, 256, 100 };
#endif

void FullBlockSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (input & IN_ACTION && item->status == ITEM_INACTIVE && !(item->flags & IFL_INVISIBLE) && CurrentSequence < 3 && lara.gun_status == LG_NO_ARMS &&
		l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH || lara.IsMoving && lara.GeneralPtr == (void*)item_number)
	{
		if (TestLaraPosition(FullBlockSwitchBounds, item, l))
		{
			if (MoveLaraPosition(&FullBlockSwitchPos, item, l))
			{
				if (item->current_anim_state == 1)
				{
					l->current_anim_state = AS_SWITCHON;
					l->anim_number = ANIM_BLOCKSWITCH;
					item->goal_anim_state = 0;
				}

				l->goal_anim_state = AS_STOP;
				l->frame_number = anims[l->anim_number].frame_base;
				item->status = ITEM_ACTIVE;
				AddActiveItem(item_number);
				AnimateItem(item);
				lara.IsMoving = 0;
				lara.head_y_rot = 0;
				lara.head_x_rot = 0;
				lara.torso_y_rot = 0;
				lara.torso_x_rot = 0;
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
}

void inject_switch(bool replace)
{
	INJECT(0x00463180, FullBlockSwitchCollision, replace);
}
