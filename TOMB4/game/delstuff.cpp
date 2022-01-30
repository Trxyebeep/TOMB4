#include "../tomb4/pch.h"
#include "delstuff.h"
#include "../specific/specificfx.h"
#include "../specific/3dmath.h"
#include "draw.h"
#include "../specific/output.h"
#include "hair.h"
#include "objects.h"
#include "control.h"

char lara_underwater_skin_sweetness_table[15] =
{
	0, 2, 3, 0, 5, 6, 7, 9, 10, 11, 12, 13, 14, 8, 0
};

long lara_mesh_sweetness_table[15] =
{
	0, 1, 2, 3, 4, 5, 6, 7, 14, 8, 9, 10, 11, 12, 13
};

char NodesToStashToScratch[14][2] =
{
	{1, 3},
	{4, 5},
	{6, 7},
	{2, 8},
	{9, 10},
	{11, 12},
	{0, 13},
	{14, 17},
	{18, 19},
	{20, 21},
	{15, 22},
	{23, 24},
	{25, 26},
	{16, 27}
};

char NodesToStashFromScratch[15][4] =
{
	{0, 1, 2, -1},
	{3, 4, -1, 0},
	{5, 6, -1, 0},
	{7, -1, 0, 0},
	{8, 9, -1, 0},
	{10, 11, -1, 0},
	{12, -1, 0, 0},
	{13, 16, 14, 15},
	{27, 28, 34, -1},
	{17, 18, -1, 0},
	{19, 20, -1, 0},
	{21, -1, 0, 0},
	{22, 23, -1, 0},
	{24, 25, -1, 0},
	{26, -1, 0, 0}
};

uchar SkinUseMatrix[14][2] =
{
	{255, 255},
	{1, 2},
	{255, 255},
	{255, 255},
	{4, 5},
	{255, 255},
	{255, 255},
	{255, 255},
	{9, 10},
	{255, 255},
	{255, 255},
	{12, 13},
	{255, 255},
	{255, 255}
};

void DrawLara(ITEM_INFO* item, long mirror)
{
	OBJECT_INFO* obj;
	PHD_VECTOR v0;
	PHD_VECTOR v1;
	short** meshpp;
	long* bone;
	short* rot;
	long top, bottom, left, right, dx, dy, dz, dist, stash, cos, sin, xRot;
	static long a = 255;

	top = phd_top;
	bottom = phd_bottom;
	left = phd_left;
	right = phd_right;
	phd_top = 0;
	phd_left = 0;
	phd_bottom = phd_winymax;
	phd_right = phd_winxmax;
	phd_PushMatrix();
	obj = &objects[item->object_number];

	if (lara.vehicle == NO_ITEM)
		S_PrintShadow(obj->shadow_size, GLaraShadowframe, item);

	if (input & IN_LOOK)
	{
		dx = lara_item->pos.x_pos - CamPos.x;
		dy = lara_item->pos.y_pos - CamPos.y - 512;
		dz = lara_item->pos.z_pos - CamPos.z;
		dist = phd_sqrt(SQUARE(dx) + SQUARE(dy) + SQUARE(dz));
		a = dist >> 2;

		if (a < 0)
			a = 0;

		if (a > 255)
			a = 255;

		GlobalAlpha = a << 24;
	}
	else
	{
		if (a < 255)
		{
			a += 8;

			if (a > 255)
				a = 255;
		}

		GlobalAlpha = a << 24;
	}

	if (!mirror)
		CalculateObjectLightingLara();

	for (int i = 0; i < 15; i++)//skin
	{
		phd_mxptr[M00] = lara_matrices[i * 12 + M00];
		phd_mxptr[M01] = lara_matrices[i * 12 + M01];
		phd_mxptr[M02] = lara_matrices[i * 12 + M02];
		phd_mxptr[M03] = lara_matrices[i * 12 + M03];
		phd_mxptr[M10] = lara_matrices[i * 12 + M10];
		phd_mxptr[M11] = lara_matrices[i * 12 + M11];
		phd_mxptr[M12] = lara_matrices[i * 12 + M12];
		phd_mxptr[M13] = lara_matrices[i * 12 + M13];
		phd_mxptr[M20] = lara_matrices[i * 12 + M20];
		phd_mxptr[M21] = lara_matrices[i * 12 + M21];
		phd_mxptr[M22] = lara_matrices[i * 12 + M22];
		phd_mxptr[M23] = lara_matrices[i * 12 + M23];

		if (LaraNodeUnderwater[i])
			bLaraUnderWater = i;
		else
			bLaraUnderWater = -1;

		phd_PutPolygons(lara.mesh_ptrs[lara_mesh_sweetness_table[i]], -1);	//no meshbits checks?

		for (int j = 0; j < 4; j++)
		{
			stash = (uchar)NodesToStashFromScratch[i][j];

			if (stash == 255)
				break;

			StashSkinVertices(stash);
		}
	}

	in_joints = 1;
	phd_PopMatrix();
	bLaraUnderWater = LaraNodeUnderwater[8] != 0 ? 8 : -1;
	DrawHair();
	phd_PushMatrix();
	obj = &objects[LARA_SKIN_JOINTS];
	meshpp = &meshes[obj->mesh_index];
	meshpp += 2;

	for (int i = 0; i < 14; i++)//joints
	{
		SkinVerticesToScratch(NodesToStashToScratch[i][0]);
		SkinVerticesToScratch(NodesToStashToScratch[i][1]);

		if (LaraNodeUnderwater[lara_underwater_skin_sweetness_table[i]])
			bLaraUnderWater = lara_underwater_skin_sweetness_table[i];
		else
			bLaraUnderWater = -1;

		if (SkinUseMatrix[i][0] >= 255)
			phd_PutPolygons(*meshpp, -1);
		else
		{
			phd_mxptr[M00] = lara_matrices[SkinUseMatrix[i][1] * 12 + M00];
			phd_mxptr[M01] = lara_matrices[SkinUseMatrix[i][1] * 12 + M01];
			phd_mxptr[M02] = lara_matrices[SkinUseMatrix[i][1] * 12 + M02];
			phd_mxptr[M03] = lara_matrices[SkinUseMatrix[i][1] * 12 + M03];
			phd_mxptr[M10] = lara_matrices[SkinUseMatrix[i][1] * 12 + M10];
			phd_mxptr[M11] = lara_matrices[SkinUseMatrix[i][1] * 12 + M11];
			phd_mxptr[M12] = lara_matrices[SkinUseMatrix[i][1] * 12 + M12];
			phd_mxptr[M13] = lara_matrices[SkinUseMatrix[i][1] * 12 + M13];
			phd_mxptr[M20] = lara_matrices[SkinUseMatrix[i][1] * 12 + M20];
			phd_mxptr[M21] = lara_matrices[SkinUseMatrix[i][1] * 12 + M21];
			phd_mxptr[M22] = lara_matrices[SkinUseMatrix[i][1] * 12 + M22];
			phd_mxptr[M23] = lara_matrices[SkinUseMatrix[i][1] * 12 + M23];
			phd_PushMatrix();
			v0.x = lara_matrices[12 * SkinUseMatrix[i][0] + M01];
			v0.y = lara_matrices[12 * SkinUseMatrix[i][0] + M11];
			v0.z = lara_matrices[12 * SkinUseMatrix[i][0] + M21];
			v1.x = lara_matrices[12 * SkinUseMatrix[i][1] + M01];
			v1.y = lara_matrices[12 * SkinUseMatrix[i][1] + M11];
			v1.z = lara_matrices[12 * SkinUseMatrix[i][1] + M21];
			cos = ((v0.x * v1.x) + (v0.y * v1.y) + (v0.z * v1.z)) >> 14;
			sin = phd_sqrt(16777216 - SQUARE(cos));

			if (i == 1 || i == 4)
				xRot = -phd_atan(cos, sin);
			else
				xRot = phd_atan(cos, sin);

			phd_RotX((short)(-xRot >> 1));
			phd_PutPolygons(*meshpp, -1);
			phd_PopMatrix();
		}

		meshpp += 2;
	}

	in_joints = 0;
	bLaraUnderWater = (LaraNodeUnderwater[0] != 0) - 1;

	if (!(gfLevelFlags & GF_YOUNGLARA))
	{
		obj = &objects[lara.holster];
		meshpp = &meshes[obj->mesh_index];
		meshpp += 8;
		phd_mxptr[M00] = lara_matrices[1 * 12 + M00];
		phd_mxptr[M01] = lara_matrices[1 * 12 + M01];
		phd_mxptr[M02] = lara_matrices[1 * 12 + M02];
		phd_mxptr[M03] = lara_matrices[1 * 12 + M03];
		phd_mxptr[M10] = lara_matrices[1 * 12 + M10];
		phd_mxptr[M11] = lara_matrices[1 * 12 + M11];
		phd_mxptr[M12] = lara_matrices[1 * 12 + M12];
		phd_mxptr[M13] = lara_matrices[1 * 12 + M13];
		phd_mxptr[M20] = lara_matrices[1 * 12 + M20];
		phd_mxptr[M21] = lara_matrices[1 * 12 + M21];
		phd_mxptr[M22] = lara_matrices[1 * 12 + M22];
		phd_mxptr[M23] = lara_matrices[1 * 12 + M23];
		phd_PutPolygons(*meshpp, -1);

		meshpp += 8;
		phd_mxptr[M00] = lara_matrices[4 * 12 + M00];
		phd_mxptr[M01] = lara_matrices[4 * 12 + M01];
		phd_mxptr[M02] = lara_matrices[4 * 12 + M02];
		phd_mxptr[M03] = lara_matrices[4 * 12 + M03];
		phd_mxptr[M10] = lara_matrices[4 * 12 + M10];
		phd_mxptr[M11] = lara_matrices[4 * 12 + M11];
		phd_mxptr[M12] = lara_matrices[4 * 12 + M12];
		phd_mxptr[M13] = lara_matrices[4 * 12 + M13];
		phd_mxptr[M20] = lara_matrices[4 * 12 + M20];
		phd_mxptr[M21] = lara_matrices[4 * 12 + M21];
		phd_mxptr[M22] = lara_matrices[4 * 12 + M22];
		phd_mxptr[M23] = lara_matrices[4 * 12 + M23];
		phd_PutPolygons(*meshpp, -1);

		if (lara.back_gun)
		{
			phd_PushMatrix();
			phd_mxptr[M00] = lara_matrices[84 + M00];
			phd_mxptr[M01] = lara_matrices[84 + M01];
			phd_mxptr[M02] = lara_matrices[84 + M02];
			phd_mxptr[M03] = lara_matrices[84 + M03];
			phd_mxptr[M10] = lara_matrices[84 + M10];
			phd_mxptr[M11] = lara_matrices[84 + M11];
			phd_mxptr[M12] = lara_matrices[84 + M12];
			phd_mxptr[M13] = lara_matrices[84 + M13];
			phd_mxptr[M20] = lara_matrices[84 + M20];
			phd_mxptr[M21] = lara_matrices[84 + M21];
			phd_mxptr[M22] = lara_matrices[84 + M22];
			phd_mxptr[M23] = lara_matrices[84 + M23];
			obj = &objects[lara.back_gun];
			bone = &bones[obj->bone_index];
			meshpp = &meshes[obj->mesh_index];
			meshpp += 28;
			phd_TranslateRel(bone[53], bone[54], bone[55]);
			rot = objects[lara.back_gun].frame_base + 9;
			gar_RotYXZsuperpack(&rot, 14);
			phd_PutPolygons(*meshpp, -1);
			phd_PopMatrix();
		}
	}

	bLaraUnderWater = 0;
	phd_PopMatrix();
	phd_top = top;
	phd_bottom = bottom;
	phd_left = left;
	phd_right = right;
	GlobalAlpha = 0xFF000000;
}

void GetLaraJointPos(PHD_VECTOR* pos, long node)
{
	phd_PushMatrix();
	phd_mxptr[M00] = lara_joint_matrices[node * 12 + M00];
	phd_mxptr[M01] = lara_joint_matrices[node * 12 + M01];
	phd_mxptr[M02] = lara_joint_matrices[node * 12 + M02];
	phd_mxptr[M03] = lara_joint_matrices[node * 12 + M03];
	phd_mxptr[M10] = lara_joint_matrices[node * 12 + M10];
	phd_mxptr[M11] = lara_joint_matrices[node * 12 + M11];
	phd_mxptr[M12] = lara_joint_matrices[node * 12 + M12];
	phd_mxptr[M13] = lara_joint_matrices[node * 12 + M13];
	phd_mxptr[M20] = lara_joint_matrices[node * 12 + M20];
	phd_mxptr[M21] = lara_joint_matrices[node * 12 + M21];
	phd_mxptr[M22] = lara_joint_matrices[node * 12 + M22];
	phd_mxptr[M23] = lara_joint_matrices[node * 12 + M23];
	phd_TranslateRel(pos->x, pos->y, pos->z);
	pos->x = phd_mxptr[M03] >> 14;
	pos->y = phd_mxptr[M13] >> 14;
	pos->z = phd_mxptr[M23] >> 14;
	pos->x += lara_item->pos.x_pos;
	pos->y += lara_item->pos.y_pos;
	pos->z += lara_item->pos.z_pos;
	phd_PopMatrix();
}

void SetLaraUnderwaterNodes()
{
	PHD_VECTOR pos;
	long bit;
	short room_num;

	pos.x = lara_item->pos.x_pos;
	pos.y = lara_item->pos.y_pos;
	pos.z = lara_item->pos.z_pos;
	room_num = lara_item->room_number;
	GetFloor(pos.x, pos.y, pos.z, &room_num);
	bLaraInWater = room[room_num].flags & ROOM_UNDERWATER ? 1 : 0;
	bit = 0;

	for (int i = 14; i >= 0; i--)
	{
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;
		GetLaraJointPos(&pos, i);

		if (lara_mesh_sweetness_table[i] == 7)
			pos.y -= 120;

		if (lara_mesh_sweetness_table[i] == 14)
			pos.y -= 60;

		room_num = lara_item->room_number;
		GetFloor(pos.x, pos.y, pos.z, &room_num);
		LaraNodeUnderwater[i] = room[room_num].flags & ROOM_UNDERWATER;

		if (room[room_num].flags & ROOM_UNDERWATER)
		{
			lara.wet[i] = 252;

			if (!(bit & 1))
			{
				LaraNodeAmbient[1] = room[room_num].ambient;
				bit |= 1;
			}
		}
		else if (!(bit & 2))
		{
			LaraNodeAmbient[0] = room[room_num].ambient;
			bit |= 2;
		}
	}
}

void inject_delstuff(bool replace)
{
	INJECT(0x0041D140, DrawLara, replace);
	INJECT(0x0041D9A0, GetLaraJointPos, replace);
	INJECT(0x0041F260, SetLaraUnderwaterNodes, replace);
}
