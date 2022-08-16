#include "../tomb4/pch.h"
#include "laradouble.h"
#include "box.h"
#include "sound.h"
#include "control.h"
#include "lara.h"

void InitialiseLaraDouble(short item_number)
{
	InitialiseCreature(item_number);
}

void LaraDoubleControl(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	SoundEffect(SFX_METAL_SCRAPE_LOOP1, &item->pos, SFX_DEFAULT);

	if (!CreatureActive(item_number))
		return;

	if (item->hit_status)
		lara_item->hit_points += item->hit_points - 1000;

	item->hit_points = 1000;
	AnimateItem(item);
}

void inject_laradouble(bool replace)
{
	INJECT(0x00405EA0, InitialiseLaraDouble, replace);
	INJECT(0x00405EB0, LaraDoubleControl, replace);
}
