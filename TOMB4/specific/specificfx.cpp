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

#ifdef SMOOTH_SHADOWS
#include "../tomb4/tomb4.h"

#define CIRCUMFERENCE_POINTS 32 // Number of points in the circumference
#endif

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
		cp[i].x = x * phd_sin(65536 * i / CIRCUMFERENCE_POINTS) >> 14;
		cp[i].z = z * phd_cos(65536 * i / CIRCUMFERENCE_POINTS) >> 14;
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
		cp[i].x = (x * phd_mxptr[M00] + z * phd_mxptr[M02] + phd_mxptr[M03]) >> 14;
		cp[i].z = (x * phd_mxptr[M20] + z * phd_mxptr[M22] + phd_mxptr[M23]) >> 14;
	}

	ccp.x = phd_mxptr[M03] >> 14;
	ccp.z = phd_mxptr[M23] >> 14;
	phd_PopMatrix();

	for (int i = 0; i < CIRCUMFERENCE_POINTS; i++)
	{
		room_number = item->room_number;
		cp[i].y = GetHeight(GetFloor(cp[i].x, item->floor, cp[i].z, &room_number), cp[i].x, item->floor, cp[i].z);

		if (ABS(cp[i].y - item->floor) > POINT_HEIGHT_CORRECTION)
			cp[i].y = item->floor;
	}

	room_number = item->room_number;
	ccp.y = GetHeight(GetFloor(ccp.x, item->floor, ccp.z, &room_number), ccp.x, item->floor, ccp.z);

	if (ABS(ccp.y - item->floor) > POINT_HEIGHT_CORRECTION)
		ccp.y = item->floor;

	phd_PushMatrix();
	phd_TranslateAbs(pos.x, y, pos.z);
	phd_RotY(item->pos.y_rot);

	for (int i = 0; i < CIRCUMFERENCE_POINTS; i++)
	{
		fx = cv[i].x;
		fy = (cp[i].y - item->floor);
		fz = cv[i].z;
		cv[i].x = (phd_mxptr[M00] * fx + phd_mxptr[M01] * fy + phd_mxptr[M02] * fz + phd_mxptr[M03]) >> 14;
		cv[i].y = (phd_mxptr[M10] * fx + phd_mxptr[M11] * fy + phd_mxptr[M12] * fz + phd_mxptr[M13]) >> 14;
		cv[i].z = (phd_mxptr[M20] * fx + phd_mxptr[M21] * fy + phd_mxptr[M22] * fz + phd_mxptr[M23]) >> 14;
	}

	fy = (ccp.y - item->floor);
	ccv.x = (phd_mxptr[M01] * fy + phd_mxptr[M03]) >> 14;
	ccv.y = (phd_mxptr[M11] * fy + phd_mxptr[M13]) >> 14;
	ccv.z = (phd_mxptr[M21] * fy + phd_mxptr[M23]) >> 14;
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
			v[0].color = 0x80000000 - (item->after_death << 24);
			v[1].color = v[0].color;
			v[2].color = v[0].color;
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
	long* sXYZ;
	long* hXZ;
	long* hY;
	long* xyz;
	float uadd, vadd;
	long sxyz[GRID_POINTS * 3];
	long hxz[GRID_POINTS * 2];
	long hy[GRID_POINTS];
	long x, y, z, xSize, zSize, xDist, zDist;
	short room_number;

	xSize = size * (box[1] - box[0]) / 160;	//x size of grid
	zSize = size * (box[5] - box[4]) / 160;	//z size of grid
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
	sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 11];
	uadd = (sprite->x2 - sprite->x1) / 3;
	vadd = (sprite->y2 - sprite->y1) / 3;

	for (int i = 0; i < 3; i++, sXYZ += 12)
	{
		xyz = sXYZ;

		for (int j = 0; j < 3; j++, xyz += 3)
		{
			setXYZ4(v, xyz[0], xyz[1], xyz[2], xyz[3], xyz[4], xyz[5], xyz[15], xyz[16], xyz[17], xyz[12], xyz[13], xyz[14], clipflags);
			v[0].color = 0xFF3C3C3C;
			v[1].color = 0xFF3C3C3C;
			v[2].color = 0xFF3C3C3C;
			v[3].color = 0xFF3C3C3C;
			v[0].specular = 0xFF000000;
			v[1].specular = 0xFF000000;
			v[2].specular = 0xFF000000;
			v[3].specular = 0xFF000000;
			Tex.drawtype = 5;
			Tex.flag = 0;
			Tex.tpage = sprite->tpage;
			Tex.u1 = j * uadd + sprite->x1;
			Tex.v1 = i * vadd + sprite->y1;
			Tex.u2 = Tex.u1 + uadd;
			Tex.v2 = Tex.v1;
			Tex.u3 = Tex.u2;
			Tex.v3 = Tex.v1 + vadd;
			Tex.u4 = Tex.u1;
			Tex.v4 = Tex.v3;
			AddQuadSorted(v, 0, 1, 2, 3, &Tex, 0);
		}
	}
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
				sx1 = (-s1h * sin) >> 12;
				sx2 = (s1h * sin) >> 12;
				sy1 = (-s2h * sin) >> 12;
				sy2 = (s2h * sin) >> 12;
				cx1 = (-s1h * cos) >> 12;
				cx2 = (s1h * cos) >> 12;
				cy1 = (-s2h * cos) >> 12;
				cy2 = (s2h * cos) >> 12;
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
	rVel = ABS(vel >> 1);

	if (rVel)
	{
		rVel += (((rVel - 4096) >> 5) * phd_sin((GlobalCounter & 7) << 13)) >> 14;

		if (rVel < 0)
			rVel = 0;
	}

	rMVel = maxVel >> 1;
	rTVel = turboVel >> 1;
	angle = -0x4000;

	for (int i = 0; i <= rTVel; i += 2048)
	{
		x0 = ((rSize * (phd_sin(angle + i)) >> 13) - ((rSize * phd_sin(angle + i)) >> 15)) * ((float)phd_winxmax / 512.0F);
		y0 = (-(rSize * phd_cos(angle + i)) >> 14) * (float)phd_winymax / 240.0F;
		x1 = ((size * (phd_sin(angle + i)) >> 13) - ((size * phd_sin(angle + i)) >> 15)) * ((float)phd_winxmax / 512.0F);
		y1 = (-(size * phd_cos(angle + i)) >> 14) * (float)phd_winymax / 240.0F;

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
	x0 = ((-4 * (phd_sin(angle + rVel)) >> 13) - ((-4 * phd_sin(angle + rVel)) >> 15)) * ((float)phd_winxmax / 512.0F);
	y0 = (-(-4 * phd_cos(angle + rVel)) >> 14) * (float)phd_winymax / 240.0F;
	x1 = ((size * (phd_sin(angle + rVel)) >> 13) - ((size * phd_sin(angle + rVel)) >> 15)) * ((float)phd_winxmax / 512.0F);
	y1 = (-(size * phd_cos(angle + rVel)) >> 14) * (float)phd_winymax / 240.0F;

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
	rVel = ABS(vel >> 1);

	if (rVel)
	{
		rVel += (((rVel - 4096) >> 5) * phd_sin((GlobalCounter & 7) << 13)) >> 14;

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
		x0 = ((rSize * (phd_sin(angle + i)) >> 13) - ((rSize * phd_sin(angle + i)) >> 15)) * ((float)phd_winxmax / 512.0F);
		y0 = (-(rSize * phd_cos(angle + i)) >> 14) * (float)phd_winymax / 240.0F;
		x1 = ((size * (phd_sin(angle + i)) >> 13) - ((size * phd_sin(angle + i)) >> 15)) * ((float)phd_winxmax / 512.0F);
		y1 = (-(size * phd_cos(angle + i)) >> 14) * (float)phd_winymax / 240.0F;

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
	x0 = ((-4 * (phd_sin(angle + rVel)) >> 13) - ((-4 * phd_sin(angle + rVel)) >> 15)) * ((float)phd_winxmax / 512.0F);
	y0 = (-(-4 * phd_cos(angle + rVel)) >> 14) * (float)phd_winymax / 240.0F;
	x1 = ((size * (phd_sin(angle + rVel)) >> 13) - ((size * phd_sin(angle + rVel)) >> 15)) * ((float)phd_winxmax / 512.0F);
	y1 = (-(size * phd_cos(angle + rVel)) >> 14) * (float)phd_winymax / 240.0F;

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
		XY[0] = short((phd_mxptr[M03] + phd_mxptr[M00] * offsets[0] + phd_mxptr[M01] * offsets[1] + phd_mxptr[M02] * offsets[2]) >> 14);
		XY[1] = short((phd_mxptr[M13] + phd_mxptr[M10] * offsets[0] + phd_mxptr[M11] * offsets[1] + phd_mxptr[M12] * offsets[2]) >> 14);
		Z[0] = (phd_mxptr[M23] + phd_mxptr[M20] * offsets[0] + phd_mxptr[M21] * offsets[1] + phd_mxptr[M22] * offsets[2]) >> 14;

		offsets[0] = dptr->XYZOffsets2[0];
		offsets[1] = dptr->XYZOffsets2[1];
		offsets[2] = dptr->XYZOffsets2[2];
		XY[2] = short((phd_mxptr[M03] + phd_mxptr[M00] * offsets[0] + phd_mxptr[M01] * offsets[1] + phd_mxptr[M02] * offsets[2]) >> 14);
		XY[3] = short((phd_mxptr[M13] + phd_mxptr[M10] * offsets[0] + phd_mxptr[M11] * offsets[1] + phd_mxptr[M12] * offsets[2]) >> 14);
		Z[1] = (phd_mxptr[M23] + phd_mxptr[M20] * offsets[0] + phd_mxptr[M21] * offsets[1] + phd_mxptr[M22] * offsets[2]) >> 14;

		offsets[0] = dptr->XYZOffsets3[0];
		offsets[1] = dptr->XYZOffsets3[1];
		offsets[2] = dptr->XYZOffsets3[2];
		XY[4] = short((phd_mxptr[M03] + phd_mxptr[M00] * offsets[0] + phd_mxptr[M01] * offsets[1] + phd_mxptr[M02] * offsets[2]) >> 14);
		XY[5] = short((phd_mxptr[M13] + phd_mxptr[M10] * offsets[0] + phd_mxptr[M11] * offsets[1] + phd_mxptr[M12] * offsets[2]) >> 14);
		Z[2] = (phd_mxptr[M23] + phd_mxptr[M20] * offsets[0] + phd_mxptr[M21] * offsets[1] + phd_mxptr[M22] * offsets[2]) >> 14;

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
	z1 = phd_mxptr[M23] >> 14;
	num = (-96 * phd_cos(item->pos.x_rot)) >> 14;
	mxx = (num * phd_sin(item->pos.y_rot)) >> 14;
	mxy = (96 * phd_sin(item->pos.x_rot)) >> 14;
	mxz = (num * phd_cos(item->pos.y_rot)) >> 14;
	xx = phd_mxptr[M00] * mxx + phd_mxptr[M01] * mxy + phd_mxptr[M02] * mxz + phd_mxptr[M03];
	yy = phd_mxptr[M10] * mxx + phd_mxptr[M11] * mxy + phd_mxptr[M12] * mxz + phd_mxptr[M13];
	zz = phd_mxptr[M20] * mxx + phd_mxptr[M21] * mxy + phd_mxptr[M22] * mxz + phd_mxptr[M23];
	zv = f_persp / (float)zz;
	x2 = short(float(xx * zv + f_centerx));
	y2 = short(float(yy * zv + f_centery));
	z2 = zz >> 14;

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
		vec[i].x = (phd_mxptr[M00] * x + phd_mxptr[M01] * y + phd_mxptr[M02] * z + phd_mxptr[M03]) >> 14;
		vec[i].y = (phd_mxptr[M10] * x + phd_mxptr[M11] * y + phd_mxptr[M12] * z + phd_mxptr[M13]) >> 14;
		vec[i].z = (phd_mxptr[M20] * x + phd_mxptr[M21] * y + phd_mxptr[M22] * z + phd_mxptr[M23]) >> 14;
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
		vec[i].x = (phd_mxptr[M00] * x + phd_mxptr[M01] * y + phd_mxptr[M02] * z + phd_mxptr[M03]) >> 14;
		vec[i].y = (phd_mxptr[M10] * x + phd_mxptr[M11] * y + phd_mxptr[M12] * z + phd_mxptr[M13]) >> 14;
		vec[i].z = (phd_mxptr[M20] * x + phd_mxptr[M21] * y + phd_mxptr[M22] * z + phd_mxptr[M23]) >> 14;
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
	Z[0] = vec.z >> 14;
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
		dx = ABS(pos.x - lara_item->pos.x_pos);
		dy = ABS(pos.y - lara_item->pos.y_pos);
		dz = ABS(pos.z - lara_item->pos.z_pos);

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

		while (ABS(pos.x) > 0x36000 || ABS(pos.y) > 0x36000 || ABS(pos.z) > 0x36000)
		{
			pos.x -= (x - camera.pos.x) >> 4;
			pos.y -= (y - camera.pos.y) >> 4;
			pos.z -= (z - camera.pos.z) >> 4;
		}

		dx = (pos.x - camera.pos.x) >> 4;
		dy = (pos.y - camera.pos.y) >> 4;
		dz = (pos.z - camera.pos.z) >> 4;

		while (ABS(pos.x - camera.pos.x) > 0x8000 || ABS(pos.y - camera.pos.y) > 0x8000 || ABS(pos.z - camera.pos.z) > 0x8000)
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
	Z[0] = pos.z >> 14;
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
}
