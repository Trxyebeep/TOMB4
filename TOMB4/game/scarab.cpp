#include "../tomb4/pch.h"
#include "scarab.h"
#include "box.h"
#include "objects.h"

void InitialiseScarab(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[BIG_BEETLE].anim_index + 3;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = 1;
	item->goal_anim_state = 1;
}

void inject_scarab(bool replace)
{
	INJECT(0x0040DE90, InitialiseScarab, replace);
}
