#include "../tomb4/pch.h"
#include "collide.h"
#include "draw.h"
#include "objects.h"

void ShiftItem(ITEM_INFO* item, COLL_INFO* coll)
{
	item->pos.x_pos += coll->shift.x;
	item->pos.y_pos += coll->shift.y;
	item->pos.z_pos += coll->shift.z;
	coll->shift.z = 0;
	coll->shift.y = 0;
	coll->shift.x = 0;
}

long GetCollidedObjects(ITEM_INFO* item, long rad, long noInvisible, ITEM_INFO** StoredItems, MESH_INFO** StoredStatics, long StoreLara)
{
	MESH_INFO* mesh;
	ROOM_INFO* r;
	ITEM_INFO* item2;
	short* doors;
	short* bounds;
	long j, sy, cy, dx, dy, dz, num;
	short rooms[22];
	short switch_bounds[6];
	short room_count, statics_count, items_count, item_number, next_item;

	rooms[0] = camera.pos.room_number;
	r = &room[rooms[0]];
	doors = r->door;
	room_count = 1;
	statics_count = 0;
	items_count = 0;

	if (doors)
	{
		for (int i = *doors++; i > 0; i--, doors += 16)
		{
			for (j = 0; j < room_count; j++)
				if (rooms[j] == *doors)
					break;

			if (j == room_count)
			{
				rooms[room_count] = *doors;
				room_count++;
			}
		}
	}

	if (StoredStatics)
	{
		for (int i = 0; i < room_count; i++)
		{
			r = &room[rooms[i]];
			mesh = r->mesh;

			for (j = r->num_meshes; j > 0; j--, mesh++)
			{
				if (mesh->Flags & 1)
				{
					bounds = &static_objects[mesh->static_number].x_minc;

					if (item->pos.y_pos + rad + 128 >= mesh->y + bounds[2] && item->pos.y_pos - rad - 128 <= mesh->y + bounds[3])
					{
						sy = phd_sin(mesh->y_rot);
						cy = phd_cos(mesh->y_rot);
						dx = item->pos.x_pos - mesh->x;
						dz = item->pos.z_pos - mesh->z;
						num = (dx * cy - sy * dz) >> 14;

						if (rad + num + 128 >= bounds[0] && num - rad - 128 <= bounds[1])
						{
							num = (dx * sy + cy * dz) >> 14;

							if (rad + num + 128 >= bounds[4] && num - rad - 128 <= bounds[5])
							{
								StoredStatics[statics_count] = mesh;
								statics_count++;

								if (!rad)
								{
									StoredItems[0] = 0;
									return 1;
								}
							}
						}
					}
				}
			}
		}

		StoredStatics[statics_count] = 0;
	}

	for (int i = 0; i < room_count; i++)
	{
		item_number = room[rooms[i]].item_number;

		while (item_number != NO_ITEM)
		{
			item2 = &items[item_number];
			next_item = item2->next_item;

			if (item2 == item)	//don't get the item we're checking for
			{
				item_number = next_item;
				continue;
			}

			if (!StoreLara && item2 == lara_item)	//don't get lara if we don't want her
			{
				item_number = next_item;
				continue;
			}

			if (item2->flags & IFL_CLEARBODY)	//don't get clearbodies
			{
				item_number = next_item;
				continue;
			}

			if (item2->object_number == BURNING_FLOOR || !objects[item2->object_number].collision && item2->object_number != LARA)	//don't get objects without collision
			{
				item_number = next_item;
				continue;
			}

#ifdef GENERAL_FIXES
			if (!objects[item2->object_number].draw_routine && item2->object_number != LARA || !item2->mesh_bits)
				//don't get objects that are not drawn
				//checking for LARA object num fixes Lara not being stored, which fixes nades not harming her.
#else
			if (!objects[item2->object_number].draw_routine || !item2->mesh_bits)	//don't get objects that are not drawn
#endif
			{
				item_number = next_item;
				continue;
			}

			if (noInvisible && item2->status == ITEM_INVISIBLE)	//don't get invisibles if we don't want them
			{
				item_number = next_item;
				continue;
			}

			dx = item->pos.x_pos - item2->pos.x_pos;
			dy = item->pos.y_pos - item2->pos.y_pos;
			dz = item->pos.z_pos - item2->pos.z_pos;

			if (dx < -2048 || dx > 2048 || dy < -2048 || dy > 2048 || dz < -2048 || dz > 2048)	//further than 2 blocks? bye
			{
				item_number = next_item;
				continue;
			}

			bounds = GetBestFrame(item2);

			if (item->pos.y_pos + rad + 128 < item2->pos.y_pos + bounds[2] || item->pos.y_pos - rad - 128 > item2->pos.y_pos + bounds[3])
			{
				item_number = next_item;
				continue;
			}

			sy = phd_sin(item2->pos.y_rot);
			cy = phd_cos(item2->pos.y_rot);
			dx = item->pos.x_pos - item2->pos.x_pos;
			dz = item->pos.z_pos - item2->pos.z_pos;
			num = (dx * cy - sy * dz) >> 14;

			if (item2->object_number == TURN_SWITCH)
			{
				switch_bounds[0] = -256;
				switch_bounds[1] = 256;
				switch_bounds[4] = -256;
				switch_bounds[5] = 256;
				bounds = switch_bounds;
			}

			if (rad + num + 128 >= bounds[0] && num - rad - 128 <= bounds[1])
			{
				num = (dx * sy + cy * dz) >> 14;

				if (rad + num + 128 >= bounds[4] && num - rad - 128 <= bounds[5])
				{
					StoredItems[items_count] = item2;
					items_count++;

					if (!rad)
						return 1;
				}
			}

			item_number = next_item;
		}
	}

	StoredItems[items_count] = 0;
	return items_count | statics_count;
}

void inject_collide(bool replace)
{
	INJECT(0x00446F70, ShiftItem, replace);
	INJECT(0x00448DA0, GetCollidedObjects, replace);
}
