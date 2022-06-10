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
#include "health.h"
#include "items.h"
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

	if (*prot >> W2V_SHIFT)
	{
		if ((*prot >> W2V_SHIFT) == 1)
			phd_RotX((short)((*prot & 4095) << 4));
		else if ((*prot >> W2V_SHIFT) == 2)
			phd_RotY((short)((*prot & 4095) << 4));
		else
			phd_RotZ((short)((*prot & 4095) << 4));
	}
	else
	{
		phd_RotYXZpack(((ushort) * *pprot << (W2V_SHIFT + 2)) + (ushort)(*pprot)[1]);
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
#ifdef GENERAL_FIXES
		phd_mxptr[M00] += (IMptr[M00] - phd_mxptr[M00]) >> 1;
		phd_mxptr[M01] += (IMptr[M01] - phd_mxptr[M01]) >> 1;
		phd_mxptr[M02] += (IMptr[M02] - phd_mxptr[M02]) >> 1;
		phd_mxptr[M03] += (IMptr[M03] - phd_mxptr[M03]) >> 1;
		phd_mxptr[M10] += (IMptr[M10] - phd_mxptr[M10]) >> 1;
		phd_mxptr[M11] += (IMptr[M11] - phd_mxptr[M11]) >> 1;
		phd_mxptr[M12] += (IMptr[M12] - phd_mxptr[M12]) >> 1;
		phd_mxptr[M13] += (IMptr[M13] - phd_mxptr[M13]) >> 1;
		phd_mxptr[M20] += (IMptr[M20] - phd_mxptr[M20]) >> 1;
		phd_mxptr[M21] += (IMptr[M21] - phd_mxptr[M21]) >> 1;
		phd_mxptr[M22] += (IMptr[M22] - phd_mxptr[M22]) >> 1;
		phd_mxptr[M23] += (IMptr[M23] - phd_mxptr[M23]) >> 1;
#else
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
#endif
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
					//empty func call here
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
					//empty func call here
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

#ifdef GENERAL_FIXES
static void DoMirrorStuff()
{
	LARA_ARM larm;
	LARA_ARM rarm;
	short old_anim, old_frame;

	larm = lara.left_arm;
	rarm = lara.right_arm;
	old_anim = lara_item->anim_number;
	old_frame = lara_item->frame_number;

	if (BinocularRange)
	{
		if (LaserSight)
		{
			if (lara.gun_type == WEAPON_REVOLVER)
			{
				lara.left_arm.anim_number = objects[SIXSHOOTER_ANIM].anim_index + 3;
				lara.right_arm.anim_number = objects[SIXSHOOTER_ANIM].anim_index + 3;
				lara.left_arm.frame_number = anims[lara.left_arm.anim_number].frame_base;
				lara.right_arm.frame_number = anims[lara.right_arm.anim_number].frame_base;
			}

			if (lara.gun_type == WEAPON_CROSSBOW)
			{
				lara.left_arm.anim_number = objects[CROSSBOW_ANIM].anim_index + 2;			
				lara.right_arm.anim_number = objects[CROSSBOW_ANIM].anim_index + 2;
				lara.left_arm.frame_number = 0;
				lara.right_arm.frame_number = 0;
			}

			lara.left_arm.frame_base = anims[lara.left_arm.anim_number].frame_ptr;
			lara.right_arm.frame_base = anims[lara.right_arm.anim_number].frame_ptr;
		}
		else
		{
			lara_item->anim_number = ANIM_BINOCS;
			lara_item->frame_number = anims[ANIM_BINOCS].frame_base;
			lara.mesh_ptrs[LM_RHAND] = meshes[objects[MESHSWAP2].mesh_index + 2 * LM_RHAND];
		}
	}
	
	Draw_Mirror_Lara();

	if (BinocularRange)
	{
		lara.left_arm = larm;
		lara.right_arm = rarm;

		if (!LaserSight)
		{
			lara_item->anim_number = old_anim;
			lara_item->frame_number = old_frame;
			lara.mesh_ptrs[LM_RHAND] = meshes[objects[LARA_SKIN].mesh_index + 2 * LM_RHAND];
		}
	}
}
#endif

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
	r->test_right = phd_winxmax;
	r->test_bottom = phd_winymax;
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
#ifdef GENERAL_FIXES
				DoMirrorStuff();
#else
				Draw_Mirror_Lara();
#endif
		}
	}

	InitDynamicLighting();
	nPolyType = 0;

	for (int i = 0; i < MAX_DYNAMICS; i++)
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
	r->test_right = phd_winxmax;
	r->test_bottom = phd_winymax;
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

long DrawPhaseGame()
{
	CalcLaraMatrices(0);
	phd_PushUnitMatrix();
	CalcLaraMatrices(1);
	phd_PopMatrix();

	if (GLOBAL_playing_cutseq)
		frigup_lara();

	SetLaraUnderwaterNodes();
	DrawRooms(camera.pos.room_number);
	DrawGameInfo(1);
	S_OutputPolyList();
	camera.number_frames = S_DumpScreen();
	S_AnimateTextures(camera.number_frames);
	return camera.number_frames;
}

void GetRoomBounds()
{
	ROOM_INFO* r;
	short* door;
	long rn, drn;

	while (room_list_start != room_list_end)
	{
		rn = draw_room_list[room_list_start % 128];
		room_list_start++;
		r = &room[rn];
		r->bound_active -= 2;

		if (r->test_left < r->left)
			r->left = r->test_left;

		if (r->test_top < r->top)
			r->top = r->test_top;

		if (r->test_right > r->right)
			r->right = r->test_right;

		if (r->test_bottom > r->bottom)
			r->bottom = r->test_bottom;

		if (!(r->bound_active & 1))
		{
			draw_rooms[number_draw_rooms] = (short)rn;
			number_draw_rooms++;
			r->bound_active |= 1;

			if (r->flags & ROOM_OUTSIDE)
				outside = ROOM_OUTSIDE;
		}

		if (r->flags & ROOM_OUTSIDE)
		{
			if (r->left < outside_left)
				outside_left = r->left;

			if (r->right > outside_right)
				outside_right = r->right;

			if (r->top < outside_top)
				outside_top = r->top;

			if (r->bottom > outside_bottom)
				outside_bottom = r->bottom;
		}

		phd_PushMatrix();
		phd_TranslateAbs(r->x, r->y, r->z);
		door = r->door;

		if (door)
		{
			for (drn = *door++; drn > 0; drn--)
			{
				rn = *door++;

				if (door[0] * (r->x + door[3] - w2v_matrix[M03]) +
					door[1] * (r->y + door[4] - w2v_matrix[M13]) +
					door[2] * (r->z + door[5] - w2v_matrix[M23]) < 0)
					SetRoomBounds(door, rn, r);

				door += 15;
			}
		}

		phd_PopMatrix();
	}
}

void SetRoomBounds(short* door, long rn, ROOM_INFO* actualRoom)
{
	ROOM_INFO* r;
	PHD_VECTOR* v;
	PHD_VECTOR* lastV;
	static PHD_VECTOR vbuf[4];
	long x, y, z, tooNear, tooFar, tL, tR, tT, tB;

	r = &room[rn];

	if (r->left <= actualRoom->test_left && r->right >= actualRoom->test_right && r->top <= actualRoom->test_top && r->bottom >= actualRoom->test_bottom)
		return;
	
	tL = actualRoom->test_right;
	tR = actualRoom->test_left;
	tB = actualRoom->test_top;
	tT = actualRoom->test_bottom;
	door += 3;
	v = vbuf;
	tooNear = 0;
	tooFar = 0;

	for (int i = 0; i < 4; i++, v++, door += 3)
	{
		v->x = phd_mxptr[M00] * door[0] + phd_mxptr[M01] * door[1] + phd_mxptr[M02] * door[2] + phd_mxptr[M03];
		v->y = phd_mxptr[M10] * door[0] + phd_mxptr[M11] * door[1] + phd_mxptr[M12] * door[2] + phd_mxptr[M13];
		v->z = phd_mxptr[M20] * door[0] + phd_mxptr[M21] * door[1] + phd_mxptr[M22] * door[2] + phd_mxptr[M23];
		x = v->x;
		y = v->y;
		z = v->z;

		if (z <= 0)
			tooNear++;
		else
		{
			if (z > phd_zfar)
				tooFar++;

			z /= phd_persp;

			if (z)
			{
				x = x / z + phd_centerx;
				y = y / z + phd_centery;
			}
			else
			{
				if (x < 0)
					x = phd_left;
				else
					x = phd_right;

				if (y < 0)
					y = phd_top;
				else
					y = phd_bottom;
			}

			if (x - 1 < tL)
				tL = x - 1;

			if (x + 1 > tR)
				tR = x + 1;

			if (y - 1 < tT)
				tT = y - 1;

			if (y + 1 > tB)
				tB = y + 1;
		}
	}

	if (tooNear == 4 || (tooFar == 4 && !outside))
		return;

	if (tooNear > 0)
	{
		v = vbuf;
		lastV = &vbuf[3];

		for (int i = 0; i < 4; i++, lastV = v, v++)
		{
			if (lastV->z <= 0 == v->z <= 0)
				continue;

			if (v->x < 0 && lastV->x < 0)
				tL = 0;
			else if (v->x > 0 && lastV->x > 0)
				tR = phd_winxmax;
			else
			{
				tL = 0;
				tR = phd_winxmax;
			}

			if (v->y < 0 && lastV->y < 0)
				tT = 0;
			else if (v->y > 0 && lastV->y > 0)
				tB = phd_winymax;
			else
			{
				tT = 0;
				tB = phd_winymax;
			}
		}
	}

	if (tL < actualRoom->test_left)
		tL = actualRoom->test_left;

	if (tR > actualRoom->test_right)
		tR = actualRoom->test_right;

	if (tT < actualRoom->test_top)
		tT = actualRoom->test_top;

	if (tB > actualRoom->test_bottom)
		tB = actualRoom->test_bottom;

	if (tL >= tR || tT >= tB)
		return;

	if (r->bound_active & 2)
	{
		if (tL < r->test_left)
			r->test_left = (short)tL;

		if (tT < r->test_top)
			r->test_top = (short)tT;

		if (tR > r->test_right)
			r->test_right = (short)tR;

		if (tB > r->test_bottom)
			r->test_bottom = (short)tB;
	}
	else
	{
		draw_room_list[room_list_end % 128] = rn;
		room_list_end++;
		r->bound_active |= 2;
		r->test_left = (short)tL;
		r->test_right = (short)tR;
		r->test_top = (short)tT;
		r->test_bottom = (short)tB;
	}
}

void DrawEffect(short fx_num)
{
	FX_INFO* fx;
	OBJECT_INFO* obj;
	short* meshp;

	fx = &effects[fx_num];
	obj = &objects[fx->object_number];

	if (obj->draw_routine && obj->loaded)
	{
		phd_PushMatrix();
		phd_TranslateAbs(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);

		if (phd_mxptr[M23] > phd_znear && phd_mxptr[M23] < phd_zfar)
		{
			phd_RotYXZ(fx->pos.y_rot, fx->pos.x_rot, fx->pos.z_rot);
			
			if (obj->nmeshes)
				meshp = meshes[obj->mesh_index];
			else
				meshp = meshes[fx->frame_number];

			//empty func call here
			phd_PutPolygons(meshp, -1);
		}

		phd_PopMatrix();
	}
}

void PrintObjects(short room_number)
{
	ROOM_INFO* r;
	MESH_INFO* mesh;
	STATIC_INFO* sinfo;
	ITEM_INFO* item;
	OBJECT_INFO* obj;
	FX_INFO* fx;
	long clip;
	short item_number, fx_number;

	current_room = room_number;
	nPolyType = 1;
	r = &room[room_number];
	r->bound_active = 0;
	phd_PushMatrix();
	phd_TranslateAbs(r->x, r->y, r->z);

	if (gfLevelFlags & GF_TRAIN)
	{
		phd_left = 0;
		phd_top = 0;
		phd_right = phd_winxmax + 1;
		phd_bottom = phd_winymax + 1;
	}
	else
	{
		phd_left = r->left;
		phd_right = r->right;
		phd_top = r->top;
		phd_bottom = r->bottom;
	}

	mesh = r->mesh;

	for (int i = r->num_meshes; i > 0; i--, mesh++)
	{
		if (mesh->Flags & 1)
		{
			phd_PushMatrix();
			phd_TranslateAbs(mesh->x, mesh->y, mesh->z);
			phd_RotY(mesh->y_rot);
			sinfo = &static_objects[mesh->static_number];
			clip = S_GetObjectBounds(&sinfo->x_minp);

			if (clip)
			{
				S_CalculateStaticMeshLight(mesh->x, mesh->y, mesh->z, mesh->shade, r);
				phd_PutPolygons(meshes[sinfo->mesh_number], clip);
			}

			phd_PopMatrix();
		}
	}

	nPolyType = 2;
	phd_left = 0;
	phd_top = 0;
	phd_right = phd_winxmax + 1;
	phd_bottom = phd_winymax + 1;

	for (item_number = r->item_number; item_number != NO_ITEM; item_number = item->next_item)
	{
		ClipRoomNum = room_number;
		item = &items[item_number];
		obj = &objects[item->object_number];

		if (item->status != ITEM_INVISIBLE)
		{
			if (item->after_death)
				GlobalAlpha = 0xFE000000 * item->after_death;	//mmmm

			if (obj->draw_routine)
				obj->draw_routine(item);

			if (obj->draw_routine_extra)
				obj->draw_routine_extra(item);

			GlobalAlpha = 0xFF000000;
		}

		if (item->after_death < 128 && item->after_death > 0)
			item->after_death++;

		if (item->after_death == 128)
			KillItem(item_number);
	}

	nPolyType = 3;

	for (fx_number = r->fx_number; fx_number != NO_ITEM; fx_number = fx->next_fx)
	{
		fx = &effects[fx_number];
		DrawEffect(fx_number);
	}

	phd_PopMatrix();
	r->left = phd_winxmax;
	r->top = phd_winymax;
	r->right = 0;
	r->bottom = 0;
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
	INJECT(0x0044EBA0, DrawPhaseGame, replace);
	INJECT(0x0044F5D0, GetRoomBounds, replace);
	INJECT(0x0044F790, SetRoomBounds, replace);
	INJECT(0x0044FB10, DrawEffect, replace);
	INJECT(0x0044F330, PrintObjects, replace);
}
