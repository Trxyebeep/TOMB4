#include "../tomb4/pch.h"
#include "dynamicshadow.h"
#include "../specific/3dmath.h"
#include "../game/control.h"
#include "../game/lara.h"
#include "../game/draw.h"
#include "../game/objects.h"
#include "../game/lara_states.h"
#include "../specific/d3dmatrix.h"
#include "../specific/output.h"
#include "../game/gameflow.h"
#include "../specific/winmain.h"
#include "../specific/dxshell.h"
#include "../specific/gamemain.h"
#include "../specific/drawroom.h"
#include "../specific/function_table.h"

float shadow_matrices[180];
float shadow_joint_matrices[180];

long ShadowMeshSweetnessTable[15] =
{
	0, 1, 2, 3, 4, 5, 6, 7, 14, 8, 9, 10, 11, 12, 13
};

char ShadowNodesToStashToScratch[14][2] =
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

char ShadowNodesToStashFromScratch[15][4] =
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

uchar ShadowUseMatrix[14][2] =
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

long ls_pitch;
long ls_yaw;

void CollectLights(ITEM_INFO* item)
{
	PCLIGHT* light;
	PHD_VECTOR best;
	long x, y, z, dist, bestdist;
	short angles[2];
	bool found;

	light = (PCLIGHT*)item->il.pCurrentLights;
	found = 0;
	best.x = 0;
	best.y = 0;
	best.z = 0;
	bestdist = 0x7FFFFFFF;

	for (int i = 0; i < item->il.nCurrentLights; i++)
	{
		if (!light[i].Active)
			continue;

		if (light[i].Type == LIGHT_SUN)	//if room has a sun, take it and exit
		{
			phd_GetVectorAngles(light[i].inx, light[i].iny, light[i].inz, angles);
			ls_pitch = angles[1];
			ls_yaw = angles[0];
			return;
		}

		if (light[i].Type == LIGHT_POINT || light[i].Type == LIGHT_SPOT)	//other lights?
		{
			x = item->pos.x_pos - light[i].ix;
			y = item->pos.y_pos - light[i].iy;
			z = item->pos.z_pos - light[i].iz;
			dist = SQUARE(x) + SQUARE(y) + SQUARE(z);

			if (dist < bestdist)	//find the closest
			{
				bestdist = dist;
				best.x = x;
				best.y = y;
				best.z = z;
				found = 1;
			}
		}
	}

	if (found)	//found a close light?
	{
		phd_GetVectorAngles(best.x, best.y, best.z, angles);
		ls_pitch = angles[1];
		ls_yaw = angles[0];
	}
	else
	{
		ls_pitch = item->pos.x_rot - 0x2000;	//no lights, do overhead
		ls_yaw = item->pos.y_rot + 0x8000;
	}
}

void ProjectLightToFloorMatrix(ITEM_INFO* item)
{
	FLOOR_INFO* floor;
	long x, y, z, h, dy;
	long pitch, yaw, c, t, rs, rc;
	short rn;

	x = item->pos.x_pos;
	y = item->pos.y_pos;
	z = item->pos.z_pos;
	rn = item->room_number;
	floor = GetFloor(x, y, z, &rn);
	h = GetHeight(floor, x, y, z);
	dy = y - h;
	x -= (dy * phd_sin(ls_yaw)) >> 15;
	y = h - 1;
	z -= (dy * phd_cos(ls_yaw)) >> 15;
	phd_TranslateAbs(x, y, z);

	pitch = ls_pitch - 16384;
	yaw = ls_yaw - 32768;
	c = phd_cos(pitch);

	if (c)
	{
		if (1024 * phd_sin(pitch) / c < 0)
			t = -(1024 * phd_sin(pitch) / c);
		else
			t = 1024 * phd_sin(pitch) / c;

		if (t > 1024)
			t = 1024;
	}
	else
		t = 1024;

	rs = t * phd_sin(yaw);

	if (rs < 0)
		rs -= 1025;

	rs >>= 10;
	rc = t * phd_cos(yaw);

	if (rc < 0)
		rc -= 1025;

	rc >>= 10;
	phd_mxptr[M01] = (phd_mxptr[M02] * rc + phd_mxptr[M00] * rs) >> 14;
	phd_mxptr[M11] = (phd_mxptr[M12] * rc + phd_mxptr[M10] * rs) >> 14;
	phd_mxptr[M21] = (phd_mxptr[M22] * rc + phd_mxptr[M20] * rs) >> 14;

	mMXPtr[M00] = (float)phd_mxptr[M00] / 16384;
	mMXPtr[M01] = (float)phd_mxptr[M01] / 16384;
	mMXPtr[M02] = (float)phd_mxptr[M02] / 16384;
	mMXPtr[M10] = (float)phd_mxptr[M10] / 16384;
	mMXPtr[M11] = (float)phd_mxptr[M11] / 16384;
	mMXPtr[M12] = (float)phd_mxptr[M12] / 16384;
	mMXPtr[M20] = (float)phd_mxptr[M20] / 16384;
	mMXPtr[M21] = (float)phd_mxptr[M21] / 16384;
	mMXPtr[M22] = (float)phd_mxptr[M22] / 16384;
}

void CalcShadowMatrices_Normal(short* frame, long* bone, long flag)
{
	PHD_VECTOR vec;
	float* matrix;
	short* rot;
	short* rot2;
	short gun;

	if (flag == 1)
		matrix = shadow_joint_matrices;
	else
		matrix = shadow_matrices;

	phd_PushMatrix();

	if (!flag || flag == 2)
		ProjectLightToFloorMatrix(lara_item);
	else
		phd_SetTrans(0, 0, 0);

	phd_RotYXZ(lara_item->pos.y_rot, lara_item->pos.x_rot, lara_item->pos.z_rot);

	if (flag == 2)
	{
		vec.x = -16384;
		vec.y = -16384;
		vec.z = -16384;
		ScaleCurrentMatrix(&vec);
	}

	phd_PushMatrix();
	rot = &frame[9];
	phd_TranslateRel(frame[6], frame[7], frame[8]);
	gar_RotYXZsuperpack(&rot, 0);
	memcpy(matrix, mMXPtr, 48);
	matrix += 12;

	phd_PushMatrix();
	phd_TranslateRel(bone[1], bone[2], bone[3]);
	gar_RotYXZsuperpack(&rot, 0);
	memcpy(matrix, mMXPtr, 48);
	matrix += 12;

	phd_TranslateRel(bone[5], bone[6], bone[7]);
	gar_RotYXZsuperpack(&rot, 0);
	memcpy(matrix, mMXPtr, 48);
	matrix += 12;

	phd_TranslateRel(bone[9], bone[10], bone[11]);
	gar_RotYXZsuperpack(&rot, 0);
	memcpy(matrix, mMXPtr, 48);
	matrix += 12;
	phd_PopMatrix();

	phd_PushMatrix();
	phd_TranslateRel(bone[13], bone[14], bone[15]);
	gar_RotYXZsuperpack(&rot, 0);
	memcpy(matrix, mMXPtr, 48);
	matrix += 12;

	phd_TranslateRel(bone[17], bone[18], bone[19]);
	gar_RotYXZsuperpack(&rot, 0);
	memcpy(matrix, mMXPtr, 48);
	matrix += 12;

	phd_TranslateRel(bone[21], bone[22], bone[23]);
	gar_RotYXZsuperpack(&rot, 0);
	memcpy(matrix, mMXPtr, 48);
	matrix += 12;
	phd_PopMatrix();

	phd_TranslateRel(bone[25], bone[26], bone[27]);
	gar_RotYXZsuperpack(&rot, 0);
	phd_RotYXZ(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);
	memcpy(matrix, mMXPtr, 48);
	matrix += 12;

	phd_PushMatrix();
	phd_TranslateRel(bone[53], bone[54], bone[55]);
	rot2 = rot;
	gar_RotYXZsuperpack(&rot2, 6);
	phd_RotYXZ(lara.head_y_rot, lara.head_x_rot, lara.head_z_rot);
	memcpy(matrix, mMXPtr, 48);
	matrix += 12;
	phd_PopMatrix();

	gun = WEAPON_NONE;

	if (lara.gun_status == LG_READY || lara.gun_status == LG_FLARE || lara.gun_status == LG_DRAW_GUNS || lara.gun_status == LG_UNDRAW_GUNS)
		gun = lara.gun_type;

	switch (gun)
	{
	case WEAPON_NONE:
	case WEAPON_FLARE:
	case WEAPON_TORCH:
		phd_PushMatrix();
		phd_TranslateRel(bone[29], bone[30], bone[31]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;
		phd_PopMatrix();

		phd_PushMatrix();
		phd_TranslateRel(bone[41], bone[42], bone[43]);

		if (lara.flare_control_left)
		{
			rot = &lara.left_arm.frame_base[(anims[lara.left_arm.anim_number].interpolation >> 8) *
				(lara.left_arm.frame_number - anims[lara.left_arm.anim_number].frame_base) + 9];

			gar_RotYXZsuperpack(&rot, 11);
		}
		else
			gar_RotYXZsuperpack(&rot, 0);

		memcpy(matrix, mMXPtr, 48);
		matrix += 12;

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;
		break;

	case WEAPON_PISTOLS:
	case WEAPON_UZI:
		phd_PushMatrix();
		phd_TranslateRel(bone[29], bone[30], bone[31]);

		mMXPtr[M00] = mMXPtr[-2 * indices_count + M00];
		mMXPtr[M01] = mMXPtr[-2 * indices_count + M01];
		mMXPtr[M02] = mMXPtr[-2 * indices_count + M02];
		mMXPtr[M10] = mMXPtr[-2 * indices_count + M10];
		mMXPtr[M11] = mMXPtr[-2 * indices_count + M11];
		mMXPtr[M12] = mMXPtr[-2 * indices_count + M12];
		mMXPtr[M20] = mMXPtr[-2 * indices_count + M20];
		mMXPtr[M21] = mMXPtr[-2 * indices_count + M21];
		mMXPtr[M22] = mMXPtr[-2 * indices_count + M22];

		phd_RotYXZ(lara.right_arm.y_rot, lara.right_arm.x_rot, lara.right_arm.z_rot);
		rot = &lara.right_arm.frame_base[(anims[lara.right_arm.anim_number].interpolation >> 8) *
			(lara.right_arm.frame_number - anims[lara.right_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 8);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;
		phd_PopMatrix();

		phd_PushMatrix();
		phd_TranslateRel(bone[41], bone[42], bone[43]);

		mMXPtr[M00] = mMXPtr[-2 * indices_count + M00];
		mMXPtr[M01] = mMXPtr[-2 * indices_count + M01];
		mMXPtr[M02] = mMXPtr[-2 * indices_count + M02];
		mMXPtr[M10] = mMXPtr[-2 * indices_count + M10];
		mMXPtr[M11] = mMXPtr[-2 * indices_count + M11];
		mMXPtr[M12] = mMXPtr[-2 * indices_count + M12];
		mMXPtr[M20] = mMXPtr[-2 * indices_count + M20];
		mMXPtr[M21] = mMXPtr[-2 * indices_count + M21];
		mMXPtr[M22] = mMXPtr[-2 * indices_count + M22];

		phd_RotYXZ(lara.left_arm.y_rot, lara.left_arm.x_rot, lara.left_arm.z_rot);
		rot = &lara.left_arm.frame_base[(anims[lara.left_arm.anim_number].interpolation >> 8) *
			(lara.left_arm.frame_number - anims[lara.left_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 11);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;
		break;

	case WEAPON_REVOLVER:
		phd_PushMatrix();
		phd_TranslateRel(bone[29], bone[30], bone[31]);

		mMXPtr[M00] = mMXPtr[-2 * indices_count + M00];
		mMXPtr[M01] = mMXPtr[-2 * indices_count + M01];
		mMXPtr[M02] = mMXPtr[-2 * indices_count + M02];
		mMXPtr[M10] = mMXPtr[-2 * indices_count + M10];
		mMXPtr[M11] = mMXPtr[-2 * indices_count + M11];
		mMXPtr[M12] = mMXPtr[-2 * indices_count + M12];
		mMXPtr[M20] = mMXPtr[-2 * indices_count + M20];
		mMXPtr[M21] = mMXPtr[-2 * indices_count + M21];
		mMXPtr[M22] = mMXPtr[-2 * indices_count + M22];

		phd_RotYXZ(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);
		rot = &lara.right_arm.frame_base[(anims[lara.right_arm.anim_number].interpolation >> 8) *
			(lara.right_arm.frame_number - anims[lara.right_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 8);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;
		phd_PopMatrix();

		phd_PushMatrix();
		phd_TranslateRel(bone[41], bone[42], bone[43]);

		mMXPtr[M00] = mMXPtr[-2 * indices_count + M00];
		mMXPtr[M01] = mMXPtr[-2 * indices_count + M01];
		mMXPtr[M02] = mMXPtr[-2 * indices_count + M02];
		mMXPtr[M10] = mMXPtr[-2 * indices_count + M10];
		mMXPtr[M11] = mMXPtr[-2 * indices_count + M11];
		mMXPtr[M12] = mMXPtr[-2 * indices_count + M12];
		mMXPtr[M20] = mMXPtr[-2 * indices_count + M20];
		mMXPtr[M21] = mMXPtr[-2 * indices_count + M21];
		mMXPtr[M22] = mMXPtr[-2 * indices_count + M22];

		phd_RotYXZ(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);
		rot = &lara.left_arm.frame_base[(anims[lara.left_arm.anim_number].interpolation >> 8) *
			(lara.left_arm.frame_number - anims[lara.left_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 11);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;
		break;

	case WEAPON_SHOTGUN:
	case WEAPON_GRENADE:
	case WEAPON_CROSSBOW:
		phd_PushMatrix();
		phd_TranslateRel(bone[29], bone[30], bone[31]);
		rot = &lara.right_arm.frame_base[lara.right_arm.frame_number *
			(anims[lara.right_arm.anim_number].interpolation >> 8) + 9];
		gar_RotYXZsuperpack(&rot, 8);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;
		phd_PopMatrix();

		phd_PushMatrix();
		phd_TranslateRel(bone[41], bone[42], bone[43]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;
		break;
	}

	phd_PopMatrix();
	phd_PopMatrix();
	phd_PopMatrix();
}

void CalcShadowMatrices_Interpolated(short* frame1, short* frame2, long frac, long rate, long* bone, long flag)
{
	PHD_VECTOR vec;
	float* matrix;
	float* arms;
	short* rot;
	short* rot2;
	short* rotcopy;
	short* rot2copy;
	short gun;

	if (flag == 1)
		matrix = shadow_joint_matrices;
	else
		matrix = shadow_matrices;

	phd_PushMatrix();

	if (!flag || flag == 2)
		ProjectLightToFloorMatrix(lara_item);
	else
		phd_SetTrans(0, 0, 0);

	phd_RotYXZ(lara_item->pos.y_rot, lara_item->pos.x_rot, lara_item->pos.z_rot);

	if (flag == 2)
	{
		vec.x = -16384;
		vec.y = -16384;
		vec.z = -16384;
		ScaleCurrentMatrix(&vec);
	}

	arms = mMXPtr;
	phd_PushMatrix();
	rot = frame1 + 9;
	rot2 = frame2 + 9;
	InitInterpolate(frac, rate);
	phd_TranslateRel_ID(frame1[6], frame1[7], frame1[8], frame2[6], frame2[7], frame2[8]);
	gar_RotYXZsuperpack_I(&rot, &rot2, 0);

	phd_PushMatrix();
	mInterpolateMatrix();
	memcpy(matrix, mMXPtr, 48);
	matrix += 12;
	phd_PopMatrix();

	phd_PushMatrix_I();
	phd_TranslateRel_I(bone[1], bone[2], bone[3]);
	gar_RotYXZsuperpack_I(&rot, &rot2, 0);
	phd_PushMatrix();
	mInterpolateMatrix();
	memcpy(matrix, mMXPtr, 48);
	matrix += 12;
	phd_PopMatrix();

	phd_TranslateRel_I(bone[5], bone[6], bone[7]);
	gar_RotYXZsuperpack_I(&rot, &rot2, 0);
	phd_PushMatrix();
	mInterpolateMatrix();
	memcpy(matrix, mMXPtr, 48);
	matrix += 12;
	phd_PopMatrix();

	phd_TranslateRel_I(bone[9], bone[10], bone[11]);
	gar_RotYXZsuperpack_I(&rot, &rot2, 0);
	phd_PushMatrix();
	mInterpolateMatrix();
	memcpy(matrix, mMXPtr, 48);
	matrix += 12;
	phd_PopMatrix();
	phd_PopMatrix_I();

	phd_PushMatrix_I();
	phd_TranslateRel_I(bone[13], bone[14], bone[15]);
	gar_RotYXZsuperpack_I(&rot, &rot2, 0);
	phd_PushMatrix();
	mInterpolateMatrix();
	memcpy(matrix, mMXPtr, 48);
	matrix += 12;
	phd_PopMatrix();

	phd_TranslateRel_I(bone[17], bone[18], bone[19]);
	gar_RotYXZsuperpack_I(&rot, &rot2, 0);
	phd_PushMatrix();
	mInterpolateMatrix();
	memcpy(matrix, mMXPtr, 48);
	matrix += 12;
	phd_PopMatrix();

	phd_TranslateRel_I(bone[21], bone[22], bone[23]);
	gar_RotYXZsuperpack_I(&rot, &rot2, 0);
	phd_PushMatrix();
	mInterpolateMatrix();
	memcpy(matrix, mMXPtr, 48);
	matrix += 12;
	phd_PopMatrix();
	phd_PopMatrix_I();

	phd_TranslateRel_I(bone[25], bone[26], bone[27]);
	gar_RotYXZsuperpack_I(&rot, &rot2, 0);
	phd_RotYXZ_I(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);
	phd_PushMatrix();
	mInterpolateMatrix();
	memcpy(matrix, mMXPtr, 48);
	matrix += 12;
	phd_PopMatrix();

	phd_PushMatrix_I();
	phd_TranslateRel_I(bone[53], bone[54], bone[55]);
	rotcopy = rot;
	rot2copy = rot2;
	gar_RotYXZsuperpack_I(&rotcopy, &rot2copy, 6);
	phd_RotYXZ_I(lara.head_y_rot, lara.head_x_rot, lara.head_z_rot);
	phd_PushMatrix();
	mInterpolateMatrix();
	memcpy(matrix, mMXPtr, 48);
	matrix += 12;
	phd_PopMatrix();
	phd_PopMatrix_I();

	gun = WEAPON_NONE;

	if (lara.gun_status == LG_READY || lara.gun_status == LG_FLARE || lara.gun_status == LG_DRAW_GUNS || lara.gun_status == LG_UNDRAW_GUNS)
		gun = lara.gun_type;

	switch (gun)
	{
	case WEAPON_NONE:
	case WEAPON_FLARE:
	case WEAPON_TORCH:
		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[29], bone[30], bone[31]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		mInterpolateMatrix();
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;
		phd_PopMatrix();

		phd_TranslateRel_I(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		mInterpolateMatrix();
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;
		phd_PopMatrix();

		phd_TranslateRel_I(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		mInterpolateMatrix();
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;
		phd_PopMatrix();
		phd_PopMatrix_I();

		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[41], bone[42], bone[43]);

		if (lara.flare_control_left)
		{
			rot2 = &lara.left_arm.frame_base[(anims[lara.left_arm.anim_number].interpolation >> 8) *
				(lara.left_arm.frame_number - anims[lara.left_arm.anim_number].frame_base) + 9];
			rot = rot2;
			gar_RotYXZsuperpack_I(&rot, &rot2, 11);
		}
		else
			gar_RotYXZsuperpack_I(&rot, &rot2, 0);

		phd_PushMatrix();
		mInterpolateMatrix();
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;
		phd_PopMatrix();

		phd_TranslateRel_I(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		mInterpolateMatrix();
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;
		phd_PopMatrix();

		phd_TranslateRel_I(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		mInterpolateMatrix();
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;
		phd_PopMatrix();
		break;

	case WEAPON_PISTOLS:
	case WEAPON_UZI:
		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[29], bone[30], bone[31]);
		mInterpolateArmMatrix(arms);
		phd_RotYXZ(lara.right_arm.y_rot, lara.right_arm.x_rot, lara.right_arm.z_rot);
		rot = &lara.right_arm.frame_base[(anims[lara.right_arm.anim_number].interpolation >> 8) *
			(lara.right_arm.frame_number - anims[lara.right_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 8);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;
		phd_PopMatrix_I();

		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[41], bone[42], bone[43]);
		mInterpolateArmMatrix(arms);
		phd_RotYXZ(lara.left_arm.y_rot, lara.left_arm.x_rot, lara.left_arm.z_rot);
		rot = &lara.left_arm.frame_base[(anims[lara.left_arm.anim_number].interpolation >> 8) *
			(lara.left_arm.frame_number - anims[lara.left_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 11);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;
		break;

	case WEAPON_REVOLVER:
		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[29], bone[30], bone[31]);
		mInterpolateArmMatrix(arms);
		phd_RotYXZ_I(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);
		rot = &lara.right_arm.frame_base[(anims[lara.right_arm.anim_number].interpolation >> 8) *
			(lara.right_arm.frame_number - anims[lara.right_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 8);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;
		phd_PopMatrix_I();

		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[41], bone[42], bone[43]);
		mInterpolateArmMatrix(arms);
		phd_RotYXZ_I(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);
		rot = &lara.left_arm.frame_base[(anims[lara.left_arm.anim_number].interpolation >> 8) *
			(lara.left_arm.frame_number - anims[lara.left_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 11);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack(&rot, 0);
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;
		break;

	case WEAPON_SHOTGUN:
	case WEAPON_GRENADE:
	case WEAPON_CROSSBOW:
		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[29], bone[30], bone[31]);
		rot2 = &lara.right_arm.frame_base[lara.right_arm.frame_number *
			(anims[lara.right_arm.anim_number].interpolation >> 8) + 9];
		rot = rot2;
		gar_RotYXZsuperpack_I(&rot, &rot2, 8);
		phd_PushMatrix();
		mInterpolateMatrix();
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;
		phd_PopMatrix();

		phd_TranslateRel_I(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		mInterpolateMatrix();
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;
		phd_PopMatrix();

		phd_TranslateRel_I(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		mInterpolateMatrix();
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;
		phd_PopMatrix();
		phd_PopMatrix_I();

		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[41], bone[42], bone[43]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		mInterpolateMatrix();
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;
		phd_PopMatrix();

		phd_TranslateRel_I(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		mInterpolateMatrix();
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;
		phd_PopMatrix();

		phd_TranslateRel_I(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		mInterpolateMatrix();
		memcpy(matrix, mMXPtr, 48);
		matrix += 12;
		phd_PopMatrix();
		break;
	}

	phd_PopMatrix();
	phd_PopMatrix();
	phd_PopMatrix();
}

void CalcShadowMatrices(long flag)
{
	long* bone;
	short* frame;
	short* frmptr[2];
	long rate, frac;
	short jerk;

	CollectLights(lara_item);
	bone = &bones[objects[lara_item->object_number].bone_index];
	frac = GetFrames(lara_item, frmptr, &rate);

	if (lara.hit_direction < 0)
	{
		if (frac)
		{
			CalcShadowMatrices_Interpolated(frmptr[0], frmptr[1], frac, rate, bone, flag);
			return;
		}
	}

	if (lara.hit_direction < 0)
		frame = *frmptr;
	else
	{
		if (lara.hit_direction == 0)
			jerk = lara.IsDucked ? ANIM_JERK_DUCKF : ANIM_JERK_FORWARD;
		else if (lara.hit_direction == 1)
			jerk = lara.IsDucked ? ANIM_JERK_DUCKR : ANIM_JERK_RIGHT;
		else if (lara.hit_direction == 2)
			jerk = lara.IsDucked ? ANIM_JERK_DUCKB : ANIM_JERK_BACK;
		else
			jerk = lara.IsDucked ? ANIM_JERK_DUCKL : ANIM_JERK_LEFT;

		frame = &anims[jerk].frame_ptr[lara.hit_frame * (anims[jerk].interpolation >> 8)];
	}

	CalcShadowMatrices_Normal(frame, bone, flag);
}

void TransformShadowMesh(D3DTLVERTEX* v, MESH_DATA* mesh)
{
	short* clip;
	float zv;
	short c;

	clip = clipflags;

	for (int i = 0; i < mesh->nVerts; i++)
	{
		v->tu = v->sx;
		v->tv = v->sy;
		c = 0;

		if (v->sz < f_mznear)
			c = -128;
		else
		{
			zv = f_mpersp / v->sz;
			v->sx = zv * v->sx + f_centerx;
			v->sy = zv * v->sy + f_centery;
			v->rhw = f_moneopersp * zv;

			if (v->sx < f_left)
				c++;
			else if (v->sx > f_right)
				c += 2;

			if (v->sy < f_top)
				c += 4;
			else if (v->sy > f_bottom)
				c += 8;
		}

		*clip++ = c;
		v->color = 0x28000000;
		v->specular = 0xFF000000;
		v++;
	}
}

void phd_PutPolygons_ShadowMesh(short* objptr, long clip)
{
	MESH_DATA* mesh;
	D3DTLVERTEX* v;
	TEXTURESTRUCT tex;
	short* quad;
	short* tri;

	SetD3DViewMatrix();
	mesh = (MESH_DATA*)objptr;

	if (!objptr)
		return;

	if (mesh->nVerts)
		DXAttempt(DestVB->ProcessVertices(D3DVOP_TRANSFORM, 0, mesh->nVerts, mesh->SourceVB, 0, App.dx._lpD3DDevice, 0));

	DestVB->Lock(DDLOCK_READONLY, (void**)&v, 0);
	TransformShadowMesh(v, mesh);

	tex.flag = 0;
	tex.tpage = 0;
	tex.drawtype = 3;
	tex.u1 = 0;
	tex.v1 = 0;
	tex.u2 = 0;
	tex.v2 = 0;
	tex.u3 = 0;
	tex.v3 = 0;
	tex.u4 = 0;
	tex.v4 = 0;

	quad = mesh->gt4;

	for (int i = 0; i < mesh->ngt4; i++, quad += 6)
		AddQuadSorted(v, quad[0], quad[1], quad[2], quad[3], &tex, 1);

	tri = mesh->gt3;

	for (int i = 0; i < mesh->ngt3; i++, tri += 5)
		AddTriSorted(v, tri[0], tri[1], tri[2], &tex, 1);

	DestVB->Unlock();
}

void DrawDynamicShadow()
{
	OBJECT_INFO* obj;
	FVECTOR v0;
	FVECTOR v1;
	short** meshpp;
	long* bone;
	short* rot;
	long top, bottom, left, right, stash, cos, sin, xRot;

	top = phd_top;
	bottom = phd_bottom;
	left = phd_left;
	right = phd_right;
	phd_top = 0;
	phd_left = 0;
	phd_bottom = phd_winymax;
	phd_right = phd_winxmax;
	phd_PushMatrix();
	obj = &objects[lara_item->object_number];

	for (int i = 0; i < 15; i++)//skin
	{
		mMXPtr[M00] = shadow_matrices[i * 12 + M00];
		mMXPtr[M01] = shadow_matrices[i * 12 + M01];
		mMXPtr[M02] = shadow_matrices[i * 12 + M02];
		mMXPtr[M03] = shadow_matrices[i * 12 + M03];
		mMXPtr[M10] = shadow_matrices[i * 12 + M10];
		mMXPtr[M11] = shadow_matrices[i * 12 + M11];
		mMXPtr[M12] = shadow_matrices[i * 12 + M12];
		mMXPtr[M13] = shadow_matrices[i * 12 + M13];
		mMXPtr[M20] = shadow_matrices[i * 12 + M20];
		mMXPtr[M21] = shadow_matrices[i * 12 + M21];
		mMXPtr[M22] = shadow_matrices[i * 12 + M22];
		mMXPtr[M23] = shadow_matrices[i * 12 + M23];

		phd_PutPolygons_ShadowMesh(lara.mesh_ptrs[ShadowMeshSweetnessTable[i]], -1);	//no meshbits checks?

		for (int j = 0; j < 4; j++)
		{
			stash = (uchar)ShadowNodesToStashFromScratch[i][j];

			if (stash == 255)
				break;

			StashSkinVertices(stash);
		}
	}

	obj = &objects[LARA_SKIN_JOINTS];
	meshpp = &meshes[obj->mesh_index];
	meshpp += 2;

	for (int i = 0; i < 14; i++)//joints
	{
		SkinVerticesToScratch(ShadowNodesToStashToScratch[i][0]);
		SkinVerticesToScratch(ShadowNodesToStashToScratch[i][1]);

		if (ShadowUseMatrix[i][0] >= 255)
			phd_PutPolygons_ShadowMesh(*meshpp, -1);
		else
		{
			mMXPtr[M00] = shadow_matrices[ShadowUseMatrix[i][1] * 12 + M00];
			mMXPtr[M01] = shadow_matrices[ShadowUseMatrix[i][1] * 12 + M01];
			mMXPtr[M02] = shadow_matrices[ShadowUseMatrix[i][1] * 12 + M02];
			mMXPtr[M03] = shadow_matrices[ShadowUseMatrix[i][1] * 12 + M03];
			mMXPtr[M10] = shadow_matrices[ShadowUseMatrix[i][1] * 12 + M10];
			mMXPtr[M11] = shadow_matrices[ShadowUseMatrix[i][1] * 12 + M11];
			mMXPtr[M12] = shadow_matrices[ShadowUseMatrix[i][1] * 12 + M12];
			mMXPtr[M13] = shadow_matrices[ShadowUseMatrix[i][1] * 12 + M13];
			mMXPtr[M20] = shadow_matrices[ShadowUseMatrix[i][1] * 12 + M20];
			mMXPtr[M21] = shadow_matrices[ShadowUseMatrix[i][1] * 12 + M21];
			mMXPtr[M22] = shadow_matrices[ShadowUseMatrix[i][1] * 12 + M22];
			mMXPtr[M23] = shadow_matrices[ShadowUseMatrix[i][1] * 12 + M23];
			phd_PushMatrix();
			v0.x = shadow_matrices[12 * ShadowUseMatrix[i][0] + M01];
			v0.y = shadow_matrices[12 * ShadowUseMatrix[i][0] + M11];
			v0.z = shadow_matrices[12 * ShadowUseMatrix[i][0] + M21];
			v1.x = shadow_matrices[12 * ShadowUseMatrix[i][1] + M01];
			v1.y = shadow_matrices[12 * ShadowUseMatrix[i][1] + M11];
			v1.z = shadow_matrices[12 * ShadowUseMatrix[i][1] + M21];
			cos = long(v0.x * v1.x + v0.y * v1.y + v0.z * v1.z);
			sin = phd_sqrt(16777216 - SQUARE(cos));

			if (i == 1 || i == 4)
				xRot = -phd_atan(cos, sin);
			else
				xRot = phd_atan(cos, sin);

			phd_RotX(short(-xRot >> 1));
			phd_PutPolygons_ShadowMesh(*meshpp, -1);
			phd_PopMatrix();
		}

		meshpp += 2;
	}

	if (!(gfLevelFlags & GF_YOUNGLARA))
	{
		obj = &objects[lara.holster];
		meshpp = &meshes[obj->mesh_index];
		meshpp += 8;
		mMXPtr[M00] = shadow_matrices[1 * 12 + M00];
		mMXPtr[M01] = shadow_matrices[1 * 12 + M01];
		mMXPtr[M02] = shadow_matrices[1 * 12 + M02];
		mMXPtr[M03] = shadow_matrices[1 * 12 + M03];
		mMXPtr[M10] = shadow_matrices[1 * 12 + M10];
		mMXPtr[M11] = shadow_matrices[1 * 12 + M11];
		mMXPtr[M12] = shadow_matrices[1 * 12 + M12];
		mMXPtr[M13] = shadow_matrices[1 * 12 + M13];
		mMXPtr[M20] = shadow_matrices[1 * 12 + M20];
		mMXPtr[M21] = shadow_matrices[1 * 12 + M21];
		mMXPtr[M22] = shadow_matrices[1 * 12 + M22];
		mMXPtr[M23] = shadow_matrices[1 * 12 + M23];
		phd_PutPolygons_ShadowMesh(*meshpp, -1);

		meshpp += 8;
		mMXPtr[M00] = shadow_matrices[4 * 12 + M00];
		mMXPtr[M01] = shadow_matrices[4 * 12 + M01];
		mMXPtr[M02] = shadow_matrices[4 * 12 + M02];
		mMXPtr[M03] = shadow_matrices[4 * 12 + M03];
		mMXPtr[M10] = shadow_matrices[4 * 12 + M10];
		mMXPtr[M11] = shadow_matrices[4 * 12 + M11];
		mMXPtr[M12] = shadow_matrices[4 * 12 + M12];
		mMXPtr[M13] = shadow_matrices[4 * 12 + M13];
		mMXPtr[M20] = shadow_matrices[4 * 12 + M20];
		mMXPtr[M21] = shadow_matrices[4 * 12 + M21];
		mMXPtr[M22] = shadow_matrices[4 * 12 + M22];
		mMXPtr[M23] = shadow_matrices[4 * 12 + M23];
		phd_PutPolygons_ShadowMesh(*meshpp, -1);

		if (lara.back_gun)
		{
			phd_PushMatrix();
			mMXPtr[M00] = shadow_matrices[84 + M00];
			mMXPtr[M01] = shadow_matrices[84 + M01];
			mMXPtr[M02] = shadow_matrices[84 + M02];
			mMXPtr[M03] = shadow_matrices[84 + M03];
			mMXPtr[M10] = shadow_matrices[84 + M10];
			mMXPtr[M11] = shadow_matrices[84 + M11];
			mMXPtr[M12] = shadow_matrices[84 + M12];
			mMXPtr[M13] = shadow_matrices[84 + M13];
			mMXPtr[M20] = shadow_matrices[84 + M20];
			mMXPtr[M21] = shadow_matrices[84 + M21];
			mMXPtr[M22] = shadow_matrices[84 + M22];
			mMXPtr[M23] = shadow_matrices[84 + M23];
			obj = &objects[lara.back_gun];
			bone = &bones[obj->bone_index];
			meshpp = &meshes[obj->mesh_index];
			meshpp += 28;
			phd_TranslateRel(bone[53], bone[54], bone[55]);
			rot = objects[lara.back_gun].frame_base + 9;
			gar_RotYXZsuperpack(&rot, 14);
			phd_PutPolygons_ShadowMesh(*meshpp, -1);
			phd_PopMatrix();
		}
	}

	phd_PopMatrix();
	phd_top = top;
	phd_bottom = bottom;
	phd_left = left;
	phd_right = right;
}
