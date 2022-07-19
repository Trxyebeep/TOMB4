#include "../tomb4/pch.h"
#include "train.h"
#include "../specific/3dmath.h"
#include "../specific/output.h"
#include "../specific/specificfx.h"
#include "sound.h"
#include "control.h"
#include "items.h"

void DrawTrainObjects()
{
	short* obj;
	long x, x2;

	trainmappos = (trainmappos + 32 * gfUVRotate) % 0x60000;
	obj= &dels_handy_train_map[96 - ((trainmappos / 6144 - lara_item->pos.x_pos / 6144) & 0x1F)];
	x = trainmappos % 6144 - 24576;
	phd_PushMatrix();
	phd_TranslateAbs(lara_item->pos.x_pos - lara_item->pos.x_pos % 6144, 256, 47168);

	for (int i = 0; i < 8; i++)
	{
		phd_PutPolygons_train(meshes[static_objects[*obj].mesh_number], x);
		obj++;
		x += 6144;
	}

	phd_PopMatrix();
	phd_PushMatrix();
	phd_TranslateAbs(x + lara_item->pos.x_pos - lara_item->pos.x_pos % 6144, 256, 47168);
	phd_PutPolygons_train(meshes[static_objects[*obj].mesh_number], 0);
	phd_PopMatrix();
	obj = &dels_handy_train_map[32 - ((trainmappos / 6144 - lara_item->pos.x_pos / 6144 + 8) & 0x1F)];
	x = trainmappos % 6144 - 24576;
	phd_PushMatrix();
	phd_TranslateAbs(lara_item->pos.x_pos - lara_item->pos.x_pos % 6144, 256, 58304);
	phd_RotY(32760);
	x2 = x + 49152;

	for (int i = 0; i < 8; i++)
	{
		phd_PutPolygons_train(meshes[static_objects[*obj].mesh_number], -x);
		obj++;
		x += 6144;
	}

	phd_PopMatrix();
	phd_PushMatrix();
	phd_TranslateAbs(x2 + lara_item->pos.x_pos - lara_item->pos.x_pos % 6144, 256, 58304);
	phd_RotY(32760);
	phd_PutPolygons_train(meshes[static_objects[*obj].mesh_number], 0);
	phd_PopMatrix();
	obj = &dels_handy_train_map[192 - 2 * ((trainmappos / 6144 - lara_item->pos.x_pos / 6144 + 8) & 0x1F)];
	x = trainmappos % 6144 - 24576;
	phd_PushMatrix();

	for (int i = 0; i < 8; i++)
	{
		if (*obj != NO_ITEM)
		{
			phd_TranslateAbs(lara_item->pos.x_pos - lara_item->pos.x_pos % 6144, 256, obj[1] + 52224);
			phd_PutPolygons_train(meshes[static_objects[*obj].mesh_number], x);
		}

		obj += 2;
		x += 6144;
	}

	if (*obj != NO_ITEM)
	{
		phd_TranslateAbs(x + lara_item->pos.x_pos - lara_item->pos.x_pos % 6144, 256, obj[1] + 52224);
		phd_PutPolygons_train(meshes[static_objects[*obj].mesh_number], 0);
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
