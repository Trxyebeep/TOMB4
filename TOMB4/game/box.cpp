#include "../tomb4/pch.h"
#include "box.h"
#include "tomb4fx.h"
#include "items.h"
#include "lot.h"
#include "objects.h"
#include "control.h"
#include "draw.h"
#include "../specific/3dmath.h"
#include "lara_states.h"

void CreatureDie(short item_number, long explode)
{
	ITEM_INFO* item;
	ITEM_INFO* pickup;
	short pickup_number, room_number;

	item = &items[item_number];
	item->hit_points = -16384;
	item->collidable = 0;

	if (explode)
	{
		if (objects[item->object_number].HitEffect == 1)
			ExplodingDeath2(item_number, -1, 258);
		else
			ExplodingDeath2(item_number, -1, 256);

		KillItem(item_number);
	}
	else
		RemoveActiveItem(item_number);

	DisableBaddieAI(item_number);
	item->flags |= IFL_INVISIBLE | IFL_CLEARBODY;

	//inlined DropBaddyPickups..

	pickup_number = item->carried_item;

	while (pickup_number != NO_ITEM)
	{
		pickup = &items[pickup_number];

		if (item->object_number == TROOPS && item->trigger_flags == 1)
		{
			pickup->pos.x_pos = ((item->pos.x_pos + ((1024 * phd_sin(item->pos.y_rot)) >> W2V_SHIFT)) & -512) | 512;
			pickup->pos.z_pos = ((item->pos.z_pos + ((1024 * phd_cos(item->pos.y_rot)) >> W2V_SHIFT)) & -512) | 512;
		}
		else
		{
			pickup->pos.x_pos = (item->pos.x_pos & -512) | 512;
			pickup->pos.z_pos = (item->pos.z_pos & -512) | 512;
		}

		room_number = item->room_number;
		pickup->pos.y_pos = GetHeight(GetFloor(pickup->pos.x_pos, item->pos.y_pos, pickup->pos.z_pos, &room_number),
			pickup->pos.x_pos, item->pos.y_pos, pickup->pos.z_pos);
		pickup->pos.y_pos -= GetBoundsAccurate(pickup)[3];
		ItemNewRoom(pickup_number, item->room_number);
		pickup->flags |= IFL_TRIGGERED;
		pickup_number = pickup->carried_item;
	}
}

void InitialiseCreature(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->collidable = 1;
	item->data = 0;
}

long CreatureActive(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->flags & IFL_CLEARBODY)
		return 0;

	if (item->status == ITEM_INVISIBLE)
	{
		if (EnableBaddieAI(item_number, 0))
			item->status = ITEM_ACTIVE;
		else
			return 0;
	}

	return 1;
}

void CreatureAIInfo(ITEM_INFO* item, AI_INFO* info)
{
	CREATURE_INFO* creature;
	OBJECT_INFO* obj;
	ITEM_INFO* enemy;
	ROOM_INFO* r;
	FLOOR_INFO* floor;
	short* zone;
	long x, y, z;
	short pivot, ang, state;

	creature = (CREATURE_INFO*)item->data;

	if (!creature)
		return;

	obj = &objects[item->object_number];

	if (item->poisoned)
	{
		if (!obj->undead && !(wibble & 0x3F) && item->hit_points > 1)
			item->hit_points--;
	}

	enemy = creature->enemy;

	if (!enemy)
	{
		enemy = lara_item;
		creature->enemy = lara_item;
	}

	zone = ground_zone[creature->LOT.zone][flip_status];
	r = &room[item->room_number];
	floor = &r->floor[((item->pos.z_pos - r->z) >> 10) + r->x_size * ((item->pos.x_pos - r->x) >> 10)];
	item->box_number = floor->box;
	info->zone_number = zone[item->box_number];

	r = &room[enemy->room_number];
	floor = &r->floor[((enemy->pos.z_pos - r->z) >> 10) + r->x_size * ((enemy->pos.x_pos - r->x) >> 10)];
	enemy->box_number = floor->box;
	info->enemy_zone = zone[enemy->box_number];

	if (boxes[enemy->box_number].overlap_index & creature->LOT.block_mask ||
		creature->LOT.node[item->box_number].search_number == (creature->LOT.search_number | 0x8000))
		info->enemy_zone |= 0x4000;

	pivot = obj->pivot_length;

	if (enemy == lara_item)
		ang = lara.move_angle;
	else
		ang = enemy->pos.y_rot;

	x = enemy->pos.x_pos + (14 * enemy->speed * phd_sin(ang) >> W2V_SHIFT) - (pivot * phd_sin(item->pos.y_rot) >> W2V_SHIFT) - item->pos.x_pos;
	y = item->pos.y_pos - enemy->pos.y_pos;
	z = enemy->pos.z_pos + (14 * enemy->speed * phd_cos(ang) >> W2V_SHIFT) - (pivot * phd_cos(item->pos.y_rot) >> W2V_SHIFT) - item->pos.z_pos;

	ang = (short)phd_atan(z, x);

	if (z > 32000 || z < -32000 || x > 32000 || x < -32000)
		info->distance = 0x7FFFFFFF;
	else if (creature->enemy)
		info->distance = SQUARE(x) + SQUARE(z);
	else
		info->distance = 0x7FFFFFFF;

	info->angle = ang - item->pos.y_rot;
	info->enemy_facing = ang - enemy->pos.y_rot + 0x8000;

	x = ABS(x);
	z = ABS(z);

	if (enemy == lara_item)
	{
		state = lara_item->current_anim_state;

		if (state == AS_DUCK || state == AS_DUCKROLL || state == AS_ALL4S || state == AS_CRAWL || state == AS_ALL4TURNL || state == AS_ALL4TURNR)
			y -= 384;
	}

	if (x > z)
		info->x_angle = (short)phd_atan(x + (z >> 1), y);
	else
		info->x_angle = (short)phd_atan(z + (x >> 1), y);

	if (info->angle > -0x4000 && info->angle < 0x4000)
		info->ahead = 1;
	else
		info->ahead = 0;

	if (info->ahead && enemy->hit_points > 0 && ABS(enemy->pos.y_pos - item->pos.y_pos) <= 512)
		info->bite = 1;
	else
		info->bite = 0;
}

void inject_box(bool replace)
{
	INJECT(0x00441080, CreatureDie, replace);
	INJECT(0x0043FB30, InitialiseCreature, replace);
	INJECT(0x0043FB70, CreatureActive, replace);
	INJECT(0x0043FBE0, CreatureAIInfo, replace);
}
