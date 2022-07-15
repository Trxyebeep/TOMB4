#include "../tomb4/pch.h"
#include "hammerhead.h"
#include "box.h"
#include "objects.h"

void InitialiseHammerhead(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[HAMMERHEAD].anim_index + 8;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = 0;
	item->goal_anim_state = 0;
}

void inject_hammerhead(bool replace)
{
	INJECT(0x00407000, InitialiseHammerhead, replace);
}
