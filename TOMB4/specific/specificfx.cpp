#include "../tomb4/pch.h"
#include "specificfx.h"
#include "3dmath.h"
#include "../game/delstuff.h"
#include "../game/control.h"
#include "function_table.h"

#define LINE_POINTS	4	//number of points in each grid line
#define POINT_HEIGHT_CORRECTION	196	//if the difference between the floor below Lara and the floor height below the point is greater than this value, point height is corrected to lara's floor level.
#define NUM_TRIS	14	//number of triangles needed to create the shadow (this depends on what shape you're doing)
#define GRID_POINTS	(LINE_POINTS * LINE_POINTS)	//number of points in the whole grid

long ShadowTable[NUM_TRIS * 3] =	//num of triangles * 3 points
{
4, 1, 5,
5, 1, 6,	//top part
6, 1, 2,
6, 2, 7,
//
8, 4, 9,
9, 4, 5,
9, 5, 10,	//middle part
10, 5, 6,
10, 6, 11,
11, 6, 7,
//
13, 8, 9,
13, 9, 14,	//bottom part
14, 9, 10,
14, 10, 11
};

void S_PrintShadow(short size, short* box, ITEM_INFO* item)
{
	TEXTURESTRUCT Tex;
	D3DTLVERTEX v[3];
	PHD_VECTOR pos;
	long* sXYZ;
	long* hXZ;
	long* hY;
	long sxyz[GRID_POINTS * 3];
	long hxz[GRID_POINTS * 2];
	long hy[GRID_POINTS];
	long triA, triB, triC;
	long x, y, z, x1, y1, z1, x2, y2, z2, x3, y3, z3, xSize, zSize, xDist, zDist;
	short room_number;

	xSize = size * (box[1] - box[0]) / 192;	//x size of grid
	zSize = size * (box[5] - box[4]) / 192;	//z size of grid
	xDist = xSize / LINE_POINTS;			//distance between each point of the grid on X
	zDist = zSize / LINE_POINTS;			//distance between each point of the grid on Z
	x = -xDist - (xDist >> 1);				//idfk
	z = zDist + (zDist >> 1);
	sXYZ = sxyz;
	hXZ = hxz;

	for (int i = 0; i < LINE_POINTS; i++, z -= zDist)
	{
		for (int j = 0; j < LINE_POINTS; j++, sXYZ += 3, hXZ += 2, x += xDist)
		{
			sXYZ[0] = x;		//fill shadow XYZ array with the points of the grid
			sXYZ[2] = z;
			hXZ[0] = x;			//fill height XZ array with the points of the grid
			hXZ[1] = z;
		}

		x = -xDist - (xDist >> 1);
	}

	phd_PushUnitMatrix();

	if (item == lara_item)	//position the grid
	{
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;
		GetLaraJointPos(&pos, LM_TORSO);
		room_number = lara_item->room_number;
		y = GetHeight(GetFloor(pos.x, pos.y, pos.z, &room_number), pos.x, pos.y, pos.z);

		if (y == NO_HEIGHT)
			y = item->floor;
	}
	else
	{
		pos.x = item->pos.x_pos;
		y = item->floor;
		pos.z = item->pos.z_pos;
	}

	y -= 16;
	phd_TranslateRel(pos.x, y, pos.z);
	phd_RotY(item->pos.y_rot);	//rot the grid to correct Y
	hXZ = hxz;

	for (int i = 0; i < GRID_POINTS; i++, hXZ += 2)
	{
		x = hXZ[0];
		z = hXZ[1];
		hXZ[0] = (x * phd_mxptr[M00] + z * phd_mxptr[M02] + phd_mxptr[M03]) >> 14;
		hXZ[1] = (x * phd_mxptr[M20] + z * phd_mxptr[M22] + phd_mxptr[M23]) >> 14;
	}

	phd_PopMatrix();

	hXZ = hxz;
	hY = hy;

	for (int i = 0; i < GRID_POINTS; i++, hXZ += 2, hY++)	//Get height on each grid point and store it in hy array
	{
		room_number = item->room_number;
		*hY = GetHeight(GetFloor(hXZ[0], item->floor, hXZ[1], &room_number), hXZ[0], item->floor, hXZ[1]);

		if (ABS(*hY - item->floor) > POINT_HEIGHT_CORRECTION)
			*hY = item->floor;
	}

	phd_PushMatrix();
	phd_TranslateAbs(pos.x, y, pos.z);
	phd_RotY(item->pos.y_rot);
	sXYZ = sxyz;
	hY = hy;

	for (int i = 0; i < GRID_POINTS; i++, sXYZ += 3, hY++)
	{
		x = sXYZ[0];
		y = *hY - item->floor;
		z = sXYZ[2];
		sXYZ[0] = (phd_mxptr[M00] * x + phd_mxptr[M01] * y + phd_mxptr[M02] * z + phd_mxptr[M03]) >> 14;
		sXYZ[1] = (phd_mxptr[M10] * x + phd_mxptr[M11] * y + phd_mxptr[M12] * z + phd_mxptr[M13]) >> 14;
		sXYZ[2] = (phd_mxptr[M20] * x + phd_mxptr[M21] * y + phd_mxptr[M22] * z + phd_mxptr[M23]) >> 14;
	}

	phd_PopMatrix();
	sXYZ = sxyz;

	for (int i = 0; i < NUM_TRIS; i++)	//draw triangles
	{
		triA = 3 * ShadowTable[(i * 3) + 0];	//get tri points
		triB = 3 * ShadowTable[(i * 3) + 1];
		triC = 3 * ShadowTable[(i * 3) + 2];
		x1 = sXYZ[triA + 0];
		y1 = sXYZ[triA + 1];
		z1 = sXYZ[triA + 2];
		x2 = sXYZ[triB + 0];
		y2 = sXYZ[triB + 1];
		z2 = sXYZ[triB + 2];
		x3 = sXYZ[triC + 0];
		y3 = sXYZ[triC + 1];
		z3 = sXYZ[triC + 2];
		setXYZ3(v, x1, y1, z1, x2, y2, z2, x3, y3, z3, clipflags);
		v[0].color = 0x4F000000;
		v[1].color = 0x4F000000;
		v[2].color = 0x4F000000;

		if (item->after_death)
		{
			v[0].color = 0x80000000 - (item->after_death << 24);
			v[1].color = v[0].color;
			v[2].color = v[0].color;
		}

		v[0].specular = 0xFF000000;
		v[1].specular = 0xFF000000;
		v[2].specular = 0xFF000000;
		Tex.flag = 0;
		Tex.tpage = 0;
		Tex.u1 = 0;
		Tex.v1 = 0;
		Tex.u2 = 0;
		Tex.v2 = 0;
		Tex.u3 = 0;
		Tex.v3 = 0;
		Tex.u4 = 0;
		Tex.v4 = 0;
		Tex.drawtype = 3;
		AddTriSorted(v, 0, 1, 2, &Tex, 1);
	}
}

void DrawTrainStrips()
{
	DrawTrainFloorStrip(-20480, -5120, &textinfo[aranges[7]], 0x1101010);
	DrawTrainFloorStrip(-20480, 3072, &textinfo[aranges[7]], 0x1101010);
	DrawTrainFloorStrip(-20480, -2048, &textinfo[aranges[5]], 0x100800);
	DrawTrainFloorStrip(-20480, 2048, &textinfo[aranges[6]], 0x810);
	DrawTrainFloorStrip(-20480, -1024, &textinfo[aranges[3]], 0);
	DrawTrainFloorStrip(-20480, 1024, &textinfo[aranges[4]], 0);
	DrawTrainFloorStrip(-20480, 0, &textinfo[aranges[2]], 0);
}

void inject_specificfx(bool replace)
{
	INJECT(0x0048B990, DrawTrainStrips, replace);
	INJECT(0x004876C0, S_PrintShadow, replace);
}
