#include "../tomb4/pch.h"
#include "jean.h"
#include "objects.h"
#include "../specific/function_stubs.h"
#include "control.h"
#include "lara.h"

void InitialiseJeanYves(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->goal_anim_state = 1;
	item->current_anim_state = 1;
	item->anim_number = objects[JEAN_YVES].anim_index;
	item->frame_number = anims[item->anim_number].frame_base;
}

void JeanYvesControl(short item_number)
{
	ITEM_INFO* item;
	short random;

	item = &items[item_number];

	if (item->trigger_flags < lara.highest_location)
	{
		if (lara.highest_location > 3)
			lara.highest_location = 3;

		random = (GetRandomControl() & 3) + 4 * lara.highest_location;
		item->goal_anim_state = random + 1;
		item->current_anim_state = item->goal_anim_state;
		item->anim_number = objects[JEAN_YVES].anim_index + random;
		item->frame_number = anims[item->anim_number].frame_base;
		item->trigger_flags = lara.highest_location;
	}
	else
	{
		if (GetRandomControl() & 3)
			item->goal_anim_state = (GetRandomControl() & 1) + 1;
		else
			item->goal_anim_state = 3 * (GetRandomControl() & 1);

		item->goal_anim_state += ((char)item->current_anim_state - 1 & 0xC) + 1;
	}

	AnimateItem(item);
}
