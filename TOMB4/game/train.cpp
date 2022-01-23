#include "../tomb4/pch.h"
#include "train.h"
#include "../specific/3dmath.h"
#include "../specific/output.h"
#include "../specific/specificfx.h"

void DrawTrainObjects()
{
	short* obj;
	long x, x2;

	TrainObjPos = (TrainObjPos + 32 * gfUVRotate) % 0x60000;
	obj= &TrainObjTable[96 - ((TrainObjPos / 6144 - lara_item->pos.x_pos / 6144) & 0x1F)];
	x = TrainObjPos % 6144 - 24576;
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
	obj = &TrainObjTable[32 - ((TrainObjPos / 6144 - lara_item->pos.x_pos / 6144 + 8) & 0x1F)];
	x = TrainObjPos % 6144 - 24576;
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
	obj = &TrainObjTable[192 - 2 * ((TrainObjPos / 6144 - lara_item->pos.x_pos / 6144 + 8) & 0x1F)];
	x = TrainObjPos % 6144 - 24576;
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

void inject_train(bool replace)
{
	INJECT(0x004640A0, DrawTrainObjects, replace);
	INJECT(0x00464030, DrawTrainFloor, replace);
}
