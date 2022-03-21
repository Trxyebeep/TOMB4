#include "../tomb4/pch.h"
#include "jeep.h"
#include "../specific/function_stubs.h"

void InitialiseJeep(short item_number)
{
	ITEM_INFO* item;
	JEEPINFO* jeep;

	item = &items[item_number];
	jeep = (JEEPINFO*)game_malloc(sizeof(JEEPINFO));
	item->data = jeep;
	jeep->velocity = 0;
	jeep->unused1 = 0;
	jeep->turn_rate = 0;
	jeep->camera_angle = 0;
	jeep->extra_rotation = 0;
	jeep->move_angle = item->pos.y_rot;
	jeep->pitch2 = 0;
	jeep->flags = 0;
	jeep->gear = 0;
	jeep->right_front_wheelrot = 0;
	jeep->left_front_wheelrot = 0;
	jeep->left_back_wheelrot = 0;
	jeep->right_back_wheelrot = 0;
	item->mesh_bits = 0x1BFFF;
}

void inject_jeep(bool replace)
{
	INJECT(0x00466F40, InitialiseJeep, replace);
}
