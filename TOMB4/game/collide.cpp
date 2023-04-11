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
#include "sound.h"
#include "lara_states.h"
#include "lara.h"
#include "../specific/file.h"

static short StarGateBounds[24] =
{
	-512, 512, -1024, -896, -96, 96,
	-512, 512, -128, 0, -96, 96,
	-512, -384, -1024, 0, -96, 96,
	384, 512, -1024, 0, -96, 96
};

short GlobalCollisionBounds[6];

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

			if (!objects[item2->object_number].draw_routine && item2->object_number != LARA || !item2->mesh_bits)	//don't get objects that are not drawn
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
		if (lara.water_status != LW_UNDERWATER && lara.water_status != LW_SURFACE)
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

void ObjectCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (TestBoundsCollide(item, l, coll->radius) && TestCollision(item, l) && coll->enable_baddie_push)
		ItemPushLara(item, l, coll, 0, 1);
}

void ObjectCollisionNoBigPush(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (TestBoundsCollide(item, l, coll->radius) && TestCollision(item, l) && coll->enable_baddie_push)
		ItemPushLara(item, l, coll, 0, 0);
}

void TrapCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->status == ITEM_ACTIVE)
	{
		if (!TestBoundsCollide(item, l, coll->radius))
			return;
	}
	else if (item->status == ITEM_INVISIBLE)
		return;

	ObjectCollision(item_number, l, coll);
}

long ItemPushLara(ITEM_INFO* item, ITEM_INFO* l, COLL_INFO* coll, long spaz, long BigPush)
{
	short* bounds;
	long dx, dz, s, c, x, z;
	long xmin, xmax, zmin, zmax, left, top, right, bottom;
	short facing;

	dx = l->pos.x_pos - item->pos.x_pos;
	dz = l->pos.z_pos - item->pos.z_pos;
	s = phd_sin(item->pos.y_rot);
	c = phd_cos(item->pos.y_rot);
	x = (dx * c - dz * s) >> W2V_SHIFT;
	z = (dx * s + dz * c) >> W2V_SHIFT;

	if (BigPush & 2)
		bounds = GlobalCollisionBounds;
	else
		bounds = GetBestFrame(item);

	xmin = bounds[0];
	xmax = bounds[1];
	zmin = bounds[4];
	zmax = bounds[5];

	if (BigPush & 1)
	{
		xmin -= coll->radius;
		xmax += coll->radius;
		zmin -= coll->radius;
		zmax += coll->radius;
	}

	if (abs(dx) > 4608 || abs(dz) > 4608 || x <= xmin || x >= xmax || z <= zmin || z >= zmax)
		return 0;

	left = x - xmin;
	top = zmax - z;
	right = xmax - x;
	bottom = z - zmin;

	if (left <= right && left <= top && left <= bottom)
		x -= left;
	else if (right <= left && right <= top && right <= bottom)
		x += right;
	else if (top <= left && top <= right && top <= bottom)
		z += top;
	else
		z -= bottom;

	l->pos.x_pos = item->pos.x_pos + ((c * x + s * z) >> W2V_SHIFT);
	l->pos.z_pos = item->pos.z_pos + ((c * z - s * x) >> W2V_SHIFT);

	if (spaz && bounds[3] - bounds[2] > 256 && item->object_number != VON_CROY && item->object_number != GUIDE && item->object_number != ENEMY_JEEP)
	{
		x = (bounds[0] + bounds[1]) / 2;
		z = (bounds[4] + bounds[5]) / 2;
		dx -= (c * x + s * z) >> W2V_SHIFT;
		dz -= (c * z - s * x) >> W2V_SHIFT;
		lara.hit_direction = ushort(l->pos.y_rot - phd_atan(dz, dx) - 24576) >> W2V_SHIFT;	//hmmmmm

		if (!lara.hit_frame)
			SoundEffect(SFX_LARA_INJURY, &l->pos, SFX_DEFAULT);

		lara.hit_frame++;

		if (lara.hit_frame > 34)
			lara.hit_frame = 34;
	}

	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	facing = coll->facing;
	coll->facing = (short)phd_atan(l->pos.z_pos - coll->old.z, l->pos.x_pos - coll->old.x);
	GetCollisionInfo(coll, l->pos.x_pos, l->pos.y_pos, l->pos.z_pos, l->room_number, 762);
	coll->facing = facing;

	if (coll->coll_type == CT_NONE)
	{
		coll->old.x = l->pos.x_pos;
		coll->old.y = l->pos.y_pos;
		coll->old.z = l->pos.z_pos;
		UpdateLaraRoom(l, -10);
	}
	else
	{
		l->pos.x_pos = coll->old.x;
		l->pos.z_pos = coll->old.z;
	}

	if (lara.IsMoving && lara.MoveCount > 15)
	{
		lara.IsMoving = 0;
		lara.gun_status = LG_NO_ARMS;
	}

	return 1;
}

long TestBoundsCollide(ITEM_INFO* item, ITEM_INFO* l, long rad)
{
	short* bounds;
	short* lbounds;
	long s, c, dx, dz, x, z;

	bounds = GetBestFrame(item);
	lbounds = GetBestFrame(l);

	if (item->pos.y_pos + bounds[3] <= l->pos.y_pos + lbounds[2] || item->pos.y_pos + bounds[2] >= l->pos.y_pos + lbounds[3])
		return 0;

	s = phd_sin(item->pos.y_rot);
	c = phd_cos(item->pos.y_rot);
	dx = l->pos.x_pos - item->pos.x_pos;
	dz = l->pos.z_pos - item->pos.z_pos;
	x = (dx * c - dz * s) >> W2V_SHIFT;
	z = (dx * s + dz * c) >> W2V_SHIFT;
	return x >= bounds[0] - rad && x <= rad + bounds[1] && z >= bounds[4] - rad && z <= rad + bounds[5];
}

long TestBoundsCollideStatic(short* bounds, PHD_3DPOS* pos, long rad)
{
	short* lbounds;
	long s, c, dx, dz, x, z;

	if (!(bounds[0] | bounds[1] | bounds[2] | bounds[3] | bounds[4] | bounds[5]))
		return 0;

	lbounds = GetBestFrame(lara_item);

	if (pos->y_pos + bounds[3] <= lara_item->pos.y_pos + lbounds[2] || pos->y_pos + bounds[2] >= lara_item->pos.y_pos + lbounds[3])
		return 0;

	s = phd_sin(pos->y_rot);
	c = phd_cos(pos->y_rot);
	dx = lara_item->pos.x_pos - pos->x_pos;
	dz = lara_item->pos.z_pos - pos->z_pos;
	x = (dx * c - dz * s) >> W2V_SHIFT;
	z = (dx * s + dz * c) >> W2V_SHIFT;
	return x >= bounds[0] - rad && x <= rad + bounds[1] && z >= bounds[4] - rad && z <= rad + bounds[5];
}

long ItemPushLaraStatic(ITEM_INFO* l, short* bounds, PHD_3DPOS* pos, COLL_INFO* coll)
{
	long dx, dz, s, c, x, z;
	long xmin, xmax, zmin, zmax, left, top, right, bottom;
	short facing;

	dx = l->pos.x_pos - pos->x_pos;
	dz = l->pos.z_pos - pos->z_pos;
	s = phd_sin(pos->y_rot);
	c = phd_cos(pos->y_rot);
	x = (dx * c - dz * s) >> W2V_SHIFT;
	z = (dx * s + dz * c) >> W2V_SHIFT;
	xmin = bounds[0] - coll->radius;
	xmax = bounds[1] + coll->radius;
	zmin = bounds[4] - coll->radius;
	zmax = bounds[5] + coll->radius;

	if (abs(dx) > 4608 || abs(dz) > 4608 || x <= xmin || x >= xmax || z <= zmin || z >= zmax)
		return 0;

	left = x - xmin;
	top = zmax - z;
	right = xmax - x;
	bottom = z - zmin;

	if (left <= right && left <= top && left <= bottom)
		x -= left;
	else if (right <= left && right <= top && right <= bottom)
		x += right;
	else if (top <= left && top <= right && top <= bottom)
		z += top;
	else
		z -= bottom;

	l->pos.x_pos = pos->x_pos + ((c * x + s * z) >> W2V_SHIFT);
	l->pos.z_pos = pos->z_pos + ((c * z - s * x) >> W2V_SHIFT);
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	facing = coll->facing;
	coll->facing = (short)phd_atan(l->pos.z_pos - coll->old.z, l->pos.x_pos - coll->old.x);
	GetCollisionInfo(coll, l->pos.x_pos, l->pos.y_pos, l->pos.z_pos, l->room_number, 762);
	coll->facing = facing;

	if (coll->coll_type == CT_NONE)
	{
		coll->old.x = l->pos.x_pos;
		coll->old.y = l->pos.y_pos;
		coll->old.z = l->pos.z_pos;
		UpdateLaraRoom(l, -10);
	}
	else
	{
		l->pos.x_pos = coll->old.x;
		l->pos.z_pos = coll->old.z;
	}

	if (l == lara_item && lara.IsMoving && lara.MoveCount > 15)
	{
		lara.IsMoving = 0;
		lara.gun_status = LG_NO_ARMS;
	}

	return 1;
}

long TestLaraPosition(short* bounds, ITEM_INFO* item, ITEM_INFO* l)
{
	PHD_VECTOR pos;
	long x, y, z;
	short xrot, yrot, zrot;

	xrot = l->pos.x_rot - item->pos.x_rot;
	yrot = l->pos.y_rot - item->pos.y_rot;
	zrot = l->pos.z_rot - item->pos.z_rot;

	if (xrot < bounds[6] || xrot > bounds[7] ||
		yrot < bounds[8] || yrot > bounds[9] ||
		zrot < bounds[10] || zrot > bounds[11])
		return 0;

	phd_PushUnitMatrix();
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	pos.x = l->pos.x_pos - item->pos.x_pos;
	pos.y = l->pos.y_pos - item->pos.y_pos;
	pos.z = l->pos.z_pos - item->pos.z_pos;
	x = long(pos.x * mMXPtr[M00] + pos.y * mMXPtr[M10] + pos.z * mMXPtr[M20]);
	y = long(pos.x * mMXPtr[M01] + pos.y * mMXPtr[M11] + pos.z * mMXPtr[M21]);
	z = long(pos.x * mMXPtr[M02] + pos.y * mMXPtr[M12] + pos.z * mMXPtr[M22]);
	phd_PopMatrix();

	return x >= bounds[0] && x <= bounds[1] && y >= bounds[2] && y <= bounds[3] && z >= bounds[4] && z <= bounds[5];
}

void AlignLaraPosition(PHD_VECTOR* pos, ITEM_INFO* item, ITEM_INFO* l)
{
	long x, y, z;

	l->pos.x_rot = item->pos.x_rot;
	l->pos.y_rot = item->pos.y_rot;
	l->pos.z_rot = item->pos.z_rot;

	phd_PushUnitMatrix();
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	x = item->pos.x_pos + long(pos->x * mMXPtr[M00] + pos->y * mMXPtr[M01] + pos->z * mMXPtr[M02]);
	y = item->pos.y_pos + long(pos->x * mMXPtr[M10] + pos->y * mMXPtr[M11] + pos->z * mMXPtr[M12]);
	z = item->pos.z_pos + long(pos->x * mMXPtr[M20] + pos->y * mMXPtr[M21] + pos->z * mMXPtr[M22]);
	phd_PopMatrix();

	l->pos.x_pos = x;
	l->pos.y_pos = y;
	l->pos.z_pos = z;
}

long Move3DPosTo3DPos(PHD_3DPOS* pos, PHD_3DPOS* dest, long speed, short rotation)
{
	long dx, dy, dz, distance;
	short adiff;

	dx = dest->x_pos - pos->x_pos;
	dy = dest->y_pos - pos->y_pos;
	dz = dest->z_pos - pos->z_pos;
	distance = phd_sqrt(SQUARE(dx) + SQUARE(dy) + SQUARE(dz));

	if (speed < distance)
	{
		pos->x_pos += speed * dx / distance;
		pos->y_pos += speed * dy / distance;
		pos->z_pos += speed * dz / distance;
	}
	else
	{
		pos->x_pos = dest->x_pos;
		pos->y_pos = dest->y_pos;
		pos->z_pos = dest->z_pos;
	}

	if (!lara.IsMoving)
	{
		if (lara.water_status != LW_UNDERWATER)
		{
			switch (((ulong(mGetAngle(dest->x_pos, dest->z_pos, pos->x_pos, pos->z_pos) + 8192) >> 14) - (ushort(dest->y_rot + 8192) >> 14)) & 3)
			{
			case 0:
				lara_item->anim_number = 65;
				lara_item->frame_number = anims[lara_item->anim_number].frame_base;
				lara_item->current_anim_state = AS_STEPLEFT;
				lara_item->goal_anim_state = AS_STEPLEFT;
				break;

			case 1:
				lara_item->anim_number = 1;
				lara_item->frame_number = anims[lara_item->anim_number].frame_base;
				lara_item->current_anim_state = AS_WALK;
				lara_item->goal_anim_state = AS_WALK;
				break;

			case 2:
				lara_item->anim_number = 67;
				lara_item->frame_number = anims[lara_item->anim_number].frame_base;
				lara_item->current_anim_state = AS_STEPRIGHT;
				lara_item->goal_anim_state = AS_STEPRIGHT;
				break;

			default:
				lara_item->anim_number = 40;
				lara_item->frame_number = anims[lara_item->anim_number].frame_base;
				lara_item->current_anim_state = AS_BACK;
				lara_item->goal_anim_state = AS_BACK;
				break;
			}

			lara.gun_status = LG_HANDS_BUSY;
		}

		lara.IsMoving = 1;
		lara.MoveCount = 0;
	}

	adiff = dest->x_rot - pos->x_rot;

	if (adiff > rotation)
		pos->x_rot += rotation;
	else if (adiff < -rotation)
		pos->x_rot -= rotation;
	else
		pos->x_rot = dest->x_rot;

	adiff = dest->y_rot - pos->y_rot;

	if (adiff > rotation)
		pos->y_rot += rotation;
	else if (adiff < -rotation)
		pos->y_rot -= rotation;
	else
		pos->y_rot = dest->y_rot;

	adiff = dest->z_rot - pos->z_rot;

	if (adiff > rotation)
		pos->z_rot += rotation;
	else if (adiff < -rotation)
		pos->z_rot -= rotation;
	else
		pos->z_rot = dest->z_rot;

	return pos->x_pos == dest->x_pos && pos->y_pos == dest->y_pos && pos->z_pos == dest->z_pos && pos->x_rot == dest->x_rot && pos->y_rot == dest->y_rot && pos->z_rot == dest->z_rot;
}

long MoveLaraPosition(PHD_VECTOR* v, ITEM_INFO* item, ITEM_INFO* l)
{
	PHD_3DPOS pos;
	long height;
	short room_number;

	pos.x_rot = item->pos.x_rot;
	pos.y_rot = item->pos.y_rot;
	pos.z_rot = item->pos.z_rot;
	phd_PushUnitMatrix();
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	pos.x_pos = item->pos.x_pos + long(v->x * mMXPtr[M00] + v->y * mMXPtr[M01] + v->z * mMXPtr[M02]);
	pos.y_pos = item->pos.y_pos + long(v->x * mMXPtr[M10] + v->y * mMXPtr[M11] + v->z * mMXPtr[M12]);
	pos.z_pos = item->pos.z_pos + long(v->x * mMXPtr[M20] + v->y * mMXPtr[M21] + v->z * mMXPtr[M22]);
	phd_PopMatrix();

	if (item->object_number == FLARE_ITEM || item->object_number == BURNING_TORCH_ITEM || item->object_number == CLOCKWORK_BEETLE)
	{
		room_number = l->room_number;
		height = GetHeight(GetFloor(pos.x_pos, pos.y_pos, pos.z_pos, &room_number), pos.x_pos, pos.y_pos, pos.z_pos);

		if (abs(height - l->pos.y_pos) > 512)
		{
			if (lara.IsMoving)
			{
				lara.IsMoving = 0;
				lara.gun_status = LG_NO_ARMS;
			}

			return 0;
		}

		if (phd_sqrt(SQUARE(pos.x_pos - l->pos.x_pos) + SQUARE(pos.y_pos - l->pos.y_pos) + SQUARE(pos.z_pos - l->pos.z_pos)) < 128)
			return 1;
	}

	return Move3DPosTo3DPos(&l->pos, &pos, 12, 364);
}

long TestBoundsCollide2(ITEM_INFO* item, ITEM_INFO* l, long rad)
{
	short* bounds;
	long s, c, dx, dz, x, z;

	bounds = GetBestFrame(l);

	if (item->pos.y_pos + GlobalCollisionBounds[3] <= l->pos.y_pos + bounds[2] ||
		item->pos.y_pos + GlobalCollisionBounds[2] >= l->pos.y_pos + bounds[3])
		return 0;

	s = phd_sin(item->pos.y_rot);
	c = phd_cos(item->pos.y_rot);
	dx = l->pos.x_pos - item->pos.x_pos;
	dz = l->pos.z_pos - item->pos.z_pos;
	x = (c * dx - s * dz) >> W2V_SHIFT;
	z = (c * dz + s * dx) >> W2V_SHIFT;
	return x >= GlobalCollisionBounds[0] - rad && x <= rad + GlobalCollisionBounds[1] && z >= GlobalCollisionBounds[4] - rad && z <= rad + GlobalCollisionBounds[5];
}

void StargateCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	SPHERE* sphere;
	short* bounds;
	long touchedBits, hurtfulBits, x, y, z;

	item = &items[item_number];

	if (item->status == ITEM_INVISIBLE)
		return;

	if (!TestBoundsCollide(item, l, coll->radius))
		return;

	bounds = StarGateBounds;

	for (int i = 0; i < 4; i++)
	{
		GlobalCollisionBounds[0] = bounds[0];
		GlobalCollisionBounds[1] = bounds[1];
		GlobalCollisionBounds[2] = bounds[2];
		GlobalCollisionBounds[3] = bounds[3];
		GlobalCollisionBounds[4] = bounds[4];
		GlobalCollisionBounds[5] = bounds[5];

		if (TestBoundsCollide2(item, l, coll->radius))
			ItemPushLara(item, l, coll, 0, 2);

		bounds += 6;
	}

	touchedBits = TestCollision(item, l);

	if (!touchedBits)
		return;

	hurtfulBits = *(long*)&item->item_flags[0] & touchedBits;
	touchedBits = *(long*)&item->item_flags[0];

	if (!hurtfulBits)
		return;

	sphere = Slist;

	while (hurtfulBits)
	{
		if (hurtfulBits & 1)
		{
			GlobalCollisionBounds[0] = short(sphere->x - sphere->r - item->pos.x_pos);
			GlobalCollisionBounds[1] = short(sphere->x + sphere->r - item->pos.x_pos);
			GlobalCollisionBounds[2] = short(sphere->y - sphere->r - item->pos.y_pos);
			GlobalCollisionBounds[3] = short(sphere->y + sphere->r - item->pos.y_pos);
			GlobalCollisionBounds[4] = short(sphere->z - sphere->r - item->pos.z_pos);
			GlobalCollisionBounds[5] = short(sphere->z + sphere->r - item->pos.z_pos);
			x = lara_item->pos.x_pos;
			y = lara_item->pos.y_pos;
			z = lara_item->pos.z_pos;

			if (ItemPushLara(item, l, coll, touchedBits & 1, 2))
			{
				if (touchedBits & 1 && (x != lara_item->pos.x_pos || y != lara_item->pos.y_pos || z != lara_item->pos.z_pos))
				{
					if (TriggerActive(item))
					{
						x = l->pos.x_pos + (GetRandomControl() & 0x3F) - 32;
						y = (GetRandomControl() & 0x1F) + sphere->y - 16;
						z = l->pos.z_pos + (GetRandomControl() & 0x3F) - 32;
						DoBloodSplat(x, y, z, (GetRandomControl() & 3) + 2, short(GetRandomControl() << 1), l->room_number);
						lara_item->hit_points -= 100;
					}
				}
			}
		}

		hurtfulBits >>= 1;
		touchedBits >>= 1;
		sphere++;
	}
}

void CogCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	long x, y, z;

	item = &items[item_number];

	if (item->status == ITEM_INVISIBLE)
		return;

	if (!TestBoundsCollide(item, l, coll->radius))
		return;

	if (TriggerActive(item))
	{
		x = l->pos.x_pos + (GetRandomControl() & 0x3F) - 32;
		y = item->pos.y_pos + (GetRandomControl() & 0x1F) - 16;
		z = l->pos.z_pos + (GetRandomControl() & 0x3F) - 32;
		DoBloodSplat(x, y, z, (GetRandomControl() & 3) + 2, short(GetRandomControl() << 1), l->room_number);
		lara_item->hit_points -= 10;
	}
	else if (coll->enable_baddie_push)
		ItemPushLara(item, l, coll, 0, 0);
}

void GetCollisionInfo(COLL_INFO* coll, long x, long y, long z, short room_number, long hite)
{
	FLOOR_INFO* floor;
	static long xfront, zfront;
	long yT, h, c, tx, tz;
	long fspeed, ang, xright, xleft, zright, zleft, xright2, xleft2, zright2, zleft2;
	short room_num, room_num2, tilt;

	coll->coll_type = CT_NONE;
	coll->shift.x = 0;
	coll->shift.y = 0;
	coll->shift.z = 0;
	coll->quadrant = ushort(coll->facing + 0x2000) / 0x4000;

	ang = abs(lara_item->pos.y_rot - coll->facing) > 0x7000 ? 0x3000 : 0x4000;
	xright2 = (250 * phd_sin(coll->facing + ang)) >> W2V_SHIFT;
	xleft2 = (250 * phd_sin(coll->facing - ang)) >> W2V_SHIFT;
	zright2 = (250 * phd_cos(coll->facing + ang)) >> W2V_SHIFT;
	zleft2 = (250 * phd_cos(coll->facing - ang)) >> W2V_SHIFT;

	yT = y - hite - 160;
	fspeed = yT - lara_item->fallspeed;

	room_num = room_number;
	floor = GetFloor(x, yT, z, &room_num);
	h = GetHeight(floor, x, yT, z);

	if (h != NO_HEIGHT)
		h -= y;

	c = GetCeiling(floor, x, fspeed, z);

	if (c != NO_HEIGHT)
		c -= y - hite;

	coll->mid_floor = h;
	coll->mid_ceiling = c;
	coll->mid_type = height_type;
	coll->trigger = trigger_index;
	tilt = GetTiltType(floor, x, lara_item->pos.y_pos, z);
	coll->tilt_x = (char)tilt;
	coll->tilt_z = tilt >> 8;

	switch (coll->quadrant)
	{
	case NORTH:
		xfront = (coll->radius * phd_sin(coll->facing)) >> W2V_SHIFT;
		zfront = coll->radius;
		xright = coll->radius;
		zright = coll->radius;
		zleft = coll->radius;
		xleft = -coll->radius;
		break;

	case EAST:
		xfront = coll->radius;
		zfront = (coll->radius * phd_cos(coll->facing)) >> W2V_SHIFT;
		xright = coll->radius;
		zright = -coll->radius;
		zleft = coll->radius;
		xleft = coll->radius;
		break;

	case SOUTH:
		xfront = (coll->radius * phd_sin(coll->facing)) >> W2V_SHIFT;
		zfront = -coll->radius;
		xright = -coll->radius;
		zright = -coll->radius;
		zleft = -coll->radius;
		xleft = coll->radius;
		break;

	case WEST:
		xfront = -coll->radius;
		zfront = (coll->radius * phd_cos(coll->facing)) >> W2V_SHIFT;
		xright = -coll->radius;
		zright = coll->radius;
		zleft = -coll->radius;
		xleft = -coll->radius;
		break;

	default:
		xfront = 0;
		zfront = 0;
		xright = 0;
		zright = 0;
		zleft = 0;
		xleft = 0;
		break;
	}

	/*front*/
	tx = x + xfront;
	tz = z + zfront;
	floor = GetFloor(tx, yT, tz, &room_num);
	h = GetHeight(floor, tx, yT, tz);

	if (h != NO_HEIGHT)
		h -= y;

	c = GetCeiling(floor, tx, fspeed, tz);

	if (c != NO_HEIGHT)
		c -= y - hite;

	coll->front_ceiling = c;
	coll->front_floor = h;
	coll->front_type = height_type;

	tx += xfront;
	tz += zfront;
	floor = GetFloor(tx, yT, tz, &room_num);
	h = GetHeight(floor, tx, yT, tz);

	if (h != NO_HEIGHT)
		h -= y;

	if (coll->slopes_are_walls && (coll->front_type == BIG_SLOPE || coll->front_type == DIAGONAL) &&
		coll->front_floor < coll->mid_floor && h < coll->front_floor && coll->front_floor < 0)
		coll->front_floor = -32767;
	else if (coll->slopes_are_pits && (coll->front_type == BIG_SLOPE || coll->front_type == DIAGONAL) && coll->front_floor > coll->mid_floor)
		coll->front_floor = 512;
	else if (coll->lava_is_pit && coll->front_floor > 0 && trigger_index && (trigger_index[0] & 0x1F) == LAVA_TYPE)
		coll->front_floor = 512;

	/*left*/
	room_num2 = room_number;
	tx = x + xleft;
	tz = z + zleft;
	floor = GetFloor(tx, yT, tz, &room_num2);
	h = GetHeight(floor, tx, yT, tz);

	if (h != NO_HEIGHT)
		h -= y;

	c = GetCeiling(floor, tx, fspeed, tz);

	if (c != NO_HEIGHT)
		c -= y - hite;

	coll->left_ceiling = c;
	coll->left_floor = h;
	coll->left_type = height_type;

	if (coll->slopes_are_walls == 1 && (coll->left_type == BIG_SLOPE || coll->left_type == DIAGONAL) && coll->left_floor < 0)
		coll->left_floor = -32767;
	else if (coll->slopes_are_pits && (coll->left_type == BIG_SLOPE || coll->left_type == DIAGONAL) && coll->left_floor > 0)
		coll->left_floor = 512;
	else if (coll->lava_is_pit && coll->left_floor > 0 && trigger_index && (trigger_index[0] & 0x1F) == LAVA_TYPE)
		coll->left_floor = 512;

	floor = GetFloor(tx, yT, tz, &room_num);
	h = GetHeight(floor, tx, yT, tz);

	if (h != NO_HEIGHT)
		h -= y;

	c = GetCeiling(floor, tx, fspeed, tz);

	if (c != NO_HEIGHT)
		c -= y - hite;

	coll->left_ceiling2 = c;
	coll->left_floor2 = h;
	coll->left_type2 = height_type;

	if (coll->slopes_are_walls == 1 && (coll->left_type2 == BIG_SLOPE || coll->left_type2 == DIAGONAL) && coll->left_floor2 < 0)
		coll->left_floor2 = -32767;
	else if (coll->slopes_are_pits && (coll->left_type2 == BIG_SLOPE || coll->left_type2 == DIAGONAL) && coll->left_floor2 > 0)
		coll->left_floor2 = 512;
	else if (coll->lava_is_pit && coll->left_floor2 > 0 && trigger_index && (trigger_index[0] & 0x1F) == LAVA_TYPE)
		coll->left_floor2 = 512;

	/*right*/
	room_num2 = room_number;
	tx = x + xright;
	tz = z + zright;
	floor = GetFloor(tx, yT, tz, &room_num2);
	h = GetHeight(floor, tx, yT, tz);

	if (h != NO_HEIGHT)
		h -= y;

	c = GetCeiling(floor, tx, fspeed, tz);

	if (c != NO_HEIGHT)
		c -= y - hite;

	coll->right_ceiling = c;
	coll->right_floor = h;
	coll->right_type = height_type;

	if (coll->slopes_are_walls == 1 && (coll->right_type == BIG_SLOPE || coll->right_type == DIAGONAL) && coll->right_floor < 0)
		coll->right_floor = -32767;
	else if (coll->slopes_are_pits && (coll->right_type == BIG_SLOPE || coll->right_type == DIAGONAL) && coll->right_floor > 0)
		coll->right_floor = 512;
	else if (coll->lava_is_pit && coll->right_floor > 0 && trigger_index && (trigger_index[0] & 0x1F) == LAVA_TYPE)
		coll->right_floor = 512;

	floor = GetFloor(tx, yT, tz, &room_num);
	h = GetHeight(floor, tx, yT, tz);

	if (h != NO_HEIGHT)
		h -= y;

	c = GetCeiling(floor, tx, fspeed, tz);

	if (c != NO_HEIGHT)
		c -= y - hite;

	coll->right_ceiling2 = c;
	coll->right_floor2 = h;
	coll->right_type2 = height_type;

	if (coll->slopes_are_walls == 1 && (coll->right_type2 == BIG_SLOPE || coll->right_type2 == DIAGONAL) && coll->right_floor2 < 0)
		coll->right_floor2 = -32767;
	else if (coll->slopes_are_pits && (coll->right_type2 == BIG_SLOPE || coll->right_type2 == DIAGONAL) && coll->right_floor2 > 0)
		coll->right_floor2 = 512;
	else if (coll->lava_is_pit && coll->right_floor2 > 0 && trigger_index && (trigger_index[0] & 0x1F) == LAVA_TYPE)
		coll->right_floor2 = 512;

	room_num2 = room_number;
	tx = x + xleft2;
	tz = z + zleft2;
	floor = GetFloor(tx, yT, tz, &room_num2);
	GetHeight(floor, tx, yT, tz);
	GetCeiling(floor, tx, yT, tz);

	room_num2 = room_number;
	tx = x + xright2;
	tz = z + zright2;
	floor = GetFloor(tx, yT, tz, &room_num2);
	GetHeight(floor, tx, yT, tz);
	GetCeiling(floor, tx, yT, tz);

	CollideStaticObjects(coll, x, y, z, room_number, hite);

	if (coll->mid_floor == NO_HEIGHT)
	{
		coll->shift.x = coll->old.x - x;
		coll->shift.y = coll->old.y - y;
		coll->shift.z = coll->old.z - z;
		coll->coll_type = CT_FRONT;
		return;
	}

	if (coll->mid_floor - coll->mid_ceiling <= 0)
	{
		coll->shift.x = coll->old.x - x;
		coll->shift.y = coll->old.y - y;
		coll->shift.z = coll->old.z - z;
		coll->coll_type = CT_CLAMP;
		return;
	}

	if (coll->mid_ceiling >= 0)
	{
		coll->hit_ceiling = 1;
		coll->shift.y = coll->mid_ceiling;
		coll->coll_type = CT_TOP;
	}

	if (coll->front_floor > coll->bad_pos || coll->front_floor < coll->bad_neg || coll->front_ceiling > coll->bad_ceiling)
	{
		if (coll->front_type == DIAGONAL || coll->front_type == SPLIT_TRI)
		{
			coll->shift.x = coll->old.x - x;
			coll->shift.z = coll->old.z - z;
		}
		else
		{
			switch (coll->quadrant)
			{
			case NORTH:
			case SOUTH:
				coll->shift.x = coll->old.x - x;
				coll->shift.z = FindGridShift(z + zfront, z);
				break;

			case EAST:
			case WEST:
				coll->shift.x = FindGridShift(x + xfront, x);
				coll->shift.z = coll->old.z - z;
				break;
			}
		}

		coll->coll_type = CT_FRONT;
		return;
	}

	if (coll->front_ceiling >= coll->bad_ceiling)
	{
		coll->shift.x = coll->old.x - x;
		coll->shift.y = coll->old.y - y;
		coll->shift.z = coll->old.z - z;
		coll->coll_type = CT_TOP_FRONT;
		return;
	}

	if (coll->left_floor > coll->bad_pos || coll->left_floor < coll->bad_neg || coll->left_ceiling>coll->bad_ceiling)
	{
		if (coll->left_type == SPLIT_TRI)
		{
			coll->shift.x = coll->old.x - x;
			coll->shift.z = coll->old.z - z;
		}
		else
		{
			switch (coll->quadrant)
			{
			case NORTH:
			case SOUTH:
				coll->shift.x = FindGridShift(x + xleft, x + xfront);
				break;

			case EAST:
			case WEST:
				coll->shift.z = FindGridShift(z + zleft, z + zfront);
				break;
			}
		}

		coll->coll_type = CT_LEFT;
		return;
	}

	if (coll->bad_pos < coll->right_floor || coll->right_floor < coll->bad_neg || coll->bad_ceiling < coll->right_ceiling)
	{
		if (coll->right_type == SPLIT_TRI)
		{
			coll->shift.x = coll->old.x - x;
			coll->shift.z = coll->old.z - z;
		}
		else
		{
			switch (coll->quadrant)
			{
			case NORTH:
			case SOUTH:
				coll->shift.x = FindGridShift(x + xright, x + xfront);
				break;

			case EAST:
			case WEST:
				coll->shift.z = FindGridShift(z + zright, z + zfront);
				break;
			}
		}

		coll->coll_type = CT_RIGHT;
		return;
	}
}
