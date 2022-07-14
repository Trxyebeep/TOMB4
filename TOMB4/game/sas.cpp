#include "../tomb4/pch.h"
#include "sas.h"
#include "objects.h"

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

void inject_sas(bool replace)
{
	INJECT(0x0040DA00, InitialiseInjuredSas, replace);
}
