#include "../tomb4/pch.h"
#include "collide.h"
#include "draw.h"
#include "objects.h"
#include "control.h"
#include "../specific/function_stubs.h"
#include "effects.h"
#include "sphere.h"
#include "../specific/3dmath.h"
#include "items.h"

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

void CreatureCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	short* bounds;
	long x, z, rx, rz, c, s;

	item = &items[item_number];

	if (TestBoundsCollide(item, l, coll->radius) && TestCollision(item, l))
	{
#ifdef GENERAL_FIXES
		if (lara.water_status != LW_UNDERWATER && lara.water_status != LW_SURFACE)
#else
		if (lara.water_status != LW_UNDERWATER && !lara.water_status != LW_SURFACE)	//dumb
#endif
		{
			if (coll->enable_baddie_push)
				ItemPushLara(item, l, coll, coll->enable_spaz, 0);
			else if (coll->enable_spaz)
			{
				bounds = GetBestFrame(item);
				s = phd_sin(l->pos.y_rot);
				c = phd_cos(l->pos.y_rot);
				x = (bounds[0] + bounds[1]) >> 1;
				z = (bounds[3] - bounds[2]) >> 1;
				rx = (l->pos.x_pos - item->pos.x_pos) - ((c * x + s * z) >> W2V_SHIFT);
				rz = (l->pos.z_pos - item->pos.z_pos) - ((c * z - s * x) >> W2V_SHIFT);

				if (bounds[3] - bounds[2] > 256)
				{
					lara.hit_direction = ushort((l->pos.y_rot - phd_atan(rz, rx) - 0x6000)) >> W2V_SHIFT;
					lara.hit_frame++;

					if (lara.hit_frame > 30)
						lara.hit_frame = 30;
				}
			}
		}
	}
}

long FindGridShift(long src, long dst)
{
	long srcw, dstw;

	srcw = src >> 10;
	dstw = dst >> 10;

	if (srcw == dstw)
		return 0;

	src &= 1023;

	if (dstw > srcw)
		return 1025 - src;
	else
		return -1 - src;
}

short GetTiltType(FLOOR_INFO* floor, long x, long y, long z)
{
	ROOM_INFO* r;
	short* data;
	short type, t0, t1, t2, t3, tilt, x2, z2, x3, y2;

	while (floor->pit_room != 255)
	{
		if (CheckNoColFloorTriangle(floor, x, z) == 1)
			break;

		r = &room[floor->pit_room];
		floor = &r->floor[((z - r->z) >> 10) + (((x - r->x) >> 10) * r->x_size)];
	}

	if (y + 512 < floor->floor << 8)
		return 0;

	if (floor->index)
	{
		data = &floor_data[floor->index];
		type = (data[0] & 0x1F);

		if (type == TILT_TYPE)
			return data[1];

		if (type == SPLIT1 || type == SPLIT2 || type == NOCOLF1T || type == NOCOLF2T || type == NOCOLF1B || type == NOCOLF2B)
		{
			tilt = data[1];
			t0 = tilt & 0xF;
			t1 = (tilt >> 4) & 0xF;
			t2 = (tilt >> 8) & 0xF;
			t3 = (tilt >> 12) & 0xF;
			x2 = x & 0x3FF;
			z2 = z & 0x3FF;
			type = type & 0x1F;

			if (type == SPLIT1 || type == NOCOLF1T || type == NOCOLF1B)
			{
				if (x2 > 1024 - z2)
				{
					x3 = t3 - t0;
					y2 = t3 - t2;
				}
				else
				{
					x3 = t2 - t1;
					y2 = t0 - t1;
				}
			}
			else if (x2 > z2)
			{
				x3 = t3 - t0;
				y2 = t0 - t1;
			}
			else
			{
				x3 = t2 - t1;
				y2 = t3 - t2;
			}

			return ((x3 << 8) | (y2 & 0xFF));
		}
	}

	return 0;
}

long CollideStaticObjects(COLL_INFO* coll, long x, long y, long z, short room_number, long hite)
{
	ROOM_INFO* r;
	MESH_INFO* mesh;
	STATIC_INFO* sinfo;
	short* door;
	long lxmin, lxmax, lymin, lymax, lzmin, lzmax;
	long xmin, xmax, ymin, ymax, zmin, zmax;
	long i, j;
	short num_nearby_rooms;
	short nearby_rooms[22];

	coll->hit_static = 0;
	lxmin = x - coll->radius;
	lxmax = x + coll->radius;
	lymin = y - hite;
	lymax = y;
	lzmin = z - coll->radius;
	lzmax = z + coll->radius;
	num_nearby_rooms = 1;
	nearby_rooms[0] = room_number;
	door = room[room_number].door;

	if (door)
	{
		for (i = *door++; i > 0; i--)
		{
			for (j = 0; j < num_nearby_rooms; j++)
			{
				if (nearby_rooms[j] == *door)
					break;
			}

			if (j == num_nearby_rooms)
			{
				nearby_rooms[num_nearby_rooms] = *door;
				num_nearby_rooms++;
			}

			door += 16;
		}
	}

	for (i = 0; i < num_nearby_rooms; i++)
	{
		r = &room[nearby_rooms[i]];
		mesh = r->mesh;

		for (j = r->num_meshes; j > 0; j--, mesh++)
		{
			sinfo = &static_objects[mesh->static_number];

			if (!(mesh->Flags & 1))
				continue;

			ymin = mesh->y + sinfo->y_minc;
			ymax = mesh->y + sinfo->y_maxc;

			if (mesh->y_rot == -0x8000)
			{
				xmin = mesh->x - sinfo->x_maxc;
				xmax = mesh->x - sinfo->x_minc;
				zmin = mesh->z - sinfo->z_maxc;
				zmax = mesh->z - sinfo->z_minc;
			}
			else if (mesh->y_rot == -0x4000)
			{
				xmin = mesh->x - sinfo->z_maxc;
				xmax = mesh->x - sinfo->z_minc;
				zmin = mesh->z + sinfo->x_minc;
				zmax = mesh->z + sinfo->x_maxc;
			}
			else if (mesh->y_rot == 0x4000)
			{
				xmin = mesh->x + sinfo->z_minc;
				xmax = mesh->x + sinfo->z_maxc;
				zmin = mesh->z - sinfo->x_maxc;
				zmax = mesh->z - sinfo->x_minc;
			}
			else
			{
				xmin = mesh->x + sinfo->x_minc;
				xmax = mesh->x + sinfo->x_maxc;
				zmin = mesh->z + sinfo->z_minc;
				zmax = mesh->z + sinfo->z_maxc;
			}

			if (lxmax <= xmin || lxmin >= xmax || lymax <= ymin || lymin >= ymax || lzmax <= zmin || lzmin >= zmax)
				continue;

			coll->hit_static = 1;
			return 1;
		}
	}

	return 0;
}

void UpdateLaraRoom(ITEM_INFO* item, long height)
{
	FLOOR_INFO* floor;
	long x, y, z;
	short room_number;

	x = item->pos.x_pos;
	y = item->pos.y_pos + height;
	z = item->pos.z_pos;
	room_number = item->room_number;
	floor = GetFloor(x, y, z, &room_number);
	item->floor = GetHeight(floor, x, y, z);

	if (item->room_number != room_number)
		ItemNewRoom(lara.item_number, room_number);

	for (int i = 0; i < 255; i++)
	{
		if (Map[i].room_number == lara_item->room_number)
		{
			Map[i].visited = 1;
			break;
		}
	}
}

void LaraBaddieCollision(ITEM_INFO* l, COLL_INFO* coll)
{
	ROOM_INFO* r;
	ITEM_INFO* item;
	MESH_INFO* mesh;
	PHD_3DPOS pos;
	short* door;
	short* bounds;
	long i, j, dx, dy, dz;
	short num_nearby_rooms, item_number, nex;
	short nearby_rooms[22];

	l->hit_status = 0;
	lara.hit_direction = -1;

	if (l->hit_points <= 0)
		return;

	num_nearby_rooms = 1;
	nearby_rooms[0] = l->room_number;
	door = room[nearby_rooms[0]].door;

	if (door)
	{
		for (i = *door++; i > 0; i--)
		{
			for (j = 0; j < num_nearby_rooms; j++)
			{
				if (nearby_rooms[j] == *door)
					break;
			}

			if (j == num_nearby_rooms)
			{
				nearby_rooms[num_nearby_rooms] = *door;
				num_nearby_rooms++;
			}

			door += 16;
		}
	}

	for (i = 0; i < num_nearby_rooms; i++)
	{
		r = &room[nearby_rooms[i]];
		item_number = r->item_number;

		while (item_number != NO_ITEM)
		{
			item = &items[item_number];
			nex = item->next_item;

			if (item->collidable && item->status != ITEM_INVISIBLE)
			{
				if (objects[item->object_number].collision)
				{
					dx = l->pos.x_pos - item->pos.x_pos;
					dy = l->pos.y_pos - item->pos.y_pos;
					dz = l->pos.z_pos - item->pos.z_pos;

					if (dx > -3072 && dx < 3072 && dy > -3072 && dy < 3072 && dz > -3072 && dz < 3072)
						objects[item->object_number].collision(item_number, l, coll);
				}
			}

			item_number = nex;
		}

		if (coll->enable_baddie_push)
		{
			r = &room[nearby_rooms[i]];
			mesh = r->mesh;

			for (j = r->num_meshes; j > 0; j--, mesh++)
			{
				if (!(mesh->Flags & 1))
					continue;

				dx = l->pos.x_pos - mesh->x;
				dy = l->pos.y_pos - mesh->y;
				dz = l->pos.z_pos - mesh->z;

				if (dx > -3072 && dx < 3072 && dy > -3072 && dy < 3072 && dz > -3072 && dz < 3072)
				{
					bounds = &static_objects[mesh->static_number].x_minc;
					pos.x_pos = mesh->x;
					pos.y_pos = mesh->y;
					pos.z_pos = mesh->z;
					pos.y_rot = mesh->y_rot;

					if (TestBoundsCollideStatic(bounds, &pos, coll->radius))
						ItemPushLaraStatic(l, bounds, &pos, coll);
				}
			}
		}
	}

	if (lara.hit_direction == -1)
		lara.hit_frame = 0;
}

void inject_collide(bool replace)
{
	INJECT(0x00446F70, ShiftItem, replace);
	INJECT(0x00448DA0, GetCollidedObjects, replace);
	INJECT(0x00448840, GenericDeadlyBoundingBoxCollision, replace);
	INJECT(0x004485A0, GenericSphereBoxCollision, replace);
	INJECT(0x00447470, CreatureCollision, replace);
	INJECT(0x00446CF0, FindGridShift, replace);
	INJECT(0x00447010, GetTiltType, replace);
	INJECT(0x00446D20, CollideStaticObjects, replace);
	INJECT(0x00446FB0, UpdateLaraRoom, replace);
	INJECT(0x004471B0, LaraBaddieCollision, replace);
}
