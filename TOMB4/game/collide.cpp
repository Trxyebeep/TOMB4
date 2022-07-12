#include "../tomb4/pch.h"
#include "collide.h"
#include "draw.h"
#include "objects.h"
#include "control.h"
#include "../specific/function_stubs.h"
#include "effects.h"
#include "sphere.h"

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

	rooms[0] = item->room_number;
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
						num = (dx * cy - sy * dz) >> W2V_SHIFT;

						if (rad + num + 128 >= bounds[0] && num - rad - 128 <= bounds[1])
						{
							num = (dx * sy + cy * dz) >> W2V_SHIFT;

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
			num = (dx * cy - sy * dz) >> W2V_SHIFT;

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
				num = (dx * sy + cy * dz) >> W2V_SHIFT;

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

void GenericDeadlyBoundingBoxCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	long dx, dy, dz;

	item = &items[item_number];

	if (item->status != ITEM_INVISIBLE && item->item_flags[3] && TestBoundsCollide(item, l, coll->radius))
	{
		dx = lara_item->pos.x_pos;
		dy = lara_item->pos.y_pos;
		dz = lara_item->pos.z_pos;

		if (ItemPushLara(item, l, coll, 1, 1))
		{
			lara_item->hit_points -= item->item_flags[3];
			dx -= lara_item->pos.x_pos;
			dy -= lara_item->pos.y_pos;
			dz -= lara_item->pos.z_pos;

			if ((dx || dy || dz) && TriggerActive(item))
				DoBloodSplat(l->pos.x_pos + (GetRandomControl() & 0x3F) - 32, l->pos.y_pos - (GetRandomControl() & 0x1FF) - 256, l->pos.z_pos + (GetRandomControl() & 0x3F) - 32, (item->item_flags[3] >> 5) + (GetRandomControl() & 0x3) + 2, (short)(2 * GetRandomControl()), l->room_number);

			if (!coll->enable_baddie_push)
			{
				lara_item->pos.x_pos += dx;
				lara_item->pos.y_pos += dy;
				lara_item->pos.z_pos += dz;
			}
		}
	}
}

void GenericSphereBoxCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	SPHERE* sptr;
	long TouchBits, DeadlyBits, dx, dy, dz;
	short y_rot;

	item = &items[item_number];

	if (item->status != ITEM_INVISIBLE && TestBoundsCollide(item, l, coll->radius))
	{
		TouchBits = TestCollision(item, l);

		if (TouchBits)
		{
			y_rot = item->pos.y_rot;
			item->pos.y_rot = 0;
			GetSpheres(item, Slist, 1);
			item->pos.y_rot = y_rot;
			DeadlyBits = *(long*)&item->item_flags[0];

			if (item->item_flags[2])
				TouchBits &= ~0x1;

			if (TouchBits)
			{
				sptr = Slist;

				do
				{
					if (TouchBits & 0x1)
					{
						GlobalCollisionBounds[0] = (short)(sptr->x - item->pos.x_pos - sptr->r);
						GlobalCollisionBounds[2] = (short)(sptr->y - item->pos.y_pos - sptr->r);
						GlobalCollisionBounds[4] = (short)(sptr->z - item->pos.z_pos - sptr->r);
						GlobalCollisionBounds[1] = (short)(sptr->x - item->pos.x_pos + sptr->r);
						GlobalCollisionBounds[3] = (short)(sptr->y - item->pos.y_pos + sptr->r);
						GlobalCollisionBounds[5] = (short)(sptr->z - item->pos.z_pos + sptr->r);
						dx = lara_item->pos.x_pos;
						dy = lara_item->pos.y_pos;
						dz = lara_item->pos.z_pos;

						if (ItemPushLara(item, l, coll, DeadlyBits & 0x1, 3) && DeadlyBits & 0x1)
						{
							lara_item->hit_points -= item->item_flags[3];
							dx -= lara_item->pos.x_pos;
							dy -= lara_item->pos.y_pos;
							dz -= lara_item->pos.z_pos;

							if ((dx || dy || dz) && TriggerActive(item))
								DoBloodSplat(l->pos.x_pos + (GetRandomControl() & 0x3F) - 32, sptr->y + (GetRandomControl() & 0x1F) - 16, l->pos.z_pos + (GetRandomControl() & 0x3F) - 32, (item->item_flags[3] >> 5) + (GetRandomControl() & 0x3) + 2, (short)(2 * GetRandomControl()), l->room_number);

							if (!coll->enable_baddie_push)
							{
								lara_item->pos.x_pos += dx;
								lara_item->pos.y_pos += dy;
								lara_item->pos.z_pos += dz;
							}
						}
					}

					TouchBits >>= 1;
					sptr++;
					DeadlyBits >>= 1;
				} while (TouchBits);
			}
		}
	}
}

void inject_collide(bool replace)
{
	INJECT(0x00446F70, ShiftItem, replace);
	INJECT(0x00448DA0, GetCollidedObjects, replace);
	INJECT(0x00448840, GenericDeadlyBoundingBoxCollision, replace);
	INJECT(0x004485A0, GenericSphereBoxCollision, replace);
}
