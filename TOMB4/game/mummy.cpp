#include "../tomb4/pch.h"
#include "mummy.h"
#include "box.h"
#include "objects.h"

void InitialiseMummy(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);

	if (item->trigger_flags == 2)
	{
		item->anim_number = objects[MUMMY].anim_index + 12;
		item->current_anim_state = 8;
		item->goal_anim_state = 8;
		item->status = ITEM_INACTIVE;
	}
	else
	{
		item->anim_number = objects[MUMMY].anim_index + 19;
		item->current_anim_state = 0;
		item->goal_anim_state = 0;
	}

	item->frame_number = anims[item->anim_number].frame_base;
}

void inject_mummy(bool replace)
{
	INJECT(0x0040A930, InitialiseMummy, replace);
}
