#include "../tomb4/pch.h"
#include "templar.h"
#include "box.h"
#include "objects.h"

void InitialiseTemplar(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[KNIGHTS_TEMPLAR].anim_index + 2;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = 1;
	item->goal_anim_state = 1;
	item->mesh_bits &= ~0x800;
}

void inject_templar(bool replace)
{
	INJECT(0x00413020, InitialiseTemplar, replace);
}
