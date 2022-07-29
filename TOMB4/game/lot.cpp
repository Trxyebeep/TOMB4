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

	lot->tail = 2047;
	lot->head = 2047;
	lot->search_number = 0;
	lot->target_box = 2047;
	lot->required_box = 2047;
	node = lot->node;

	for (int i = 0; i < num_boxes; i++)
	{
		node->next_expansion = 2047;
		node->exit_box = 2047;
		node->search_number = 0;
		node++;
	}
}

void CreateZone(ITEM_INFO* item)
{
	CREATURE_INFO* creature;
	ROOM_INFO* r;
	BOX_NODE* node;
	short* zone;
	short* flip;
	short zone_number, flip_number;

	creature = (CREATURE_INFO*)item->data;
	r = &room[item->room_number];
	item->box_number = r->floor[((item->pos.z_pos - r->z) >> 10) + r->x_size * ((item->pos.x_pos - r->x) >> 10)].box;

	if (creature->LOT.fly)
	{
		creature->LOT.zone_count = 0;
		node = creature->LOT.node;

		for (int i = 0; i < num_boxes; i++)
		{
			node->box_number = i;
			node++;
			creature->LOT.zone_count++;
		}
	}
	else
	{
		zone = ground_zone[creature->LOT.zone][0];
		flip = ground_zone[creature->LOT.zone][1];
		zone_number = zone[item->box_number];
		flip_number = flip[item->box_number];
		creature->LOT.zone_count = 0;
		node = creature->LOT.node;

		for (int i = 0; i < num_boxes; i++)
		{
			if (*zone == zone_number || *flip == flip_number)
			{
				node->box_number = i;
				node++;
				creature->LOT.zone_count++;
			}

			zone++;
			flip++;
		}
	}
}

void inject_lot(bool replace)
{
	INJECT(0x00455200, InitialiseLOTarray, replace);
	INJECT(0x00455270, DisableBaddieAI, replace);
	INJECT(0x004557B0, ClearLOT, replace);
	INJECT(0x004556A0, CreateZone, replace);
}
