#include "../tomb4/pch.h"
#include "people.h"
#include "sphere.h"
#include "../specific/function_stubs.h"
#include "effects.h"
#include "sound.h"
#include "draw.h"
#include "control.h"
#include "objects.h"
#include "effect2.h"

short GunShot(long x, long y, long z, short speed, short yrot, short room_number)
{
	return -1;
}

short GunHit(long x, long y, long z, short speed, short yrot, short room_number)
{
	PHD_VECTOR pos;

	pos.x = 0;
	pos.y = 0;
	pos.z = 0;
	GetJointAbsPosition(lara_item, &pos, (25 * GetRandomControl()) / 0x7FFF);
	DoBloodSplat(pos.x, pos.y, pos.z, (GetRandomControl() & 3) + 3, lara_item->pos.y_rot, lara_item->room_number);

#ifdef GENERAL_FIXES
	SoundEffect(SFX_LARA_INJURY, &lara_item->pos, SFX_DEFAULT);
#else
	SoundEffect(SFX_UNDERWATER_DOOR, &lara_item->pos, SFX_DEFAULT);
#endif

	return GunShot(x, y, z, speed, yrot, room_number);
}

short GunMiss(long x, long y, long z, short speed, short yrot, short room_number)
{
	GAME_VECTOR pos;

	pos.x = lara_item->pos.x_pos + ((GetRandomControl() - 0x4000) << 9) / 0x7FFF;
	pos.y = lara_item->floor;
	pos.z = lara_item->pos.z_pos + ((GetRandomControl() - 0x4000) << 9) / 0x7FFF;
	pos.room_number = lara_item->room_number;
	Richochet(&pos);
	return GunShot(x, y, z, speed, yrot, room_number);
}

long TargetVisible(ITEM_INFO* item, AI_INFO* info)
{
	ITEM_INFO* enemy;
	CREATURE_INFO* creature;
	GAME_VECTOR start;
	GAME_VECTOR target;
	short* bounds;

	creature = (CREATURE_INFO*)item->data;
	enemy = creature->enemy;

	if (!enemy || enemy->hit_points <= 0 || !enemy->data || info->angle - creature->joint_rotation[2] <= -0x4000 ||
		info->angle - creature->joint_rotation[2] >= 0x4000 || info->distance >= 0x4000000)
		return 0;

	bounds = GetBestFrame(enemy);

	start.x = item->pos.x_pos;
	start.y = item->pos.y_pos - 768;
	start.z = item->pos.z_pos;
	start.room_number = item->room_number;

	target.x = enemy->pos.x_pos;
	target.y = enemy->pos.y_pos + ((bounds[3] + 3 * bounds[2]) >> 2);
	target.z = enemy->pos.z_pos;
	return LOS(&start, &target);
}

long Targetable(ITEM_INFO* item, AI_INFO* info)
{
	ITEM_INFO* enemy;
	CREATURE_INFO* creature;
	GAME_VECTOR start;
	GAME_VECTOR target;
	short* bounds;

	creature = (CREATURE_INFO*)item->data;
	enemy = creature->enemy;

	if (!enemy || enemy->hit_points <= 0 || !enemy->data || !info->ahead || info->distance >= 0x4000000 && item->object_number != SETHA)
		return 0;

	bounds = GetBestFrame(item);
	start.x = item->pos.x_pos;
	start.y = item->pos.y_pos + ((bounds[3] + 3 * bounds[2]) >> 2);
	start.z = item->pos.z_pos;
	start.room_number = item->room_number;

	bounds = GetBestFrame(enemy);
	target.x = enemy->pos.x_pos;
	target.y = enemy->pos.y_pos + ((bounds[3] + 3 * bounds[2]) >> 2);
	target.z = enemy->pos.z_pos;
	return LOS(&start, &target);
}

void inject_people(bool replace)
{
	INJECT(0x0040B050, GunShot, replace);
	INJECT(0x0040B060, GunHit, replace);
	INJECT(0x0040B120, GunMiss, replace);
	INJECT(0x0040AEB0, TargetVisible, replace);
	INJECT(0x0040AF80, Targetable, replace);
}
