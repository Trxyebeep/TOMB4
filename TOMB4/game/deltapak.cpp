#include "../tomb4/pch.h"
#include "deltapak.h"
#include "tomb4fx.h"
#include "lara_states.h"
#include "items.h"
#include "../specific/audio.h"
#include "../specific/3dmath.h"
#include "control.h"
#include "hair.h"
#include "delstuff.h"
#include "../specific/function_stubs.h"
#include "draw.h"
#include "../specific/output.h"
#include "../specific/lighting.h"
#include "objects.h"
#include "lara2gun.h"
#include "effect2.h"
#include "effects.h"
#include "lot.h"
#include "spotcam.h"
#include "camera.h"
#include "../specific/winmain.h"
#include "lara.h"
#include "gameflow.h"
#ifdef TIMES_LEVEL
#include "../specific/function_table.h"
#endif

static short frig_shadow_bbox[6] = { -165, 150, -777, 1, -87, 78 };
static short frig_jeep_shadow_bbox[6] = { -600, 600, -777, 1, -600, 600 };
static short troop_chat_ranges4[14] = { 485, 587, 558, 625, 639, 652, 661, 721, 729, 793, 805, 872, -1, -1 };
static short lara_chat_ranges6[4] = { 257, 345, -1, -1 };
static short voncroy_chat_ranges6[4] = { 6, 209, -1, -1 };
static short lara_chat_ranges7[4] = { 675, 877, -1, -1 };
static short voncroy_chat_ranges7[4] = { 396, 654, -1, -1 };
static short lara_chat_ranges8[4] = { 592, 688, -1, -1 };
static short voncroy_chat_ranges8[6] = { 160, 273, 323, 446, -1, -1 };
static short lara_chat_ranges9[18] = { 149, 276, 297, 348, 844, 957, 1180, 1217, 1249, 1277, 2231, 2251, 2714, 2786, 3019, 3037, -1, -1 };
static short voncroy_chat_ranges9[22] = { 12, 24, 30, 145, 361, 528, 539, 556, 564, 577, 581, 765, 781, 839, 985, 1136, 1921, 2084, 2464, 2645, -1, -1 };
static short lara_chat_ranges10[18] = { 283, 296, 305, 603, 1244, 1312, 1759, 1795, 1806, 1848, 1878, 1912, 1925, 1947, 1960, 2000, -1, -1 };
static short voncroy_chat_ranges10[16] = { 963, 1056, 1069, 1137, 1153, 1204, 1339, 1739, 2027, 2193, 2200, 2333, 2368, 2406, -1, -1 };
static short lara_chat_ranges11[12] = { 1216, 1232, 1241, 1323, 1337, 1393, 1678, 1706, 1927, 1954, -1, -1 };
static short captain_chat_ranges11[12] = { 1141, 1181, 1509, 1666, 1758, 1914, 1964, 2016, 2194, 2261, -1, -1 };
static short lara_chat_ranges15[10] = { 684, 715, 1314, 1372, 1392, 1417, 1792, 1824, -1, -1 };
static short voncroy_chat_ranges15[10] = { 290, 317, 330, 375, 388, 673, 729, 1301, -1, -1 };
static short lara_chat_ranges16[10] = { 386, 419, 430, 489, 1021, 1058, 1072, 1175, -1, -1 };
static short jeanyves_chat_ranges16[18] = { 97, 136, 174, 197, 214, 308, 329, 372, 511, 672, 684, 781, 812, 845, 863, 988, -1, -1 };
static short voncroy_chat_ranges16[10] = { 1614, 1641, 1657, 1726, 1741, 1760, 1775, 1974, -1, -1 };
static short jeanyves_chat_ranges17[24] = { 45, 86, 122, 196, 258, 370, 398, 445, 467, 490, 518, 537, 547, 564, 579, 619, 641, 650, 668, 714, 733, 788, -1, -1 };
static short jeanyves_chat_ranges18[14] = { 47, 59, 75, 113, 123, 250, 331, 419, 437, 570, 592, 652, -1, -1 };
static short jeanyves_chat_ranges19[20] = { 224, 251, 288, 304, 328, 472, 502, 794, 1028, 1042, 1067, 1182, 1206, 1294, 1330, 1386, 1402, 1427, -1, -1 };
static short jeanyves_chat_ranges20[28] = { 150, 158, 174, 187, 208, 312, 334, 418, 437, 457, 479, 525, 547, 623, 640, 662, 751, 811, 828, 859, 879, 894, 908, 942, 955, 962, -1, -1 };
static short lara_chat_ranges24[16] = { 91, 122, 139, 172, 504, 552, 565, 637, 648, 678, 1596, 1616, 1774, 1821, -1, -1 };
static short troop_chat_ranges24[16] = { 209, 490, 701, 975, 1146, 1583, 1654, 1768, 1839, 1879, 1893, 1930, 1940, 2160, -1, -1 };
static short lara_chat_ranges25[14] = { 476, 578, 976, 1018, 1028, 1069, 1317, 1340, 2065, 2178, 2835, 2865, -1, -1 };
static short troop_chat_ranges25[26] = { 289, 363, 378, 449, 596, 604, 635, 744, 764, 780, 811, 959, 1088, 1157, 1191, 1313, 1360, 2046, 2227, 2295, 2330, 2821, 2877, 3322, -1, -1 };
static short troop_chat_ranges26[4] = { 108, 432, -1, -1 };
#ifdef TIMES_LEVEL
static short lara_chat_ranges_times[28] = { 188, 271, 307, 330, 584, 638, 668, 700, 728, 755, 763, 819, 1181, 1238, 1263, 1290, 1363, 1411, 1926, 1934, 1981, 2011, 2017, 2045, 3030, 3095, -1, -1 };
static short editor_chat_ranges_times[40] =
{
	340, 424, 432, 462, 480, 518, 521, 552, 843, 907, 931, 969, 974, 1002, 1020, 1057, 1060, 1090, 1110, 1119,
	1123, 1172, 1299, 1352, 1417, 1423, 1442, 1513, 1544, 1627, 1634, 1691, 1710, 1770, 1775, 1843, 1864, 1902, -1, -1
};
#endif

static CUTSEQ_ROUTINES cutseq_control_routines[] =
{
	{0, 0, 0},
	{do_spade_meshswap, 0, 0},
	{0, 0, do_spade_meshswap},
	{0, third_cutseq_control, 0},
	{fourth_cutseq_init, fourth_cutseq_control, fourth_cutseq_end},
	{0, fifth_cutseq_control, fifth_cutseq_end},
	{sixth_cutseq_init, sixth_cutseq_control, sixth_cutseq_end},
	{seventh_cutseq_init, seventh_control, 0},
	{seventh_cutseq_init, eighth_control, 0},
	{ninth_cutseq_init, ninth_control, 0},
	{tenth_cutseq_init, tenth_cutseq_control, tenth_cutseq_end},
	{eleventh_cutseq_init, eleventh_cutseq_control, 0},
	{twelth_cutseq_init, 0, twelth_cutseq_end},
	{0, 0, thirteen_end},
	{0, fourteen_control, fourteen_end},
	{fifteen_init, fifteen_control, fifteen_end},
	{sixteen_init, sixteen_control, sixteen_end},
	{nineteen_init, seventeen_control, seventeen_end},
	{eighteen_init, eighteen_control, eighteen_end},
	{nineteen_init, nineteen_control, nineteen_end},
	{nineteen_init, twenty_control, twenty_end},
	{twentyone_init, 0, twentyone_end},
	{twentytwo_init, 0, twentytwo_end},
	{twentythree_init, twentythree_control, twentythree_end},
	{twentyfour_init, twentyfour_control, twentyfour_end},
	{twentyfive_init, twentyfive_control, twentyfive_end},
	{twentyfive_init, twentysix_control, twentyfive_end},
	{twentyseven_init, twentyseven_control, twentyseven_end},
	{special1_init, 0, special1_end},
	{special2_init, 0, special2_end},
	{special2_init, special3_control, special3_end},
#ifdef TIMES_LEVEL
	{times_init, times_control, 0}
#endif
};

long cutseq_trig = 0;
long cutseq_num = 0;
long GLOBAL_playing_cutseq = 0;
long GLOBAL_cutseq_frame;

static NEW_CUTSCENE* GLOBAL_cutme;
static ITEM_INFO* horus_item_thing;
static PACKNODE* camera_pnodes;
static PACKNODE* actor_pnodes[10];
static ITEM_INFO duff_item[10];
static char* GLOBAL_resident_depack_buffers;	//not really used
static camera_type GLOBAL_oldcamtype;
static ulong cutseq_meshbits[10];
static ulong cutseq_meshswapbits[10];
static long GLOBAL_numcutseq_frames;
static long lastcamnum;
static long numnailed;
static short old_lara_holster;
static short temp_rotation_buffer[160];
static char cutseq_busy_timeout = 0;
static char lara_chat_cnt = 0;
static char actor_chat_cnt = 0;
static char old_status_flags[16];

void handle_cutseq_triggering(long name)
{
	long n, goin, fuck;

	if (!cutseq_num)
		return;

	if (!cutseq_trig)
	{
		if ((lara.gun_status != LG_NO_ARMS || lara.gun_type == WEAPON_FLARE) && cutseq_num != 27 &&
			(lara.gun_type == WEAPON_FLARE || lara.gun_status != LG_HANDS_BUSY))
			lara.gun_status = LG_UNDRAW_GUNS;

		cutseq_trig = 1;
		memset(cutseq_meshswapbits, 0, sizeof(cutseq_meshswapbits));
		cutseq_busy_timeout = 50;
		memset(cutseq_meshbits, -1, sizeof(cutseq_meshbits));

		if (gfCurrentLevel)
			SetFadeClip(28, 1);

		if (!ScreenFadedOut)
			// if gfCurrentLevel -> empty func call here
			SetScreenFadeOut(16, 0);
	}
	else if (cutseq_trig == 1)
	{
		if (cutseq_num == 11)
			lara.vehicle = NO_ITEM;

		fuck = 0;
		n = lara_item->current_anim_state;

		if (ScreenFadedOut)
		{
			cutseq_busy_timeout--;

			if (!cutseq_busy_timeout)
			{
				cutseq_busy_timeout = 0;
				fuck = 1;
			}

			if (cutseq_num == 27 || lara.gun_status == LG_HANDS_BUSY || lara.gun_status == LG_NO_ARMS && !lara.flare_control_left ||
				n == AS_ALL4S || n == AS_CRAWL || n == AS_ALL4TURNL || n == AS_ALL4TURNR || n == AS_CRAWLBACK || fuck)
			{
				if (!(gfLevelFlags & GF_YOUNGLARA))
				{
					lara.gun_type = WEAPON_NONE;
					lara.request_gun_type = WEAPON_NONE;
					lara.gun_status = LG_NO_ARMS;
					lara.last_gun_type = WEAPON_PISTOLS;
					lara.mesh_ptrs[LM_LHAND] = meshes[objects[0].mesh_index + 2 * LM_LHAND];
					lara.mesh_ptrs[LM_RHAND] = meshes[objects[0].mesh_index + 2 * LM_RHAND];
					lara.left_arm.frame_number = 0;
					lara.right_arm.frame_number = 0;
					lara.target = 0;
					lara.right_arm.lock = 0;
					lara.left_arm.lock = 0;
					lara_item->goal_anim_state = AS_STOP;
					lara_item->current_anim_state = AS_STOP;
					lara_item->frame_number = anims[ANIM_STOP].frame_base;
					lara_item->anim_number = ANIM_STOP;
					lara_item->speed = 0;
					lara_item->fallspeed = 0;
					lara_item->gravity_status = 0;
					lara.back_gun = 0;

					if (lara.weapon_item != NO_ITEM)
					{
						KillItem(lara.weapon_item);
						lara.weapon_item = NO_ITEM;
					}
				}

				if (gfCurrentLevel)
					S_CDStop();

				goin = cutseq_num;
				numnailed = 0;
				GLOBAL_oldcamtype = camera.type;
				ScreenFading = 0;
				SetScreenFadeIn(16);
				Load_and_Init_Cutseq(goin);
				cutseq_trig = 2;
					
				if (cutseq_control_routines[goin].init_func)
					cutseq_control_routines[goin].init_func();

				AlterFOV(11488);

				if (GLOBAL_cutme->audio_track != -1)
					S_StartSyncedAudio(GLOBAL_cutme->audio_track);
			}
		}
	}
	else if (cutseq_trig == 3)
	{
		SetScreenFadeOut(16, 1);

	//	if (cutseq_num != 1 && gfCurrentLevel)
			//empty func call here

		cutseq_trig = 4;
	}
	else if (cutseq_trig == 4)
	{
		if (ScreenFadedOut)
		{
			if (gfCurrentLevel)
				S_CDStop();

			ScreenFadedOut = 0;
			numnailed = 0;
			fuck = cutseq_num;

			if (cutseq_control_routines[fuck].end_func)
				cutseq_control_routines[fuck].end_func();

			cutseq_trig = 0;
			GLOBAL_playing_cutseq = 0;

			if (fuck == 1)
			{
				FlipMap(0);
				cutseq_num = 2;
				Load_and_Init_Cutseq(2);
				cutseq_trig = 2;
				lara.puzzleitems[0] = 0;
			}
			else if (bDoCredits)
			{
				switch (fuck)
				{
				case 28:
					cutseq_num = 29;
					break;

				case 29:
					cutseq_num = 30;
					break;

				case 30:
					cutseq_num = 28;
				}

				Load_and_Init_Cutseq(cutseq_num);
				cutseq_trig = 2;
			}
			else if (fuck == 7 || fuck == 8)
			{
				cutseq_num = 9;
				Load_and_Init_Cutseq(9);
				cutseq_trig = 2;

				if (cutseq_control_routines[cutseq_num].init_func)
					cutseq_control_routines[cutseq_num].init_func();

				if (GLOBAL_cutme->audio_track != -1)
					S_StartSyncedAudio(GLOBAL_cutme->audio_track);
			}
#ifdef TIMES_LEVEL
			else if (fuck == 31)
#else
			else if (fuck == 9 || fuck == 11 || fuck == 15 || fuck == 23)
#endif
			{
				gfLevelComplete = gfCurrentLevel + 1;
#ifdef TIMES_LEVEL
				skipped_level = 1;
#endif
				gfRequiredStartPos = 0;
				cutseq_num = 0;
				GLOBAL_playing_cutseq = 0;
				cutseq_trig = 0;
				AlterFOV(14560);
				ScreenFade = 0;
				dScreenFade = 0;
				ScreenFadeBack = 0;
				ScreenFadedOut = 0;
				ScreenFading = 0;
				ScreenFadeSpeed = 8;
			}
			else
			{
				finish_cutseq();
				camera.type = GLOBAL_oldcamtype;

				cutseq_num = 0;

				if (gfCurrentLevel)
					SetFadeClip(0, 1);

				AlterFOV(14560);

				if (gfCurrentLevel)
					S_CDPlay(CurrentAtmosphere, 1);

				IsAtmospherePlaying = 1;
			}
		}
	}
}

void do_new_cutscene_camera()
{
	if (cutseq_control_routines[cutseq_num].control_func)
		cutseq_control_routines[cutseq_num].control_func();

	DecodeAnim(camera_pnodes, 2, GLOBAL_cutseq_frame, 0xFFFF);
	camera.target.x = GLOBAL_cutme->orgx + 2 * camera_pnodes->xrot_run;
	camera.target.y = GLOBAL_cutme->orgy + 2 * camera_pnodes->yrot_run;
	camera.target.z = GLOBAL_cutme->orgz + 2 * camera_pnodes->zrot_run;
	camera.pos.x = GLOBAL_cutme->orgx + 2 * camera_pnodes[1].xrot_run;
	camera.pos.y = GLOBAL_cutme->orgy + 2 * camera_pnodes[1].yrot_run;
	camera.pos.z = GLOBAL_cutme->orgz + 2 * camera_pnodes[1].zrot_run;
	IsRoomOutsideNo = -1;
	IsRoomOutside(camera.pos.x, camera.pos.y, camera.pos.z);

	if (IsRoomOutsideNo != -1)
		camera.pos.room_number = IsRoomOutsideNo;

	phd_LookAt(camera.pos.x, camera.pos.y, camera.pos.z, camera.target.x, camera.target.y, camera.target.z, 0);
	DecodeAnim(actor_pnodes[0], 16, GLOBAL_cutseq_frame, 1023);

	for (int i = 1; i < GLOBAL_cutme->numactors; i++)
		DecodeAnim(actor_pnodes[i], GLOBAL_cutme->actor_data[i].nodes + 1, GLOBAL_cutseq_frame, 1023);

	GLOBAL_cutseq_frame++;

	if (GLOBAL_cutseq_frame > GLOBAL_numcutseq_frames - 8 && cutseq_trig == 2)
		cutseq_trig = 3;

	if (GLOBAL_cutseq_frame > GLOBAL_numcutseq_frames)
		GLOBAL_cutseq_frame = GLOBAL_numcutseq_frames;
}

void InitPackNodes(NODELOADHEADER* lnode, PACKNODE* pnode, char* packed, long numnodes)
{
	long offset, xoff, yoff, zoff;

	offset = ((numnodes << 3) - numnodes) << 1;

	for (int i = 0; i < numnodes; i++)
	{
		pnode->xkey = (ushort)lnode->xkey;
		pnode->ykey = (ushort)lnode->ykey;
		pnode->zkey = (ushort)lnode->zkey;
		pnode->decode_x.packmethod = (lnode->packmethod >> 10) & 0xF;
		pnode->decode_y.packmethod = (lnode->packmethod >> 5) & 0xF;
		pnode->decode_z.packmethod = (lnode->packmethod) & 0xF;
		pnode->xlength = lnode->xlength;
		pnode->ylength = lnode->ylength;
		pnode->zlength = lnode->zlength;
		xoff = ((lnode->xlength * pnode->decode_x.packmethod) >> 3) + 4;
		yoff = ((lnode->ylength * pnode->decode_y.packmethod) >> 3) + 4;
		zoff = ((lnode->zlength * pnode->decode_z.packmethod) >> 3) + 4;
		lnode++;
		pnode->xpacked = &packed[offset];
		pnode->ypacked = &packed[offset + xoff];
		pnode->zpacked = &packed[offset + xoff + yoff];
		pnode++;
		offset += xoff + yoff + zoff;
	}
}

void DecodeAnim(PACKNODE* node, long num_nodes, long frame, long flags)
{
	if (frame)
	{
		node->xrot_run += DecodeTrack(node->xpacked, &node->decode_x);
		node->yrot_run += DecodeTrack(node->ypacked, &node->decode_y);
		node->zrot_run += DecodeTrack(node->zpacked, &node->decode_z);
		node++;

		for (int i = 1; i < num_nodes; i++, node++)
		{
			node->xrot_run += DecodeTrack(node->xpacked, &node->decode_x);
			node->yrot_run += DecodeTrack(node->ypacked, &node->decode_y);
			node->zrot_run += DecodeTrack(node->zpacked, &node->decode_z);
			node->xrot_run &= flags;
			node->yrot_run &= flags;
			node->zrot_run &= flags;
		}
	}
	else
	{
		for (int i = num_nodes; i; i--, node++)
		{
			node->decode_x.off = 0;
			node->decode_x.counter = 0;
			node->decode_x.data = 0;
			node->decode_x.decodetype = 0;
			node->decode_y.off = 0;
			node->decode_y.counter = 0;
			node->decode_y.data = 0;
			node->decode_y.decodetype = 0;
			node->decode_z.off = 0;
			node->decode_z.counter = 0;
			node->decode_z.data = 0;
			node->decode_z.decodetype = 0;
			node->xrot_run = node->xkey;
			node->yrot_run = node->ykey;
			node->zrot_run = node->zkey;
			node->decode_x.length = node->xlength;
			node->decode_y.length = node->ylength;
			node->decode_z.length = node->zlength;
		}
	}
}

short DecodeTrack(char* packed, RTDECODE* decode)
{
	short word;

	if (!decode->decodetype)
	{
		word = GetTrackWord(decode->off, packed, decode->packmethod);

		if (word & 0x20)
		{
			if (word & 0xF)
				decode->counter = word & 0xF;
			else
				decode->counter = 16;

			decode->decodetype = 1;
			decode->off++;
			decode->length--;
		}
		else
		{
			decode->decodetype = 2;

			if (word & 0x10)
			{
				GetTrackWord(decode->off + 1, packed, decode->packmethod);
				decode->counter = ((word & 7) << 5) | (GetTrackWord(decode->off + 1, packed, decode->packmethod) & 0x1F);
				decode->data = GetTrackWord(decode->off + 2, packed, decode->packmethod);
				decode->off += 3;
				decode->length -= 3;
			}
			else
			{
				decode->data = GetTrackWord(decode->off + 1, packed, decode->packmethod);
				decode->counter = word & 0x7;
				decode->off += 2;
				decode->length -= 2;
			}
		}
	}

	if (decode->decodetype == 2)
	{
		decode->counter--;

		if (!decode->counter)
			decode->decodetype = 0;

		return decode->data;
	}
	else
	{
		word = GetTrackWord(decode->off, packed, decode->packmethod);
		decode->off++;
		decode->length--;
		decode->counter--;

		if (!decode->counter)
			decode->decodetype = 0;

		return word;
	}
}

short GetTrackWord(long off, char* packed, long packmethod)
{
	long offset, offset2;
	short ret;

	offset = packmethod * off;
	offset2 = offset >> 3;

	ret = ((1 << packmethod) - 1) & ((ulong)(*(uchar*)(packed + offset2) |
		((*(uchar*)(packed + offset2 + 1) |
			(*(ushort*)(packed + offset2 + 2) << 8)) << 8)) >> (offset & 7));

	if (((1 << (packmethod - 1)) & ret) != 0)
		return (ulong)(ret | ~((1 << packmethod) - 1));

	return ret;
}

void frigup_lara()
{
	long* bone;

	lara_item->pos.x_pos = GLOBAL_cutme->orgx;
	lara_item->pos.y_pos = GLOBAL_cutme->orgy;
	lara_item->pos.z_pos = GLOBAL_cutme->orgz;
	bone = &bones[objects[lara_item->object_number].bone_index];
	updateAnimFrame(actor_pnodes[0], 16, temp_rotation_buffer);
	Rich_CalcLaraMatrices_Normal(temp_rotation_buffer, bone, 0);
	phd_PushUnitMatrix();
	Rich_CalcLaraMatrices_Normal(temp_rotation_buffer, bone, 1);
	phd_PopMatrix();
	HairControl(0, 0, temp_rotation_buffer);

	if (gfLevelFlags & GF_YOUNGLARA)
		HairControl(0, 1, temp_rotation_buffer);

	if (cutseq_num == 12)
		GLaraShadowframe = frig_jeep_shadow_bbox;
	else
		GLaraShadowframe = frig_shadow_bbox;
}

void updateAnimFrame(PACKNODE* node, int flags, short* frame)
{
	short* nex;

	frame[6] = 3 * node->xrot_run;
	frame[7] = 3 * node->yrot_run;
	frame[8] = 3 * node->zrot_run;
	nex = frame + 9;

	for (int i = 1; i < flags; i++, nex += 2)
	{
		nex[0] = (node[i].zrot_run | ((node[i].yrot_run | (node[i].xrot_run << 10)) << 10)) >> 16;
		nex[1] = node[i].zrot_run | ((node[i].yrot_run | (node[i].xrot_run << 10)) << 10);
	}
}

void* cutseq_malloc(long size)
{
	return game_malloc(size);
}

void finish_cutseq()
{
	GLOBAL_playing_cutseq = 0;
	InitialiseHair();
}

void DrawCutSeqActors()
{
	OBJECT_INFO* obj;
	short** mesh;
	long* bone;
	short* rot;
	long n;

	phd_PushMatrix();

	for (int i = 1; i < GLOBAL_cutme->numactors; i++)
	{
		phd_PushMatrix();

		if (cutseq_meshbits[i] & 0x80000000)
		{
			n = 1;
			updateAnimFrame(actor_pnodes[i], GLOBAL_cutme->actor_data[i].nodes + 1, temp_rotation_buffer);
			phd_TranslateAbs(GLOBAL_cutme->orgx, GLOBAL_cutme->orgy, GLOBAL_cutme->orgz);
			obj = &objects[GLOBAL_cutme->actor_data[i].objslot];
			bone = &bones[obj->bone_index];
			mesh = &meshes[obj->mesh_index];
			CalcActorLighting(&duff_item[i - 1], obj, temp_rotation_buffer);
			phd_TranslateRel(temp_rotation_buffer[6], temp_rotation_buffer[7], temp_rotation_buffer[8]);
			rot = &temp_rotation_buffer[9];
			gar_RotYXZsuperpack(&rot, 0);

			if (cutseq_meshbits[i] & 1)
			{
				if (cutseq_meshswapbits[i] & 1)
					phd_PutPolygons(mesh[1], -1);
				else
					phd_PutPolygons(mesh[0], -1);
			}

			mesh += 2;

			for (int j = 0; j < obj->nmeshes - 1; j++, bone += 4, mesh += 2)
			{
				if (*bone & 1)
					phd_PopMatrix();

				if (*bone & 2)
					phd_PushMatrix();

				phd_TranslateRel(bone[1], bone[2], bone[3]);
				gar_RotYXZsuperpack(&rot, 0);
				n <<= 1;

				if (cutseq_meshbits[i] & n)
				{
					if (cutseq_meshswapbits[i] & n)
						phd_PutPolygons(mesh[1], -1);
					else
						phd_PutPolygons(mesh[0], -1);
				}
			}
		}

		phd_PopMatrix();
	}

	phd_PopMatrix();
}

void CalcActorLighting(ITEM_INFO* item, OBJECT_INFO* obj, short* rot)
{
	PHD_VECTOR pos;

	GetJointAbsPositionCutSeq(item, obj, rot, &pos);
	IsRoomOutsideNo = -1;
	IsRoomOutside(pos.x, pos.y, pos.z);

	if (IsRoomOutsideNo != -1)
		item->room_number = IsRoomOutsideNo;

	current_item = item;
	item->il.fcnt = -1;
	item->il.item_pos.x = pos.x;
	item->il.item_pos.y = pos.y;
	item->il.item_pos.z = pos.z;
	CalcAmbientLight(item);
	CreateLightList(item);
}

void GetJointAbsPositionCutSeq(ITEM_INFO* item, OBJECT_INFO* obj, short* rot, PHD_VECTOR* pos)
{
	short* rot2;

	phd_PushUnitMatrix();
	phd_TranslateRel(rot[6], rot[7], rot[8]);
	rot2 = rot + 9;
	gar_RotYXZsuperpack(&rot2, 0);
	pos->x = (long)mMXPtr[M03];
	pos->y = (long)mMXPtr[M13];
	pos->z = (long)mMXPtr[M23];
	pos->x += item->pos.x_pos;
	pos->y += item->pos.y_pos;
	pos->z += item->pos.z_pos;
	phd_PopMatrix();
}

void CalculateObjectLightingLaraCutSeq()
{
	PHD_VECTOR pos;
	short room_num, room_num2;

	pos.x = 0;
	pos.y = 0;
	pos.z = 0;
	GetLaraJointPos(&pos, 7);
	room_num = lara_item->room_number;
	IsRoomOutsideNo = -1;
	IsRoomOutside(pos.x, pos.y, pos.z);

	if (IsRoomOutsideNo != -1)
		room_num = IsRoomOutsideNo;

	room_num2 = lara_item->room_number;
	current_item = lara_item;
	lara_item->il.item_pos.x = pos.x;
	lara_item->il.item_pos.y = pos.y;
	lara_item->il.item_pos.z = pos.z;
	lara_item->room_number = room_num;
	CalcAmbientLight(lara_item);
	CreateLightList(lara_item);
	lara_item->room_number = room_num2;
}

void force_sky_lightning()	//optimized out, in Mac symbols.
{
	LightningCount = (GetRandomDraw() & 0x1F) + 64;
	dLightningRand = (GetRandomDraw() & 0xFF) + 256;
}

void third_cutseq_control()
{
	if (GLOBAL_cutseq_frame == 1890)
		force_sky_lightning();
}

void fourth_cutseq_init()
{
	old_lara_holster = lara.holster;
	lara.holster = LARA_HOLSTERS;
	draw_pistol_meshes(1);
}

void fourth_cutseq_control()
{
	long frame;

	frame = GLOBAL_cutseq_frame;

	if (frame == 309)
	{
		undraw_pistol_mesh_left(1);
		undraw_pistol_mesh_right(1);
		lara.holster = old_lara_holster;
	}
	else
	{
		if (frame == 51 || frame == 68)
			cutseq_shoot_pistols(11);

		if (frame == 51 || frame == 64 || frame == 73)
			cutseq_shoot_pistols(14);

		if (frame == 108 || frame == 114 || frame == 120 || frame == 128 || frame == 134 || frame == 141 || frame == 147 || frame == 158)
		{
			cutseq_shoot_pistols(11);
			cutseq_shoot_pistols(14);
		}
	}

	deal_with_pistols();
	handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 8, 1, TROOPS, troop_chat_ranges4);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void fourth_cutseq_end()
{
	trigger_item_in_room(101, ANIMATING9);
	lara.keyitems |= 0xC00;
}

void do_backpack_meshswap()	//optimized out, in Mac symbols.
{
	short* temp;

	temp = lara.mesh_ptrs[LM_TORSO];
	lara.mesh_ptrs[LM_TORSO] = meshes[objects[PISTOLS_ANIM].mesh_index + LM_TORSO * 2];
	meshes[objects[PISTOLS_ANIM].mesh_index + LM_TORSO * 2] = temp;
}

void fifth_cutseq_control()
{
	if (GLOBAL_cutseq_frame == 1350)
		do_backpack_meshswap();
}

void fifth_cutseq_end()
{
	DelsHandyTeleportLara(100938, 768, 58040, -32552);
}

void sixth_cutseq_init()
{
	init_voncroy_meshbits(1);
}

void sixth_cutseq_control()
{
	handle_lara_chatting(lara_chat_ranges6);
	handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 21, 1, VON_CROY, voncroy_chat_ranges6);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void sixth_cutseq_end()
{
	DelsHandyTeleportLara(5632, 1280, 86528, 28987);
}

void seventh_cutseq_init()
{
	init_voncroy_meshbits(1);
	cutseq_kill_item(ANIMATING3);
	nail_intelligent_object(VON_CROY);
}

void seventh_control()
{
	handle_lara_chatting(lara_chat_ranges7);
	handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 21, 1, VON_CROY, voncroy_chat_ranges7);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void eighth_control()
{
	handle_lara_chatting(lara_chat_ranges8);
	handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 21, 1, VON_CROY, voncroy_chat_ranges8);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void ninth_cutseq_init()
{
	init_voncroy_meshbits(1);
	cutseq_meshbits[3] &= ~0x80000000;
	cutseq_meshbits[4] &= ~0x80000000;
	cutseq_meshbits[5] &= ~0x80000000;
	cutseq_meshbits[6] &= ~0x80000000;
	cutseq_meshbits[7] &= ~0x80000000;
}

void ninth_control()
{
	switch (GLOBAL_cutseq_frame)
	{
	case 3000:
		cutseq_meshbits[1] &= ~0x80000000;
		cutseq_meshbits[3] |= 0x80000000;
		cutseq_meshbits[4] |= 0x80000000;
		cutseq_meshbits[5] |= 0x80000000;
		cutseq_meshbits[6] |= 0x80000000;
		cutseq_meshbits[7] |= 0x80000000;
		break;

	case 1300:
		cutseq_meshbits[1] &= ~0x80000000;
		break;

	case 1677:
		cutseq_meshbits[1] |= 0x80000000;
		break;
	}

	handle_lara_chatting(lara_chat_ranges9);
	handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 21, 1, VON_CROY, voncroy_chat_ranges9);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void tenth_cutseq_init()
{
	cutseq_kill_item(DOOR_TYPE5);
	cutseq_meshbits[1] &= ~0x80000000;
	cutseq_meshbits[2] &= ~0x80000000;
	cutseq_meshbits[3] &= ~0x80000000;
	cutseq_meshbits[4] &= ~0x80000000;
}

void tenth_cutseq_control()
{
	long frame, r, g, b;

	frame = GLOBAL_cutseq_frame;

	if (frame >= 2360 && frame <= 2450 && (GetRandomControl() & 7) >= 4)
	{
		r = (GetRandomControl() & 0x3F) + 192;
		g = (GetRandomControl() & 0x1F) + 128;
		b = GetRandomControl() & 0x3F;
		TriggerDynamic(68646, -2304, (GetRandomControl() & 0x3FF) + 59714, 15, r, g, b);
	}

	if (frame >= 1203 && frame <= 1205)
		InitialiseHair();

	if (frame == 1205 || frame == 1747)
	{
		cutseq_meshbits[1] &= ~0x80000000;
		cutseq_meshbits[2] &= ~0x80000000;
		cutseq_meshbits[3] &= ~0x80000000;
		cutseq_meshbits[4] &= ~0x80000000;
	}

	if (frame == 2241)
	{
		cutseq_meshbits[2] &= ~0x80000000;
		cutseq_meshbits[3] &= ~0x80000000;
		cutseq_meshbits[4] &= ~0x80000000;
	}

	if (frame == 906 || frame == 1316 || frame == 2007)
	{
		cutseq_meshbits[1] |= 0x80000000;
		cutseq_meshbits[2] |= 0x80000000;
		cutseq_meshbits[3] |= 0x80000000;
		cutseq_meshbits[4] |= 0x80000000;
	}

	handle_lara_chatting(lara_chat_ranges10);
	handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 21, 1, ANIMATING1, voncroy_chat_ranges10);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void tenth_cutseq_end()
{
	cutseq_restore_item(DOOR_TYPE5);
	DelsHandyTeleportLara(68756, 512, 56493, 983);
}

void eleventh_cutseq_init()
{
	ITEM_INFO* item;

	item = find_a_fucking_item(ANIMATING5);
	old_status_flags[numnailed] = item->status;
	item->status = ITEM_INVISIBLE;
	item->flags &= ~IFL_CODEBITS;
	numnailed++;
	KillActiveBaddies(0);
	cutseq_kill_item(MOTORBIKE);
	cutseq_kill_item(ANIMATING4);
	cutseq_kill_item(ANIMATING1);
	cutseq_meshbits[3] &= ~0x80000000;
	cutseq_meshbits[4] &= ~0x80000000;
	cutseq_meshbits[5] &= ~0x80000000;
	cutseq_meshbits[6] &= ~0x80000000;
}

void eleventh_cutseq_control()
{
	long frame, r, g, b;

	frame = GLOBAL_cutseq_frame;

	if (frame == 581 || frame == 1423 || frame == 1950)
		cutseq_meshbits[3] |= 0x80000000;

	if (frame == 694 || frame == 1460 || frame == 2251)
		cutseq_meshbits[3] &= ~0x80000000;

	if (GLOBAL_cutseq_frame == 671)
		cutseq_meshbits[5] |= 0x80000000;
	else
	{
		if (frame == 763 || frame == 1201 || frame == 1668 || frame == 2383)
			cutseq_meshbits[1] |= 0x80000000;

		if (frame == 581 || frame == 1030 || frame == 1394 || frame == 1708)
			cutseq_meshbits[1] &= ~0x80000000;

		if (frame == 2280)
			cutseq_meshbits[2] &= ~0x80000000;
		else if (frame == 2790)
		{
			cutseq_meshbits[4] |= 0x80000000;
			cutseq_meshbits[6] |= 0x80000000;
		}
		else
		{
			if (frame >= 2536 && frame <= 2710)
			{
				if (GetRandomControl() & 1)
				{
					r = (GetRandomControl() & 0x3F) + 192;
					g = (GetRandomControl() & 0x1F) + 128;
					b = GetRandomControl() & 0x3F;
					TriggerDynamic(58672 - (GetRandomControl() & 0xFFF), -2816, 57865, 24, r, g, b);
				}
					
				r = (GetRandomControl() & 0x3F) + 192;
				g = (GetRandomControl() & 0x1F) + 128;
				b = GetRandomControl() & 0x3F;
				TriggerDynamic(60672 - (GetRandomControl() & 0xFFF), -2816, 56865, 31, r, g, b);
			}

			if (frame == 2772)
				SetScreenFadeOut(16, 1);
		}
	}

	handle_lara_chatting(lara_chat_ranges11);
	handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 14, 2, ANIMATING8, captain_chat_ranges11);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void twelth_cutseq_init()
{
	cutseq_kill_item(ANIMATING6);
}

void twelth_cutseq_end()
{
	cutseq_restore_item(ANIMATING6);
}

void thirteen_end()
{
	DelsHandyTeleportLara(79194, 0, 40951, -20741);
}

void fourteen_end()
{
	DelsHandyTeleportLara(27282, 256, 65654, 0);
	FlipMap(1);
	lara.questitems |= 1;
}

void fourteen_control()
{
	ITEM_INFO* item;

	if (GLOBAL_cutseq_frame == 393)
	{
		item = find_a_fucking_item(SARCOPHAGUS_CUT);
		item->flags = IFL_TRIGGERED;
		item->mesh_bits = 3;
	}
}

void fifteen_init()
{
	cutseq_meshbits[6] &= 0xFFFFFFFE;
	trigger_item_in_room(184, FLAME_EMITTER2);
	trigger_item_in_room(177, AMBER_LIGHT);
	cutseq_meshbits[1] &= ~0x80000000;
	cutseq_meshbits[2] &= ~0x80000000;
	cutseq_meshbits[3] &= ~0x80000000;
	cutseq_meshbits[4] &= ~0x80000000;
	cutseq_meshbits[5] &= ~0x80000000;
	cutseq_meshbits[6] &= ~0x80000000;
	cutseq_meshbits[7] &= ~0x80000000;
	cutseq_meshbits[8] &= ~0x80000000;
}

void fifteen_control()
{
	long frame;

	frame = GLOBAL_cutseq_frame;

	if (frame == 201 || frame == 781 || frame == 1691)
		cutseq_meshbits[1] |= 0x80000000;

	if (frame == 681 || frame == 1306 || frame == 1750)
		cutseq_meshbits[1] &= ~0x80000000;

	if (frame == 360 || frame == 781 || frame == 1402 || frame == 1691)
	{
		cutseq_meshbits[2] |= 0x80000000;
		cutseq_meshbits[3] |= 0x80000000;
		cutseq_meshbits[4] |= 0x80000000;
		cutseq_meshbits[5] |= 0x80000000;
	}

	if (frame == 681 || frame == 1306 || frame == 1552 || frame == 1750)
	{
		cutseq_meshbits[2] &= ~0x80000000;
		cutseq_meshbits[3] &= ~0x80000000;
		cutseq_meshbits[4] &= ~0x80000000;
		cutseq_meshbits[5] &= ~0x80000000;
	}

	if (frame == 1601)
	{
		cutseq_meshbits[6] |= 0x80000000;
		cutseq_meshbits[7] |= 0x80000000;
	}
	else if (frame == 1691)
	{
		cutseq_meshbits[8] |= 0x80000000;
		cutseq_meshbits[6] &= ~0x80000000;
	}

	handle_lara_chatting(lara_chat_ranges15);
	handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 21, 1, ANIMATING9_MIP, voncroy_chat_ranges15);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void fifteen_end()
{
	lara.questitems |= 1;
}

void sixteen_init()
{
	cutseq_meshbits[6] &= 0xFFFFFFFE;
	trigger_item_in_room(169, FLAME_EMITTER2);
	trigger_item_in_room(184, FLAME_EMITTER2);
	cutseq_meshbits[1] &= ~0x80000000;
	cutseq_meshbits[2] &= ~0x80000000;
	cutseq_meshbits[4] &= ~0x80000000;
	cutseq_meshbits[5] &= ~0x80000000;
	cutseq_meshbits[6] &= ~0x80000000;
	cutseq_meshbits[7] &= ~0x80000000;
	cutseq_meshbits[8] &= ~0x80000000;
	cutseq_meshbits[9] &= ~0x80000000;
}

void sixteen_control()
{
	long frame;

	frame = GLOBAL_cutseq_frame;

	if (frame == 1194)
		cutseq_meshbits[3] &= ~0x80000000;
	else
	{
		if (frame == 1257 || frame == 1740)
			cutseq_meshbits[2] |= 0x80000000;

		if (frame == 1374 || frame == 2130)
			cutseq_meshbits[2] &= ~0x80000000;

		if (frame == 1300 || frame == 2820)
			cutseq_meshbits[1] |= 0x80000000;


		if (frame == 2401)
			cutseq_meshbits[1] &= ~0x80000000;
		else if (frame == 1300)
			cutseq_meshbits[9] |= 0x80000000;
		else if (frame == 2130)
			cutseq_meshbits[9] &= ~0x80000000;
		else if (frame == 2160)
			cutseq_meshbits[6] |= 0x80000000;
		else if (frame == 2340)
			cutseq_meshbits[7] |= 0x80000000;
		else if (frame == 2501)
		{
			cutseq_meshbits[6] &= ~0x80000000;
			cutseq_meshbits[7] &= ~0x80000000;
			cutseq_meshbits[4] |= 0x80000000;
			cutseq_meshbits[5] |= 0x80000000;
			cutseq_meshbits[8] |= 0x80000000;
		}
		else if (frame == 2980)
			trigger_item_in_room(177, AMBER_LIGHT);
	}

	handle_lara_chatting(lara_chat_ranges16);
	handle_actor_chatting(ACTOR2_SPEECH_HEAD1, 18, 3, ANIMATING14_MIP, jeanyves_chat_ranges16);
	handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 21, 1, ANIMATING9_MIP, voncroy_chat_ranges16);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void sixteen_end()
{
	untrigger_item_in_room(169, FLAME_EMITTER2);
	untrigger_item_in_room(184, FLAME_EMITTER2);
	untrigger_item_in_room(177, AMBER_LIGHT);
	DelsHandyTeleportLara(61868, 0, 44994, -16060);
}

void seventeen_control()
{
	handle_actor_chatting(ACTOR2_SPEECH_HEAD1, 18, 1, JEAN_YVES, jeanyves_chat_ranges17);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void seventeen_end()
{
	cutseq_restore_item(ANIMATING4);
	cutseq_restore_item(JEAN_YVES);
	DelsHandyTeleportLara(54706, -8192, 50470, 4514);
}

void eighteen_init()
{
	cutseq_kill_item(JEAN_YVES);
}

void eighteen_control()
{
	handle_actor_chatting(ACTOR2_SPEECH_HEAD1, 18, 1, JEAN_YVES, jeanyves_chat_ranges18);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void eighteen_end()
{
	cutseq_restore_item(JEAN_YVES);
	DelsHandyTeleportLara(55311, -8192, 53160, 2304);
}

void nineteen_init()
{
	cutseq_kill_item(ANIMATING4);
	cutseq_kill_item(JEAN_YVES);
}

void nineteen_control()
{
	handle_actor_chatting(ACTOR2_SPEECH_HEAD1, 18, 1, JEAN_YVES, jeanyves_chat_ranges19);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void nineteen_end()
{
	cutseq_restore_item(ANIMATING4);
	cutseq_restore_item(JEAN_YVES);
	DelsHandyTeleportLara(53469, -8192, 48687, 23355);
}

void twenty_control()
{
	handle_actor_chatting(ACTOR2_SPEECH_HEAD1, 18, 1, JEAN_YVES, jeanyves_chat_ranges20);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void twenty_end()
{
	cutseq_restore_item(ANIMATING4);
	cutseq_restore_item(JEAN_YVES);
	DelsHandyTeleportLara(55348, -8192, 50436, -9731);
}

void twentyone_init()
{
	cutseq_kill_item(ANIMATING10);
	do_key_meshswap();
}

void twentyone_end()
{
	do_key_meshswap();
	DelsHandyTeleportLara(33279, -4352, 40697, 21847);
}

void twentythree_init()
{
	horus_item_thing = find_a_fucking_item(HORUS_STATUE);
}

void twentythree_control()
{
	horus_item_thing->flags |= IFL_TRIGGERED;

	switch (GLOBAL_cutseq_frame)
	{
	case 210:
		horus_item_thing->mesh_bits &= ~0x200;
		horus_item_thing->mesh_bits |= 0x20;
		break;

	case 251:
		horus_item_thing->mesh_bits &= ~0x400;
		horus_item_thing->mesh_bits |= 0x10;
		break;

	case 350:
		horus_item_thing->mesh_bits &= ~0x40;
		horus_item_thing->mesh_bits |= 8;
		break;

	case 440:
		horus_item_thing->mesh_bits &= ~4;
		horus_item_thing->mesh_bits |= 0x80;
		break;

	case 500:
		horus_item_thing->mesh_bits &= ~2;
		horus_item_thing->mesh_bits |= 0x100;
		break;

	case 556:
		horus_item_thing->mesh_bits |= 0x800;
		break;
	}
}

void twentythree_end()
{
	DelsHandyTeleportLara(17476, 29952, 15872, -16135);
	lara.questitems = 0;
}

void twentyfour_init()
{
	cutseq_kill_item(ANIMATING13);
	old_lara_holster = lara.holster;
	lara.holster = LARA_HOLSTERS;
	draw_pistol_meshes(1);
}

void twentyfour_control()
{
	if (GLOBAL_cutseq_frame == 552)
	{
		undraw_pistol_mesh_left(1);
		undraw_pistol_mesh_right(1);
		lara.holster = old_lara_holster;
	}

	handle_lara_chatting(lara_chat_ranges24);
	handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 14, 1, ANIMATING13, troop_chat_ranges24);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void twentyfour_end()
{
	cutseq_restore_item(ANIMATING13);
	trigger_item_in_room(107, PUZZLE_ITEM8_COMBO1);
}

void twentyfive_control()
{
	handle_lara_chatting(lara_chat_ranges25);
	handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 14, 1, ANIMATING8, troop_chat_ranges25);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void twentyfive_end()
{
	ITEM_INFO* item;

	item = find_a_fucking_item(ANIMATING5);
	item->flags |= IFL_CODEBITS;
	item->status = old_status_flags[numnailed];
	numnailed++;
	DelsHandyTeleportLara(55396, -1792, 77122, -7105);
}

void twentyfive_init()
{
	ITEM_INFO* item;

	item = find_a_fucking_item(ANIMATING5);
	old_status_flags[numnailed] = item->status;
	item->status = ITEM_INVISIBLE;
	item->flags &= ~IFL_CODEBITS;
	numnailed++;
}

void twentysix_control()
{
	handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 14, 1, ANIMATING8, troop_chat_ranges26);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void twentyseven_init()
{
	ITEM_INFO* item;

	item = find_a_fucking_item(ANIMATING4);
	item->flags &= ~IFL_CODEBITS;
	cutseq_meshbits[1] &= ~0x200;
	lara.questitems = 0;
}

void twentyseven_control()
{
	if (GLOBAL_cutseq_frame == 283)
		cutseq_meshbits[1] |= 0x200;
}

void twentyseven_end()
{
	ITEM_INFO* item;

	item = find_a_fucking_item(ANIMATING5);
	item->status = ITEM_ACTIVE;
	DelsHandyTeleportLara(18982, 18176, 15841, -16184);
	FlipMap(8);
	nail_intelligent_object(SETHA);
}

void twentytwo_init()
{
	cutseq_kill_item(DOOR_TYPE6);
}

void twentytwo_end()
{
	DelsHandyTeleportLara(72770, -11776, 54077, 0);
	cutseq_restore_item(DOOR_TYPE6);
	trigger_item_in_room(39, DOOR_TYPE6);
	trigger_item_in_room(39, DEMIGOD3);
}

void do_spade_meshswap()
{
	short* temp;

	temp = lara.mesh_ptrs[LM_LHAND];
	lara.mesh_ptrs[LM_LHAND] = meshes[objects[MESHSWAP1].mesh_index + LM_LHAND * 2];
	meshes[objects[MESHSWAP1].mesh_index + LM_LHAND * 2] = temp;
}

void do_key_meshswap()
{
	short* temp;

	temp = lara.mesh_ptrs[LM_RHAND];
	lara.mesh_ptrs[LM_RHAND] = meshes[objects[MESHSWAP1].mesh_index + LM_RHAND * 2];
	meshes[objects[MESHSWAP1].mesh_index + LM_RHAND * 2] = temp;
}

void cutseq_shoot_pistols(long left_or_right)
{
	if (left_or_right == 14)
	{
		lara.left_arm.flash_gun = 4;
		SmokeCountL = 16;
	}
	else
	{
		lara.right_arm.flash_gun = 4;
		SmokeCountR = 16;
	}
}

void trigger_weapon_dynamics(long left_or_right)
{
	PHD_VECTOR pos;

	pos.x = (GetRandomControl() & 0xFF) - 128;
	pos.y = (GetRandomControl() & 0x7F) - 63;
	pos.z = (GetRandomControl() & 0xFF) - 128;
	GetLaraJointPos(&pos, left_or_right);
	TriggerDynamic(pos.x, pos.y, pos.z, 10, (GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 128, (GetRandomControl() & 0x3F));
}

void deal_with_pistols()
{
	PHD_VECTOR pos;

	if (SmokeCountL)
	{
		pos.x = 4;
		pos.y = 128;
		pos.z = 40;
		GetLaraJointPos(&pos, 14);
		TriggerGunSmoke(pos.x, pos.y, pos.z, 0, 0, 0, 0, SmokeWeapon, SmokeCountL);
	}

	if (SmokeCountR)
	{
		pos.x = -16;
		pos.y = 128;
		pos.z = 40;
		GetLaraJointPos(&pos, 11);
		TriggerGunSmoke(pos.x, pos.y, pos.z, 0, 0, 0, 0, SmokeWeapon, SmokeCountR);
	}

	if (lara.left_arm.flash_gun)
	{
		lara.left_arm.flash_gun--;
		trigger_weapon_dynamics(14);
	}

	if (lara.right_arm.flash_gun)
	{
		lara.right_arm.flash_gun--;
		trigger_weapon_dynamics(11);
	}
}

void cutseq_kill_item(long num)
{
	ITEM_INFO* item;

	for (int i = 0; i < level_items; i++)
	{
		item = &items[i];

		if (item->object_number == num)
		{
			old_status_flags[numnailed] = item->status;
			item->status = ITEM_INVISIBLE;
			numnailed++;
		}
	}
}

ITEM_INFO* cutseq_restore_item(long num)
{
	ITEM_INFO* item;

	for (int i = 0; i < level_items; i++)
	{
		item = &items[i];

		if (item->object_number == num)
		{
			item->status = old_status_flags[numnailed];
			numnailed++;
			return item;
		}
	}

	return 0;
}

long Load_and_Init_Cutseq(long num)
{
	ACTORME* actor;
	long* headerbuf;
	char* packed;
	long Offset, Length;

	Log(5, "Initialising Cut Scene");
	SetCutPlayed(num);

	headerbuf = (long*)cutseqpakPtr + (2 * num);	//cutseq.pak header is offsets then lengths, go to current num
	Offset = headerbuf[0];							//first long, offset
	Length = headerbuf[1];							//second long, length

	Log(5, "Offset=%d, Length=%d\n", Offset, Length);
	packed = &cutseqpakPtr[Offset];
	GLOBAL_cutme = (NEW_CUTSCENE*)packed;	//go to current cut data
	Log(5, "Loaded cutscene data...");
	Log(5, "num actors = %d\n", GLOBAL_cutme->numactors);
	Log(5, "num frames = %d\n", GLOBAL_cutme->numframes);
	Log(5, "OrgX=%d,OrgY=%d,OrgZ=%d\n", GLOBAL_cutme->orgx, GLOBAL_cutme->orgy, GLOBAL_cutme->orgz);
	Log(5, "CameraOffset=%d\n", GLOBAL_cutme->camera_offset);

	for (int i = 0; i < GLOBAL_cutme->numactors; i++)
	{
		actor = &GLOBAL_cutme->actor_data[i];
		Log(5, "Actor %d --- offset=%d,slot=%d,nodes=%d\n", i, actor->offset, actor->objslot, actor->nodes);
	}
		
	init_cutseq_actors(packed, 0);
	return 0;
}

void init_cutseq_actors(char* data, long resident)
{
	ITEM_INFO* item;
	char* packed;
	char* resident_addr;
	long pda_nodes, offset;

	resident_addr = GLOBAL_resident_depack_buffers;
	lastcamnum = -1;
	GLOBAL_playing_cutseq = 0;
	GLOBAL_numcutseq_frames = GLOBAL_cutme->numframes;

	for (int i = 0; i < GLOBAL_cutme->numactors; i++)
	{
		item = &duff_item[i];
		offset = GLOBAL_cutme->actor_data[i].offset;
		packed = &data[offset];
		pda_nodes = GLOBAL_cutme->actor_data[i].nodes;

		if (resident)
		{
			actor_pnodes[i] = (PACKNODE*)resident_addr;
			resident_addr += (pda_nodes + 1) * sizeof(PACKNODE);
		}
		else
			actor_pnodes[i] = (PACKNODE*)cutseq_malloc((pda_nodes + 1) * sizeof(PACKNODE));

		InitPackNodes((NODELOADHEADER*)packed, actor_pnodes[i], packed, pda_nodes + 1);
		memset(item, 0, sizeof(ITEM_INFO));
		item->il.ambient = lara_item->il.ambient;
		item->il.fcnt = -1;
		item->il.room_number = -1;
		item->il.RoomChange = 0;
		item->il.nCurrentLights = 0;
		item->il.nPrevLights = 0;
		item->il.pCurrentLights = &item->il.CurrentLights;
		item->il.pPrevLights = &item->il.PrevLights;
		item->pos.x_pos = GLOBAL_cutme->orgx;
		item->pos.y_pos = GLOBAL_cutme->orgy;
		item->pos.z_pos = GLOBAL_cutme->orgz;
		item->pos.x_rot = 0;
		item->pos.y_rot = 0;
		item->pos.z_rot = 0;
		item->room_number = 0;
	}

	offset = GLOBAL_cutme->camera_offset;
	packed = &data[offset];

	if (resident)
		camera_pnodes = (PACKNODE*)resident_addr;
	else
		camera_pnodes = (PACKNODE*)cutseq_malloc(2 * sizeof(PACKNODE));

	InitPackNodes((NODELOADHEADER*)packed, camera_pnodes, packed, 2);
	GLOBAL_playing_cutseq = 1;
	GLOBAL_cutseq_frame = 0;
	DelsHandyTeleportLara(GLOBAL_cutme->orgx, GLOBAL_cutme->orgy, GLOBAL_cutme->orgz, 0);
	camera.pos.x = lara_item->pos.x_pos;
	camera.pos.y = lara_item->pos.y_pos;
	camera.pos.z = lara_item->pos.z_pos;
	camera.pos.room_number = lara_item->room_number;
	InitialiseHair();
}

void init_voncroy_meshbits(long num)
{
	cutseq_meshswapbits[num] = 0x240080;
}

void DelsHandyTeleportLara(long x, long y, long z, long yrot)
{
	lara_item->pos.x_pos = x;
	lara_item->pos.y_pos = y;
	lara_item->pos.z_pos = z;
	lara.head_x_rot = 0;
	lara.head_y_rot = 0;
	lara.torso_x_rot = 0;
	lara.torso_y_rot = 0;
	lara_item->pos.x_rot = 0;
	lara_item->pos.y_rot = (short)yrot;
	lara_item->pos.z_rot = 0;
	IsRoomOutside(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);

	if (IsRoomOutsideNo != lara_item->room_number)
		ItemNewRoom(lara.item_number, IsRoomOutsideNo);

	lara_item->current_anim_state = AS_STOP;
	lara_item->goal_anim_state = AS_STOP;
	lara_item->anim_number = ANIM_STOP;
	lara_item->frame_number = anims[ANIM_STOP].frame_base;
	lara_item->speed = 0;
	lara_item->fallspeed = 0;
	lara_item->gravity_status = 0;
	lara.gun_status = LG_NO_ARMS;
	camera.fixed_camera = 1;
}

void nail_intelligent_object(short num)
{
	ITEM_INFO* item;

	for (int i = 0; i < level_items; i++)
	{
		item = &items[i];

		if (item->object_number == num)
		{
			item->status = ITEM_INVISIBLE;
			RemoveActiveItem(i);
			DisableBaddieAI(i);
		}
	}
}

void handle_lara_chatting(short* _ranges)
{
	long r1, r2, f;

	lara_chat_cnt = (lara_chat_cnt - 1) & 1;
	f = GLOBAL_cutseq_frame;

	while (1)
	{
		r1 = _ranges[0];
		r2 = _ranges[1];

		if (r1 == -1)
		{
			lara.mesh_ptrs[LM_HEAD] = meshes[objects[LARA_SKIN].mesh_index + 2 * LM_HEAD];
			return;
		}

		if (f > r1 && f < r2)
			break;

		_ranges += 2;
	}

	if (!lara_chat_cnt)
		lara.mesh_ptrs[LM_HEAD] = meshes[objects[(GetRandomControl() & 3) + LARA_SPEECH_HEAD1].mesh_index + 2 * LM_HEAD];
}

void handle_actor_chatting(long speechslot, long node, long slot, long objslot, short* _ranges)
{
	long r1, r2, f, rnd;

	rnd = GetRandomControl() & 1;
	f = GLOBAL_cutseq_frame;

	while (1)
	{
		r1 = _ranges[0];
		r2 = _ranges[1];

		if (r1 == -1)
		{
			cutseq_meshswapbits[slot] &= ~(1 << node);
			return;
		}

		if (f > r1 && f < r2)
			break;

		_ranges += 2;
	}

	if (!actor_chat_cnt)
	{
		cutseq_meshswapbits[slot] |= (1 << node);
		meshes[objects[objslot].mesh_index + (2 * node) + 1] = meshes[objects[speechslot + rnd].mesh_index + 2 * node];

		if ((GetRandomControl() & 7) >= 6)
			cutseq_meshswapbits[slot] &= ~(1 << node);
	}
}

void trigger_item_in_room(long room_number, long object_number)
{
	ITEM_INFO* item;
	short item_number;

	for (item_number = room[room_number].item_number; item_number != NO_ITEM; item_number = item->next_item)
	{
		item = &items[item_number];

		if (item->object_number == object_number)
		{
			AddActiveItem(item_number);

			if (object_number != DEMIGOD3)
				item->status = ITEM_ACTIVE;

			item->flags |= IFL_CODEBITS;
		}
	}
}

void untrigger_item_in_room(long room_number, long object_number)
{
	ITEM_INFO* item;
	short item_number;

	for (item_number = room[room_number].item_number; item_number != NO_ITEM; item_number = item->next_item)
	{
		item = &items[item_number];

		if (item->object_number == object_number)
		{
			RemoveActiveItem(item_number);
			item->status = ITEM_INACTIVE;
			item->flags &= ~IFL_CODEBITS;
		}
	}
}

ITEM_INFO* find_a_fucking_item(long object_number)
{
	ITEM_INFO* item;

	for (int i = 0; i < level_items; i++)
	{
		item = &items[i];

		if (item->object_number == object_number)
			return item;
	}

	return 0;
}

void special2_end()
{
	if (!bDoCredits)
	{
		InitialiseSpotCam(3);
		lara_item->mesh_bits = 0;
	}

	Chris_Menu = 0;
	title_controls_locked_out = 0;
}

void special2_init()
{
	lara_item->mesh_bits = -1;
	Chris_Menu = 0;
}

void special3_end()
{
	if (!bDoCredits)
	{
		InitialiseSpotCam(1);
		lara_item->mesh_bits = 0;
	}

	CutSceneTriggered = 0;
	Chris_Menu = 0;
	title_controls_locked_out = 0;
}

void special3_control()
{
	if (GLOBAL_cutseq_frame >= 348 && GLOBAL_cutseq_frame <= 358 && GLOBAL_cutseq_frame & 1)
		DoBloodSplat(6799 - (GetRandomDraw() & 0xFF), (GetRandomDraw() & 0x1FF) - 768, 76209, 7, -1, 24);
}

void special1_init()
{
	ITEM_INFO* item;

	for (int i = 0; i < level_items; i++)
	{
		item = &items[i];

		if (item->object_number == ANIMATING6)
		{
			item->anim_number = objects[ANIMATING6].anim_index;
			item->frame_number = anims[item->anim_number].frame_base;
			item->flags &= ~IFL_CODEBITS;
		}
	}

	lara_item->mesh_bits = -1;
	Chris_Menu = 0;
}

void special1_end()
{
	if (!bDoCredits)
	{
		lara_item->mesh_bits = 0;
		InitialiseSpotCam(2);
	}

	Chris_Menu = 0;
	title_controls_locked_out = 0;
}

#ifdef TIMES_LEVEL
void times_init()
{
	SetFogColor(128, 128, 128);
}

void times_control()
{
	handle_lara_chatting(lara_chat_ranges_times);
	handle_actor_chatting(ACTOR2_SPEECH_HEAD1, 11, 1, ANIMATING16, editor_chat_ranges_times);
	actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}
#endif
