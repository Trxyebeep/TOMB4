#include "../tomb4/pch.h"
#include "specificfx.h"
#include "3dmath.h"
#include "../game/delstuff.h"
#include "../game/control.h"
#include "function_table.h"
#include "../game/objects.h"
#include "polyinsert.h"
#include "function_stubs.h"
#include "drawroom.h"
#include "winmain.h"
#include "mmx.h"
#include "../game/debris.h"
#include "../game/rope.h"
#include "../game/tomb4fx.h"
#include "../game/camera.h"
#include "../game/effect2.h"
#include "gamemain.h"

#ifdef SMOOTH_SHADOWS
#include "../tomb4/tomb4.h"

#define CIRCUMFERENCE_POINTS 32 // Number of points in the circumference
#endif
#include "texture.h"
#include "file.h"
#include "../game/lara.h"

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

char flare_table[121] =
{
//	r, g, b, size, XY?, sprite
	96, 80, 0, 6, 0, 31,
	48, 32, 32, 10, -6, 31,
	32, 24, 24, 18, -1, 31,
	80, 104, 64, 5, -3, 30,
	64, 64, 64, 20, 0, 32,
	96, 56, 56, 14, 0, 11,
	80, 40, 32, 9, 0, 29,
	16, 24, 40, 2, 5, 31,
	8, 8, 24, 7, 8, 31,
	8, 16, 32, 4, 10, 31,
	48, 24, 0, 2, 13, 31,
	40, 96, 72, 1, 16, 11,
	40, 96, 72, 3, 20, 11,
	32, 16, 0, 6, 22, 31,
	32, 16, 0, 9, 23, 30,
	32, 16, 0, 3, 24, 31,
	32, 48, 24, 4, 26, 31,
	8, 40, 112, 3, 27, 11,
	8, 16, 0, 10, 29, 30,
	16, 16, 24, 17, 31, 29,
	-1
};

uchar TargetGraphColTab[48] =
{
	0, 0, 255,
	0, 0, 255,
	255, 255, 0,
	255, 255, 0,
	0, 0, 255,
	0, 0, 255,
	255, 255, 0,
	255, 255, 0,
	0, 0, 255,
	0, 0, 255,
	0, 0, 255,
	0, 0, 255,
	255, 255, 0,
	255, 255, 0,
	255, 255, 0,
	255, 255, 0
};

uchar SplashLinks[347]
{
	16, 18, 0, 2,
	18, 20, 2, 4,
	20, 22, 4, 6,
	22, 24, 6, 8,
	24, 26, 8, 10,
	26, 28, 10, 12,
	28, 30, 12, 14,
	30, 16, 14, 0,
	84, 111, 109, 98,
	32, 82, 97, 105,
	100, 101, 114, 32,
	73, 86, 32, 45,
	32, 84, 104, 101,
	32, 76, 97, 115,
	116, 32, 82, 101,
	118, 101, 108, 97,
	116, 105, 111, 110,
	32, 32, 45, 45,
	32, 68, 101, 100,
	105, 99, 97, 116,
	101, 100, 32, 116,
	111, 32, 109, 121,
	32, 102, 105, 97,
	110, 99, 101, 32,
	74, 97, 121, 32,
	102, 111, 114, 32,
	112, 117, 116, 116,
	105, 110, 103, 32,
	117, 112, 32, 119,
	105, 116, 104, 32,
	116, 104, 105, 115,
	32, 103, 97, 109,
	101, 32, 116, 97,
	107, 105, 110, 103,
	32, 111, 118, 101,
	114, 32, 111, 117,
	114, 32, 108, 105,
	102, 101, 115, 44,
	109, 121, 32, 115,
	116, 101, 112, 32,
	115, 111, 110, 115,
	32, 67, 114, 97,
	105, 103, 44, 74,
	97, 109, 105, 101,
	32, 38, 32, 65,
	105, 100, 101, 110,
	32, 40, 83, 104,
	111, 119, 32, 116,
	104, 105, 115, 32,
	116, 111, 32, 121,
	111, 117, 114, 32,
	109, 97, 116, 101,
	115, 32, 97, 116,
	32, 115, 99, 104,
	111, 111, 108, 44,
	32, 116, 104, 101,
	121, 39, 108, 108,
	32, 98, 101, 108,
	105, 101, 118, 101,
	32, 121, 111, 117,
	32, 110, 111, 119,
	33, 33, 41, 44,
	97, 108, 115, 111,
	32, 102, 111, 114,
	32, 109, 121, 32,
	100, 97, 117, 103,
	104, 116, 101, 114,
	115, 32, 83, 111,
	112, 104, 105, 101,
	32, 97, 110, 100,
	32, 74, 111, 100, 121, 32,
	45, 32, 83, 101,
	101, 32, 121, 111,
	117, 32, 105, 110,
	32, 97, 110, 111,
	116, 104, 101,
	114, 32, 104, 101,
	120, 32, 100, 117,
	109, 112, 32, 45,
	32, 82, 105, 99,
	104, 97, 114, 100,
	32, 70, 108, 111,
	119, 101, 114,
	32, 49, 49,
	47, 49, 49, 47,
	49, 57, 57, 57,
	0, 0, 0, 0
};

MESH_DATA* targetMeshP;
long DoFade;

static MESH_DATA* binocsMeshP;
static WATER_DUST uwdust[256];
static long FadeVal;
static long FadeStep;
static long FadeCnt;
static long FadeEnd;

#ifdef SMOOTH_SHADOWS
static void S_PrintCircleShadow(short size, short* box, ITEM_INFO* item)
{
	TEXTURESTRUCT Tex;
	D3DTLVERTEX v[3];
	PHD_VECTOR pos;
	PHD_VECTOR cv[CIRCUMFERENCE_POINTS];
	PHD_VECTOR cp[CIRCUMFERENCE_POINTS];
	PHD_VECTOR ccv;
	PHD_VECTOR ccp;
	long fx, fy, fz, x, y, z, x1, y1, z1, x2, y2, z2, x3, y3, z3, xSize, zSize, xDist, zDist;
	short room_number;

	xSize = size * (box[1] - box[0]) / 192;	//x size of grid
	zSize = size * (box[5] - box[4]) / 192;	//z size of grid
	xDist = xSize / LINE_POINTS;			//distance between each point of the grid on X
	zDist = zSize / LINE_POINTS;			//distance between each point of the grid on Z
	x = xDist + (xDist >> 1);
	z = zDist + (zDist >> 1);

	for (int i = 0; i < CIRCUMFERENCE_POINTS; i++)
	{
		cp[i].x = x * phd_sin(65536 * i / CIRCUMFERENCE_POINTS) >> W2V_SHIFT;
		cp[i].z = z * phd_cos(65536 * i / CIRCUMFERENCE_POINTS) >> W2V_SHIFT;
		cv[i].x = cp[i].x;
		cv[i].z = cp[i].z;
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

	for (int i = 0; i < CIRCUMFERENCE_POINTS; i++)
	{
		x = cp[i].x;
		z = cp[i].z;
		cp[i].x = (x * phd_mxptr[M00] + z * phd_mxptr[M02] + phd_mxptr[M03]) >> W2V_SHIFT;
		cp[i].z = (x * phd_mxptr[M20] + z * phd_mxptr[M22] + phd_mxptr[M23]) >> W2V_SHIFT;
	}

	ccp.x = phd_mxptr[M03] >> W2V_SHIFT;
	ccp.z = phd_mxptr[M23] >> W2V_SHIFT;
	phd_PopMatrix();

	for (int i = 0; i < CIRCUMFERENCE_POINTS; i++)
	{
		room_number = item->room_number;
		cp[i].y = GetHeight(GetFloor(cp[i].x, item->floor, cp[i].z, &room_number), cp[i].x, item->floor, cp[i].z);

		if (abs(cp[i].y - item->floor) > POINT_HEIGHT_CORRECTION)
			cp[i].y = item->floor;
	}

	room_number = item->room_number;
	ccp.y = GetHeight(GetFloor(ccp.x, item->floor, ccp.z, &room_number), ccp.x, item->floor, ccp.z);

	if (abs(ccp.y - item->floor) > POINT_HEIGHT_CORRECTION)
		ccp.y = item->floor;

	phd_PushMatrix();
	phd_TranslateAbs(pos.x, y, pos.z);
	phd_RotY(item->pos.y_rot);

	for (int i = 0; i < CIRCUMFERENCE_POINTS; i++)
	{
		fx = cv[i].x;
		fy = (cp[i].y - item->floor);
		fz = cv[i].z;
		cv[i].x = (phd_mxptr[M00] * fx + phd_mxptr[M01] * fy + phd_mxptr[M02] * fz + phd_mxptr[M03]) >> W2V_SHIFT;
		cv[i].y = (phd_mxptr[M10] * fx + phd_mxptr[M11] * fy + phd_mxptr[M12] * fz + phd_mxptr[M13]) >> W2V_SHIFT;
		cv[i].z = (phd_mxptr[M20] * fx + phd_mxptr[M21] * fy + phd_mxptr[M22] * fz + phd_mxptr[M23]) >> W2V_SHIFT;
	}

	fy = (ccp.y - item->floor);
	ccv.x = (phd_mxptr[M01] * fy + phd_mxptr[M03]) >> W2V_SHIFT;
	ccv.y = (phd_mxptr[M11] * fy + phd_mxptr[M13]) >> W2V_SHIFT;
	ccv.z = (phd_mxptr[M21] * fy + phd_mxptr[M23]) >> W2V_SHIFT;
	phd_PopMatrix();

	for (int i = 0; i < CIRCUMFERENCE_POINTS; i++) // Draw the pizza
	{
		x1 = (long)cv[i].x;
		y1 = (long)cv[i].y;
		z1 = (long)cv[i].z;
		x2 = (long)cv[(i + 1) % CIRCUMFERENCE_POINTS].x;
		y2 = (long)cv[(i + 1) % CIRCUMFERENCE_POINTS].y;
		z2 = (long)cv[(i + 1) % CIRCUMFERENCE_POINTS].z;
		x3 = (long)ccv.x;
		y3 = (long)ccv.y;
		z3 = (long)ccv.z;
		setXYZ3(v, x1, y1, z1, x2, y2, z2, x3, y3, z3, clipflags);

		if (tomb4.shadow_mode == 3)	//psx like?
		{
			v[0].color = 0x00000000;
			v[1].color = 0x00000000;
			v[2].color = 0xFF000000;
		}
		else
		{
			v[0].color = 0x4F000000;
			v[1].color = 0x4F000000;
			v[2].color = 0x4F000000;
		}

		if (item->after_death)
		{
			if (tomb4.shadow_mode == 3)
				v[2].color = 0xFF000000 - (item->after_death << 24);
			else
			{
				v[0].color = 0x80000000 - (item->after_death << 24);
				v[1].color = v[0].color;
				v[2].color = v[0].color;
			}
		}

		v[0].specular = 0xFF000000;
		v[1].specular = 0xFF000000;
		v[2].specular = 0xFF000000;
		Tex.flag = 0;
		Tex.tpage = 0;
		Tex.drawtype = 3;
		Tex.u1 = 0;
		Tex.v1 = 0;
		Tex.u2 = 0;
		Tex.v2 = 0;
		Tex.u3 = 0;
		Tex.v3 = 0;
		Tex.u4 = 0;
		Tex.v4 = 0;
		AddTriSorted(v, 0, 1, 2, &Tex, 1);
	}
}

static void S_PrintSpriteShadow(short size, short* box, ITEM_INFO* item)
{
	SPRITESTRUCT* sprite;
	TEXTURESTRUCT Tex;
	D3DTLVERTEX v[4];
	PHD_VECTOR pos;
	long xSize, zSize, x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4, opt;

	sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 11];
	xSize = size * (box[1] - box[0]) / 160;
	zSize = size * (box[5] - box[4]) / 160;
	xSize >>= 1;
	zSize >>= 1;

	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->floor, item->pos.z_pos);
	phd_RotY(item->pos.y_rot);

	pos.x = -xSize;
	pos.y = -16;
	pos.z = zSize;
	x1 = (phd_mxptr[M00] * pos.x + phd_mxptr[M01] * pos.y + phd_mxptr[M02] * pos.z + phd_mxptr[M03]) >> W2V_SHIFT;
	y1 = (phd_mxptr[M10] * pos.x + phd_mxptr[M11] * pos.y + phd_mxptr[M12] * pos.z + phd_mxptr[M13]) >> W2V_SHIFT;
	z1 = (phd_mxptr[M20] * pos.x + phd_mxptr[M21] * pos.y + phd_mxptr[M22] * pos.z + phd_mxptr[M23]) >> W2V_SHIFT;

	pos.x = xSize;
	pos.y = -16;
	pos.z = zSize;
	x2 = (phd_mxptr[M00] * pos.x + phd_mxptr[M01] * pos.y + phd_mxptr[M02] * pos.z + phd_mxptr[M03]) >> W2V_SHIFT;
	y2 = (phd_mxptr[M10] * pos.x + phd_mxptr[M11] * pos.y + phd_mxptr[M12] * pos.z + phd_mxptr[M13]) >> W2V_SHIFT;
	z2 = (phd_mxptr[M20] * pos.x + phd_mxptr[M21] * pos.y + phd_mxptr[M22] * pos.z + phd_mxptr[M23]) >> W2V_SHIFT;

	pos.x = xSize;
	pos.y = -16;
	pos.z = -zSize;
	x3 = (phd_mxptr[M00] * pos.x + phd_mxptr[M01] * pos.y + phd_mxptr[M02] * pos.z + phd_mxptr[M03]) >> W2V_SHIFT;
	y3 = (phd_mxptr[M10] * pos.x + phd_mxptr[M11] * pos.y + phd_mxptr[M12] * pos.z + phd_mxptr[M13]) >> W2V_SHIFT;
	z3 = (phd_mxptr[M20] * pos.x + phd_mxptr[M21] * pos.y + phd_mxptr[M22] * pos.z + phd_mxptr[M23]) >> W2V_SHIFT;

	pos.x = -xSize;
	pos.y = -16;
	pos.z = -zSize;
	x4 = (phd_mxptr[M00] * pos.x + phd_mxptr[M01] * pos.y + phd_mxptr[M02] * pos.z + phd_mxptr[M03]) >> W2V_SHIFT;
	y4 = (phd_mxptr[M10] * pos.x + phd_mxptr[M11] * pos.y + phd_mxptr[M12] * pos.z + phd_mxptr[M13]) >> W2V_SHIFT;
	z4 = (phd_mxptr[M20] * pos.x + phd_mxptr[M21] * pos.y + phd_mxptr[M22] * pos.z + phd_mxptr[M23]) >> W2V_SHIFT;
	phd_PopMatrix();

	setXYZ4(v, x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4, clipflags);
	
	for (int i = 0; i < 4; i++)
	{
		v[i].color = 0xFF3C3C3C;
		v[i].specular = 0xFF000000;
	}

	Tex.drawtype = 5;
	Tex.flag = 0;
	Tex.tpage = sprite->tpage;
	Tex.u1 = sprite->x2;
	Tex.v1 = sprite->y2;
	Tex.u2 = sprite->x1;
	Tex.v2 = sprite->y2;
	Tex.u3 = sprite->x1;
	Tex.v3 = sprite->y1;
	Tex.u4 = sprite->x2;
	Tex.v4 = sprite->y1;
	opt = nPolyType;
	nPolyType = 6;
	AddQuadSorted(v, 0, 1, 2, 3, &Tex, 0);
	nPolyType = opt;
}
#endif

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

#ifdef SMOOTH_SHADOWS
	if (tomb4.shadow_mode != 1)
	{
		if (tomb4.shadow_mode == 4)
			S_PrintSpriteShadow(size, box, item);
		else
			S_PrintCircleShadow(size, box, item);

		return;
	}
#endif

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
		hXZ[0] = (x * phd_mxptr[M00] + z * phd_mxptr[M02] + phd_mxptr[M03]) >> W2V_SHIFT;
		hXZ[1] = (x * phd_mxptr[M20] + z * phd_mxptr[M22] + phd_mxptr[M23]) >> W2V_SHIFT;
	}

	phd_PopMatrix();

	hXZ = hxz;
	hY = hy;

	for (int i = 0; i < GRID_POINTS; i++, hXZ += 2, hY++)	//Get height on each grid point and store it in hy array
	{
		room_number = item->room_number;
		*hY = GetHeight(GetFloor(hXZ[0], item->floor, hXZ[1], &room_number), hXZ[0], item->floor, hXZ[1]);

		if (abs(*hY - item->floor) > POINT_HEIGHT_CORRECTION)
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
		sXYZ[0] = (phd_mxptr[M00] * x + phd_mxptr[M01] * y + phd_mxptr[M02] * z + phd_mxptr[M03]) >> W2V_SHIFT;
		sXYZ[1] = (phd_mxptr[M10] * x + phd_mxptr[M11] * y + phd_mxptr[M12] * z + phd_mxptr[M13]) >> W2V_SHIFT;
		sXYZ[2] = (phd_mxptr[M20] * x + phd_mxptr[M21] * y + phd_mxptr[M22] * z + phd_mxptr[M23]) >> W2V_SHIFT;
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

void S_DrawDrawSparks(SPARKS* sptr, long smallest_size, short* xyptr, long* zptr)
{
	SPRITESTRUCT* sprite;
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	long x1, y1, z1, x2, y2, z2, x3, y3, x4, y4;
	long cR, cG, cB, c1, c2, s1, s2, s1h, s2h, scale;
	long sin, cos, sx1, sx2, sy1, sy2, cx1, cx2, cy1, cy2;

	if (sptr->Flags & 8)
	{
		z1 = zptr[0];
		
		if (z1 <= 0)
			return;

		if (z1 >= 0x5000)
		{
			sptr->On = 0;
			return;
		}

		if (sptr->Flags & 2)
		{
			scale = sptr->Size << sptr->Scalar;
			s1 = ((phd_persp * sptr->Size) << sptr->Scalar) / z1;
			s2 = ((phd_persp * sptr->Size) << sptr->Scalar) / z1;

			if (s1 > scale)
				s1 = scale;
			else if (s1 < smallest_size)
				s1 = smallest_size;

			if (s2 > scale)
				s2 = scale;
			else if (s2 < smallest_size)
				s2 = smallest_size;
		}
		else
		{
			s1 = sptr->Size;
			s2 = sptr->Size;
		}

		x1 = xyptr[0];
		y1 = xyptr[1];
		s1h = s1 >> 1;
		s2h = s2 >> 1;

		if (x1 + s1h >= phd_winxmin && x1 - s1h < phd_winxmax && y1 + s2h >= phd_winymin && y1 - s2h < phd_winymax)
		{
			if (sptr->Flags & 0x10)
			{
				sin = rcossin_tbl[sptr->RotAng << 1];
				cos = rcossin_tbl[(sptr->RotAng << 1) + 1];
				sx1 = (-s1h * sin) >> (W2V_SHIFT - 2);
				sx2 = (s1h * sin) >> (W2V_SHIFT - 2);
				sy1 = (-s2h * sin) >> (W2V_SHIFT - 2);
				sy2 = (s2h * sin) >> (W2V_SHIFT - 2);
				cx1 = (-s1h * cos) >> (W2V_SHIFT - 2);
				cx2 = (s1h * cos) >> (W2V_SHIFT - 2);
				cy1 = (-s2h * cos) >> (W2V_SHIFT - 2);
				cy2 = (s2h * cos) >> (W2V_SHIFT - 2);
				x1 = sx1 - cy1 + xyptr[0];
				x2 = sx2 - cy1 + xyptr[0];
				x3 = sx2 - cy2 + xyptr[0];
				x4 = sx1 - cy2 + xyptr[0];
				y1 = cx1 + sy1 + xyptr[1];
				y2 = cx2 + sy1 + xyptr[1];
				y3 = cx2 + sy2 + xyptr[1];
				y4 = cx1 + sy2 + xyptr[1];
				setXY4(v, x1, y1, x2, y2, x3, y3, x4, y4, z1, clipflags);
			}
			else
			{
				x1 = xyptr[0] - s1h;
				x2 = xyptr[0] + s1h;
				y1 = xyptr[1] - s2h;
				y2 = xyptr[1] + s2h;
				setXY4(v, x1, y1, x2, y1, x2, y2, x1, y2, z1, clipflags);
			}

			sprite = &spriteinfo[sptr->Def];

			if (z1 <= 0x3000)
			{
				cR = sptr->R;
				cG = sptr->G;
				cB = sptr->B;
			}
			else
			{
				cR = ((0x5000 - z1) * sptr->R) >> 13;
				cG = ((0x5000 - z1) * sptr->G) >> 13;
				cB = ((0x5000 - z1) * sptr->B) >> 13;
			}

			c1 = RGBA(cR, cG, cB, 0xFF);
			v[0].color = c1;
			v[1].color = c1;
			v[2].color = c1;
			v[3].color = c1;
			v[0].specular = 0xFF000000;
			v[1].specular = 0xFF000000;
			v[2].specular = 0xFF000000;
			v[3].specular = 0xFF000000;

#ifdef GENERAL_FIXES
			if (sptr->TransType == 3)
				tex.drawtype = 5;
			else
#endif
			if (sptr->TransType)
				tex.drawtype = 2;
			else
				tex.drawtype = 1;

			tex.tpage = sprite->tpage;
			tex.u1 = sprite->x1;
			tex.v1 = sprite->y1;
			tex.u2 = sprite->x2;
			tex.v2 = sprite->y1;
			tex.u3 = sprite->x2;
			tex.v3 = sprite->y2;
			tex.u4 = sprite->x1;
			tex.v4 = sprite->y2;
			AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
		}
	}
	else
	{
		x1 = xyptr[0];
		y1 = xyptr[1];
		x2 = xyptr[2];
		y2 = xyptr[3];
		z1 = zptr[0];
		z2 = zptr[1];

		if (z1 <= 0x3000)
		{
			cR = sptr->R;
			cG = sptr->G;
			cB = sptr->B;
		}
		else
		{
			cR = ((0x5000 - z1) * sptr->R) >> 13;
			cG = ((0x5000 - z1) * sptr->G) >> 13;
			cB = ((0x5000 - z1) * sptr->B) >> 13;
		}

		c1 = RGBA(cR, cG, cB, 0xFF);
		c2 = RGBA(cR >> 1, cG >> 1, cB >> 1, 0xFF);
		
		if (ClipLine(x1, y1, z1, x2, y2, z2, phd_winxmin, phd_winymin, phd_winxmax, phd_winymax))
		{
			v[0].sx = (float)x1;
			v[0].sy = (float)y1;
			v[0].rhw = f_mpersp / z1 * f_moneopersp;
			v[0].sz = f_a - v[0].rhw * f_boo;
			v[0].color = c1;
			v[0].specular = 0xFF000000;
			v[1].sx = (float)x2;
			v[1].sy = (float)y2;
			v[1].rhw = f_mpersp / z1 * f_moneopersp;
			v[1].sz = f_a - v[1].rhw * f_boo;
			v[1].color = c2;
			v[1].specular = 0xFF000000;
			AddLineSorted(v, &v[1], 6);
		}
	}
}

void DrawBikeSpeedo(long ux, long uy, long vel, long maxVel, long turboVel, long size, long unk)	//ux and uy are not used
{
	D3DTLVERTEX v[2];
	float x, y, x0, y0, x1, y1;
	long rSize, rVel, rMVel, rTVel, angle;

	x = (float)phd_winxmax / 512.0F * 448.0F;
	y = (float)phd_winymax / 240.0F * 224.0F;
	rSize = (7 * size) >> 3;
	rVel = abs(vel >> 1);

	if (rVel)
	{
		rVel += (((rVel - 4096) >> 5) * phd_sin((GlobalCounter & 7) << (W2V_SHIFT - 1))) >> W2V_SHIFT;

		if (rVel < 0)
			rVel = 0;
	}

	rMVel = maxVel >> 1;
	rTVel = turboVel >> 1;
	angle = -0x4000;

	for (int i = 0; i <= rTVel; i += 2048)
	{
		x0 = ((rSize * (phd_sin(angle + i)) >> (W2V_SHIFT - 1)) - ((rSize * phd_sin(angle + i)) >> (W2V_SHIFT + 1))) * ((float)phd_winxmax / 512.0F);
		y0 = (-(rSize * phd_cos(angle + i)) >> W2V_SHIFT) * (float)phd_winymax / 240.0F;
		x1 = ((size * (phd_sin(angle + i)) >> (W2V_SHIFT - 1)) - ((size * phd_sin(angle + i)) >> (W2V_SHIFT + 1))) * ((float)phd_winxmax / 512.0F);
		y1 = (-(size * phd_cos(angle + i)) >> W2V_SHIFT) * (float)phd_winymax / 240.0F;

		v[0].sx = x + x0;
		v[0].sy = y + y0;
		v[0].sz = f_mznear;
		v[0].rhw = f_moneoznear;

		v[1].sx = x + x1;
		v[1].sy = y + y1;
		v[1].sz = f_mznear;
		v[1].rhw = f_moneoznear;

		if (i > rMVel)
		{
			v[0].color = 0xFFFF0000;
			v[1].color = 0xFFFF0000;
		}
		else
		{
			v[0].color = 0xFFFFFFFF;
			v[1].color = 0xFFFFFFFF;
		}

#ifdef GENERAL_FIXES
		v[0].specular = 0xFF000000;	//originally uninitialized..
#endif

		v[1].specular = v[0].specular;
		AddLineSorted(v, &v[1], 6);
	}

	size -= size >> 4;
	x0 = ((-4 * (phd_sin(angle + rVel)) >> (W2V_SHIFT - 1)) - ((-4 * phd_sin(angle + rVel)) >> (W2V_SHIFT + 1))) * ((float)phd_winxmax / 512.0F);
	y0 = (-(-4 * phd_cos(angle + rVel)) >> W2V_SHIFT) * (float)phd_winymax / 240.0F;
	x1 = ((size * (phd_sin(angle + rVel)) >> (W2V_SHIFT - 1)) - ((size * phd_sin(angle + rVel)) >> (W2V_SHIFT + 1))) * ((float)phd_winxmax / 512.0F);
	y1 = (-(size * phd_cos(angle + rVel)) >> W2V_SHIFT) * (float)phd_winymax / 240.0F;

	v[0].sx = x + x0;
	v[0].sy = y + y0;
	v[0].sz = f_mznear;
	v[0].rhw = f_moneoznear;

	v[1].sx = x + x1;
	v[1].sy = y + y1;
	v[1].sz = f_mznear;
	v[1].rhw = f_moneoznear;

#ifdef GENERAL_FIXES
	v[0].color = 0xFFA0C0FF;	//blueish color, PSX does A0C0E0, changed blue to FF to be as visible
	v[0].specular = 0xFF000000;	//originally uninitialized
#endif

	v[1].color = v[0].color;
	v[1].specular = v[0].specular;
	AddLineSorted(v, &v[1], 6);
}

void Draw2DSprite(long x, long y, long slot, long unused, long unused2)
{
	SPRITESTRUCT* sprite;
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	long x0, y0;

	sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + slot];
	x0 = long(x + (sprite->width >> 8) * ((float)phd_centerx / 320.0F));
	y0 = long(y + 1 + (sprite->height >> 8) * ((float)phd_centery / 240.0F));
	setXY4(v, x, y, x0, y, x0, y0, x, y0, (long)f_mznear, clipflags);
	v[0].specular = 0xFF000000;
	v[1].specular = 0xFF000000;
	v[2].specular = 0xFF000000;
	v[3].specular = 0xFF000000;

#ifdef GENERAL_FIXES
	if (slot == unused)	//'unused' is now the current gear, fight me
	{
		v[0].color = 0xFFFFFFFF;
		v[1].color = 0xFFFFFFFF;
		v[2].color = 0xFFFFFFFF;
		v[3].color = 0xFFFFFFFF;
	}
	else
	{
		v[0].color = 0xFF404040;
		v[1].color = 0xFF404040;
		v[2].color = 0xFF404040;
		v[3].color = 0xFF404040;
	}
#endif

	tex.drawtype = 1;
	tex.flag = 0;
	tex.tpage = sprite->tpage;
	tex.u1 = sprite->x1;
	tex.v1 = sprite->y1;
	tex.u2 = sprite->x2;
	tex.v2 = sprite->y1;
	tex.u3 = sprite->x2;
	tex.v3 = sprite->y2;
	tex.u4 = sprite->x1;
	tex.v4 = sprite->y2;
	AddQuadClippedSorted(v, 0, 1, 2, 3, &tex, 0);
}

void DrawJeepSpeedo(long ux, long uy, long vel, long maxVel, long turboVel, long size, long spriteSlot)	//ux and uy are not used
{
	D3DTLVERTEX v[2];
	float x, y, x0, y0, x1, y1;
	long rSize, rVel, rMVel, rTVel, angle;
#ifdef GENERAL_FIXES
	long sX, sY;
#endif

	x = (float)phd_winxmax / 512.0F * 448.0F;
	y = (float)phd_winymax / 240.0F * 224.0F;
#ifndef GENERAL_FIXES
	Draw2DSprite(long(x + 24), long(y - 16), spriteSlot + 17, RGBONLY(GetRandomDraw() & 0xFF, GetRandomDraw() & 0xFF, GetRandomDraw() & 0xFF), 0);
#endif
	rSize = (7 * size) >> 3;
	rVel = abs(vel >> 1);

	if (rVel)
	{
		rVel += (((rVel - 4096) >> 5) * phd_sin((GlobalCounter & 7) << (W2V_SHIFT - 1))) >> W2V_SHIFT;

		if (rVel < 0)
			rVel = 0;
	}

	rMVel = maxVel >> 1;
	rTVel = turboVel >> 1;
#ifdef GENERAL_FIXES
	rTVel += rTVel >> 1;
#endif
	angle = -0x4000;

#ifdef GENERAL_FIXES
	for (int i = 0; i <= rTVel; i += 1536)
#else
	for (int i = 0; i <= rTVel; i += 2048)
#endif
	{
		x0 = ((rSize * (phd_sin(angle + i)) >> (W2V_SHIFT - 1)) - ((rSize * phd_sin(angle + i)) >> (W2V_SHIFT + 1))) * ((float)phd_winxmax / 512.0F);
		y0 = (-(rSize * phd_cos(angle + i)) >> W2V_SHIFT) * (float)phd_winymax / 240.0F;
		x1 = ((size * (phd_sin(angle + i)) >> (W2V_SHIFT - 1)) - ((size * phd_sin(angle + i)) >> (W2V_SHIFT + 1))) * ((float)phd_winxmax / 512.0F);
		y1 = (-(size * phd_cos(angle + i)) >> W2V_SHIFT) * (float)phd_winymax / 240.0F;

		v[0].sx = x + x0;
		v[0].sy = y + y0;
		v[0].sz = f_mznear;
		v[0].rhw = f_moneoznear;

		v[1].sx = x + x1;
		v[1].sy = y + y1;
		v[1].sz = f_mznear;
		v[1].rhw = f_moneoznear;

		if (i > rMVel)
		{
			v[0].color = 0xFFFF0000;
			v[1].color = 0xFFFF0000;
		}
		else
		{
			v[0].color = 0xFFFFFFFF;
			v[1].color = 0xFFFFFFFF;
		}

#ifdef GENERAL_FIXES
		v[0].specular = 0xFF000000;	//originally uninitialized..
#endif

		v[1].specular = v[0].specular;
		AddLineSorted(v, &v[1], 6);
	}

	size -= size >> 4;
	x0 = ((-4 * (phd_sin(angle + rVel)) >> (W2V_SHIFT - 1)) - ((-4 * phd_sin(angle + rVel)) >> (W2V_SHIFT + 1))) * ((float)phd_winxmax / 512.0F);
	y0 = (-(-4 * phd_cos(angle + rVel)) >> W2V_SHIFT) * (float)phd_winymax / 240.0F;
	x1 = ((size * (phd_sin(angle + rVel)) >> (W2V_SHIFT - 1)) - ((size * phd_sin(angle + rVel)) >> (W2V_SHIFT + 1))) * ((float)phd_winxmax / 512.0F);
	y1 = (-(size * phd_cos(angle + rVel)) >> W2V_SHIFT) * (float)phd_winymax / 240.0F;

	v[0].sx = x + x0;
	v[0].sy = y + y0;
	v[0].sz = f_mznear;
	v[0].rhw = f_moneoznear;

	v[1].sx = x + x1;
	v[1].sy = y + y1;
	v[1].sz = f_mznear;
	v[1].rhw = f_moneoznear;

#ifdef GENERAL_FIXES
	v[0].color = 0xFFA0C0FF;	//blueish color, PSX does A0C0E0, changed blue to FF to be as visible
	v[0].specular = 0xFF000000;	//originally uninitialized
	sX = long(x + 16 * ((float)phd_winxmax / 512.0F));
	sY = long(y - 20 * ((float)phd_winymax / 240.0F));
	Draw2DSprite(sX, sY, 17, spriteSlot + 17, 0);
	sY = long(y - 6 * ((float)phd_winymax / 240.0F));
	Draw2DSprite(sX, sY, 18, spriteSlot + 17, 0);
#endif

	v[1].color = v[0].color;
	v[1].specular = v[0].specular;
	AddLineSorted(v, &v[1], 6);
}

void DrawDebris()
{
	DEBRIS_STRUCT* dptr;
	TEXTURESTRUCT* tex;
	D3DTLVERTEX v[3];
	long* Z;
	short* XY;
	short* offsets;
	long r, g, b, c;
	ushort drawbak;

	XY = (short*)&scratchpad[0];
	Z = (long*)&scratchpad[256];
	offsets = (short*)&scratchpad[512];

	for (int i = 0; i < 256; i++)
	{
		dptr = &debris[i];

		if (!dptr->On)
			continue;

		phd_PushMatrix();
		phd_TranslateAbs(dptr->x, dptr->y, dptr->z);

#ifdef GENERAL_FIXES				//PSX & TR5 have this
		phd_RotY(dptr->YRot << 8);
		phd_RotX(dptr->XRot << 8);
#endif

		offsets[0] = dptr->XYZOffsets1[0];
		offsets[1] = dptr->XYZOffsets1[1];
		offsets[2] = dptr->XYZOffsets1[2];
		XY[0] = short((phd_mxptr[M03] + phd_mxptr[M00] * offsets[0] + phd_mxptr[M01] * offsets[1] + phd_mxptr[M02] * offsets[2]) >> W2V_SHIFT);
		XY[1] = short((phd_mxptr[M13] + phd_mxptr[M10] * offsets[0] + phd_mxptr[M11] * offsets[1] + phd_mxptr[M12] * offsets[2]) >> W2V_SHIFT);
		Z[0] = (phd_mxptr[M23] + phd_mxptr[M20] * offsets[0] + phd_mxptr[M21] * offsets[1] + phd_mxptr[M22] * offsets[2]) >> W2V_SHIFT;

		offsets[0] = dptr->XYZOffsets2[0];
		offsets[1] = dptr->XYZOffsets2[1];
		offsets[2] = dptr->XYZOffsets2[2];
		XY[2] = short((phd_mxptr[M03] + phd_mxptr[M00] * offsets[0] + phd_mxptr[M01] * offsets[1] + phd_mxptr[M02] * offsets[2]) >> W2V_SHIFT);
		XY[3] = short((phd_mxptr[M13] + phd_mxptr[M10] * offsets[0] + phd_mxptr[M11] * offsets[1] + phd_mxptr[M12] * offsets[2]) >> W2V_SHIFT);
		Z[1] = (phd_mxptr[M23] + phd_mxptr[M20] * offsets[0] + phd_mxptr[M21] * offsets[1] + phd_mxptr[M22] * offsets[2]) >> W2V_SHIFT;

		offsets[0] = dptr->XYZOffsets3[0];
		offsets[1] = dptr->XYZOffsets3[1];
		offsets[2] = dptr->XYZOffsets3[2];
		XY[4] = short((phd_mxptr[M03] + phd_mxptr[M00] * offsets[0] + phd_mxptr[M01] * offsets[1] + phd_mxptr[M02] * offsets[2]) >> W2V_SHIFT);
		XY[5] = short((phd_mxptr[M13] + phd_mxptr[M10] * offsets[0] + phd_mxptr[M11] * offsets[1] + phd_mxptr[M12] * offsets[2]) >> W2V_SHIFT);
		Z[2] = (phd_mxptr[M23] + phd_mxptr[M20] * offsets[0] + phd_mxptr[M21] * offsets[1] + phd_mxptr[M22] * offsets[2]) >> W2V_SHIFT;

		setXYZ3(v, XY[0], XY[1], Z[0], XY[2], XY[3], Z[1], XY[4], XY[5], Z[2], clipflags);
		phd_PopMatrix();

		c = dptr->color1 & 0xFF;
		r = ((c * dptr->r) >> 8) + CLRR(dptr->ambient);
		g = ((c * dptr->g) >> 8) + CLRG(dptr->ambient);
		b = ((c * dptr->b) >> 8) + CLRB(dptr->ambient);

		if (r > 255)
			r = 255;

		if (g > 255)
			g = 255;

		if (b > 255)
			b = 255;

		c = RGBONLY(r, g, b);
		CalcColorSplit(c, &v[0].color);

		c = dptr->color2 & 0xFF;
		r = ((c * dptr->r) >> 8) + CLRR(dptr->ambient);
		g = ((c * dptr->g) >> 8) + CLRG(dptr->ambient);
		b = ((c * dptr->b) >> 8) + CLRB(dptr->ambient);

		if (r > 255)
			r = 255;

		if (g > 255)
			g = 255;

		if (b > 255)
			b = 255;

		c = RGBONLY(r, g, b);
		CalcColorSplit(c, &v[1].color);

		c = dptr->color3 & 0xFF;
		r = ((c * dptr->r) >> 8) + CLRR(dptr->ambient);
		g = ((c * dptr->g) >> 8) + CLRG(dptr->ambient);
		b = ((c * dptr->b) >> 8) + CLRB(dptr->ambient);

		if (r > 255)
			r = 255;

		if (g > 255)
			g = 255;

		if (b > 255)
			b = 255;

		c = RGBONLY(r, g, b);
		CalcColorSplit(c, &v[2].color);

		v[0].color |= 0xFF000000;
		v[1].color |= 0xFF000000;
		v[2].color |= 0xFF000000;
		v[0].specular |= 0xFF000000;
		v[1].specular |= 0xFF000000;
		v[2].specular |= 0xFF000000;

		tex = &textinfo[(long)dptr->TextInfo & 0x7FFF];
		drawbak = tex->drawtype;

		if (dptr->flags & 1)
			tex->drawtype = 2;

		if (!tex->drawtype)
			AddTriZBuffer(v, 0, 1, 2, tex, 1);
		else if (tex->drawtype <= 2)
			AddTriSorted(v, 0, 1, 2, tex, 1);

		tex->drawtype = drawbak;
	}
}

void DoScreenFade()
{
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	long a;

	a = FadeVal << 24;
	FadeVal += FadeStep;
	FadeCnt++;

	if (FadeCnt > 8)
	{
		DoFade = 2;
		a = FadeEnd << 24;
	}

	v[0].sx = 0;
	v[0].sy = 0;
	v[0].sz = 0;
	v[0].rhw = f_moneoznear;
	v[0].color = a;
	v[0].specular = 0xFF000000;

	v[1].sx = float(phd_winxmax + 1);
	v[1].sy = (float)phd_winymin;
	v[1].sz = 0;
	v[1].rhw = f_moneoznear;
	v[1].color = a;
	v[1].specular = 0xFF000000;

	v[2].sx = float(phd_winxmax + 1);
	v[2].sy = float(phd_winymax + 1);
	v[2].sz = 0;
	v[2].rhw = f_moneoznear;
	v[2].color = a;
	v[2].specular = 0xFF000000;

	v[3].sx = (float)phd_winxmin;
	v[3].sy = float(phd_winymax + 1);
	v[3].sz = 0;
	v[3].rhw = f_moneoznear;
	v[3].color = a;
	v[3].specular = 0xFF000000;

	tex.drawtype = 3;
	tex.flag = 0;
	tex.tpage = 0;
	clipflags[0] = 0;
	clipflags[1] = 0;
	clipflags[2] = 0;
	clipflags[3] = 0;
	AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
}

void DrawPsxTile(long x_y, long height_width, long color, long u0, long u1)
{
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	float x, y, z, rhw, w, h;
	long col;
	ushort drawtype;

	nPolyType = 6;

	if ((color & 0xFF000000) == 0x62000000)
	{
		drawtype = 3;
		col = color << 24;
	}
	else
	{
		drawtype = 2;
		col = color | 0xFF000000;
	}

	if (!gfCurrentLevel)
	{
		z = f_znear + 10;
		rhw = f_moneoznear + 50;
	}
	else
	{
		z = f_znear;
		rhw = f_moneoznear;
	}

	x = float(x_y >> 16);
	y = float(x_y & 0xFFFF);
	w = float(height_width & 0xFFFF);
	h = float(height_width >> 16);

	v[0].sx = x;
	v[0].sy = y;
	v[0].sz = z;
	v[0].rhw = rhw;
	v[0].color = col;
	v[0].specular = 0xFF000000;

	v[1].sx = x + w + 1;
	v[1].sy = y;
	v[1].sz = z;
	v[1].rhw = rhw;
	v[1].color = col;
	v[1].specular = 0xFF000000;

	v[2].sx = x + w + 1;
	v[2].sy = y + h + 1;
	v[2].sz = z;
	v[2].rhw = rhw;
	v[2].color = col;
	v[2].specular = 0xFF000000;

	v[3].sx = x;
	v[3].sy = y + h + 1;
	v[3].sz = z;
	v[3].rhw = rhw;
	v[3].color = col;
	v[3].specular = 0xFF000000;

	tex.drawtype = drawtype;
	tex.flag = 0;
	tex.tpage = 0;
	clipflags[0] = 0;
	clipflags[1] = 0;
	clipflags[2] = 0;
	clipflags[3] = 0;
	AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
}

void DrawFlash()
{
	long r, g, b;

	r = ((FlashFadeR * FlashFader) >> 5) & 0xFF;
	g = ((FlashFadeG * FlashFader) >> 5) & 0xFF;
	b = ((FlashFadeB * FlashFader) >> 5) & 0xFF;
	DrawPsxTile(0, phd_winwidth | (phd_winheight << 16), RGBA(r, g, b, 0x62), 1, 0);
	DrawPsxTile(0, phd_winwidth | (phd_winheight << 16), RGBA(r, g, b, 0xFF), 2, 0);
}

void S_DrawDarts(ITEM_INFO* item)
{
	D3DTLVERTEX v[2];
	long x1, y1, z1, x2, y2, z2, num, mxx, mxy, mxz, xx, yy, zz;
	float zv;

	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	zv = f_persp / (float)phd_mxptr[M23];
	x1 = short(float(phd_mxptr[M03] * zv + f_centerx));
	y1 = short(float(phd_mxptr[M13] * zv + f_centery));
	z1 = phd_mxptr[M23] >> W2V_SHIFT;
	num = (-96 * phd_cos(item->pos.x_rot)) >> W2V_SHIFT;
	mxx = (num * phd_sin(item->pos.y_rot)) >> W2V_SHIFT;
	mxy = (96 * phd_sin(item->pos.x_rot)) >> W2V_SHIFT;
	mxz = (num * phd_cos(item->pos.y_rot)) >> W2V_SHIFT;
	xx = phd_mxptr[M00] * mxx + phd_mxptr[M01] * mxy + phd_mxptr[M02] * mxz + phd_mxptr[M03];
	yy = phd_mxptr[M10] * mxx + phd_mxptr[M11] * mxy + phd_mxptr[M12] * mxz + phd_mxptr[M13];
	zz = phd_mxptr[M20] * mxx + phd_mxptr[M21] * mxy + phd_mxptr[M22] * mxz + phd_mxptr[M23];
	zv = f_persp / (float)zz;
	x2 = short(float(xx * zv + f_centerx));
	y2 = short(float(yy * zv + f_centery));
	z2 = zz >> W2V_SHIFT;

	if (ClipLine(x1, y1, z1, x2, y2, z2, phd_winxmin, phd_winymin, phd_winxmax, phd_winymax))
	{
		zv = f_mpersp / (float)z1 * f_moneopersp;
		v[0].sx = (float)x1;
		v[0].sy = (float)y1;
		v[0].sz = f_a - zv * f_boo;
		v[0].rhw = zv;
		v[0].color = 0xFF000000;
		v[0].specular = 0xFF000000;
		v[1].sx = (float)x2;
		v[1].sy = (float)y2;
		v[1].sz = f_a - zv * f_boo;
		v[1].rhw = zv;
		v[1].color = 0xFF783C14;
		v[1].specular = 0xFF000000;
		AddLineSorted(v, &v[1], 6);
	}

	phd_PopMatrix();
}

void ClipCheckPoint(D3DTLVERTEX* v, float x, float y, float z, short* clip)
{
	float perspz;
	short clipdistance;

	v->tu = x;
	v->tv = y;
	v->sz = z;
	clipdistance = 0;

	if (v->sz < f_mznear)
		clipdistance = -128;
	else
	{
		perspz = f_mpersp / v->sz;

		if (v->sz > FogEnd)
		{
			v->sz = f_zfar;
			clipdistance = 256;
		}

		v->sx = perspz * v->tu + f_centerx;
		v->sy = perspz * v->tv + f_centery;
		v->rhw = perspz * f_moneopersp;

		if (v->sx < phd_winxmin)
			clipdistance++;
		else if (phd_winxmax < v->sx)
			clipdistance += 2;

		if (v->sy < phd_winymin)
			clipdistance += 4;
		else if (v->sy > phd_winymax)
			clipdistance += 8;
	}

	clip[0] = clipdistance;
}

void DrawFlatSky(ulong color, long zpos, long ypos, long drawtype)
{
	PHD_VECTOR vec[4];
	D3DTLVERTEX v[4];
	TEXTURESTRUCT Tex;
	short* clip;
	long x, y, z;

	phd_PushMatrix();
	phd_TranslateRel(zpos, ypos, 0);

	vec[0].x = -5632;
	vec[0].y = 0;
	vec[0].z = 4864;
	vec[1].x = 5632;
	vec[1].y = 0;
	vec[1].z = 4864;
	vec[2].x = 5632;
	vec[2].y = 0;
	vec[2].z = -4864;
	vec[3].x = -5632;
	vec[3].y = 0;
	vec[3].z = -4864;

	for (int i = 0; i < 4; i++)
	{
		x = vec[i].x;
		y = vec[i].y;
		z = vec[i].z;
		vec[i].x = (phd_mxptr[M00] * x + phd_mxptr[M01] * y + phd_mxptr[M02] * z + phd_mxptr[M03]) >> W2V_SHIFT;
		vec[i].y = (phd_mxptr[M10] * x + phd_mxptr[M11] * y + phd_mxptr[M12] * z + phd_mxptr[M13]) >> W2V_SHIFT;
		vec[i].z = (phd_mxptr[M20] * x + phd_mxptr[M21] * y + phd_mxptr[M22] * z + phd_mxptr[M23]) >> W2V_SHIFT;
		v[i].color = color | 0xFF000000;
		v[i].specular = 0xFF000000;
		CalcColorSplit(color, &v[i].color);
	}

	clip = clipflags;
	ClipCheckPoint(&v[0], (float)vec[0].x, (float)vec[0].y, (float)vec[0].z, clip);	//originally inlined
	clip++;
	ClipCheckPoint(&v[1], (float)vec[1].x, (float)vec[1].y, (float)vec[1].z, clip);	//originally inlined
	clip++;
	ClipCheckPoint(&v[2], (float)vec[2].x, (float)vec[2].y, (float)vec[2].z, clip);	//originally inlined
	clip++;
	ClipCheckPoint(&v[3], (float)vec[3].x, (float)vec[3].y, (float)vec[3].z, clip);	//the only one that survived
	Tex.drawtype = (ushort)drawtype;
	Tex.flag = 0;
	Tex.tpage = ushort(nTextures - 1);
	Tex.u1 = 0;
	Tex.v1 = 0;
	Tex.u2 = 1;
	Tex.v2 = 0;
	Tex.u3 = 1;
	Tex.v3 = 1;
	Tex.u4 = 0;
	Tex.v4 = 1;
	AddQuadSorted(v, 3, 2, 1, 0, &Tex, 1);

	phd_TranslateRel(-11264, 0, 0);
	vec[0].x = -5632;
	vec[0].y = 0;
	vec[0].z = 4864;
	vec[1].x = 5632;
	vec[1].y = 0;
	vec[1].z = 4864;
	vec[2].x = 5632;
	vec[2].y = 0;
	vec[2].z = -4864;
	vec[3].x = -5632;
	vec[3].y = 0;
	vec[3].z = -4864;

	for (int i = 0; i < 4; i++)
	{
		x = vec[i].x;
		y = vec[i].y;
		z = vec[i].z;
		vec[i].x = (phd_mxptr[M00] * x + phd_mxptr[M01] * y + phd_mxptr[M02] * z + phd_mxptr[M03]) >> W2V_SHIFT;
		vec[i].y = (phd_mxptr[M10] * x + phd_mxptr[M11] * y + phd_mxptr[M12] * z + phd_mxptr[M13]) >> W2V_SHIFT;
		vec[i].z = (phd_mxptr[M20] * x + phd_mxptr[M21] * y + phd_mxptr[M22] * z + phd_mxptr[M23]) >> W2V_SHIFT;
		v[i].color |= 0xFF000000;
		v[i].specular = 0xFF000000;
		CalcColorSplit(color, &v[i].color);
	}

	clip = clipflags;
	ClipCheckPoint(&v[0], (float)vec[0].x, (float)vec[0].y, (float)vec[0].z, clip);	//originally inlined
	clip++;
	ClipCheckPoint(&v[1], (float)vec[1].x, (float)vec[1].y, (float)vec[1].z, clip);	//originally inlined
	clip++;
	ClipCheckPoint(&v[2], (float)vec[2].x, (float)vec[2].y, (float)vec[2].z, clip);	//originally inlined
	clip++;
	ClipCheckPoint(&v[3], (float)vec[3].x, (float)vec[3].y, (float)vec[3].z, clip);	//the only one that survived
	AddQuadSorted(v, 3, 2, 1, 0, &Tex, 1);
	phd_PopMatrix();
}

void OutputSky()
{
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, 0);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 0);
	DrawBuckets();
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, 1);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 1);
	SortPolyList(SortCount, SortList);
	RestoreFPCW(FPCW);
	MMXSetPerspecLimit(0);
	DrawSortList();
	MMXSetPerspecLimit(0.6F);
	MungeFPCW(&FPCW);
	InitBuckets();
	InitialiseSortList();
}

void setXY4(D3DTLVERTEX* v, long x1, long y1, long x2, long y2, long x3, long y3, long x4, long y4, long z, short* clip)
{
	float zv;
	short clip_distance;

	clip_distance = 0;
	zv = f_mpersp / (float)z;
	v->sx = (float)x1;
	v->sy = (float)y1;
	v->sz = (float)z;
	v->rhw = f_moneopersp * zv;

	if (phd_winxmin > v->sx)
		clip_distance = 1;
	else if (phd_winxmax < v->sx)
		clip_distance = 2;

	if (phd_winymin > v->sy)
		clip_distance += 4;
	else if (phd_winymax < v->sy)
		clip_distance += 8;

	clip[0] = clip_distance;
	v++;
	clip_distance = 0;
	v->sx = (float)x2;
	v->sy = (float)y2;
	v->sz = (float)z;
	v->rhw = f_moneopersp * zv;

	if (phd_winxmin > v->sx)
		clip_distance = 1;
	else if (phd_winxmax < v->sx)
		clip_distance = 2;

	if (phd_winymin > v->sy)
		clip_distance += 4;
	else if (phd_winymax < v->sy)
		clip_distance += 8;

	clip[1] = clip_distance;
	v++;
	clip_distance = 0;
	v->sx = (float)x3;
	v->sy = (float)y3;
	v->sz = (float)z;
	v->rhw = f_moneopersp * zv;

	if (phd_winxmin > v->sx)
		clip_distance = 1;
	else if (phd_winxmax < v->sx)
		clip_distance = 2;

	if (phd_winymin > v->sy)
		clip_distance += 4;
	else if (phd_winymax < v->sy)
		clip_distance += 8;

	clip[2] = clip_distance;
	v++;
	clip_distance = 0;
	v->sx = (float)x4;
	v->sy = (float)y4;
	v->sz = (float)z;
	v->rhw = f_moneopersp * zv;

	if (phd_winxmin > v->sx)
		clip_distance = 1;
	else if (phd_winxmax < v->sx)
		clip_distance = 2;

	if (phd_winymin > v->sy)
		clip_distance += 4;
	else if (phd_winymax < v->sy)
		clip_distance += 8;

	clip[3] = clip_distance;
}

void setXY3(D3DTLVERTEX* v, long x1, long y1, long x2, long y2, long x3, long y3, long z, short* clip)
{
	float zv;
	short clip_distance;

	clip_distance = 0;
	zv = f_mpersp / (float)z;
	v->sx = (float)x1;
	v->sy = (float)y1;
	v->sz = (float)z;
	v->rhw = f_moneopersp * zv;

	if (phd_winxmin > v->sx)
		clip_distance = 1;
	else if (phd_winxmax < v->sx)
		clip_distance = 2;

	if (phd_winymin > v->sy)
		clip_distance += 4;
	else if (phd_winymax < v->sy)
		clip_distance += 8;

	clip[0] = clip_distance;
	v++;
	clip_distance = 0;
	v->sx = (float)x2;
	v->sy = (float)y2;
	v->sz = (float)z;
	v->rhw = f_moneopersp * zv;

	if (phd_winxmin > v->sx)
		clip_distance = 1;
	else if (phd_winxmax < v->sx)
		clip_distance = 2;

	if (phd_winymin > v->sy)
		clip_distance += 4;
	else if (phd_winymax < v->sy)
		clip_distance += 8;

	clip[1] = clip_distance;
	v++;
	clip_distance = 0;
	v->sx = (float)x3;
	v->sy = (float)y3;
	v->sz = (float)z;
	v->rhw = f_moneopersp * zv;

	if (phd_winxmin > v->sx)
		clip_distance = 1;
	else if (phd_winxmax < v->sx)
		clip_distance = 2;

	if (phd_winymin > v->sy)
		clip_distance += 4;
	else if (phd_winymax < v->sy)
		clip_distance += 8;

	clip[2] = clip_distance;
}

void setXYZ4(D3DTLVERTEX* v, long x1, long y1, long z1, long x2, long y2, long z2, long x3, long y3, long z3, long x4, long y4, long z4, short* clip)
{
	float zv;
	short clip_distance;

	clip_distance = 0;
	v->tu = (float)x1;
	v->tv = (float)y1;
	v->sz = (float)z1;

	if (v->sz < f_mznear)
		clip_distance = -128;
	else
	{
		zv = f_mpersp / v->sz;

		if (v->sz > FogEnd)
		{
			clip_distance = 256;
			v->sz = f_zfar;
		}

		v->sx = zv * v->tu + f_centerx;
		v->sy = zv * v->tv + f_centery;
		v->rhw = f_moneopersp * zv;

		if (phd_winxmin > v->sx)
			clip_distance++;
		else if (phd_winxmax < v->sx)
			clip_distance += 2;

		if (phd_winymin > v->sy)
			clip_distance += 4;
		else if (phd_winymax < v->sy)
			clip_distance += 8;
	}

	v++;
	clip[0] = clip_distance;
	clip_distance = 0;
	v->tu = (float)x2;
	v->tv = (float)y2;
	v->sz = (float)z2;

	if (v->sz < f_mznear)
		clip_distance = -128;
	else
	{
		zv = f_mpersp / v->sz;

		if (v->sz > FogEnd)
		{
			clip_distance = 256;
			v->sz = f_zfar;
		}

		v->sx = zv * v->tu + f_centerx;
		v->sy = zv * v->tv + f_centery;
		v->rhw = f_moneopersp * zv;

		if (phd_winxmin > v->sx)
			clip_distance++;
		else if (phd_winxmax < v->sx)
			clip_distance += 2;

		if (phd_winymin > v->sy)
			clip_distance += 4;
		else if (phd_winymax < v->sy)
			clip_distance += 8;
	}

	v++;
	clip[1] = clip_distance;
	clip_distance = 0;
	v->tu = (float)x3;
	v->tv = (float)y3;
	v->sz = (float)z3;

	if (v->sz < f_mznear)
		clip_distance = -128;
	else
	{
		zv = f_mpersp / v->sz;

		if (v->sz > FogEnd)
		{
			clip_distance = 256;
			v->sz = f_zfar;
		}

		v->sx = zv * v->tu + f_centerx;
		v->sy = zv * v->tv + f_centery;
		v->rhw = f_moneopersp * zv;

		if (phd_winxmin > v->sx)
			clip_distance++;
		else if (phd_winxmax < v->sx)
			clip_distance += 2;

		if (phd_winymin > v->sy)
			clip_distance += 4;
		else if (phd_winymax < v->sy)
			clip_distance += 8;
	}

	v++;
	clip[2] = clip_distance;
	clip_distance = 0;
	v->tu = (float)x4;
	v->tv = (float)y4;
	v->sz = (float)z4;

	if (v->sz < f_mznear)
		clip_distance = -128;
	else
	{
		zv = f_mpersp / v->sz;

		if (v->sz > FogEnd)
		{
			clip_distance = 256;
			v->sz = f_zfar;
		}

		v->sx = zv * v->tu + f_centerx;
		v->sy = zv * v->tv + f_centery;
		v->rhw = f_moneopersp * zv;

		if (phd_winxmin > v->sx)
			clip_distance++;
		else if (phd_winxmax < v->sx)
			clip_distance += 2;

		if (phd_winymin > v->sy)
			clip_distance += 4;
		else if (phd_winymax < v->sy)
			clip_distance += 8;
	}

	clip[3] = clip_distance;
}

void setXYZ3(D3DTLVERTEX* v, long x1, long y1, long z1, long x2, long y2, long z2, long x3, long y3, long z3, short* clip)
{
	float zv;
	short clip_distance;

	clip_distance = 0;
	v->tu = (float)x1;
	v->tv = (float)y1;
	v->sz = (float)z1;

	if (v->sz < f_mznear)
		clip_distance = -128;
	else
	{
		zv = f_mpersp / v->sz;

		if (v->sz > FogEnd)
		{
			clip_distance = 256;
			v->sz = f_zfar;
		}

		v->sx = zv * v->tu + f_centerx;
		v->sy = zv * v->tv + f_centery;
		v->rhw = f_moneopersp * zv;

		if (phd_winxmin > v->sx)
			clip_distance++;
		else if (phd_winxmax < v->sx)
			clip_distance += 2;

		if (phd_winymin > v->sy)
			clip_distance += 4;
		else if (phd_winymax < v->sy)
			clip_distance += 8;
	}

	v++;
	clip[0] = clip_distance;
	clip_distance = 0;
	v->tu = (float)x2;
	v->tv = (float)y2;
	v->sz = (float)z2;

	if (v->sz < f_mznear)
		clip_distance = -128;
	else
	{
		zv = f_mpersp / v->sz;

		if (v->sz > FogEnd)
		{
			clip_distance = 256;
			v->sz = f_zfar;
		}

		v->sx = zv * v->tu + f_centerx;
		v->sy = zv * v->tv + f_centery;
		v->rhw = f_moneopersp * zv;

		if (phd_winxmin > v->sx)
			clip_distance++;
		else if (phd_winxmax < v->sx)
			clip_distance += 2;

		if (phd_winymin > v->sy)
			clip_distance += 4;
		else if (phd_winymax < v->sy)
			clip_distance += 8;
	}

	v++;
	clip[1] = clip_distance;
	clip_distance = 0;
	v->tu = (float)x3;
	v->tv = (float)y3;
	v->sz = (float)z3;

	if (v->sz < f_mznear)
		clip_distance = -128;
	else
	{
		zv = f_mpersp / v->sz;

		if (v->sz > FogEnd)
		{
			clip_distance = 256;
			v->sz = f_zfar;
		}

		v->sx = zv * v->tu + f_centerx;
		v->sy = zv * v->tv + f_centery;
		v->rhw = f_moneopersp * zv;

		if (phd_winxmin > v->sx)
			clip_distance++;
		else if (phd_winxmax < v->sx)
			clip_distance += 2;

		if (phd_winymin > v->sy)
			clip_distance += 4;
		else if (phd_winymax < v->sy)
			clip_distance += 8;
	}

	clip[2] = clip_distance;
}

void SetFade(long start, long end)
{
	DoFade = 1;
	FadeVal = start;
	FadeStep = (end - start) >> 3;
	FadeCnt = 0;
	FadeEnd = end;
}

void DrawLaserSightSprite()
{
	SPRITESTRUCT* sprite;
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	PHD_VECTOR vec;
	long* Z;
	short* XY;
	short* pos;
	float perspz;

	XY = (short*)&scratchpad[0];
	Z = (long*)&scratchpad[256];
	pos = (short*)&scratchpad[512];
	phd_PushMatrix();
	phd_TranslateAbs(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);
	pos[0] = short(LaserSightX - lara_item->pos.x_pos);
	pos[1] = short(LaserSightY - lara_item->pos.y_pos);
	pos[2] = short(LaserSightZ - lara_item->pos.z_pos);
	vec.x = phd_mxptr[M00] * pos[0] + phd_mxptr[M01] * pos[1] + phd_mxptr[M02] * pos[2] + phd_mxptr[M03];
	vec.y = phd_mxptr[M10] * pos[0] + phd_mxptr[M11] * pos[1] + phd_mxptr[M12] * pos[2] + phd_mxptr[M13];
	vec.z = phd_mxptr[M20] * pos[0] + phd_mxptr[M21] * pos[1] + phd_mxptr[M22] * pos[2] + phd_mxptr[M23];
	perspz = f_persp / (float)vec.z;
	XY[0] = short(float(vec.x * perspz + f_centerx));
	XY[1] = short(float(vec.y * perspz + f_centery));
	Z[0] = vec.z >> W2V_SHIFT;
	phd_PopMatrix();

	sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 14];
	setXY4(v, XY[0] - 2, XY[1] - 2, XY[0] + 2, XY[1] - 2, XY[0] - 2, XY[1] + 2, XY[0] + 2, XY[1] + 2, (long)f_mznear, clipflags);
	v[0].color = 0xFFFF0000;
	v[1].color = 0xFFFF0000;
	v[2].color = 0xFFFF0000;
	v[3].color = 0xFFFF0000;
	v[0].specular = 0xFF000000;
	v[1].specular = 0xFF000000;
	v[2].specular = 0xFF000000;
	v[3].specular = 0xFF000000;
	tex.drawtype = 2;
	tex.flag = 0;
	tex.tpage = sprite->tpage;
	tex.u1 = sprite->x1;
	tex.v1 = sprite->y1;
	tex.u2 = sprite->x2;
	tex.v2 = sprite->y1;
	tex.u3 = sprite->x2;
	tex.v3 = sprite->y2;
	tex.u4 = sprite->x1;
	tex.v4 = sprite->y2;
	AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
	LaserSightActive = 0;
}

void DrawSprite(long x, long y, long slot, long col, long size, long z)
{
	SPRITESTRUCT* sprite;
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	long s;

	s = long(float(phd_winwidth / 640.0F) * (size << 1));

	if (z)
		setXY4(v, x - s, y - s, x + s, y - s, x - s, y + s, x + s, y + s, long(z + f_mznear), clipflags);
	else
		setXY4(v, x - s, y - s, x + s, y - s, x - s, y + s, x + s, y + s, (long)f_mzfar, clipflags);

	sprite = &spriteinfo[slot + objects[DEFAULT_SPRITES].mesh_index];
	v[0].specular = 0xFF000000;
	v[1].specular = 0xFF000000;
	v[2].specular = 0xFF000000;
	v[3].specular = 0xFF000000;
	CalcColorSplit(col, &v[0].color);
	v[1].color = v[0].color | 0xFF000000;
	v[2].color = v[0].color | 0xFF000000;
	v[3].color = v[0].color | 0xFF000000;
	tex.drawtype = 2;
	tex.flag = 0;
	tex.tpage = sprite->tpage;
	tex.u1 = sprite->x1;
	tex.v1 = sprite->y1;
	tex.u2 = sprite->x2;
	tex.v2 = sprite->y1;
	tex.u3 = sprite->x2;
	tex.v3 = sprite->y2;
	tex.u4 = sprite->x1;
	tex.v4 = sprite->y2;
	AddQuadSorted(v, 0, 1, 3, 2, &tex, 0);
}

void ShowTitle()
{
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	long x, y, w;

	clipflags[0] = 0;
	clipflags[1] = 0;
	clipflags[2] = 0;
	clipflags[3] = 0;
	nPolyType = 4;
	x = long(phd_winxmin - float((phd_winxmax / 640.0F) * -64));
	w = long(float((phd_winxmax / 640.0F) * 256));
	y = long(phd_winymin + float((phd_winymax / 480.0F) * 256));

	v[0].sx = (float)x;
	v[0].sy = (float)phd_winymin;
	v[0].sz = 0;
	v[0].rhw = f_moneoznear;
	v[0].color = 0xFFFFFFFF;
	v[0].specular = 0xFF000000;

	v[1].sx = (float)(w + x);
	v[1].sy = (float)phd_winymin;
	v[1].sz = 0;
	v[1].rhw = f_moneoznear;
	v[1].color = 0xFFFFFFFF;
	v[1].specular = 0xFF000000;

	v[2].sx = (float)(w + x);
	v[2].sy = (float)y;
	v[2].sz = 0;
	v[2].rhw = f_moneoznear;
	v[2].color = 0xFFFFFFFF;
	v[2].specular = 0xFF000000;

	v[3].sx = (float)x;
	v[3].sy = (float)y;
	v[3].sz = 0;
	v[3].rhw = f_moneoznear;
	v[3].color = 0xFFFFFFFF;
	v[3].specular = 0xFF000000;

	tex.drawtype = 1;
	tex.flag = 0;
	tex.tpage = ushort(nTextures - 4);
	tex.u1 = 0.00390625F;
	tex.v1 = 0.00390625F;
	tex.u2 = 0.99609375F;
	tex.v2 = 0.00390625F;
	tex.u3 = 0.99609375F;
	tex.v3 = 0.99609375F;
	tex.u4 = 0.00390625F;
	tex.v4 = 0.99609375F;
	AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);

	v[0].sx = (float)(w + x);
	v[0].sy = (float)phd_winymin;
	v[0].sz = 0;
	v[0].rhw = f_moneoznear;
	v[0].color = 0xFFFFFFFF;
	v[0].specular = 0xFF000000;

	v[1].sx = (float)(2 * w + x);
	v[1].sy = (float)phd_winymin;
	v[1].sz = 0;
	v[1].rhw = f_moneoznear;
	v[1].color = 0xFFFFFFFF;
	v[1].specular = 0xFF000000;

	v[2].sx = (float)(2 * w + x);
	v[2].sy = (float)y;
	v[2].sz = 0;
	v[2].rhw = f_moneoznear;
	v[2].color = 0xFFFFFFFF;
	v[2].specular = 0xFF000000;

	v[3].sx = (float)(w + x);
	v[3].sy = (float)y;
	v[3].sz = 0;
	v[3].rhw = f_moneoznear;
	v[3].color = 0xFFFFFFFF;
	v[3].specular = 0xFF000000;

	tex.drawtype = 1;
	tex.flag = 0;
	tex.tpage = ushort(nTextures - 3);
	tex.u1 = 0.00390625F;
	tex.v1 = 0.00390625F;
	tex.u2 = 0.99609375F;
	tex.v2 = 0.00390625F;
	tex.u3 = 0.99609375F;
	tex.v3 = 0.99609375F;
	tex.u4 = 0.00390625F;
	tex.v4 = 0.99609375F;
	AddQuadSorted(v, 0, 1, 2, 3, &tex, 1);
}

void SetUpLensFlare(long x, long y, long z, GAME_VECTOR* lfobj)
{
	PHD_VECTOR pos;
	GAME_VECTOR start;
	GAME_VECTOR target;
	long* Z;
	short* vec;
	short* XY;
	float perspz;
	long dx, dy, dz, r, g, b, r2, g2, b2, los, num, flash;
	short rn;

	los = 0;

	if (lfobj)
	{
		pos.x = lfobj->x;
		pos.y = lfobj->y;
		pos.z = lfobj->z;
		dx = abs(pos.x - lara_item->pos.x_pos);
		dy = abs(pos.y - lara_item->pos.y_pos);
		dz = abs(pos.z - lara_item->pos.z_pos);

		if (dx > 0x8000 || dy > 0x8000 || dz > 0x8000)
			return;

		r = 255;
		g = 255;
		b = 255;
		rn = lfobj->room_number;
	}
	else
	{
		if (room[camera.pos.room_number].flags & ROOM_NO_LENSFLARE)
			return;

		r = (uchar)gfLensFlareColour.r;
		g = (uchar)gfLensFlareColour.g;
		b = (uchar)gfLensFlareColour.b;
		pos.x = x;
		pos.y = y;
		pos.z = z;

		while (abs(pos.x) > 0x36000 || abs(pos.y) > 0x36000 || abs(pos.z) > 0x36000)
		{
			pos.x -= (x - camera.pos.x) >> 4;
			pos.y -= (y - camera.pos.y) >> 4;
			pos.z -= (z - camera.pos.z) >> 4;
		}

		dx = (pos.x - camera.pos.x) >> 4;
		dy = (pos.y - camera.pos.y) >> 4;
		dz = (pos.z - camera.pos.z) >> 4;

		while (abs(pos.x - camera.pos.x) > 0x8000 || abs(pos.y - camera.pos.y) > 0x8000 || abs(pos.z - camera.pos.z) > 0x8000)
		{
			pos.x -= dx;
			pos.y -= dy;
			pos.z -= dz;
		}

		dx = (pos.x - camera.pos.x) >> 4;
		dy = (pos.y - camera.pos.y) >> 4;
		dz = (pos.z - camera.pos.z) >> 4;

		for (int i = 0; i < 16; i++)
		{
			IsRoomOutsideNo = 255;
			IsRoomOutside(pos.x, pos.y, pos.z);
			rn = IsRoomOutsideNo;

			if (rn != 255)
				break;

			pos.x -= dx;
			pos.y -= dy;
			pos.z -= dz;
		}
	}

	if (rn != 255)
	{
		if (room[rn].flags & ROOM_NOT_INSIDE || lfobj)
		{
			start.y = camera.pos.y;
			start.z = camera.pos.z;
			start.x = camera.pos.x;
			start.room_number = camera.pos.room_number;
			target.x = pos.x;
			target.y = pos.y;
			target.z = pos.z;
			los = LOS(&start, &target);
		}
	}

	if (!los && lfobj)	//can't see object, don't bother
		return;

	vec = (short*)&scratchpad[0];
	XY = (short*)&scratchpad[16];
	Z = (long*)&scratchpad[32];

	phd_PushMatrix();
	phd_TranslateAbs(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);

	if (lfobj)
	{
		vec[0] = short(pos.x - lara_item->pos.x_pos);
		vec[1] = short(pos.y - lara_item->pos.y_pos);
		vec[2] = short(pos.z - lara_item->pos.z_pos);
	}
	else
	{
		pos.x = x - lara_item->pos.x_pos;
		pos.y = y - lara_item->pos.y_pos;
		pos.z = z - lara_item->pos.z_pos;
		
		while (pos.x > 0x7F00 || pos.x < -0x7F00 || pos.y > 0x7F00 || pos.y < -0x7F00 || pos.z > 0x7F00 || pos.z < -0x7F00)
		{
			pos.x >>= 1;
			pos.y >>= 1;
			pos.z >>= 1;
		}

		vec[0] = (short)pos.x;
		vec[1] = (short)pos.y;
		vec[2] = (short)pos.z;
	}

	pos.x = phd_mxptr[M00] * vec[0] + phd_mxptr[M01] * vec[1] + phd_mxptr[M02] * vec[2] + phd_mxptr[M03];
	pos.y = phd_mxptr[M10] * vec[0] + phd_mxptr[M11] * vec[1] + phd_mxptr[M12] * vec[2] + phd_mxptr[M13];
	pos.z = phd_mxptr[M20] * vec[0] + phd_mxptr[M21] * vec[1] + phd_mxptr[M22] * vec[2] + phd_mxptr[M23];
	perspz = f_persp / (float)pos.z;
	XY[0] = short(float(pos.x * perspz + f_centerx));
	XY[1] = short(float(pos.y * perspz + f_centery));
	Z[0] = pos.z >> W2V_SHIFT;
	phd_PopMatrix();
	num = 0;

	if (lfobj)
		num += 6;

	if (Z[0] > 0)
	{
		dx = XY[0] - phd_centerx;
		dy = XY[1] - phd_centery;
		dz = phd_sqrt(SQUARE(dx) + SQUARE(dy));

		if (dz < 640)
		{
			dz = 640 - dz;

			if (los)
			{
				flash = dz - 544;

				if (flash > 0)
				{
					FlashFader = 32;
					FlashFadeR = short((r * flash) / 640);
					FlashFadeG = short((g * flash) / 640);
					FlashFadeB = short((b * flash) / 640);
				}
			}

			while (flare_table[num] != -1)
			{
				if (num)
				{
					r2 = dz * flare_table[num] / 640;
					g2 = dz * flare_table[num + 1] / 640;
					b2 = dz * flare_table[num + 2] / 640;
				}
				else
				{
					if (lfobj)
						continue;

					r2 = flare_table[0] + (GetRandomDraw() & 8);
					g2 = flare_table[1];
					b2 = flare_table[2] + (GetRandomDraw() & 8);
				}

				r2 = (r * r2) >> 8;
				g2 = (g * g2) >> 8;
				b2 = (b * b2) >> 8;

				if (r2 | g2 | b2)
				{
					pos.x = XY[0] - ((dx * flare_table[num + 4]) >> 4);
					pos.y = XY[1] - ((dy * flare_table[num + 4]) >> 4);
					DrawSprite(pos.x, pos.y, flare_table[num + 5], RGBONLY(r2, g2, b2), flare_table[num + 3] << 1, num);
				}

				if (!los)
					return;

				num += 6;
			}
		}
	}
}

void InitTarget_2()
{
	OBJECT_INFO* obj;
	D3DTLVERTEX* v;

	obj = &objects[TARGET_GRAPHICS];

	if (!obj->loaded)
		return;

	targetMeshP = (MESH_DATA*)meshes[obj->mesh_index];
	targetMeshP->SourceVB->Lock(DDLOCK_READONLY, (void**)&v, 0);

	for (int i = 0; i < targetMeshP->nVerts; i++)
	{
		v[i].sx = (v[i].sx * 80) / 96;
		v[i].sy = (v[i].sy * 60) / 224;
		v[i].sz = 0;
		v[i].rhw = f_mpersp / f_mznear * f_moneopersp;
		v[i].color = 0xFF000000;
		v[i].specular = 0xFF000000;
	}

	targetMeshP->SourceVB->Unlock();
}

void InitBinoculars()
{
	OBJECT_INFO* obj;
	D3DTLVERTEX* v;

	obj = &objects[BINOCULAR_GRAPHICS];

	if (!obj->loaded)
		return;

	binocsMeshP = (MESH_DATA*)meshes[obj->mesh_index];
	binocsMeshP->SourceVB->Lock(DDLOCK_READONLY, (void**)&v, 0);

	for (int i = 0; i < binocsMeshP->nVerts; i++)
	{
		v[i].sx = (v[i].sx * 32) / 96;
		v[i].sy = (v[i].sy * 30) / 224;
		v[i].sz = 0;
		v[i].rhw = f_mpersp / f_mznear * f_moneopersp;
		v[i].color = 0xFF000000;
		v[i].specular = 0xFF000000;
	}

	binocsMeshP->SourceVB->Unlock();
}

void DrawBinoculars()
{
	MESH_DATA* mesh;
	D3DTLVERTEX* v;
	TEXTURESTRUCT* tex;
	D3DTLVERTEX vtx[256];
	short* clip;
	short* quad;
	short* tri;
	ushort drawbak;
	short clipdistance;

	if (LaserSight)
		mesh = targetMeshP;
	else
		mesh = binocsMeshP;

	mesh->SourceVB->Lock(DDLOCK_READONLY, (void**)&v, 0);
	clip = clipflags;

	for (int i = 0; i < mesh->nVerts; i++)
	{
		clipdistance = 0;
		vtx[i] = v[i];
		vtx[i].sx = (vtx[i].sx * float(phd_winxmax / 512.0F)) + f_centerx;
		vtx[i].sy = (vtx[i].sy * float(phd_winymax / 240.0F)) + f_centery;

		if (vtx[i].sx < f_left)
			clipdistance = 1;
		else if (vtx[i].sx > f_right)
			clipdistance = 2;

		if (vtx[i].sy < f_top)
			clipdistance += 4;
		else if (vtx[i].sy > f_bottom)
			clipdistance += 8;

		*clip++ = clipdistance;
	}

	mesh->SourceVB->Unlock();
	quad = mesh->gt4;
	tri = mesh->gt3;

	if (LaserSight)
	{
		for (int i = 0; i < mesh->ngt4; i++, quad += 6)
		{
			tex = &textinfo[quad[4] & 0x7FFF];
			drawbak = tex->drawtype;
			tex->drawtype = 0;

			if (quad[5] & 1)
			{
				vtx[quad[0]].color = 0xFF000000;
				vtx[quad[1]].color = 0xFF000000;
				vtx[quad[2]].color = 0;
				vtx[quad[3]].color = 0;
				tex->drawtype = 3;
			}

			AddQuadSorted(vtx, quad[0], quad[1], quad[2], quad[3], tex, 1);
			tex->drawtype = drawbak;
		}

		for (int i = 0, j = 0; i < mesh->ngt3; i++, tri += 5)
		{
			tex = &textinfo[tri[3] & 0x7FFF];
			drawbak = tex->drawtype;
			tex->drawtype = 0;

			if (tri[4] & 1)
			{
				vtx[tri[0]].color = TargetGraphColTab[j] << 24;
				vtx[tri[1]].color = TargetGraphColTab[j + 1] << 24;
				vtx[tri[2]].color = TargetGraphColTab[j + 2] << 24;
				tex->drawtype = 3;
				j += 3;
			}

			AddTriSorted(vtx, tri[0], tri[1], tri[2], tex, 1);
			tex->drawtype = drawbak;
		}
	}
	else
	{
		for (int i = 0; i < mesh->ngt4; i++, quad += 6)
		{
			tex = &textinfo[quad[4] & 0x7FFF];
			drawbak = tex->drawtype;
			tex->drawtype = 0;

			if (quad[5] & 1)
			{
				vtx[quad[0]].color = 0xFF000000;
				vtx[quad[1]].color = 0xFF000000;
				vtx[quad[2]].color = 0;
				vtx[quad[3]].color = 0;
				tex->drawtype = 3;
			}

			AddQuadSorted(vtx, quad[0], quad[1], quad[2], quad[3], tex, 1);
			tex->drawtype = drawbak;
		}

		for (int i = 0; i < mesh->ngt3; i++, tri += 5)
		{
			tex = &textinfo[tri[3] & 0x7FFF];
			drawbak = tex->drawtype;
			tex->drawtype = 0;

			if (tri[4] & 1)
			{
				vtx[tri[0]].color = 0;
				vtx[tri[1]].color = 0xFF000000;
				vtx[tri[2]].color = 0;
				tex->drawtype = 3;
			}

			AddTriSorted(vtx, tri[0], tri[1], tri[2], tex, 1);
			tex->drawtype = drawbak;
		}
	}
}

void DrawWraithTrail(ITEM_INFO* item)
{
	WRAITH_STRUCT* wraith;
	PHD_VECTOR pos;
	D3DTLVERTEX v[2];
	long* Z;
	short* XY;
	short* offsets;
	float perspz;
	ulong r, g, b;
	long c0, c1, x0, y0, z0, x1, y1, z1;

	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	for (int i = 0; i < 5; i++)
	{
		if (!i)
			phd_RotY(-1092);
		else if (i == 2)
			phd_RotY(1092);
		else if (i == 3)
			phd_RotZ(-1092);
		else if (i == 4)
			phd_RotZ(1092);

		XY = (short*)&scratchpad[0];
		Z = (long*)&scratchpad[256];
		offsets = (short*)&scratchpad[512];
		wraith = (WRAITH_STRUCT*)item->data;

		for (int j = 0; j < 8; j++, XY += 2, Z += 2, wraith++)
		{
			offsets[0] = short(wraith->pos.x - item->pos.x_pos);
			offsets[1] = short(wraith->pos.y - item->pos.y_pos);
			offsets[2] = short(wraith->pos.z - item->pos.z_pos);
			pos.x = offsets[0] * phd_mxptr[M00] + offsets[1] * phd_mxptr[M01] + offsets[2] * phd_mxptr[M02] + phd_mxptr[M03];
			pos.y = offsets[0] * phd_mxptr[M10] + offsets[1] * phd_mxptr[M11] + offsets[2] * phd_mxptr[M12] + phd_mxptr[M13];
			pos.z = offsets[0] * phd_mxptr[M20] + offsets[1] * phd_mxptr[M21] + offsets[2] * phd_mxptr[M22] + phd_mxptr[M23];
			perspz = f_persp / (float)pos.z;
			XY[0] = short(float(pos.x * perspz + f_centerx));
			XY[1] = short(float(pos.y * perspz + f_centery));
			Z[0] = pos.z >> W2V_SHIFT;

			if (!j || j == 7)
				Z[1] = 0;
			else
				Z[1] = RGBONLY(wraith->r, wraith->g, wraith->b);
		}

		XY = (short*)&scratchpad[0];
		Z = (long*)&scratchpad[256];

		for (int j = 0; j < 7; j++, XY += 2, Z += 2)
		{
			if (Z[0] <= f_mznear || Z[0] >= 20480)
				continue;

			if (Z[0] > 12288)
			{
				r = ((Z[1] & 0xFF) * (12288 - Z[0])) >> 13;
				g = (((Z[1] >> 8) & 0xFF) * (12288 - Z[0])) >> 13;
				b = (((Z[1] >> 16) & 0xFF) * (12288 - Z[0])) >> 13;
				c0 = RGBA(r, g, b, 0xFF);
				r = ((Z[3] & 0xFF) * (12288 - Z[0])) >> 13;
				g = (((Z[3] >> 8) & 0xFF) * (12288 - Z[0])) >> 13;
				b = (((Z[3] >> 16) & 0xFF) * (12288 - Z[0])) >> 13;
				c1 = RGBA(r, g, b, 0xFF);
			}
			else
			{
				c0 = Z[1];
				c1 = Z[3];
			}

			x0 = XY[0];
			y0 = XY[1];
			z0 = Z[0];
			x1 = XY[2];
			y1 = XY[3];
			z1 = Z[2];

			if (ClipLine(x0, y0, z0, x1, y1, z1, phd_winxmin, phd_winymin, phd_winxmax, phd_winymax))
			{
				v[0].sx = (float)x0;
				v[0].sy = (float)y0;
				v[0].sz = (float)z0;
				v[0].rhw = f_mpersp / v[0].sz * f_moneopersp;
				v[0].color = c0;
				v[0].specular = 0xFF000000;

				v[1].sx = (float)x1;
				v[1].sy = (float)y1;
				v[1].sz = (float)z1;
				v[1].rhw = f_mpersp / v[1].sz * f_moneopersp;
				v[1].color = c1;
				v[1].specular = 0xFF000000;

				AddLineSorted(v, &v[1], 6);
			}
		}
	}

	phd_PopMatrix();
}

void DrawDrips()
{
	DRIP_STRUCT* drip;
	PHD_VECTOR vec;
	D3DTLVERTEX v[3];
	long* Z;
	short* XY;
	short* pos;
	float perspz;
	long x0, y0, z0, x1, y1, z1, r, g, b;

	phd_PushMatrix();
	phd_TranslateAbs(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);

	for (int i = 0; i < 32; i++)
	{
		drip = &Drips[i];

		if (!drip->On)
			continue;

		XY = (short*)&scratchpad[0];
		Z = (long*)&scratchpad[256];
		pos = (short*)&scratchpad[512];
		pos[0] = short(drip->x - lara_item->pos.x_pos);
		pos[1] = short(drip->y - lara_item->pos.y_pos);
		pos[2] = short(drip->z - lara_item->pos.z_pos);

		if (pos[0] < -20480 || pos[0] > 20480 || pos[1] < -20480 || pos[1] > 20480 || pos[2] < -20480 || pos[2] > 20480)
			continue;

		vec.x = pos[0] * phd_mxptr[M00] + pos[1] * phd_mxptr[M01] + pos[2] * phd_mxptr[M02] + phd_mxptr[M03];
		vec.y = pos[0] * phd_mxptr[M10] + pos[1] * phd_mxptr[M11] + pos[2] * phd_mxptr[M12] + phd_mxptr[M13];
		vec.z = pos[0] * phd_mxptr[M20] + pos[1] * phd_mxptr[M21] + pos[2] * phd_mxptr[M22] + phd_mxptr[M23];

		perspz = f_persp / (float)vec.z;
		XY[0] = short(float(vec.x * perspz + f_centerx));
		XY[1] = short(float(vec.y * perspz + f_centery));
		Z[0] = vec.z >> W2V_SHIFT;

		pos[1] -= drip->Yvel >> 6;

		if (room[drip->RoomNumber].flags & ROOM_NOT_INSIDE)
		{
			pos[0] -= short(SmokeWindX >> 1);
			pos[1] -= short(SmokeWindZ >> 1);
		}

		vec.x = pos[0] * phd_mxptr[M00] + pos[1] * phd_mxptr[M01] + pos[2] * phd_mxptr[M02] + phd_mxptr[M03];
		vec.y = pos[0] * phd_mxptr[M10] + pos[1] * phd_mxptr[M11] + pos[2] * phd_mxptr[M12] + phd_mxptr[M13];
		vec.z = pos[0] * phd_mxptr[M20] + pos[1] * phd_mxptr[M21] + pos[2] * phd_mxptr[M22] + phd_mxptr[M23];

		perspz = f_persp / (float)vec.z;
		XY[2] = short(float(vec.x * perspz + f_centerx));
		XY[3] = short(float(vec.y * perspz + f_centery));
		Z[1] = vec.z >> W2V_SHIFT;

		if (!Z[0])
			continue;

		if (Z[0] > 20480)
		{
			drip->On = 0;
			continue;
		}

		x0 = XY[0];
		y0 = XY[1];
		z0 = Z[0];
		x1 = XY[2];
		y1 = XY[3];
		z1 = Z[1];

		if (ClipLine(x0, y0, z0, x1, y1, z1, phd_winxmin, phd_winymin, phd_winxmax, phd_winymax))
		{
			r = drip->R << 2;
			g = drip->G << 2;
			b = drip->B << 2;

			v[0].sx = (float)x0;
			v[0].sy = (float)y0;
			v[0].sz = (float)z0;
			v[0].rhw = f_mpersp / v[0].sz * f_moneopersp;
			v[0].color = RGBA(r, g, b, 0xFF);
			v[0].specular = 0xFF000000;

			r >>= 1;
			g >>= 1;
			b >>= 1;

			v[1].sx = (float)x1;
			v[1].sy = (float)y1;
			v[1].sz = (float)z1;
			v[1].rhw = f_mpersp / v[1].sz * f_moneopersp;
			v[1].color = RGBA(r, g, b, 0xFF);
			v[1].specular = 0xFF000000;

			AddLineSorted(v, &v[1], 6);
		}
	}

	phd_PopMatrix();
}

void DrawBubbles()
{
	BUBBLE_STRUCT* bubble;
	SPRITESTRUCT* sprite;
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	PHD_VECTOR pos;
	long* Z;
	short* XY;
	short* offsets;
	float perspz;
	long dx, dy, dz, size, x1, y1, x2, y2;

	phd_PushMatrix();
	phd_TranslateAbs(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);
	bubble = Bubbles;

	XY = (short*)&scratchpad[0];
	Z = (long*)&scratchpad[256];
	offsets = (short*)&scratchpad[512];

	for (int i = 0; i < 40; i++)
	{
		if (!bubble->size)
		{
			bubble++;
			continue;
		}

		dx = bubble->pos.x - lara_item->pos.x_pos;
		dy = bubble->pos.y - lara_item->pos.y_pos;
		dz = bubble->pos.z - lara_item->pos.z_pos;

		if (dx < -0x5000 || dx > 0x5000 || dy < -0x5000 || dy > 0x5000 || dz < -0x5000 || dz > 0x5000)
		{
			bubble->size = 0;
			bubble++;
			continue;
		}

		offsets[0] = (short)dx;
		offsets[1] = (short)dy;
		offsets[2] = (short)dz;
		pos.x = offsets[0] * phd_mxptr[M00] + offsets[1] * phd_mxptr[M01] + offsets[2] * phd_mxptr[M02] + phd_mxptr[M03];
		pos.y = offsets[0] * phd_mxptr[M10] + offsets[1] * phd_mxptr[M11] + offsets[2] * phd_mxptr[M12] + phd_mxptr[M13];
		pos.z = offsets[0] * phd_mxptr[M20] + offsets[1] * phd_mxptr[M21] + offsets[2] * phd_mxptr[M22] + phd_mxptr[M23];
		perspz = f_persp / (float)pos.z;
		XY[0] = short(float(pos.x * perspz + f_centerx));
		XY[1] = short(float(pos.y * perspz + f_centery));
		Z[0] = pos.z >> W2V_SHIFT;

		if (Z[0] < 32)
		{
			bubble++;
			continue;
		}

		if (Z[0] > 0x5000)
		{
			bubble->size = 0;
			bubble++;
			continue;
		}

		size = phd_persp * (bubble->size >> 1) / Z[0];

		if (size > 128)
		{
			bubble->size = 0;
			continue;
		}

		if (size < 4)
			size = 4;

		size >>= 1;

		x1 = XY[0] - size;
		y1 = XY[1] - size;
		x2 = XY[0] + size;
		y2 = XY[1] + size;

		if (x2 < phd_winxmin || x1 >= phd_winxmax || y2 < phd_winymin || y1 >= phd_winymax)
		{
			bubble++;
			continue;
		}

		sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 13];
		setXY4(v, x1, y1, x2, y1, x2, y2, x1, y2, Z[0], clipflags);
		v[0].color = RGBA(bubble->shade, bubble->shade, bubble->shade, 0xFF);
		v[1].color = RGBA(bubble->shade, bubble->shade, bubble->shade, 0xFF);
		v[2].color = RGBA(bubble->shade, bubble->shade, bubble->shade, 0xFF);
		v[3].color = RGBA(bubble->shade, bubble->shade, bubble->shade, 0xFF);
		v[0].specular = 0xFF000000;
		v[1].specular = 0xFF000000;
		v[2].specular = 0xFF000000;
		v[3].specular = 0xFF000000;
		tex.drawtype = 2;
		tex.flag = 0;
		tex.tpage = sprite->tpage;
		tex.u1 = sprite->x1;
		tex.v1 = sprite->y1;
		tex.u2 = sprite->x2;
		tex.v3 = sprite->y2;
		tex.v2 = sprite->y1;
		tex.u3 = sprite->x2;
		tex.u4 = sprite->x1;
		tex.v4 = sprite->y2;
		AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
		bubble++;
	}

	phd_PopMatrix();
}

void DrawShockwaves()
{
	SHOCKWAVE_STRUCT* wave;
	SPRITESTRUCT* sprite;
	D3DTLVERTEX vtx[4];
	TEXTURESTRUCT tex;
	PHD_VECTOR p1, p2, p3;
	long* Z;
	short* XY;
	short* offsets;
	long v, x1, y1, x2, y2, x3, y3, x4, y4, r, g, b, c;
	short rad;

	sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 8];
	offsets = (short*)&scratchpad[768];

	for (int i = 0; i < 16; i++)
	{
		wave = &ShockWaves[i];

		if (!wave->life)
			continue;

		XY = (short*)&scratchpad[0];
		Z = (long*)&scratchpad[256];
		phd_PushMatrix();
		phd_TranslateAbs(wave->x, wave->y, wave->z);
		phd_RotX(wave->XRot);
		offsets[1] = 0;
		offsets[5] = 0;
		offsets[9] = 0;
		rad = wave->OuterRad;

		for (int j = 0; j < 2; j++)
		{
			offsets[0] = (rad * phd_sin(0)) >> W2V_SHIFT;
			offsets[2] = (rad * phd_cos(0)) >> W2V_SHIFT;
			offsets[4] = (rad * phd_sin(0x1000)) >> W2V_SHIFT;
			offsets[6] = (rad * phd_cos(0x1000)) >> W2V_SHIFT;
			offsets[8] = (rad * phd_sin(0x2000)) >> W2V_SHIFT;
			offsets[10] = (rad * phd_cos(0x2000)) >> W2V_SHIFT;

			for (int k = 1; k < 7; k++)
			{
				v = k * 0x3000;

				p1.x = (offsets[0] * phd_mxptr[M00] + offsets[1] * phd_mxptr[M01] + offsets[2] * phd_mxptr[M02] + phd_mxptr[M03]) >> W2V_SHIFT;
				p1.y = (offsets[0] * phd_mxptr[M10] + offsets[1] * phd_mxptr[M11] + offsets[2] * phd_mxptr[M12] + phd_mxptr[M13]) >> W2V_SHIFT;
				p1.z = (offsets[0] * phd_mxptr[M20] + offsets[1] * phd_mxptr[M21] + offsets[2] * phd_mxptr[M22] + phd_mxptr[M23]) >> W2V_SHIFT;

				p2.x = (offsets[4] * phd_mxptr[M00] + offsets[5] * phd_mxptr[M01] + offsets[6] * phd_mxptr[M02] + phd_mxptr[M03]) >> W2V_SHIFT;
				p2.y = (offsets[4] * phd_mxptr[M10] + offsets[5] * phd_mxptr[M11] + offsets[6] * phd_mxptr[M12] + phd_mxptr[M13]) >> W2V_SHIFT;
				p2.z = (offsets[4] * phd_mxptr[M20] + offsets[5] * phd_mxptr[M21] + offsets[6] * phd_mxptr[M22] + phd_mxptr[M23]) >> W2V_SHIFT;

				p3.x = (offsets[8] * phd_mxptr[M00] + offsets[9] * phd_mxptr[M01] + offsets[10] * phd_mxptr[M02] + phd_mxptr[M03]) >> W2V_SHIFT;
				p3.y = (offsets[8] * phd_mxptr[M10] + offsets[9] * phd_mxptr[M11] + offsets[10] * phd_mxptr[M12] + phd_mxptr[M13]) >> W2V_SHIFT;
				p3.z = (offsets[8] * phd_mxptr[M20] + offsets[9] * phd_mxptr[M21] + offsets[10] * phd_mxptr[M22] + phd_mxptr[M23]) >> W2V_SHIFT;

				offsets[0] = (rad * phd_sin(v)) >> W2V_SHIFT;
				offsets[2] = (rad * phd_cos(v)) >> W2V_SHIFT;
				offsets[4] = (rad * phd_sin(v + 0x1000)) >> W2V_SHIFT;
				offsets[6] = (rad * phd_cos(v + 0x1000)) >> W2V_SHIFT;
				offsets[8] = (rad * phd_sin(v + 0x2000)) >> W2V_SHIFT;
				offsets[10] = (rad * phd_cos(v + 0x2000)) >> W2V_SHIFT;

				XY[0] = (short)p1.x;
				XY[1] = (short)p1.y;
				Z[0] = p1.z;

				XY[2] = (short)p2.x;
				XY[3] = (short)p2.y;
				Z[1] = p2.z;

				XY[4] = (short)p3.x;
				XY[5] = (short)p3.y;
				Z[2] = p3.z;

				XY += 6;
				Z += 3;
			}

			rad = wave->InnerRad;
		}

		phd_PopMatrix();
		XY = (short*)&scratchpad[0];
		Z = (long*)&scratchpad[256];

		for (int j = 0; j < 16; j++)
		{
			x1 = XY[0];
			y1 = XY[1];
			x2 = XY[2];
			y2 = XY[3];
			x3 = XY[36];
			y3 = XY[37];
			x4 = XY[38];
			y4 = XY[39];
			setXYZ4(vtx, x1, y1, Z[0], x2, y2, Z[1], x4, y4, Z[19], x3, y3, Z[18], clipflags);

			r = wave->r;
			g = wave->g;
			b = wave->b;

			if (wave->life < 8)
			{
				r = (r * wave->life) >> 3;
				g = (g * wave->life) >> 3;
				b = (b * wave->life) >> 3;
			}

			c = RGBA(b, g, r, 0xFF);
			vtx[0].color = c;
			vtx[1].color = c;
			vtx[2].color = c;
			vtx[3].color = c;
			vtx[0].specular = 0xFF000000;
			vtx[1].specular = 0xFF000000;
			vtx[2].specular = 0xFF000000;
			vtx[3].specular = 0xFF000000;

			tex.drawtype = 2;
			tex.flag = 0;
			tex.tpage = sprite->tpage;
			tex.u1 = sprite->x1;
			tex.v1 = sprite->y2;
			tex.u2 = sprite->x2;
			tex.v2 = sprite->y2;
			tex.u3 = sprite->x2;
			tex.v3 = sprite->y1;
			tex.u4 = sprite->x1;
			tex.v4 = sprite->y1;
			AddQuadSorted(vtx, 0, 1, 2, 3, &tex, 1);

			XY += 2;
			Z++;
		}
	}
}

void DrawTrainFloorStrip(long x, long z, TEXTURESTRUCT* tex, long y_and_flags)
{
	SVECTOR* offsets;
	D3DTLVERTEX v[4];
	PHD_VECTOR p1, p2, p3;
	long* Z;
	short* XY;
	long num, z1, z2, z3, z4, spec;
	short x1, y1, x2, y2, x3, y3, x4, y4;

	num = 0;
	offsets = (SVECTOR*)&scratchpad[984];
	offsets[0].z = (short)z;
	offsets[1].z = short(z + 512);
	offsets[2].z = short(z + 1024);

	if (y_and_flags & 0x1000000)
	{
		offsets[1].z += 1024;
		offsets[2].z += 2048;
	}

	offsets[0].y = ((y_and_flags >> 16) & 0xFF) << 4;
	offsets[1].y = ((y_and_flags >> 8) & 0xFF) << 4;
	offsets[2].y = (y_and_flags & 0xFF) << 4;
	
	offsets[0].x = (short)x;
	offsets[1].x = (short)x;
	offsets[2].x = (short)x;

	for (int i = 0; i < 2; i++)
	{
		XY = (short*)&scratchpad[0];
		Z = (long*)&scratchpad[492];
		XY -= 6;
		Z -= 3;

		for (int j = 0; j < 41; j++)
		{
			p1.x = (offsets[0].x * phd_mxptr[M00] + offsets[0].y * phd_mxptr[M01] + offsets[0].z * phd_mxptr[M02] + phd_mxptr[M03]) >> W2V_SHIFT;
			p1.y = (offsets[0].x * phd_mxptr[M10] + offsets[0].y * phd_mxptr[M11] + offsets[0].z * phd_mxptr[M12] + phd_mxptr[M13]) >> W2V_SHIFT;
			p1.z = (offsets[0].x * phd_mxptr[M20] + offsets[0].y * phd_mxptr[M21] + offsets[0].z * phd_mxptr[M22] + phd_mxptr[M23]) >> W2V_SHIFT;

			p2.x = (offsets[1].x * phd_mxptr[M00] + offsets[1].y * phd_mxptr[M01] + offsets[1].z * phd_mxptr[M02] + phd_mxptr[M03]) >> W2V_SHIFT;
			p2.y = (offsets[1].x * phd_mxptr[M10] + offsets[1].y * phd_mxptr[M11] + offsets[1].z * phd_mxptr[M12] + phd_mxptr[M13]) >> W2V_SHIFT;
			p2.z = (offsets[1].x * phd_mxptr[M20] + offsets[1].y * phd_mxptr[M21] + offsets[1].z * phd_mxptr[M22] + phd_mxptr[M23]) >> W2V_SHIFT;

			p3.x = (offsets[2].x * phd_mxptr[M00] + offsets[2].y * phd_mxptr[M01] + offsets[2].z * phd_mxptr[M02] + phd_mxptr[M03]) >> W2V_SHIFT;
			p3.y = (offsets[2].x * phd_mxptr[M10] + offsets[2].y * phd_mxptr[M11] + offsets[2].z * phd_mxptr[M12] + phd_mxptr[M13]) >> W2V_SHIFT;
			p3.z = (offsets[2].x * phd_mxptr[M20] + offsets[2].y * phd_mxptr[M21] + offsets[2].z * phd_mxptr[M22] + phd_mxptr[M23]) >> W2V_SHIFT;

			offsets[0].x += 512;
			offsets[1].x += 512;
			offsets[2].x += 512;
			XY += 6;
			Z += 3;

			XY[0] = (short)p1.x;
			XY[1] = (short)p1.y;
			Z[0] = p1.z;

			XY[2] = (short)p2.x;
			XY[3] = (short)p2.y;
			Z[1] = p2.z;

			XY[4] = (short)p3.x;
			XY[5] = (short)p3.y;
			Z[2] = p3.z;
		}

		offsets[0].x -= 512;
		offsets[1].x -= 512;
		offsets[2].x -= 512;
		XY = (short*)&scratchpad[0];
		Z = (long*)&scratchpad[492];

		for (int j = num; j < num + 20; j++, XY += 12, Z += 6)
		{
			z1 = Z[0];
			z2 = Z[2];
			z3 = Z[6];
			z4 = Z[8];

			if (!(z1 | z2 | z3 | z4))
				continue;

			x1 = XY[0];
			y1 = XY[1];
			x2 = XY[4];
			y2 = XY[5];
			x3 = XY[12];
			y3 = XY[13];
			x4 = XY[16];
			y4 = XY[17];

			if (j < 7)
				spec = (j + 1) * 0x101010;
			else if (j >= 33)
				spec = (40 - j) * 0x101010;
			else
				spec = 0x808080;

			setXYZ4(v, x1, y1, z1, x2, y2, z2, x4, y4, z4, x3, y3, z3, clipflags);
			spec = ((spec & 0xFF) - 1) << 25;
			v[0].color = 0xFFFFFFFF;
			v[1].color = 0xFFFFFFFF;
			v[2].color = 0xFFFFFFFF;
			v[3].color = 0xFFFFFFFF;
			v[0].specular = spec;
			v[1].specular = spec;
			v[2].specular = spec;
			v[3].specular = spec;
			AddQuadSorted(v, 0, 1, 2, 3, tex, 0);
		}

		num += 20;
	}
}

void S_DrawSplashes()	//	(also draws ripples and underwater blood (which is a ripple in disguise))
{
	SPLASH_STRUCT* splash;
	RIPPLE_STRUCT* ripple;
	SPRITESTRUCT* sprite;
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	long* Z;
	short* XY;
	short* offsets;
	uchar* links;
	ulong c0, c1;
	long x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4, linkNum, r, g, b;
	short rads[6];
	short yVals[6];

	offsets = (short*)&scratchpad[768];

	for (int i = 0; i < 4; i++)
	{
		splash = &splashes[i];

		if (!(splash->flags & 1))
			continue;

		phd_PushMatrix();
		phd_TranslateAbs(splash->x, splash->y, splash->z);
		XY = (short*)&scratchpad[0];
		Z = (long*)&scratchpad[256];

		rads[0] = splash->InnerRad;
		rads[1] = splash->InnerRad + splash->InnerSize;
		rads[2] = splash->MiddleRad;
		rads[3] = splash->MiddleRad + splash->MiddleSize;
		rads[4] = splash->OuterRad;
		rads[5] = splash->OuterRad + splash->OuterSize;

		yVals[0] = 0;
		yVals[1] = splash->InnerY;
		yVals[2] = 0;
		yVals[3] = splash->MiddleY;
		yVals[4] = 0;
		yVals[5] = 0;

		for (int j = 0; j < 6; j++)
		{
			for (int k = 0; k < 0x10000; k += 0x2000)
			{
				offsets[0] = (rads[j] * phd_sin(k)) >> (W2V_SHIFT - 1);
				offsets[1] = yVals[j] >> 3;
				offsets[2] = (rads[j] * phd_cos(k)) >> (W2V_SHIFT - 1);
				*XY++ = short((phd_mxptr[M00] * offsets[0] + phd_mxptr[M01] * offsets[1] + phd_mxptr[M02] * offsets[2] + phd_mxptr[M03]) >> W2V_SHIFT);
				*XY++ = short((phd_mxptr[M10] * offsets[0] + phd_mxptr[M11] * offsets[1] + phd_mxptr[M12] * offsets[2] + phd_mxptr[M13]) >> W2V_SHIFT);
				*Z++ = (phd_mxptr[M20] * offsets[0] + phd_mxptr[M21] * offsets[1] + phd_mxptr[M22] * offsets[2] + phd_mxptr[M23]) >> W2V_SHIFT;
				Z++;	//?
			}
		}

		phd_PopMatrix();
		XY = (short*)&scratchpad[0];
		Z = (long*)&scratchpad[256];

		for (int j = 0; j < 3; j++)
		{
			if (j == 2 || (!j && splash->flags & 4) || (j == 1 && splash->flags & 8))
				sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 4 + ((wibble >> 4) & 3)];
			else
				sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 8];

			links = SplashLinks;
			linkNum = j << 5;

			for (int k = 0; k < 8; k++)
			{
				x1 = XY[links[0] + linkNum];
				y1 = XY[links[0] + linkNum + 1];
				z1 = Z[links[0] + linkNum];
				links++;

				x2 = XY[links[0] + linkNum];
				y2 = XY[links[0] + linkNum + 1];
				z2 = Z[links[0] + linkNum];
				links++;

				x3 = XY[links[0] + linkNum];
				y3 = XY[links[0] + linkNum + 1];
				z3 = Z[links[0] + linkNum];
				links++;

				x4 = XY[links[0] + linkNum];
				y4 = XY[links[0] + linkNum + 1];
				z4 = Z[links[0] + linkNum];
				links++;

				setXYZ4(v, x1, y1, z1, x2, y2, z2, x4, y4, z4, x3, y3, z3, clipflags);

				r = splash->life << 1;
				g = splash->life << 1;
				b = splash->life << 1;
				
				if (r > 255)
					r = 255;

				if (g > 255)
					g = 255;

				if (b > 255)
					b = 255;

				c0 = RGBA(r, g, b, 0xFF);

				r = (splash->life - (splash->life >> 2)) << 1;
				g = (splash->life - (splash->life >> 2)) << 1;
				b = (splash->life - (splash->life >> 2)) << 1;

				if (r > 255)
					r = 255;

				if (g > 255)
					g = 255;

				if (b > 255)
					b = 255;

				c1 = RGBA(r, g, b, 0xFF);

				v[0].color = c0;
				v[1].color = c0;
				v[2].color = c1;
				v[3].color = c1;
				v[0].specular = 0xFF000000;
				v[1].specular = 0xFF000000;
				v[2].specular = 0xFF000000;
				v[3].specular = 0xFF000000;
				tex.drawtype = 2;
				tex.flag = 0;
				tex.tpage = sprite->tpage;
				tex.u1 = sprite->x1;
				tex.v1 = sprite->y1;
				tex.u2 = sprite->x2;
				tex.v2 = sprite->y1;
				tex.v3 = sprite->y2;
				tex.u3 = sprite->x2;
				tex.u4 = sprite->x1;
				tex.v4 = sprite->y2;
				AddQuadSorted(v, 0, 1, 2, 3, &tex, 1);
			}
		}
	}

	for (int i = 0; i < 16; i++)
	{
		ripple = &ripples[i];

		if (!(ripple->flags & 1))
			continue;

		phd_PushMatrix();
		phd_TranslateAbs(ripple->x, ripple->y, ripple->z);

		XY = (short*)&scratchpad[0];
		Z = (long*)&scratchpad[256];

		offsets[0] = -ripple->size;
		offsets[1] = 0;
		offsets[2] = -ripple->size;
		*XY++ = short((phd_mxptr[M00] * offsets[0] + phd_mxptr[M01] * offsets[1] + phd_mxptr[M02] * offsets[2] + phd_mxptr[M03]) >> W2V_SHIFT);
		*XY++ = short((phd_mxptr[M10] * offsets[0] + phd_mxptr[M11] * offsets[1] + phd_mxptr[M12] * offsets[2] + phd_mxptr[M13]) >> W2V_SHIFT);
		*Z++ = (phd_mxptr[M20] * offsets[0] + phd_mxptr[M21] * offsets[1] + phd_mxptr[M22] * offsets[2] + phd_mxptr[M23]) >> W2V_SHIFT;
		Z++;

		offsets[0] = -ripple->size;
		offsets[1] = 0;
		offsets[2] = ripple->size;
		*XY++ = short((phd_mxptr[M00] * offsets[0] + phd_mxptr[M01] * offsets[1] + phd_mxptr[M02] * offsets[2] + phd_mxptr[M03]) >> W2V_SHIFT);
		*XY++ = short((phd_mxptr[M10] * offsets[0] + phd_mxptr[M11] * offsets[1] + phd_mxptr[M12] * offsets[2] + phd_mxptr[M13]) >> W2V_SHIFT);
		*Z++ = (phd_mxptr[M20] * offsets[0] + phd_mxptr[M21] * offsets[1] + phd_mxptr[M22] * offsets[2] + phd_mxptr[M23]) >> W2V_SHIFT;
		Z++;

		offsets[0] = ripple->size;
		offsets[1] = 0;
		offsets[2] = ripple->size;
		*XY++ = short((phd_mxptr[M00] * offsets[0] + phd_mxptr[M01] * offsets[1] + phd_mxptr[M02] * offsets[2] + phd_mxptr[M03]) >> W2V_SHIFT);
		*XY++ = short((phd_mxptr[M10] * offsets[0] + phd_mxptr[M11] * offsets[1] + phd_mxptr[M12] * offsets[2] + phd_mxptr[M13]) >> W2V_SHIFT);
		*Z++ = (phd_mxptr[M20] * offsets[0] + phd_mxptr[M21] * offsets[1] + phd_mxptr[M22] * offsets[2] + phd_mxptr[M23]) >> W2V_SHIFT;
		Z++;

		offsets[0] = ripple->size;
		offsets[1] = 0;
		offsets[2] = -ripple->size;
		*XY++ = short((phd_mxptr[M00] * offsets[0] + phd_mxptr[M01] * offsets[1] + phd_mxptr[M02] * offsets[2] + phd_mxptr[M03]) >> W2V_SHIFT);
		*XY++ = short((phd_mxptr[M10] * offsets[0] + phd_mxptr[M11] * offsets[1] + phd_mxptr[M12] * offsets[2] + phd_mxptr[M13]) >> W2V_SHIFT);
		*Z++ = (phd_mxptr[M20] * offsets[0] + phd_mxptr[M21] * offsets[1] + phd_mxptr[M22] * offsets[2] + phd_mxptr[M23]) >> W2V_SHIFT;
		Z++;

		phd_PopMatrix();

		XY = (short*)&scratchpad[0];
		Z = (long*)&scratchpad[256];

		if (ripple->flags & 0x20)
			sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index];
		else
			sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 9];

		x1 = *XY++;
		y1 = *XY++;
		z1 = *Z++;
		Z++;

		x2 = *XY++;
		y2 = *XY++;
		z2 = *Z++;
		Z++;

		x3 = *XY++;
		y3 = *XY++;
		z3 = *Z++;
		Z++;

		x4 = *XY++;
		y4 = *XY++;
		z4 = *Z++;
		Z++;

		setXYZ4(v, x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4, clipflags);

		if (ripple->flags & 0x10)
		{
			if (ripple->flags & 0x20)
			{
				if (ripple->init)
				{
					r = (ripple->init >> 1) << 1;
					g = 0;
					b = (ripple->init >> 4) << 1;
				}
				else
				{
					r = (ripple->life >> 1) << 1;
					g = 0;
					b = (ripple->life >> 4) << 1;
				}
			}
			else
			{
				if (ripple->init)
				{
					r = ripple->init << 1;
					g = ripple->init << 1;
					b = ripple->init << 1;
				}
				else
				{
					r = ripple->life << 1;
					g = ripple->life << 1;
					b = ripple->life << 1;
				}
			}
		}
		else
		{
			if (ripple->init)
			{
				r = ripple->init << 2;
				g = ripple->init << 2;
				b = ripple->init << 2;
			}
			else
			{
				r = ripple->life << 2;
				g = ripple->life << 2;
				b = ripple->life << 2;
			}
		}

		if (r > 255)
			r = 255;

		if (g > 255)
			g = 255;

		if (b > 255)
			b = 255;

		c0 = RGBA(r, g, b, 0xFF);

		v[0].color = c0;
		v[1].color = c0;
		v[2].color = c0;
		v[3].color = c0;
		v[0].specular = 0xFF000000;
		v[1].specular = 0xFF000000;
		v[2].specular = 0xFF000000;
		v[3].specular = 0xFF000000;
		tex.drawtype = 2;
		tex.flag = 0;
		tex.tpage = sprite->tpage;
		tex.u1 = sprite->x1;
		tex.v1 = sprite->y1;
		tex.u2 = sprite->x2;
		tex.v2 = sprite->y1;
		tex.v3 = sprite->y2;
		tex.u3 = sprite->x2;
		tex.u4 = sprite->x1;
		tex.v4 = sprite->y2;
		AddQuadSorted(v, 0, 1, 2, 3, &tex, 1);
	}
}

bool ClipLine(long& x1, long& y1, long z1, long& x2, long& y2, long z2, long xMin, long yMin, long w, long h)
{
	float clip;

	if (z1 < 20 || z2 < 20)
		return 0;

	if (x1 < xMin && x2 < xMin || y1 < yMin && y2 < yMin)
		return 0;

	if (x1 > w && x2 > w || y1 > h && y2 > h)
		return 0;

	if (x1 > w)
	{
		clip = ((float)w - x2) / float(x1 - x2);
		x1 = w;
		y1 = long((y1 - y2) * clip + y2);
	}

	if (x2 > w)
	{
		clip = ((float)w - x1) / float(x2 - x1);
		x2 = w;
		y2 = long((y2 - y1) * clip + y1);
	}

	if (x1 < xMin)
	{
		clip = ((float)xMin - x1) / float(x2 - x1);
		x1 = xMin;
		y1 = long((y2 - y1) * clip + y1);
	}

	if (x2 < xMin)
	{
		clip = ((float)xMin - x2) / float(x1 - x2);
		x2 = xMin;
		y2 = long((y1 - y2) * clip + y2);
	}

	if (y1 > h)
	{
		clip = ((float)h - y2) / float(y1 - y2);
		y1 = h;
		x1 = long((x1 - x2) * clip + x2);
	}

	if (y2 > h)
	{
		clip = ((float)h - y1) / float(y2 - y1);
		y2 = h;
		x2 = long((x2 - x1) * clip + x1);
	}

	if (y1 < yMin)
	{
		clip = ((float)yMin - y1) / float(y2 - y1);
		y1 = yMin;
		x1 = long((x2 - x1) * clip + x1);
	}

	if (y2 < yMin)
	{
		clip = ((float)yMin - y2) / float(y1 - y2);
		y2 = yMin;
		x2 = long((x1 - x2) * clip + x2);
	}

	return 1;
}

void S_DrawFireSparks(long size, long life)
{
	FIRE_SPARKS* sptr;
	SPRITESTRUCT* sprite;
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	PHD_VECTOR pos;
	long* Z;
	short* XY;
	short* offsets;
	float perspz;
	ulong r, g, b, col;
	long newSize, s, c, sx1, cx1, sx2, cx2;
	long dx, dy, dz, x1, y1, x2, y2, x3, y3, x4, y4;
	short ang;

	XY = (short*)&scratchpad[0];
	Z = (long*)&scratchpad[256];
	offsets = (short*)&scratchpad[512];

	for (int i = 0; i < 20; i++)
	{
		sptr = &fire_spark[i];

		if (!sptr->On)
			continue;

		dx = sptr->x >> (2 - size);
		dy = sptr->y >> (2 - size);
		dz = sptr->z >> (2 - size);

		if (dx < -0x5000 || dx > 0x5000 || dy < -0x5000 || dy > 0x5000 || dz < -0x5000 || dz > 0x5000)
			continue;

		offsets[0] = (short)dx;
		offsets[1] = (short)dy;
		offsets[2] = (short)dz;
		pos.x = offsets[0] * phd_mxptr[M00] + offsets[1] * phd_mxptr[M01] + offsets[2] * phd_mxptr[M02] + phd_mxptr[M03];
		pos.y = offsets[0] * phd_mxptr[M10] + offsets[1] * phd_mxptr[M11] + offsets[2] * phd_mxptr[M12] + phd_mxptr[M13];
		pos.z = offsets[0] * phd_mxptr[M20] + offsets[1] * phd_mxptr[M21] + offsets[2] * phd_mxptr[M22] + phd_mxptr[M23];
		perspz = f_persp / (float)pos.z;
		XY[0] = short(float(pos.x * perspz + f_centerx));
		XY[1] = short(float(pos.y * perspz + f_centery));
		Z[0] = pos.z >> W2V_SHIFT;


		if (Z[0] <= 0 || Z[0] >= 0x5000)
			continue;

		newSize = (((phd_persp * sptr->Size) << 2) / Z[0]) >> (2 - size);

		if (newSize > (sptr->Size << 2))
			newSize = (sptr->Size << 2);
		else if (newSize < 4)
			newSize = 4;

		newSize >>= 1;

		if (XY[0] + newSize < phd_winxmin || XY[0] - newSize >= phd_winxmax || XY[1] + newSize < phd_winymin || XY[1] - newSize >= phd_winymax)
			continue;

		if (sptr->Flags & 0x10)
		{
			ang = sptr->RotAng << 1;
			s = rcossin_tbl[ang];
			c = rcossin_tbl[ang + 1];
			sx1 = (-newSize * s) >> (W2V_SHIFT - 2);
			cx1 = (-newSize * c) >> (W2V_SHIFT - 2);
			sx2 = (newSize * s) >> (W2V_SHIFT - 2);
			cx2 = (newSize * c) >> (W2V_SHIFT - 2);
			x1 = XY[0] + (sx1 - cx1);
			y1 = XY[1] + sx1 + cx1;
			x2 = XY[0] + (sx2 - cx1);
			y2 = XY[1] + sx1 + cx2;
			x3 = XY[0] + (sx2 - cx2);
			y3 = XY[1] + sx2 + cx2;
			x4 = XY[0] + (sx1 - cx2);
			y4 = XY[1] + sx2 + cx1;
			setXY4(v, x1, y1, x2, y2, x3, y3, x4, y4, Z[0], clipflags);
		}
		else
		{
			x1 = XY[0] - newSize;
			x2 = XY[0] + newSize;
			y1 = XY[1] - newSize;
			y2 = XY[1] + newSize;
			setXY4(v, x1, y1, x2, y1, x2, y2, x1, y2, Z[0], clipflags);
		}

		sprite = &spriteinfo[sptr->Def];

		if (Z[0] <= 0x3000)
		{
			r = sptr->R;
			g = sptr->G;
			b = sptr->B;
		}
		else
		{
			r = ((0x5000 - Z[0]) * sptr->R) >> 13;
			g = ((0x5000 - Z[0]) * sptr->G) >> 13;
			b = ((0x5000 - Z[0]) * sptr->B) >> 13;
		}

		r = (r * life) >> 8;
		g = (g * life) >> 8;
		b = (b * life) >> 8;
		col = RGBA(r, g, b, 0xFF);

		v[0].color = col;
		v[1].color = col;
		v[2].color = col;
		v[3].color = col;
		v[0].specular = 0xFF000000;
		v[1].specular = 0xFF000000;
		v[2].specular = 0xFF000000;
		v[3].specular = 0xFF000000;
		tex.drawtype = 2;
		tex.flag = 0;
		tex.tpage = sprite->tpage;
		tex.u1 = sprite->x1;
		tex.v1 = sprite->y1;
		tex.u2 = sprite->x2;
		tex.v2 = sprite->y1;
		tex.u3 = sprite->x2;
		tex.v3 = sprite->y2;
		tex.u4 = sprite->x1;
		tex.v4 = sprite->y2;
		AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
		AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
	}
}

void DrawRope(ROPE_STRUCT* rope)
{
	SPRITESTRUCT* sprite;
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	long dx, dy, d, b, w, spec;
	long x1, y1, z1, x2, y2, z2, x3, y3, x4, y4;

	ProjectRopePoints(rope);
	dx = rope->Coords[1][0] - rope->Coords[0][0];
	dy = rope->Coords[1][1] - rope->Coords[0][1];
	d = SQUARE(dx) + SQUARE(dy);
	d = phd_sqrt(abs(d));

	dx <<= W2V_SHIFT + 2;
	dy <<= W2V_SHIFT + 2;
	d <<= W2V_SHIFT + 2;

	if (d)
	{
		d = ((0x1000000 / (d >> 8)) << 8) >> 8;
		b = dx;
		dx = ((__int64)-dy * (__int64)d) >> (W2V_SHIFT + 2);
		dy = ((__int64)b * (__int64)d) >> (W2V_SHIFT + 2);
	}

	w = 0x60000;

	if (rope->Coords[0][2])
	{
		w = 0x60000 * phd_persp / rope->Coords[0][2];

		if (w < 1)
			w = 1;
	}

	w <<= (W2V_SHIFT + 2);
	dx = (((__int64)dx * (__int64)w) >> (W2V_SHIFT + 2)) >> (W2V_SHIFT + 2);
	dy = (((__int64)dy * (__int64)w) >> (W2V_SHIFT + 2)) >> (W2V_SHIFT + 2);
	x1 = rope->Coords[0][0] - dx;
	y1 = rope->Coords[0][1] - dy;
	z1 = rope->Coords[0][2] >> W2V_SHIFT;
	x4 = rope->Coords[0][0] + dx;
	y4 = rope->Coords[0][1] + dy;

	for (int i = 0; i < 23; i++)
	{
		dx = rope->Coords[i + 1][0] - rope->Coords[i][0];
		dy = rope->Coords[i + 1][1] - rope->Coords[i][1];
		d = SQUARE(dx) + SQUARE(dy);
		d = phd_sqrt(abs(d));

		dx <<= W2V_SHIFT + 2;
		dy <<= W2V_SHIFT + 2;
		d <<= W2V_SHIFT + 2;

		if (d)
		{
			d = ((0x1000000 / (d >> 8)) << 8) >> 8;
			b = dx;
			dx = ((__int64)-dy * (__int64)d) >> (W2V_SHIFT + 2);
			dy = ((__int64)b * (__int64)d) >> (W2V_SHIFT + 2);
		}

		w = 0x60000;

		if (rope->Coords[i][2])
		{
			w = 0x60000 * phd_persp / rope->Coords[i][2];

			if (w < 3)
				w = 3;
		}

		w <<= (W2V_SHIFT + 2);
		dx = (((__int64)dx * (__int64)w) >> (W2V_SHIFT + 2)) >> (W2V_SHIFT + 2);
		dy = (((__int64)dy * (__int64)w) >> (W2V_SHIFT + 2)) >> (W2V_SHIFT + 2);
		x2 = rope->Coords[i + 1][0] - dx;
		y2 = rope->Coords[i + 1][1] - dy;
		z2 = rope->Coords[i + 1][2] >> W2V_SHIFT;
		x3 = rope->Coords[i + 1][0] + dx;
		y3 = rope->Coords[i + 1][1] + dy;

		if ((double)z1 > f_mznear && (double)z2 > f_mznear)
		{
			setXY4(v, x1, y1, x2, y2, x3, y3, x4, y4, z1, clipflags);
			v[0].color = 0xFF7F7F7F;
			v[1].color = 0xFF7F7F7F;
			v[2].color = 0xFF7F7F7F;
			v[3].color = 0xFF7F7F7F;

			spec = 255;

			if (z1 > 0x3000)
				spec = (255 * (0x5000 - z1)) >> 13;

			v[0].specular = spec << 24;
			v[1].specular = spec << 24;
			v[2].specular = spec << 24;
			v[3].specular = spec << 24;
			sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 16];
			tex.drawtype = 1;
			tex.flag = 0;
			tex.tpage = sprite->tpage;
			tex.u1 = sprite->x1;
			tex.v1 = sprite->y1;
			tex.u2 = sprite->x1;
			tex.v2 = sprite->y2;
			tex.u3 = sprite->x2;
			tex.v3 = sprite->y2;
			tex.u4 = sprite->x2;
			tex.v4 = sprite->y1;
			AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
		}

		x1 = x2;
		y1 = y2;
		z1 = z2;
		x4 = x3;
		y4 = y3;
	}
}

void DrawBlood()
{
	BLOOD_STRUCT* bptr;
	SPRITESTRUCT* sprite;
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	PHD_VECTOR pos;
	long* Z;
	short* XY;
	short* offsets;
	float perspz;
	ulong r, col;
	long size, s, c;
	long dx, dy, dz, x1, y1, x2, y2, x3, y3, x4, y4;
	short ang;

	phd_PushMatrix();
	phd_TranslateAbs(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);
	sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 15];
	XY = (short*)&scratchpad[0];
	Z = (long*)&scratchpad[256];
	offsets = (short*)&scratchpad[512];

	for (int i = 0; i < 32; i++)
	{
		bptr = &blood[i];

		if (!bptr->On)
			continue;

		dx = bptr->x - lara_item->pos.x_pos;
		dy = bptr->y - lara_item->pos.y_pos;
		dz = bptr->z - lara_item->pos.z_pos;

		if (dx < -0x5000 || dx > 0x5000 || dy < -0x5000 || dy > 0x5000 || dz < -0x5000 || dz > 0x5000)
			continue;

		offsets[0] = (short)dx;
		offsets[1] = (short)dy;
		offsets[2] = (short)dz;
		pos.x = offsets[0] * phd_mxptr[M00] + offsets[1] * phd_mxptr[M01] + offsets[2] * phd_mxptr[M02] + phd_mxptr[M03];
		pos.y = offsets[0] * phd_mxptr[M10] + offsets[1] * phd_mxptr[M11] + offsets[2] * phd_mxptr[M12] + phd_mxptr[M13];
		pos.z = offsets[0] * phd_mxptr[M20] + offsets[1] * phd_mxptr[M21] + offsets[2] * phd_mxptr[M22] + phd_mxptr[M23];
		perspz = f_persp / (float)pos.z;
		XY[0] = short(float(pos.x * perspz + f_centerx));
		XY[1] = short(float(pos.y * perspz + f_centery));
		Z[0] = pos.z >> W2V_SHIFT;

		if (Z[0] <= 0 || Z[0] >= 0x5000)
			continue;

		size = ((phd_persp * bptr->Size) << 1) / Z[0];

		if (size > (bptr->Size << 1))
			size = (bptr->Size << 1);
		else if (size < 4)
			size = 4;

		size <<= 1;
		ang = bptr->RotAng << 1;
		s = (size * rcossin_tbl[ang]) >> (W2V_SHIFT - 2);
		c = (size * rcossin_tbl[ang + 1]) >> (W2V_SHIFT - 2);
		x1 = c + XY[0] - s;
		y1 = XY[1] - c - s;
		x2 = s + c + XY[0];
		y2 = c + XY[1] - s;
		x3 = s + XY[0] - c;
		y3 = s + XY[1] + c;
		x4 = XY[0] - c - s;
		y4 = XY[1] - c + s;
		setXY4(v, x1, y1, x2, y2, x3, y3, x4, y4, Z[0], clipflags);

		if (Z[0] <= 0x3000)
			col = RGBA(bptr->Shade, 0, 0, 0xFF);
		else
		{
			r = ((0x5000 - Z[0]) * bptr->Shade) >> 13;
			col = RGBA(r, 0, 0, 0xFF);
		}

		v[0].color = col;
		v[1].color = col;
		v[2].color = col;
		v[3].color = col;
		v[0].specular = 0xFF000000;
		v[1].specular = 0xFF000000;
		v[2].specular = 0xFF000000;
		v[3].specular = 0xFF000000;
		tex.drawtype = 2;
		tex.flag = 0;
		tex.tpage = sprite->tpage;
		tex.u1 = sprite->x1;
		tex.v1 = sprite->y1;
		tex.u2 = sprite->x2;
		tex.v2 = sprite->y1;
		tex.u3 = sprite->x2;
		tex.v3 = sprite->y2;
		tex.u4 = sprite->x1;
		tex.v4 = sprite->y2;
		AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
	}

	phd_PopMatrix();
}

void S_DrawSmokeSparks()
{
	SMOKE_SPARKS* sptr;
	SPRITESTRUCT* sprite;
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	PHD_VECTOR pos;
	long* Z;
	short* XY;
	short* offsets;
	float perspz;
	long is_mirror, size, col, s, c, ss, cs, sm, cm;
	long dx, dy, dz, x1, y1, x2, y2, x3, y3, x4, y4;
	short ang;

	phd_PushMatrix();
	phd_TranslateAbs(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);
	XY = (short*)&scratchpad[0];
	Z = (long*)&scratchpad[256];
	offsets = (short*)&scratchpad[512];
	is_mirror = 0;
	sptr = &smoke_spark[0];

	for (int i = 0; i < 32; i++)
	{
		if (!sptr->On)
		{
			sptr++;
			continue;
		}

		if (sptr->mirror && !is_mirror)
			is_mirror = 1;
		else
			is_mirror = 0;

		dx = sptr->x - lara_item->pos.x_pos;
		dy = sptr->y - lara_item->pos.y_pos;
		dz = sptr->z - lara_item->pos.z_pos;

		if (is_mirror)
			dz = 2 * gfMirrorZPlane - lara_item->pos.z_pos - sptr->z;

		if (dx < -0x5000 || dx > 0x5000 || dy < -0x5000 || dy > 0x5000 || dz < -0x5000 || dz > 0x5000)
		{
			if (!is_mirror)
				sptr++;

			continue;
		}

		offsets[0] = (short)dx;
		offsets[1] = (short)dy;
		offsets[2] = (short)dz;
		pos.x = offsets[0] * phd_mxptr[M00] + offsets[1] * phd_mxptr[M01] + offsets[2] * phd_mxptr[M02] + phd_mxptr[M03];
		pos.y = offsets[0] * phd_mxptr[M10] + offsets[1] * phd_mxptr[M11] + offsets[2] * phd_mxptr[M12] + phd_mxptr[M13];
		pos.z = offsets[0] * phd_mxptr[M20] + offsets[1] * phd_mxptr[M21] + offsets[2] * phd_mxptr[M22] + phd_mxptr[M23];
		perspz = f_persp / (float)pos.z;
		XY[0] = short(float(pos.x * perspz + f_centerx));
		XY[1] = short(float(pos.y * perspz + f_centery));
		Z[0] = pos.z >> W2V_SHIFT;

		if (Z[0] <= 0 || Z[0] >= 0x5000)
		{
			if (!is_mirror)
				sptr++;

			continue;
		}

		size = ((phd_persp * sptr->Size) << 2) / Z[0];

		if (size > (sptr->Size << 2))
			size = (sptr->Size << 2);
		else if (size < 4)
			size = 4;

		size >>= 1;

		if (XY[0] + size < phd_winxmin || XY[0] - size >= phd_winxmax || XY[1] + size < phd_winymin || XY[1] - size >= phd_winymax)
		{
			if (!is_mirror)
				sptr++;

			continue;
		}

		if (sptr->Flags & 0x10)
		{
			ang = sptr->RotAng << 1;
			s = rcossin_tbl[ang];
			c = rcossin_tbl[ang + 1];
			ss = (s * size) >> (W2V_SHIFT - 2);
			cs = (c * size) >> (W2V_SHIFT - 2);
			sm = (s * -size) >> (W2V_SHIFT - 2);
			cm = (c * -size) >> (W2V_SHIFT - 2);

			x1 = sm + XY[0] - cm;
			y1 = sm + XY[1] + cm;
			x2 = XY[0] - cm + ss;
			y2 = sm + XY[1] + cs;
			x3 = ss + XY[0] - cs;
			y3 = cs + XY[1] + ss;
			x4 = sm + XY[0] - cs;
			y4 = ss + XY[1] + cm;

			setXY4(v, x1, y1, x2, y2, x3, y3, x4, y4, Z[0], clipflags);
		}
		else
		{
			x1 = XY[0] - size;
			y1 = XY[1] - size;
			x2 = XY[0] + size;
			y2 = XY[1] + size;
			setXY4(v, x1, y1, x2, y1, x2, y2, x1, y2, Z[0], clipflags);
		}

		sprite = &spriteinfo[sptr->Def];

		if (Z[0] <= 0x3000)
			col = sptr->Shade;
		else
			col = ((0x5000 - Z[0]) * sptr->Shade) >> 13;

		v[0].color = RGBA(col, col, col, 0xFF);
		v[1].color = RGBA(col, col, col, 0xFF);
		v[2].color = RGBA(col, col, col, 0xFF);
		v[3].color = RGBA(col, col, col, 0xFF);
		v[0].specular = 0xFF000000;
		v[1].specular = 0xFF000000;
		v[2].specular = 0xFF000000;
		v[3].specular = 0xFF000000;
		tex.drawtype = 2;
		tex.flag = 0;
		tex.tpage = sprite->tpage;
		tex.u1 = sprite->x1;
		tex.v1 = sprite->y1;
		tex.u2 = sprite->x2;
		tex.v3 = sprite->y2;
		tex.v2 = sprite->y1;
		tex.u3 = sprite->x2;
		tex.u4 = sprite->x1;
		tex.v4 = sprite->y2;
		AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);

		if (!is_mirror)
			sptr++;
	}

	phd_PopMatrix();
}

void DoUwEffect()
{
	WATER_DUST* p;
	SPRITESTRUCT* sprite;
	D3DTLVERTEX v[3];
	TEXTURESTRUCT tex;
	PHD_VECTOR pos;
	long* Z;
	short* XY;
	short* offsets;
	float perspz;
	long num_alive, rad, ang, x, y, z, size, col, yv;

	num_alive = 0;

	for (int i = 0; i < 256; i++)
	{
		p = &uwdust[i];

		if (!p->pos.x && num_alive < 16)
		{
			num_alive++;
			rad = GetRandomDraw() & 0xFFF;
			ang = GetRandomDraw() & 0x1FFE;
			x = (rad * rcossin_tbl[ang]) >> (W2V_SHIFT - 2);
			y = (GetRandomDraw() & 0x7FF) - 1024;
			z = (rad * rcossin_tbl[ang + 1]) >> (W2V_SHIFT - 2);
			p->pos.x = lara_item->pos.x_pos + x;
			p->pos.y = lara_item->pos.y_pos + y;
			p->pos.z = lara_item->pos.z_pos + z;

			if (IsRoomOutside(p->pos.x, p->pos.y, p->pos.z) < 0 || !(room[IsRoomOutsideNo].flags & ROOM_UNDERWATER))
			{
				p->pos.x = 0;
				continue;
			}

			p->life = (GetRandomDraw() & 7) + 16;
			p->xvel = GetRandomDraw() & 3;

			if (p->xvel == 2)
				p->xvel = -1;

			p->yvel = ((GetRandomDraw() & 7) + 8) << 3;
			p->zvel = GetRandomDraw() & 3;

			if (p->zvel == 2)
				p->zvel = -1;
		}

		p->pos.x += p->xvel;
		p->pos.y += (p->yvel & ~7) >> 6;
		p->pos.z += p->zvel;

		if (!p->life)
		{
			p->pos.x = 0;
			continue;
		}

		p->life--;

		if ((p->yvel & 7) < 7)
			p->yvel++;
	}

	sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 10];
	XY = (short*)&scratchpad[0];
	Z = (long*)&scratchpad[256];
	offsets = (short*)&scratchpad[512];
	phd_PushMatrix();
	phd_TranslateAbs(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);

	for (int i = 0; i < 256; i++)
	{
		p = &uwdust[i];

		if (!p->pos.x)
			continue;

		x = p->pos.x - lara_item->pos.x_pos;
		y = p->pos.y - lara_item->pos.y_pos;
		z = p->pos.z - lara_item->pos.z_pos;
		offsets[0] = (short)x;
		offsets[1] = (short)y;
		offsets[2] = (short)z;
		pos.x = offsets[0] * phd_mxptr[M00] + offsets[1] * phd_mxptr[M01] + offsets[2] * phd_mxptr[M02] + phd_mxptr[M03];
		pos.y = offsets[0] * phd_mxptr[M10] + offsets[1] * phd_mxptr[M11] + offsets[2] * phd_mxptr[M12] + phd_mxptr[M13];
		pos.z = offsets[0] * phd_mxptr[M20] + offsets[1] * phd_mxptr[M21] + offsets[2] * phd_mxptr[M22] + phd_mxptr[M23];
		perspz = f_persp / (float)pos.z;
		XY[0] = short(float(pos.x * perspz + f_centerx));
		XY[1] = short(float(pos.y * perspz + f_centery));
		Z[0] = pos.z >> W2V_SHIFT;

		if (Z[0] < 32)
		{
			if (p->life > 16)
				p->life = 16;

			continue;
		}

		if (XY[0] < phd_winxmin || XY[0] > phd_winxmax || XY[1] < phd_winymin || XY[1] > phd_winymax)
			continue;

		size = (phd_persp * (p->yvel >> 3)) / (Z[0] >> 4);

		if (size < 1)
			size = 6;
		else if (size > 12)
			size = 12;

		size = (0x5556 * size) >> 16;

		if (phd_winwidth > 512)
			size = long(float(phd_winwidth / 512.0F) * (float)size);

		if ((p->yvel & 7) == 7)
		{
			if (p->life > 18)
				col = 0xFF404040;
			else
				col = (p->life | ((p->life | ((p->life | 0xFFFFFFC0) << 8)) << 8)) << 2;	//decipher me
		}
		else
		{
			yv = (p->yvel & 7) << 2;
			col = (yv | ((yv | ((yv | 0xFFFFFF80) << 8)) << 8)) << 1;	//decipher me
		}

		setXY3(v, XY[0] + size, XY[1] - (size << 1), XY[0] + size, XY[1] + size, XY[0] - (size << 1), XY[1] + size, Z[0], clipflags);
		v[0].color = col;
		v[1].color = col;
		v[2].color = col;
		v[0].specular = 0xFF000000;
		v[1].specular = 0xFF000000;
		v[2].specular = 0xFF000000;
		tex.drawtype = 2;
		tex.flag = 0;
		tex.tpage = sprite->tpage;
		tex.u1 = sprite->x2;
		tex.v1 = sprite->y1;
		tex.u2 = sprite->x2;
		tex.v2 = sprite->y2;
		tex.u3 = sprite->x1;
		tex.v3 = sprite->y2;
		AddTriSorted(v, 0, 1, 2, &tex, 0);
	}

	phd_PopMatrix();
}

void DrawLightning()
{
	LIGHTNING_STRUCT* pL;
	SPRITESTRUCT* sprite;
	PHD_VECTOR* vec;
	SVECTOR* offsets;
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	PHD_VECTOR p1, p2, p3;
	long* Z;
	short* XY;
	float perspz;
	long c, xsize, ysize, r, g, b;
	long x1, y1, z1, x2, y2, z2, z;

	phd_PushMatrix();
	phd_TranslateAbs(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);
	sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 28];

	for (int i = 0; i < 16; i++)
	{
		pL = &Lightning[i];

		if (!pL->Life)
			continue;

		vec = (PHD_VECTOR*)&scratchpad[128];
		memcpy(&vec[0], &pL->Point[0], sizeof(PHD_VECTOR));
		memcpy(&vec[1], &pL->Point[0], 4 * sizeof(PHD_VECTOR));
		memcpy(&vec[5], &pL->Point[3], sizeof(PHD_VECTOR));

		for (int j = 0; j < 6; j++)
		{
			vec[j].x -= lara_item->pos.x_pos;
			vec[j].y -= lara_item->pos.y_pos;
			vec[j].z -= lara_item->pos.z_pos;
		}

		offsets = (SVECTOR*)&scratchpad[0];
		XY = (short*)&scratchpad[256];
		Z = (long*)&scratchpad[512];
		CalcLightningSpline(vec, offsets, pL);

		if (vec[0].x > 0x6000 || vec[0].y > 0x6000 || vec[0].z > 0x6000)
			continue;

		for (int j = 0; j < pL->Segments; j++)
		{
			p1.x = (offsets[0].x * phd_mxptr[M00] + offsets[0].y * phd_mxptr[M01] + offsets[0].z * phd_mxptr[M02] + phd_mxptr[M03]) >> W2V_SHIFT;
			p1.y = (offsets[0].x * phd_mxptr[M10] + offsets[0].y * phd_mxptr[M11] + offsets[0].z * phd_mxptr[M12] + phd_mxptr[M13]) >> W2V_SHIFT;
			p1.z = (offsets[0].x * phd_mxptr[M20] + offsets[0].y * phd_mxptr[M21] + offsets[0].z * phd_mxptr[M22] + phd_mxptr[M23]) >> W2V_SHIFT;

			p2.x = (offsets[1].x * phd_mxptr[M00] + offsets[1].y * phd_mxptr[M01] + offsets[1].z * phd_mxptr[M02] + phd_mxptr[M03]) >> W2V_SHIFT;
			p2.y = (offsets[1].x * phd_mxptr[M10] + offsets[1].y * phd_mxptr[M11] + offsets[1].z * phd_mxptr[M12] + phd_mxptr[M13]) >> W2V_SHIFT;
			p2.z = (offsets[1].x * phd_mxptr[M20] + offsets[1].y * phd_mxptr[M21] + offsets[1].z * phd_mxptr[M22] + phd_mxptr[M23]) >> W2V_SHIFT;

			p3.x = (offsets[2].x * phd_mxptr[M00] + offsets[2].y * phd_mxptr[M01] + offsets[2].z * phd_mxptr[M02] + phd_mxptr[M03]) >> W2V_SHIFT;
			p3.y = (offsets[2].x * phd_mxptr[M10] + offsets[2].y * phd_mxptr[M11] + offsets[2].z * phd_mxptr[M12] + phd_mxptr[M13]) >> W2V_SHIFT;
			p3.z = (offsets[2].x * phd_mxptr[M20] + offsets[2].y * phd_mxptr[M21] + offsets[2].z * phd_mxptr[M22] + phd_mxptr[M23]) >> W2V_SHIFT;

			XY[0] = (short)p1.x;
			XY[1] = (short)p1.y;
			Z[0] = p1.z;

			XY[2] = (short)p2.x;
			XY[3] = (short)p2.y;
			Z[1] = p2.z;

			XY[4] = (short)p3.x;
			XY[5] = (short)p3.y;
			Z[2] = p3.z;

			offsets += 3;
			XY += 6;
			Z += 3;
		}

		XY = (short*)&scratchpad[256];
		Z = (long*)&scratchpad[512];

		for (int j = 0; j < 3 * pL->Segments - 1; j++)
		{
			if (pL->Life < 16)
				c = pL->Life << 2;
			else
				c = 64;

			if (Z[0] > 0x3000)
				c = (c * (0x5000 - *Z)) >> 13;

			c = RGBA(c, c, c, 66);

			x1 = XY[0];
			y1 = XY[1];
			z1 = Z[0];
			x2 = XY[2];
			y2 = XY[3];
			z2 = Z[1];
			setXYZ4(v, x1, y1, z1, x2, y2, z2, x1, y1, z1, x2, y2, z2, clipflags);
			x1 = (long)v[0].sx;
			y1 = (long)v[0].sy;
			z1 = (long)v[0].sz;
			x2 = (long)v[1].sx;
			y2 = (long)v[1].sy;
			z2 = (long)v[1].sz;

			if (ClipLine(x1, y1, z1, x2, y2, z2, phd_winxmin, phd_winymin, phd_winxmax, phd_winymax))
			{
				perspz = f_mpersp / Z[0] * f_moneopersp;

				v[0].sx = (float)x1;
				v[0].sy = (float)y1;
				v[0].sz = f_a - perspz * f_boo;
				v[0].rhw = perspz;
				v[0].color = c;
				v[0].specular = 0xFF000000;

				v[1].sx = (float)x2;
				v[1].sy = (float)y2;
				v[1].sz = f_a - perspz * f_boo;
				v[1].rhw = perspz;
				v[1].color = c;
				v[1].specular = 0xFF000000;

				AddLineSorted(&v[0], &v[1], 6);

				if (Z[0] > 0x4000)
					xsize = 1;
				else
					xsize = ((0x4000 - Z[0]) * pL->Size) >> 16;

				if (xsize < 4)
					xsize = 4;

				if (pL->Life < 16)
				{
					r = (pL->Life * pL->r) >> 4;
					g = (pL->Life * pL->g) >> 4;
					b = (pL->Life * pL->b) >> 4;
				}
				else
				{
					r = pL->r;
					g = pL->g;
					b = pL->b;
				}

				if (Z[0] > 0x3000)
				{
					r = (r * (0x5000 - Z[0])) >> 13;
					g = (g * (0x5000 - Z[0])) >> 13;
					b = (b * (0x5000 - Z[0])) >> 13;
				}

				x1 = XY[0];
				y1 = XY[1];
				z1 = Z[0];
				x2 = XY[2];
				y2 = XY[3];
				z2 = Z[1];
				setXYZ4(v, x1, y1, z1, x2, y2, z2, x1, y1, z1, x2, y2, z2, clipflags);
				x1 = (long)v[0].sx;
				y1 = (long)v[0].sy;
				z1 = (long)v[0].sz;
				x2 = (long)v[1].sx;
				y2 = (long)v[1].sy;
				z2 = (long)v[1].sz;

				vec[0].x = (x1 - x2) << 8;
				vec[0].y = (y1 - y2) << 8;
				vec[0].z = 0;
				Normalise(vec);
				vec[0].z = vec[0].x;
				vec[0].x = -vec[0].y;
				vec[0].y = vec[0].z;

				ysize = (vec[0].y * xsize) >> 12;
				xsize = (vec[0].x * xsize) >> 12;
				z = (z1 + z2) >> 1;

				if (z > 64)
				{
					setXY4(v, x1 + xsize, y1 + ysize, x2 + xsize, y2 + ysize, x1 - xsize, y1 - ysize, x2 - xsize, y2 - ysize, z, clipflags);

					c = RGBA(b, g, r, 0xFF);

					v[0].color = c;
					v[1].color = c;
					v[2].color = c;
					v[3].color = c;
					v[0].specular = 0xFF000000;
					v[1].specular = 0xFF000000;
					v[2].specular = 0xFF000000;
					v[3].specular = 0xFF000000;
					tex.drawtype = 2;
					tex.flag = 0;
					tex.tpage = sprite->tpage;
					tex.u1 = sprite->x1;
					tex.v1 = sprite->y2;
					tex.u2 = sprite->x2;
					tex.v2 = sprite->y2;
					tex.u3 = sprite->x2;
					tex.v3 = sprite->y1;
					tex.u4 = sprite->x1;
					tex.v4 = sprite->y1;
					AddQuadClippedSorted(v, 0, 1, 3, 2, &tex, 0);
				}
			}

			XY += 2;
			Z++;
		}
	}

	phd_PopMatrix();
}

void inject_specificfx(bool replace)
{
	INJECT(0x0048B990, DrawTrainStrips, replace);
	INJECT(0x004876C0, S_PrintShadow, replace);
	INJECT(0x00488140, S_DrawDrawSparks, replace);
	INJECT(0x0048C6C0, DrawBikeSpeedo, replace);
	INJECT(0x0048D3D0, Draw2DSprite, replace);
	INJECT(0x0048D580, DrawJeepSpeedo, replace);
	INJECT(0x00489B90, DrawDebris, replace);
	INJECT(0x0048C280, DoScreenFade, replace);
	INJECT(0x0048C400, DrawPsxTile, replace);
	INJECT(0x0048C5F0, DrawFlash, replace);
	INJECT(0x0048D160, S_DrawDarts, replace);
	INJECT(0x00489360, ClipCheckPoint, replace);
	INJECT(0x00488950, DrawFlatSky, replace);
	INJECT(0x00489480, OutputSky, replace);
	INJECT(0x00485600, setXY4, replace);
	INJECT(0x004857E0, setXY3, replace);
	INJECT(0x00485950, setXYZ4, replace);
	INJECT(0x00485D90, setXYZ3, replace);
	INJECT(0x0048C240, SetFade, replace);
	INJECT(0x00489950, DrawLaserSightSprite, replace);
	INJECT(0x0048BAB0, DrawSprite, replace);
	INJECT(0x0048B130, ShowTitle, replace);
	INJECT(0x0048BC30, SetUpLensFlare, replace);
	INJECT(0x00487B60, InitTarget_2, replace);
	INJECT(0x00487C30, InitBinoculars, replace);
	INJECT(0x00487D00, DrawBinoculars, replace);
	INJECT(0x0048AD60, DrawWraithTrail, replace);
	INJECT(0x0048A430, DrawDrips, replace);
	INJECT(0x004860D0, DrawBubbles, replace);
	INJECT(0x0048CAA0, DrawShockwaves, replace);
	INJECT(0x0048B480, DrawTrainFloorStrip, replace);
	INJECT(0x00486430, S_DrawSplashes, replace);
	INJECT(0x00488690, ClipLine, replace);
	INJECT(0x00486DD0, S_DrawFireSparks, replace);
	INJECT(0x00489540, DrawRope, replace);
	INJECT(0x0048A0C0, DrawBlood, replace);
	INJECT(0x00487260, S_DrawSmokeSparks, replace);
	INJECT(0x0048A840, DoUwEffect, replace);
	INJECT(0x0048D9B0, DrawLightning, replace);
}
