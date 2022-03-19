#include "../tomb4/pch.h"
#include "people.h"
#include "sphere.h"
#include "../specific/function_stubs.h"
#include "effects.h"
#include "sound.h"

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

void inject_people(bool replace)
{
	INJECT(0x0040B050, GunShot, replace);
	INJECT(0x0040B060, GunHit, replace);
}
