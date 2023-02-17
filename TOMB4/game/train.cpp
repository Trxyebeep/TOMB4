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
#include "gameflow.h"
#include "objects.h"

static short dels_handy_train_map[128] =
{
	//36: ARCHITECTURE6
	//37: ARCHITECTURE7
	//38: ARCHITECTURE8
	//39: ARCHITECTURE9
	36, 36, 36, 37, 38, 39, 36, 37, 38, 38, 38, 39, 37,
	38, 39, 36, 36, 36, 36, 36, 37, 39, 36, 36, 37, 38,
	38, 39, 36, 36, 37, 39, 36, 36, 36, 37, 38, 39, 36,
	37, 38, 38, 38, 39, 37, 38, 39, 36, 36, 36, 36, 36,
	37, 39, 36, 36, 37, 38, 38, 39, 36, 36, 37, 39, 38,
	37, 36, 39, 37, 36, 36, 36, 39, 38, 38, 38, 38, 37,
	36, 39, 38, 38, 37, 39, 38, 38, 38, 38, 38, 38, 37,
	36, 36, 36, 39, 38, 38, 37, 36, 39, 37, 36, 36, 36,
	39, 38, 38, 38, 38, 37, 36, 39, 38, 38, 37, 39, 38,
	38, 38, 38, 38, 38, 37, 36, 36, 36, 39, 38
};

static TRAIN_STATIC dels_handy_train_map2[64] =
{
	{ROCK2, 4096}, {NO_ITEM, 0}, {NO_ITEM, 0},
	{ROCK0, -3072}, {NO_ITEM, 0}, {NO_ITEM, 0}, {NO_ITEM, 0},
	{ROCK0, 4096}, {NO_ITEM, 0}, {NO_ITEM, 0}, {NO_ITEM, 0}, {NO_ITEM, 0}, {NO_ITEM, 0},
	{ROCK0, -3072}, {NO_ITEM, 0}, {NO_ITEM, 0}, {NO_ITEM, 0},
	{ROCK0, 4096}, {NO_ITEM, 0},
	{ROCK3, -3072}, {NO_ITEM, 0},
	{ROCK0, -3072}, {NO_ITEM, 0}, {NO_ITEM, 0},
	{ROCK0, 4096}, {NO_ITEM, 0}, {NO_ITEM, 0},
	{ROCK0, 4096},
	{ROCK3, -2048}, {NO_ITEM, 0}, {NO_ITEM, 0}, {NO_ITEM, 0},
	{ROCK2, 4096}, {NO_ITEM, 0}, {NO_ITEM, 0},
	{ROCK0, -3072}, {NO_ITEM, 0}, {NO_ITEM, 0}, {NO_ITEM, 0},
	{ROCK0, 4096}, {NO_ITEM, 0}, {NO_ITEM, 0}, {NO_ITEM, 0}, {NO_ITEM, 0}, {NO_ITEM, 0},
	{ROCK0, -3072}, {NO_ITEM, 0}, {NO_ITEM, 0}, {NO_ITEM, 0},
	{ROCK0, 4096}, {NO_ITEM, 0},
	{ROCK3, -3072}, {NO_ITEM, 0},
	{ROCK0, -3072}, {NO_ITEM, 0}, {NO_ITEM, 0},
	{ROCK0, 4096}, {NO_ITEM, 0}, {NO_ITEM, 0},
	{ROCK0, 4096},
	{ROCK3, -2048}, {NO_ITEM, 0}, {NO_ITEM, 0}, {NO_ITEM, 0}
};

long trainmappos;

void DrawTrainObjects()
{
	TRAIN_STATIC* p;
	short* obj;
	long x, x2;

	trainmappos = (trainmappos + (gfUVRotate << 5)) % 0x60000;
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

	p = &dels_handy_train_map2[32 - ((trainmappos / 6144 - lara_item->pos.x_pos / 6144 + 8) & 0x1F)];
	x = trainmappos % 6144 - 24576;
	phd_PushMatrix();

	for (int i = 0; i < 8; i++)
	{
		if (p->type != NO_ITEM)
		{
			phd_TranslateAbs(lara_item->pos.x_pos - lara_item->pos.x_pos % 6144, 256, p->zoff + 52224);
			phd_PutPolygons_train(meshes[static_objects[p->type].mesh_number], x);
		}

		p++;
		x += 6144;
	}

	if (p->type != NO_ITEM)
	{
		phd_TranslateAbs(x + lara_item->pos.x_pos - lara_item->pos.x_pos % 6144, 256, p->zoff + 52224);
		phd_PutPolygons_train(meshes[static_objects[p->type].mesh_number], 0);
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
			return;
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

	if (item->room_number != room_number)
		ItemNewRoom(item_number, room_number);

	if (item2->item_flags[0] > -40 || item2->hit_points < 1)
		item->item_flags[0] += 3;

	if (item->item_flags[0] > 400)
		KillItem(item_number);
}
