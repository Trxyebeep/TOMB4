#include "../tomb4/pch.h"
#include "train.h"
#include "../specific/3dmath.h"
#include "../specific/output.h"
#include "../specific/specificfx.h"
#include "sound.h"
#include "control.h"
#include "items.h"
#include "camera.h"
#include "draw.h"
#include "lara.h"

static short dels_handy_train_map[256] =	//test me
{
	36, 0, 36, 0, 36, 0, 37, 0, 38, 0, 39, 0, 36, 0, 37,
	0, 38, 0, 38, 0, 38, 0, 39, 0, 37, 0, 38, 0, 39, 0,
	36, 0, 36, 0, 36, 0, 36, 0, 36, 0, 37, 0, 39, 0, 36,
	0, 36, 0, 37, 0, 38, 0, 38, 0, 39, 0, 36, 0, 36, 0,
	37, 0, 39, 0, 36, 0, 36, 0, 36, 0, 37, 0, 38, 0, 39,
	0, 36, 0, 37, 0, 38, 0, 38, 0, 38, 0, 39, 0, 37, 0,
	38, 0, 39, 0, 36, 0, 36, 0, 36, 0, 36, 0, 36, 0, 37,
	0, 39, 0, 36, 0, 36, 0, 37, 0, 38, 0, 38, 0, 39, 0,
	36, 0, 36, 0, 37, 0, 39, 0, 38, 0, 37, 0, 36, 0, 39,
	0, 37, 0, 36, 0, 36, 0, 36, 0, 39, 0, 38, 0, 38, 0,
	38, 0, 38, 0, 37, 0, 36, 0, 39, 0, 38, 0, 38, 0, 37,
	0, 39, 0, 38, 0, 38, 0, 38, 0, 38, 0, 38, 0, 38, 0,
	37, 0, 36, 0, 36, 0, 36, 0, 39, 0, 38, 0, 38, 0, 37,
	0, 36, 0, 39, 0, 37, 0, 36, 0, 36, 0, 36, 0, 39, 0,
	38, 0, 38, 0, 38, 0, 38, 0, 37, 0, 36, 0, 39, 0, 38,
	0, 38, 0, 37, 0, 39, 0, 38, 0, 38, 0, 38, 0, 38, 0,
	38, 0, 38, 0, 37, 0, 36, 0, 36, 0, 36, 0, 39, 0, 38, 0
};

static char dels_handy_train_map2[256] =	//and me
{
	22, 0, 0, 16, -1, -1, 0, 0, -1, -1, 0, 0, 20,
	0, 0, -12, -1, -1, 0, 0, -1, -1, 0, 0, -1, -1,
	0, 0, 20, 0, 0, 16, -1, -1, 0, 0, -1, -1, 0, 0,
	-1, -1, 0, 0, -1, -1, 0, 0, -1, -1, 0, 0, 20,
	0, 0, -12, -1, -1, 0, 0, -1, -1, 0, 0, -1, -1,
	0, 0, 20, 0, 0, 16, -1, -1, 0, 0, 23, 0, 0, -12, -1,
	-1, 0, 0, 20, 0, 0, -12, -1, -1, 0, 0, -1, -1,
	0, 0, 20, 0, 0, 16, -1, -1, 0, 0, -1, -1, 0, 0,
	20, 0, 0, 16, 23, 0, 0, -8, -1, -1, 0, 0, -1, -1,
	0, 0, -1, -1, 0, 0, 22, 0, 0, 16, -1, -1, 0, 0,
	-1, -1, 0, 0, 20, 0, 0, -12, -1, -1, 0, 0, -1,
	-1, 0, 0, -1, -1, 0, 0, 20, 0, 0, 16, -1, -1, 0,
	0, -1, -1, 0, 0, -1, -1, 0, 0, -1, -1, 0, 0, -1,
	-1, 0, 0, 20, 0, 0, -12, -1, -1, 0, 0, -1, -1,
	0, 0, -1, -1, 0, 0, 20, 0, 0, 16, -1, -1, 0, 0,
	23, 0, 0, -12, -1, -1, 0, 0, 20, 0, 0, -12, -1, -1,
	0, 0, -1, -1, 0, 0, 20, 0, 0, 16, -1, -1, 0, 0,
	-1, -1, 0, 0, 20, 0, 0, 16, 23, 0, 0, -8, -1, -1,
	0, 0, -1, -1, 0, 0, -1, -1, 0, 0
};

long trainmappos;

void DrawTrainObjects()
{
	short* obj;
	char* obj2;
	long x, x2;

	trainmappos = (trainmappos + 32 * gfUVRotate) % 0x60000;
	obj= &dels_handy_train_map[96 - ((trainmappos / 6144 - lara_item->pos.x_pos / 6144) & 0x1F)];
	x = trainmappos % 6144 - 24576;
	phd_PushMatrix();
	phd_TranslateAbs(lara_item->pos.x_pos - lara_item->pos.x_pos % 6144, 256, 47168);

	for (int i = 0; i < 8; i++)
	{
		phd_PutPolygons_train(meshes[static_objects[obj[0]].mesh_number], x);
		obj++;
		x += 6144;
	}

	phd_PopMatrix();
	phd_PushMatrix();
	phd_TranslateAbs(x + lara_item->pos.x_pos - lara_item->pos.x_pos % 6144, 256, 47168);
	phd_PutPolygons_train(meshes[static_objects[obj[0]].mesh_number], 0);
	phd_PopMatrix();
	obj = &dels_handy_train_map[32 - ((trainmappos / 6144 - lara_item->pos.x_pos / 6144 + 8) & 0x1F)];
	x = trainmappos % 6144 - 24576;
	phd_PushMatrix();
	phd_TranslateAbs(lara_item->pos.x_pos - lara_item->pos.x_pos % 6144, 256, 58304);
	phd_RotY(32760);
	x2 = x + 49152;

	for (int i = 0; i < 8; i++)
	{
		phd_PutPolygons_train(meshes[static_objects[obj[0]].mesh_number], -x);
		obj++;
		x += 6144;
	}

	phd_PopMatrix();
	phd_PushMatrix();
	phd_TranslateAbs(x2 + lara_item->pos.x_pos - lara_item->pos.x_pos % 6144, 256, 58304);
	phd_RotY(32760);
	phd_PutPolygons_train(meshes[static_objects[obj[0]].mesh_number], 0);
	phd_PopMatrix();
	obj2 = &dels_handy_train_map2[32 - ((trainmappos / 6144 - lara_item->pos.x_pos / 6144 + 8) & 0x1F)];
	x = trainmappos % 6144 - 24576;
	phd_PushMatrix();

	for (int i = 0; i < 8; i++)
	{
		if (obj2[0] != -1)
		{
			phd_TranslateAbs(lara_item->pos.x_pos - lara_item->pos.x_pos % 6144, 256, obj2[1] + 52224);
			phd_PutPolygons_train(meshes[static_objects[obj2[0]].mesh_number], x);
		}

		obj2 += 2;
		x += 6144;
	}

	if (obj2[0] != -1)
	{
		phd_TranslateAbs(x + lara_item->pos.x_pos - lara_item->pos.x_pos % 6144, 256, obj2[1] + 52224);
		phd_PutPolygons_train(meshes[static_objects[obj2[0]].mesh_number], 0);
	}

	phd_PopMatrix();
}

void DrawTrainFloor()
{
	long x;

	x = lara_item->pos.x_pos;
	lara_item->pos.x_pos = camera.pos.x;
	phd_PushMatrix();
	phd_TranslateAbs(lara_item->pos.x_pos & -1024, 0, 52224);
	DrawTrainStrips();
	camera.bounce = -12;
	phd_PopMatrix();
	DrawTrainObjects();
	lara_item->pos.x_pos = x;
}

void InitialiseTrainJeep(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* item2;

	item = &items[item_number];
	item->item_flags[0] = -80;

	for (int i = 0; i < level_items; i++)	//find your raghead
	{
		item2 = &items[i];

		if (item != item2 && item2->trigger_flags == item->trigger_flags)
		{
			item->item_flags[1] = i;
			item2->item_flags[0] = -80;
			item2->pos.y_pos = item->pos.y_pos - 1024;
		}
	}
}

void TrainJeepControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* item2;
	short room_number;

	item = &items[item_number];
	item2 = &items[item->item_flags[1]];

	if (item->item_flags[0] == -80)
	{
		if (item->item_flags[2] < 0x4000)
			item->item_flags[2] += 32;
	}
	else if (item->item_flags[2] > 1024)
		item->item_flags[2] -= 512;

	SoundEffect(SFX_JEEP_MOVE, &item->pos, (item->item_flags[2] << 9) + (0x1000000 | SFX_SETPITCH));
	item->pos.x_pos += item->item_flags[0];
	AnimateItem(item);
	room_number = item->room_number;
	GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

	if (room_number != item->room_number)
		ItemNewRoom(item_number, room_number);

	if (item2->item_flags[0] > -40 || item2->hit_points < 1)
		item->item_flags[0] += 3;

	if (item->item_flags[0] > 400)
		KillItem(item_number);
}

void inject_train(bool replace)
{
	INJECT(0x004640A0, DrawTrainObjects, replace);
	INJECT(0x00464030, DrawTrainFloor, replace);
	INJECT(0x004644B0, InitialiseTrainJeep, replace);
	INJECT(0x00464520, TrainJeepControl, replace);
}
