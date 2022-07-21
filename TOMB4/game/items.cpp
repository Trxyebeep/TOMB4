#include "../tomb4/pch.h"
#include "items.h"

void InitialiseItemArray(short num)
{
	ITEM_INFO* item;

	item = &items[level_items];
	next_item_free = (short)level_items;
	next_item_active = NO_ITEM;

	for (int i = level_items + 1; i < num; i++)
	{
		item->next_item = i;
		item->active = 0;
		item++;
	}

	item->next_item = NO_ITEM;
}

void inject_items(bool replace)
{
	INJECT(0x00454140, InitialiseItemArray, replace);
}
