#include "../tomb4/pch.h"
#include "init.h"
#include "objects.h"

void InitialiseMapper(short item_number)
{
	items[item_number].mesh_bits = 0xFFFFFFFD;	//hide laser
}

void InitialiseLightningConductor(short item_number)
{
	ITEM_INFO* item;
	long pack;

	item = &items[item_number];

	if (item->trigger_flags == 2)
	{
		pack = 0;

		for (int i = 0; i < level_items; i++)
		{
			if (items[i].object_number == ANIMATING8)
			{
				item->item_flags[2] |= i << (pack != 0 ? 8 : 0);
				pack++;
			}
		}
	}
}

void inject_init(bool replace)
{
	INJECT(0x004537D0, InitialiseMapper, replace);
	INJECT(0x00453800, InitialiseLightningConductor, replace);
}
