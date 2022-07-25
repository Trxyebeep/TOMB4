#include "../tomb4/pch.h"
#include "lot.h"
#include "../specific/function_stubs.h"

void InitialiseLOTarray(long allocmem)
{
	CREATURE_INFO* creature;

	if (allocmem)
		baddie_slots = (CREATURE_INFO*)game_malloc(5 * sizeof(CREATURE_INFO));

	for (int i = 0; i < 5; i++)
	{
		creature = &baddie_slots[i];
		creature->item_num = NO_ITEM;

		if (allocmem)
			creature->LOT.node = (BOX_NODE*)game_malloc(sizeof(BOX_NODE) * num_boxes);
	}

	slots_used = 0;
}

void DisableBaddieAI(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* creature;

	item = &items[item_number];
	creature = (CREATURE_INFO*)item->data;
	item->data = 0;

	if (creature)
	{
		creature->item_num = NO_ITEM;
		slots_used--;
	}
}

void ClearLOT(LOT_INFO* lot)
{
	BOX_NODE* node;
	long i;

	i = 0;
	lot->tail = 2047;
	lot->head = 2047;
	lot->search_number = 0;
	lot->target_box = 2047;
	lot->required_box = 2047;

	for (node = lot->node; i < num_boxes; node)
	{
		node->next_expansion = 2047;
		node->exit_box = 2047;
		node->search_number = 0;
		i++;
	}
}

void inject_lot(bool replace)
{
	INJECT(0x00455200, InitialiseLOTarray, replace);
	INJECT(0x00455270, DisableBaddieAI, replace);
	INJECT(0x004557B0, ClearLOT, replace);
}
