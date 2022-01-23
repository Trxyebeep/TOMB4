#include "../tomb4/pch.h"
#include "footprnt.h"
#include "delstuff.h"
#include "control.h"
#include "sound.h"

static char footsounds[14] =
{
	0, 5, 3, 2, 1, 9, 9, 4, 6, 5, 3, 9, 4, 6
};

void AddFootPrint(ITEM_INFO* item)
{
	FOOTPRINT* print;
	FLOOR_INFO* floor;
	PHD_VECTOR pos;
	short room_num;

	pos.x = 0;
	pos.y = 0;
	pos.z = 0;

	if (FXType == SFX_LANDONLY)
		GetLaraJointPos(&pos, LM_LFOOT);
	else
		GetLaraJointPos(&pos, LM_RFOOT);

	room_num = item->room_number;
	floor = GetFloor(pos.x, pos.y, pos.z, &room_num);

	if (floor->fx != 6 && floor->fx != 5 && floor->fx != 11)
		SoundEffect(footsounds[floor->fx] + 288, &lara_item->pos, 0);

	if (floor->fx < 3 && !OnObject)
	{
		print = &FootPrint[FootPrintNum];
		print->x = pos.x;
		print->y = GetHeight(floor, pos.x, pos.y, pos.z);
		print->z = pos.z;
		print->YRot = item->pos.y_rot;
		print->Active = 512;
		FootPrintNum = (FootPrintNum + 1) & 0x1F;
	}
}

void inject_footprint(bool replace)
{
	INJECT(0x00437D50, AddFootPrint, replace);
}
