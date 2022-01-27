#include "../tomb4/pch.h"
#include "footprnt.h"
#include "delstuff.h"
#include "control.h"
#include "sound.h"
#ifdef FOOTPRINTS
#include "objects.h"
#include "../specific/specificfx.h"
#include "../specific/function_table.h"
#include "../specific/3dmath.h"
#include "../tomb4/tomb4.h"
#endif

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
		SoundEffect(footsounds[floor->fx] + SFX_FOOTSTEPS_MUD, &lara_item->pos, SFX_DEFAULT);

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

#ifdef FOOTPRINTS
static void ProjectTriPoints(FOOTPRINT* print, PHD_VECTOR* pos, long& x, long& y, long& z)
{
	FLOOR_INFO* floor;
	long height, cy;
	short room_number;

	room_number = lara_item->room_number;
	floor = GetFloor(print->x + pos->x, print->y, print->z + pos->z, &room_number);
	height = GetHeight(floor, print->x + pos->x, print->y, print->z + pos->z);
	cy = height > print->y ? print->y - height : height - print->y;

	if (ABS(cy) < 128)
		pos->y = cy;

	x = (phd_mxptr[M00] * pos->x + phd_mxptr[M01] * pos->y + phd_mxptr[M02] * pos->z + phd_mxptr[M03]) >> 14;
	y = (phd_mxptr[M10] * pos->x + phd_mxptr[M11] * pos->y + phd_mxptr[M12] * pos->z + phd_mxptr[M13]) >> 14;
	z = (phd_mxptr[M20] * pos->x + phd_mxptr[M21] * pos->y + phd_mxptr[M22] * pos->z + phd_mxptr[M23]) >> 14;
}

void S_DrawFootPrints()
{
	FOOTPRINT* print;
	SPRITESTRUCT* sprite;
	D3DTLVERTEX v[3];
	PHD_VECTOR pos;
	TEXTURESTRUCT tex;
	float u1, v1, u2, v2;
	long x1, y1, z1, x2, y2, z2, x3, y3, z3, col;
	
	for (int i = 0; i < 32; i++)
	{
		print = &FootPrint[i];

		if (print->Active)
		{
			if (!tomb4.footprints)
			{
				print->Active = 0;
				continue;
			}

			print->Active--;

			if (print->Active > 0xFF)
				col = 0xFF;
			else
				col = print->Active;

			phd_PushMatrix();
			phd_TranslateAbs(print->x, print->y - 1, print->z);
			phd_RotY(print->YRot);

			pos.x = 0;
			pos.y = 0;
			pos.z = -64;
			ProjectTriPoints(print, &pos, x1, y1, z1);

			pos.x = -128;
			pos.y = 0;
			pos.z = 64;
			ProjectTriPoints(print, &pos, x2, y2, z2);

			pos.x = 128;
			pos.y = 0;
			pos.z = 64;
			ProjectTriPoints(print, &pos, x3, y3, z3);

			phd_PopMatrix();
			setXYZ3(v, x1, y1, z1, x2, y2, z2, x3, y3, z3, clipflags);

			for (int i = 0; i < 3; i++)
			{
				v[i].color = RGBA(col, col, col, 0xFF);
				v[i].specular = 0xFF000000;
			}

			sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 10];
			tex.drawtype = 5;
			tex.flag = 0;
			tex.tpage = sprite->tpage;
			u1 = sprite->x1;
			u2 = sprite->x2;
			v1 = sprite->y1;
			v2 = sprite->y2;
			tex.u1 = u1;
			tex.v1 = v1;	//top left
			tex.u2 = u2;
			tex.v2 = v1;	//top right
			tex.u3 = u1;
			tex.v3 = v2;	//bottom left
			AddTriSorted(v, 0, 1, 2, &tex, 1);
		}
	}
}
#endif

void inject_footprint(bool replace)
{
	INJECT(0x00437D50, AddFootPrint, replace);
}
