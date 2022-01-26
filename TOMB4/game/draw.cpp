#include "../tomb4/pch.h"
#include "draw.h"
#include "../specific/3dmath.h"
#include "../specific/output.h"
#include "../specific/lighting.h"
#include "deltapak.h"
#include "lara_states.h"
#include "control.h"
#include "delstuff.h"
#include "../specific/specificfx.h"
#include "objects.h"
#include "../specific/function_stubs.h"
#include "train.h"
#include "tomb4fx.h"
#include "rope.h"
#include "sound.h"
#include "../specific/polyinsert.h"
#include "mirror.h"
#include "scarab.h"
#include "croc.h"
#ifdef FOOTPRINTS
#include "footprnt.h"
#endif

void InitInterpolate(long frac, long rate)
{
	IM_rate = rate;
	IM_frac = frac;
	IMptr = IMstack;
	memcpy(IMstack, phd_mxptr, 48);
}

void phd_PopMatrix_I()
{
	phd_PopMatrix();
	IMptr -= 12;
}

void phd_PushMatrix_I()
{
	phd_PushMatrix();
	memcpy(IMptr + 12, IMptr, 48);
	IMptr += 12;
}

void phd_RotY_I(short ang)
{
	long* ptr;

	phd_RotY(ang);
	ptr = phd_mxptr;
	phd_mxptr = IMptr;
	phd_RotY(ang);
	phd_mxptr = ptr;
}

void phd_RotX_I(short ang)
{
	long* ptr;

	phd_RotX(ang);
	ptr = phd_mxptr;
	phd_mxptr = IMptr;
	phd_RotX(ang);
	phd_mxptr = ptr;
}

void phd_RotZ_I(short ang)
{
	long* ptr;

	phd_RotZ(ang);
	ptr = phd_mxptr;
	phd_mxptr = IMptr;
	phd_RotZ(ang);
	phd_mxptr = ptr;
}

void phd_TranslateRel_I(long x, long y, long z)
{
	long* ptr;

	phd_TranslateRel(x, y, z);
	ptr = phd_mxptr;
	phd_mxptr = IMptr;
	phd_TranslateRel(x, y, z);
	phd_mxptr = ptr;
}

void phd_TranslateRel_ID(long x, long y, long z, long x2, long y2, long z2)
{
	long* ptr;

	phd_TranslateRel(x, y, z);
	ptr = phd_mxptr;
	phd_mxptr = IMptr;
	phd_TranslateRel(x2, y2, z2);
	phd_mxptr = ptr;
}

void phd_RotYXZ_I(short y, short x, short z)
{
	long* ptr;

	phd_RotYXZ(y, x, z);
	ptr = phd_mxptr;
	phd_mxptr = IMptr;
	phd_RotYXZ(y, x, z);
	phd_mxptr = ptr;
}

void gar_RotYXZsuperpack_I(short** pprot1, short** pprot2, long skip)
{
	long* ptr;

	gar_RotYXZsuperpack(pprot1, skip);
	ptr = phd_mxptr;
	phd_mxptr = IMptr;
	gar_RotYXZsuperpack(pprot2, skip);
	phd_mxptr = ptr;
}

void gar_RotYXZsuperpack(short** pprot, long skip)
{
	ushort* prot;

	while (skip)
	{
		prot = (ushort*)*pprot;

		if (*prot & 0xC000)
			*pprot += 1;
		else
			*pprot += 2;

		skip--;
	}

	prot = (ushort*)*pprot;

	if (*prot >> 14)
	{
		if ((*prot >> 14) == 1)
			phd_RotX((short)((*prot & 4095) << 4));
		else if ((*prot >> 14) == 2)
			phd_RotY((short)((*prot & 4095) << 4));
		else
			phd_RotZ((short)((*prot & 4095) << 4));
	}
	else
	{
		phd_RotYXZpack(((ushort) * *pprot << 16) + (ushort)(*pprot)[1]);
		++*pprot;
	}

	++*pprot;
}

void phd_PutPolygons_I(short* ptr, long clip)
{
	phd_PushMatrix();
	InterpolateMatrix();
	phd_PutPolygons(ptr, clip);
	phd_PopMatrix();
}

void InterpolateMatrix()
{
	if (IM_rate == 2 || (IM_frac == 2 && IM_rate == 4))
	{
		phd_mxptr[M00] = (phd_mxptr[M00] + IMptr[M00]) >> 1;
		phd_mxptr[M01] = (phd_mxptr[M01] + IMptr[M01]) >> 1;
		phd_mxptr[M02] = (phd_mxptr[M02] + IMptr[M02]) >> 1;
		phd_mxptr[M03] = (phd_mxptr[M03] + IMptr[M03]) >> 1;
		phd_mxptr[M10] = (phd_mxptr[M10] + IMptr[M10]) >> 1;
		phd_mxptr[M11] = (phd_mxptr[M11] + IMptr[M11]) >> 1;
		phd_mxptr[M12] = (phd_mxptr[M12] + IMptr[M12]) >> 1;
		phd_mxptr[M13] = (phd_mxptr[M13] + IMptr[M13]) >> 1;
		phd_mxptr[M20] = (phd_mxptr[M20] + IMptr[M20]) >> 1;
		phd_mxptr[M21] = (phd_mxptr[M21] + IMptr[M21]) >> 1;
		phd_mxptr[M22] = (phd_mxptr[M22] + IMptr[M22]) >> 1;
		phd_mxptr[M23] = (phd_mxptr[M23] + IMptr[M23]) >> 1;
	}
	else if (IM_frac == 1)
	{
		phd_mxptr[M00] += (IMptr[M00] - phd_mxptr[M00]) >> 2;
		phd_mxptr[M01] += (IMptr[M01] - phd_mxptr[M01]) >> 2;
		phd_mxptr[M02] += (IMptr[M02] - phd_mxptr[M02]) >> 2;
		phd_mxptr[M03] += (IMptr[M03] - phd_mxptr[M03]) >> 2;
		phd_mxptr[M10] += (IMptr[M10] - phd_mxptr[M10]) >> 2;
		phd_mxptr[M11] += (IMptr[M11] - phd_mxptr[M11]) >> 2;
		phd_mxptr[M12] += (IMptr[M12] - phd_mxptr[M12]) >> 2;
		phd_mxptr[M13] += (IMptr[M13] - phd_mxptr[M13]) >> 2;
		phd_mxptr[M20] += (IMptr[M20] - phd_mxptr[M20]) >> 2;
		phd_mxptr[M21] += (IMptr[M21] - phd_mxptr[M21]) >> 2;
		phd_mxptr[M22] += (IMptr[M22] - phd_mxptr[M22]) >> 2;
		phd_mxptr[M23] += (IMptr[M23] - phd_mxptr[M23]) >> 2;
	}
	else
	{
		phd_mxptr[M00] = IMptr[M00] - ((IMptr[M00] - phd_mxptr[M00]) >> 2);
		phd_mxptr[M01] = IMptr[M01] - ((IMptr[M01] - phd_mxptr[M01]) >> 2);
		phd_mxptr[M02] = IMptr[M02] - ((IMptr[M02] - phd_mxptr[M02]) >> 2);
		phd_mxptr[M03] = IMptr[M03] - ((IMptr[M03] - phd_mxptr[M03]) >> 2);
		phd_mxptr[M10] = IMptr[M10] - ((IMptr[M10] - phd_mxptr[M10]) >> 2);
		phd_mxptr[M11] = IMptr[M11] - ((IMptr[M11] - phd_mxptr[M11]) >> 2);
		phd_mxptr[M12] = IMptr[M12] - ((IMptr[M12] - phd_mxptr[M12]) >> 2);
		phd_mxptr[M13] = IMptr[M13] - ((IMptr[M13] - phd_mxptr[M13]) >> 2);
		phd_mxptr[M20] = IMptr[M20] - ((IMptr[M20] - phd_mxptr[M20]) >> 2);
		phd_mxptr[M21] = IMptr[M21] - ((IMptr[M21] - phd_mxptr[M21]) >> 2);
		phd_mxptr[M22] = IMptr[M22] - ((IMptr[M22] - phd_mxptr[M22]) >> 2);
		phd_mxptr[M23] = IMptr[M23] - ((IMptr[M23] - phd_mxptr[M23]) >> 2);
	}
}

void InterpolateArmMatrix(long* mx)
{
	phd_mxptr[M00] = mx[M00];
	phd_mxptr[M01] = mx[M01];
	phd_mxptr[M02] = mx[M02];
	phd_mxptr[M10] = mx[M10];
	phd_mxptr[M11] = mx[M11];
	phd_mxptr[M12] = mx[M12];
	phd_mxptr[M20] = mx[M20];
	phd_mxptr[M21] = mx[M21];
	phd_mxptr[M22] = mx[M22];

	if (IM_rate == 2 || (IM_frac == 2 && IM_rate == 4))
	{
		phd_mxptr[M03] = (phd_mxptr[M03] + IMptr[M03]) >> 1;
		phd_mxptr[M13] = (phd_mxptr[M13] + IMptr[M13]) >> 1;
		phd_mxptr[M23] = (phd_mxptr[M23] + IMptr[M23]) >> 1;
	}
	else if (IM_frac == 1)
	{
		phd_mxptr[M03] += (IMptr[M03] - phd_mxptr[M03]) >> 2;
		phd_mxptr[M13] += (IMptr[M13] - phd_mxptr[M13]) >> 2;
		phd_mxptr[M23] += (IMptr[M23] - phd_mxptr[M23]) >> 2;
	}
	else
	{
		phd_mxptr[M03] = IMptr[M03] - ((IMptr[M03] - phd_mxptr[M03]) >> 2);
		phd_mxptr[M13] = IMptr[M13] - ((IMptr[M13] - phd_mxptr[M13]) >> 2);
		phd_mxptr[M23] = IMptr[M23] - ((IMptr[M23] - phd_mxptr[M23]) >> 2);
	}
}

void S_InsertRoom(short room_number)
{
	ROOM_INFO* r;

	current_room = room_number;
	r = &room[room_number];
	phd_TranslateAbs(0, 0, 0);
	phd_left = r->left;
	phd_right = r->right;
	phd_top = r->top;
	phd_bottom = r->bottom;
	_InsertRoom(r);
}

void CalculateObjectLighting(ITEM_INFO* item, short* frame)
{
	long x, y, z;

	if (item->shade >= 0)
		S_CalculateStaticMeshLight(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->shade & 0x7FFF, &room[item->room_number]);
	else
	{
		phd_PushUnitMatrix();
		phd_mxptr[M03] = 0;
		phd_mxptr[M13] = 0;
		phd_mxptr[M23] = 0;
		phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
		phd_TranslateRel((frame[0] + frame[1]) >> 1, (frame[2] + frame[3]) >> 1, (frame[4] + frame[5]) >> 1);
		x = item->pos.x_pos + (phd_mxptr[M03] >> 14);
		y = item->pos.y_pos + (phd_mxptr[M13] >> 14);
		z = item->pos.z_pos + (phd_mxptr[M23] >> 14);
		phd_PopMatrix();
		current_item = item;
		item->il.item_pos.x = x;
		item->il.item_pos.y = y;
		item->il.item_pos.z = z;
		CalcAmbientLight(item);
		CreateLightList(item);
	}
}

void CalculateObjectLightingLara()
{
	PHD_VECTOR pos;
	short room_no;

	if (GLOBAL_playing_cutseq)
		CalculateObjectLightingLaraCutSeq();
	else
	{
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;

		if (lara_item->anim_number == ANIM_DUCKBREATHE || lara_item->anim_number == ANIM_ALL4S || lara_item->anim_number == ANIM_BREATH)
		{
			pos.x = lara_item->pos.x_pos;

			if (lara_item->anim_number == ANIM_BREATH)
				pos.y = lara_item->pos.y_pos - 512;
			else
				pos.y = lara_item->pos.y_pos - 192;

			pos.z = lara_item->pos.z_pos;
			room_no = lara_item->room_number;
			GetFloor(pos.x, pos.y, pos.z, &room_no);
		}
		else
		{
			GetLaraJointPos(&pos, 7);
			room_no = lara_item->room_number;
			GetFloor(pos.x, pos.y, pos.z, &room_no);
		}

		current_item = lara_item;
		lara_item->il.item_pos.x = pos.x;
		lara_item->il.item_pos.y = pos.y;
		lara_item->il.item_pos.z = pos.z;
		CalcAmbientLight(lara_item);
		CreateLightList(lara_item);
	}
}

void DrawAnimatingItem(ITEM_INFO* item)
{
	OBJECT_INFO* obj;
	BITE_INFO* bite;
	short** meshpp;
	long* bone;
	short* frm[2];
	short* data;
	short* rot;
	short* rot2;
	long frac, rate, clip, bit, rnd;

	frac = GetFrames(item, frm, &rate);
	obj = &objects[item->object_number];
	bite = &EnemyBites[obj->bite_offset];

	if (obj->shadow_size)
		S_PrintShadow(obj->shadow_size, frm[0], item);

	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);

	if (obj->object_mip && (obj + 1)->loaded && phd_mxptr[M23] >> 16 > obj->object_mip)
		obj++;

	if (item->object_number < ENEMY_JEEP || item->object_number > SETHA_MIP)
		calc_animating_item_clip_window(item, frm[0]);
	else
	{
		phd_left = 0;
		phd_right = phd_winwidth;
		phd_top = 0;
		phd_bottom = phd_winheight;
	}

	clip = S_GetObjectBounds(frm[0]);

	if (clip)
	{
		CalculateObjectLighting(item, frm[0]);

		if (!item->data)
			data = no_rotation;
		else
			data = (short*)item->data;

		bit = 1;
		meshpp = &meshes[obj->mesh_index];
		bone = &bones[obj->bone_index];

		if (frac)
		{
			InitInterpolate(frac, rate);
			phd_TranslateRel_ID(frm[0][6], frm[0][7], frm[0][8], frm[1][6], frm[1][7], frm[1][8]);
			rot = frm[0] + 9;
			rot2 = frm[1] + 9;
			gar_RotYXZsuperpack_I(&rot, &rot2, 0);

			if (item->mesh_bits & 1)
			{
				if (item->meshswap_meshbits & 1)
					phd_PutPolygons_I(meshpp[1], clip);
				else
					phd_PutPolygons_I(meshpp[0], clip);
			}

			meshpp += 2;

			for (int i = 0; i < obj->nmeshes - 1; i++, bone += 4, meshpp += 2)
			{
				if (bone[0] & 1)
					phd_PopMatrix_I();

				if (bone[0] & 2)
					phd_PushMatrix_I();

				phd_TranslateRel_I(bone[1], bone[2], bone[3]);
				gar_RotYXZsuperpack_I(&rot, &rot2, 0);

				if (bone[0] & 0x1C)
				{
					if (bone[0] & 8)
						phd_RotY_I(*data++);

					if (bone[0] & 4)
						phd_RotX_I(*data++);

					if (bone[0] & 16)
						phd_RotZ_I(*data++);
				}

				bit <<= 1;

				if (bit & item->mesh_bits)
				{
					if (bit & item->meshswap_meshbits)
						phd_PutPolygons_I(meshpp[1], clip);
					else
						phd_PutPolygons_I(meshpp[0], clip);
				}

				if (item->fired_weapon && i == bite->mesh_num - 1)
				{
					rnd = GetRandomDraw();
					phd_PushMatrix_I();
					phd_TranslateRel_I(bite->x, bite->y, bite->z);
					phd_RotYXZ_I(0, -0x3FFC, short((rnd << 14) + (rnd >> 2) - 4096));
					InterpolateMatrix();
					phd_PutPolygons(meshes[objects[GUN_FLASH].mesh_index], clip);
					phd_PopMatrix_I();
					item->fired_weapon--;
				}
			}
		}
		else
		{
			phd_TranslateRel(frm[0][6], frm[0][7], frm[0][8]);
			rot = frm[0] + 9;
			gar_RotYXZsuperpack(&rot, 0);

			if (item->mesh_bits & 1)
			{
				if (item->meshswap_meshbits & 1)
					phd_PutPolygons(meshpp[1], clip);
				else
					phd_PutPolygons(meshpp[0], clip);
			}

			meshpp += 2;

			for (int i = 0; i < obj->nmeshes - 1; i++, bone += 4, meshpp += 2)
			{
				if (bone[0] & 1)
					phd_PopMatrix();

				if (bone[0] & 2)
					phd_PushMatrix();

				phd_TranslateRel(bone[1], bone[2], bone[3]);
				gar_RotYXZsuperpack(&rot, 0);

				if (bone[0] & 0x1C)
				{
					if (bone[0] & 8)
						phd_RotY(*data++);

					if (bone[0] & 4)
						phd_RotX(*data++);

					if (bone[0] & 16)
						phd_RotZ(*data++);
				}

				bit <<= 1;

				if (bit & item->mesh_bits)
				{
					if (bit & item->meshswap_meshbits)
						phd_PutPolygons(meshpp[1], clip);
					else
						phd_PutPolygons(meshpp[0], clip);
				}

				if (item->fired_weapon && i == bite->mesh_num - 1)
				{
					phd_PushMatrix();
					phd_RotX(-16380);
					phd_TranslateRel(bite->x, bite->y, bite->z);
					phd_PutPolygons(meshes[objects[GUN_FLASH].mesh_index], clip);
					phd_PopMatrix();
					item->fired_weapon--;
				}
			}
		}
	}

	phd_left = 0;
	phd_top = 0;
	phd_right = phd_winwidth;
	phd_bottom = phd_winheight;
	phd_PopMatrix();
}

void DrawRooms(short CurrentRoom)
{
	ROOM_INFO* r;
	long lx, ly, lz;
	short lr;

	current_room = CurrentRoom;
	r = &room[CurrentRoom];
	r->test_left = 0;
	r->test_top = 0;
	phd_left = 0;
	phd_top = 0;
	phd_right = phd_winxmax;
	phd_bottom = phd_winymax;
	r->test_right = (short)phd_winxmax;
	r->test_bottom = (short)phd_winymax;
	outside = r->flags & 8;
	camera_underwater = r->flags & 1;
	r->bound_active = 2;
	draw_room_list[0] = CurrentRoom;
	room_list_start = 0;
	room_list_end = 1;
	number_draw_rooms = 0;

	if (outside)
	{
		outside_top = 0;
		outside_left = 0;
		outside_right = phd_winxmax;
		outside_bottom = phd_winymax;
	}
	else
	{
		outside_left = phd_winxmax;
		outside_top = phd_winymax;
		outside_bottom = 0;
		outside_right = 0;
	}

	GetRoomBounds();
	InitialiseFogBulbs();
	CreateFXBulbs();

	if (outside)	//inlined SkyDrawPhase? did it exist?
	{
		if (!objects[HORIZON].loaded)
			outside = -1;
		else
		{
			if (BinocularRange)
				AlterFOV(14560 - (short)BinocularRange);

			phd_PushMatrix();
			phd_TranslateAbs(camera.pos.x, camera.pos.y, camera.pos.z);

			if (gfLevelFlags & GF_LIGHTNING)
			{
				if (!LightningCount && !LightningRand)
				{
					if (!(GetRandomDraw() & 127))
					{
						LightningCount = (GetRandomDraw() & 0x1F) + 16;
						dLightningRand = (GetRandomDraw() & 0xFF) + 256;
						LightningSFXDelay = (GetRandomDraw() & 3) + 12;
					}
				}
				else
				{
					UpdateSkyLightning();

					if (LightningSFXDelay > -1)
						LightningSFXDelay--;

					if (!LightningSFXDelay)
						SoundEffect(SFX_THUNDER_RUMBLE, 0, SFX_DEFAULT);
				}
			}

			nPolyType = 6;
			phd_PushMatrix();

			if (gfLevelFlags & GF_LAYER1)
			{
				phd_RotY(32760);

				if (gfLevelFlags & GF_LIGHTNING)
					DrawFlatSky(RGBA(LightningRGB[0], LightningRGB[1], LightningRGB[2], 44), SkyPos, -1536, 4);
				else
					DrawFlatSky(*(ulong*)&gfLayer1Col, SkyPos, -1536, 4);
			}

			if (gfLevelFlags & GF_LAYER2)
				DrawFlatSky(0xFF000000 | *(ulong*)&gfLayer2Col, SkyPos2, -1536, 2);

			if (gfLevelFlags & GF_LAYER1 || gfLevelFlags & GF_LAYER2)
				OutputSky();

			phd_PopMatrix();

			if (gfLevelFlags & GF_HORIZON)
			{
				phd_PutPolygonSkyMesh(meshes[objects[HORIZON].mesh_index], -1);
				OutputSky();
			}

			phd_PopMatrix();

			if (BinocularRange)
				AlterFOV(7 * (2080 - (short)BinocularRange));
		}
	}

	if (objects[LARA].loaded)
	{
		if (!(lara_item->flags & IFL_INVISIBLE))
		{
			nPolyType = 4;

			if (lara_item->mesh_bits)
			{
				DrawLara(lara_item, 0);
				phd_PushMatrix();

				if (lara.right_arm.flash_gun)
				{
					phd_mxptr[M00] = lara_matrices[132 + M00];
					phd_mxptr[M01] = lara_matrices[132 + M01];
					phd_mxptr[M02] = lara_matrices[132 + M02];
					phd_mxptr[M03] = lara_matrices[132 + M03];
					phd_mxptr[M10] = lara_matrices[132 + M10];
					phd_mxptr[M11] = lara_matrices[132 + M11];
					phd_mxptr[M12] = lara_matrices[132 + M12];
					phd_mxptr[M13] = lara_matrices[132 + M13];
					phd_mxptr[M20] = lara_matrices[132 + M20];
					phd_mxptr[M21] = lara_matrices[132 + M21];
					phd_mxptr[M22] = lara_matrices[132 + M22];
					phd_mxptr[M23] = lara_matrices[132 + M23];
					SetGunFlash(lara.gun_type);
				}

				if (lara.left_arm.flash_gun)
				{
					phd_mxptr[M00] = lara_matrices[168 + M00];
					phd_mxptr[M01] = lara_matrices[168 + M01];
					phd_mxptr[M02] = lara_matrices[168 + M02];
					phd_mxptr[M03] = lara_matrices[168 + M03];
					phd_mxptr[M10] = lara_matrices[168 + M10];
					phd_mxptr[M11] = lara_matrices[168 + M11];
					phd_mxptr[M12] = lara_matrices[168 + M12];
					phd_mxptr[M13] = lara_matrices[168 + M13];
					phd_mxptr[M20] = lara_matrices[168 + M20];
					phd_mxptr[M21] = lara_matrices[168 + M21];
					phd_mxptr[M22] = lara_matrices[168 + M22];
					phd_mxptr[M23] = lara_matrices[168 + M23];
					SetGunFlash(lara.gun_type);
				}

				phd_PopMatrix();
				DrawGunflashes();
			}

			if (gfLevelFlags & GF_MIRROR && lara_item->room_number == gfMirrorRoom)
				Draw_Mirror_Lara();
		}
	}

	InitDynamicLighting();
	nPolyType = 0;

	for (int i = 0; i < 32; i++)
	{
		if (dynamics[i].on)
		{
			if (dynamics[i].x < 0)
				dynamics[i].x = 0;

			if (dynamics[i].z < 0)
				dynamics[i].z = 0;
		}
	}

	for (int i = 0; i < number_draw_rooms; i++)
		S_InsertRoom(draw_rooms[i]);

	if (gfLevelFlags & GF_TRAIN)
		DrawTrainFloor();

	DrawGunshells();
	nPolyType = 3;

	if (GLOBAL_playing_cutseq)
		DrawCutSeqActors();

	DrawRopeList();
	S_DrawSparks();
	lx = lara_item->pos.x_pos;
	ly = lara_item->pos.y_pos;
	lz = lara_item->pos.z_pos;
	lr = lara_item->room_number;
	lara_item->pos.x_pos = camera.pos.x;
	lara_item->pos.y_pos = camera.pos.y;
	lara_item->pos.z_pos = camera.pos.z;
	lara_item->room_number = camera.pos.room_number;
	DoUwEffect();
	S_DrawFires();
	S_DrawSmokeSparks();
	S_DrawSplashes();
	DrawBubbles();
	DrawDebris();
	DrawBlood();
	DrawDrips();
	DrawShockwaves();
	DrawScarabs();
	DrawLocusts();
	DrawLightning();
#ifdef FOOTPRINTS
	S_DrawFootPrints();
#endif
	lara_item->pos.x_pos = lx;
	lara_item->pos.y_pos = ly;
	lara_item->pos.z_pos = lz;
	lara_item->room_number = lr;

	if (gfLevelFlags & GF_LENSFLARE)
		SetUpLensFlare(gfLensFlare.x, gfLensFlare.y - 4096, gfLensFlare.z, 0);

	if (LaserSightActive)
		DrawLaserSightSprite();

	for (int i = 0; i < number_draw_rooms; i++)
		PrintObjects(draw_rooms[i]);
}

void RenderIt(short CurrentRoom)
{
	ROOM_INFO* r;

	current_room = CurrentRoom;
	r = &room[CurrentRoom];
	r->test_left = 0;
	r->test_top = 0;
	phd_left = 0;
	phd_top = 0;
	phd_right = phd_winxmax;
	phd_bottom = phd_winymax;
	r->test_right = (short)phd_winxmax;
	r->test_bottom = (short)phd_winymax;
	outside = r->flags & ROOM_OUTSIDE;
	camera_underwater = r->flags & ROOM_UNDERWATER;
	r->bound_active = 2;
	draw_room_list[0] = CurrentRoom;
	room_list_start = 0;
	room_list_end = 1;
	number_draw_rooms = 0;

	if (outside)
	{
		outside_top = 0;
		outside_left = 0;
		outside_right = phd_winxmax;
		outside_bottom = phd_winymax;
	}
	else
	{
		outside_left = phd_winxmax;
		outside_top = phd_winymax;
		outside_bottom = 0;
		outside_right = 0;
	}

	GetRoomBounds();
	InitialiseFogBulbs();
	CreateFXBulbs();

	if (outside)
	{
		if (!objects[HORIZON].loaded)
			outside = -1;
		else
		{
			if (BinocularRange)
				AlterFOV(14560 - (short)BinocularRange);

			phd_PushMatrix();
			phd_TranslateAbs(camera.pos.x, camera.pos.y, camera.pos.z);
			nPolyType = 6;
			phd_PushMatrix();

			if (gfLevelFlags & GF_LAYER1)
			{
				phd_RotY(32760);

				if (gfLevelFlags & GF_LIGHTNING)
					DrawFlatSky(RGBA(LightningRGB[0], LightningRGB[1], LightningRGB[2], 44), SkyPos, -1536, 4);
				else
					DrawFlatSky(*(ulong*)&gfLayer1Col, SkyPos, -1536, 4);
			}

			if (gfLevelFlags & GF_LAYER2)
				DrawFlatSky(0xFF000000 | *(ulong*)&gfLayer2Col, SkyPos2, -1536, 2);

			if (gfLevelFlags & GF_LAYER1 || gfLevelFlags & GF_LAYER2)
				OutputSky();

			phd_PopMatrix();

			if (gfLevelFlags & GF_HORIZON)
			{
				phd_PutPolygonSkyMesh(meshes[objects[HORIZON].mesh_index], -1);
				OutputSky();
			}

			phd_PopMatrix();
		}
	}

	InitDynamicLighting();
	nPolyType = 0;

	for (int i = 0; i < number_draw_rooms; i++)
		S_InsertRoom(draw_rooms[i]);

	if (gfLevelFlags & GF_TRAIN)
		DrawTrainFloor();

	for (int i = 0; i < number_draw_rooms; i++)
		PrintObjects(draw_rooms[i]);
}

void inject_draw(bool replace)
{
	INJECT(0x00450520, InitInterpolate, replace);
	INJECT(0x00450560, phd_PopMatrix_I, replace);
	INJECT(0x00450590, phd_PushMatrix_I, replace);
	INJECT(0x004505C0, phd_RotY_I, replace);
	INJECT(0x004505F0, phd_RotX_I, replace);
	INJECT(0x00450620, phd_RotZ_I, replace);
	INJECT(0x00450650, phd_TranslateRel_I, replace);
	INJECT(0x00450690, phd_TranslateRel_ID, replace);
	INJECT(0x004506E0, phd_RotYXZ_I, replace);
	INJECT(0x00450720, gar_RotYXZsuperpack_I, replace);
	INJECT(0x00450760, gar_RotYXZsuperpack, replace);
	INJECT(0x00450810, phd_PutPolygons_I, replace);
	INJECT(0x00450840, InterpolateMatrix, replace);
	INJECT(0x00450AB0, InterpolateArmMatrix, replace);
	INJECT(0x0044F2D0, S_InsertRoom, replace);
	INJECT(0x00450BB0, CalculateObjectLighting, replace);
	INJECT(0x00450CB0, CalculateObjectLightingLara, replace);
	INJECT(0x0044FF60, DrawAnimatingItem, replace);
	INJECT(0x0044EC10, DrawRooms, replace);
	INJECT(0x00451240, RenderIt, replace);
}
