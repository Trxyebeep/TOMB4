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

void inject_specificfx(bool replace)
{
	INJECT(0x0048B990, DrawTrainStrips, replace);
	INJECT(0x004876C0, S_PrintShadow, replace);
	INJECT(0x00488140, S_DrawDrawSparks, replace);
}
