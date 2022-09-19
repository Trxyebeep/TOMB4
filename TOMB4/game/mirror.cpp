#include "../tomb4/pch.h"
#include "mirror.h"
#include "delstuff.h"
#include "../specific/function_table.h"
#include "tomb4fx.h"
#include "../specific/3dmath.h"
#include "hair.h"
#include "lara.h"
#include "gameflow.h"

void Draw_Mirror_Lara()
{
	HAIR_STRUCT* hair;
	GUNSHELL_STRUCT* gunshell;
	long zplane;

	zplane = gfMirrorZPlane << 1;
	hair = &hairs[0][0];

	for (int i = 0; i < 6; i++)
	{
		hair->pos.z_pos = zplane - hair->pos.z_pos;
		hair->pos.y_rot = 0x8000 - hair->pos.y_rot;
		hair++;
	}

	for (int i = 0; i < 24; i++)
	{
		gunshell = &Gunshells[i];

		if (gunshell->counter)
			gunshell->pos.z_pos = zplane - gunshell->pos.z_pos;
	}

	lara_item->pos.z_pos = zplane - lara_item->pos.z_pos;
	lara_item->pos.x_rot = -lara_item->pos.x_rot;
	lara_item->pos.y_rot = -lara_item->pos.y_rot;
	lara_item->pos.z_rot += 0x8000;
	CalcLaraMatrices(2);
	SetCullCW();
	DrawLara(lara_item, 1);
	DrawGunshells();
	SetCullCCW();
	lara_item->pos.z_pos = zplane - lara_item->pos.z_pos;
	lara_item->pos.x_rot = -lara_item->pos.x_rot;
	lara_item->pos.y_rot = -lara_item->pos.y_rot;
	lara_item->pos.z_rot += 0x8000;
	phd_PushMatrix();

	if (lara.right_arm.flash_gun)
	{
		mMXPtr[M00] = lara_matrices[11 * indices_count + M00];
		mMXPtr[M01] = lara_matrices[11 * indices_count + M01];
		mMXPtr[M02] = lara_matrices[11 * indices_count + M02];
		mMXPtr[M03] = lara_matrices[11 * indices_count + M03];
		mMXPtr[M10] = lara_matrices[11 * indices_count + M10];
		mMXPtr[M11] = lara_matrices[11 * indices_count + M11];
		mMXPtr[M12] = lara_matrices[11 * indices_count + M12];
		mMXPtr[M13] = lara_matrices[11 * indices_count + M13];
		mMXPtr[M20] = lara_matrices[11 * indices_count + M20];
		mMXPtr[M21] = lara_matrices[11 * indices_count + M21];
		mMXPtr[M22] = lara_matrices[11 * indices_count + M22];
		mMXPtr[M23] = lara_matrices[11 * indices_count + M23];
		SetGunFlash(lara.gun_type);
	}

	if (lara.left_arm.flash_gun)
	{
		mMXPtr[M00] = lara_matrices[14 * indices_count + M00];
		mMXPtr[M01] = lara_matrices[14 * indices_count + M01];
		mMXPtr[M02] = lara_matrices[14 * indices_count + M02];
		mMXPtr[M03] = lara_matrices[14 * indices_count + M03];
		mMXPtr[M10] = lara_matrices[14 * indices_count + M10];
		mMXPtr[M11] = lara_matrices[14 * indices_count + M11];
		mMXPtr[M12] = lara_matrices[14 * indices_count + M12];
		mMXPtr[M13] = lara_matrices[14 * indices_count + M13];
		mMXPtr[M20] = lara_matrices[14 * indices_count + M20];
		mMXPtr[M21] = lara_matrices[14 * indices_count + M21];
		mMXPtr[M22] = lara_matrices[14 * indices_count + M22];
		mMXPtr[M23] = lara_matrices[14 * indices_count + M23];
		SetGunFlash(lara.gun_type);
	}

	phd_PopMatrix();

	for (int i = 0; i < 24; i++)
	{
		gunshell = &Gunshells[i];

		if (gunshell->counter)
			gunshell->pos.z_pos = zplane - gunshell->pos.z_pos;
	}

	hair = &hairs[0][0];

	for (int i = 0; i < 6; i++)
	{
		hair->pos.z_pos = zplane - hair->pos.z_pos;
		hair->pos.y_rot = 0x8000 - hair->pos.y_rot;
		hair++;
	}
}
