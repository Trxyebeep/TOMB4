#include "../tomb4/pch.h"
#include "troops.h"
#include "box.h"
#include "objects.h"

void InitialiseTroop(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);

	if (item->trigger_flags == 1)
	{
		item->anim_number = objects[TROOPS].anim_index + 27;
		item->current_anim_state = 16;
		item->goal_anim_state = 16;
	}
	else
	{
		item->anim_number = objects[TROOPS].anim_index + 12;
		item->current_anim_state = 1;
		item->goal_anim_state = 1;
	}

	item->frame_number = anims[item->anim_number].frame_base;
}

void inject_troops(bool replace)
{
	INJECT(0x004183B0, InitialiseTroop, replace);
}
