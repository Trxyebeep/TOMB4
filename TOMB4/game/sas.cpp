#include "../tomb4/pch.h"
#include "sas.h"
#include "objects.h"
#include "../specific/function_stubs.h"
#include "control.h"

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

void inject_sas(bool replace)
{
	INJECT(0x0040DA00, InitialiseInjuredSas, replace);
	INJECT(0x0040DA70, InjuredSasControl, replace);
}
