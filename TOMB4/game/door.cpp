#include "../tomb4/pch.h"
#include "door.h"

void ShutThatDoor(DOORPOS_DATA* d)
{
	CREATURE_INFO* cinfo;

	if (d->floor)
	{
		d->floor->box = -1;
		d->floor->ceiling = -127;
		d->floor->floor = -127;
		d->floor->index = 0;
		d->floor->pit_room = -1;
		d->floor->sky_room = -1;

		if (d->block != 2047)
		{
			boxes[d->block].overlap_index |= 0x4000;

			for (short slot = 0; slot < 5; slot++)
			{
				cinfo = &baddie_slots[slot];
				cinfo->LOT.target_box = 2047;
			}
		}
	}
}

void OpenThatDoor(DOORPOS_DATA* d)
{
	CREATURE_INFO* cinfo;

	if (d->floor)
	{
		*d->floor = d->data;

		if (d->block != 2047)
		{
			boxes[d->block].overlap_index |= ~0x4000;

			for (short slot = 0; slot < 5; slot++)
			{
				cinfo = &baddie_slots[slot];
				cinfo->LOT.target_box = 2047;
			}
		}
	}
}

void inject_door(bool replace)
{
	INJECT(0x0044DF60, ShutThatDoor, replace);
	INJECT(0x0044DFC0, OpenThatDoor, replace);
}
