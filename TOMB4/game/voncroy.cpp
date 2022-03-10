#include "../tomb4/pch.h"
#include "voncroy.h"
#include "camera.h"
#include "../specific/3dmath.h"
#include "control.h"
#include "lara_states.h"
#include "../specific/audio.h"
#include "tomb4fx.h"
#include "objects.h"
#include "items.h"
#include "../specific/function_stubs.h"
#include "box.h"
#include "lot.h"
#include "effects.h"

static BITE_INFO VonCroyBite = { 0, 35, 130, 18 };

void SetCutSceneCamera(ITEM_INFO* item)
{
	VonCroyCutData* cut;

	bVoncroyCutScene = 1;
	camera.old_type = FIXED_CAMERA;
	camera.type = CHASE_CAMERA;
	camera.speed = 1;
	CalculateCamera();
	actualCameraTarget.x = camera.target.x;
	actualCameraTarget.y = camera.target.y;
	actualCameraTarget.z = camera.target.z;
	actualFOV = CurrentFov;
	actualRoomNumber = camera.pos.room_number;
	camera.underwater = 0;
	actualCameraPos.x = camera.pos.x;
	actualCameraPos.y = camera.pos.y;
	actualCameraPos.z = camera.pos.z;
	cut = &VonCroyCutscenes[VonCroyCutIndices[lara.locationPad]];

	if (cut->FOV)
		AlterFOV(182 * cut->FOV);

	if (cut->flags & 4)
	{
		camera.pos.x = cut->CameraPos.x;
		camera.pos.y = cut->CameraPos.y;
		camera.pos.z = cut->CameraPos.z;
	}
	else
	{
		camera.pos.x += cut->CameraPos.x;
		camera.pos.y += cut->CameraPos.y;
		camera.pos.z += cut->CameraPos.z;
	}

	if (cut->flags & 8)
	{
		camera.target.x = cut->CameraTarget.x;
		camera.target.y = cut->CameraTarget.y;
		camera.target.z = cut->CameraTarget.z;
	}
	else
	{
		camera.target.x += cut->CameraTarget.x;
		camera.target.y += cut->CameraTarget.y;
		camera.target.z += cut->CameraTarget.z;
	}

	if (cut->flags & 2)
	{
		camera.target.x = item->pos.x_pos;
		camera.target.y = item->pos.y_pos - 256;
		camera.target.z = item->pos.z_pos;
	}

	if (IsRoomOutside(camera.pos.x, camera.pos.y, camera.pos.z) == -2)
	{
		camera.pos.x = actualCameraPos.x;
		camera.pos.y = actualCameraPos.y;
		camera.pos.z = actualCameraPos.z;
		camera.pos.room_number = (short)actualRoomNumber;
	}
	else
		camera.pos.room_number = IsRoomOutsideNo;

	phd_LookAt(camera.pos.x, camera.pos.y, camera.pos.z, camera.target.x, camera.target.y, camera.target.z, 0);
}

void ClearCutSceneCamera()
{
	camera.pos.x = actualCameraPos.x;
	camera.pos.y = actualCameraPos.y;
	camera.pos.z = actualCameraPos.z;
	camera.target.x = actualCameraTarget.x;
	camera.target.y = actualCameraTarget.y;
	camera.target.z = actualCameraTarget.z;
	camera.pos.room_number = (short)actualRoomNumber;
	camera.speed = 1;
	camera.old_type = FIXED_CAMERA;
	camera.type = CHASE_CAMERA;
	bVoncroyCutScene = 0;
	AlterFOV((short)actualFOV);
}

void GetAIEnemy(CREATURE_INFO* info, long tfl)
{
	AIOBJECT* ai;

	for (int i = 0; i < nAIObjects; i++)
	{
		ai = &AIObjects[i];

		if (ai->trigger_flags == tfl && ai->room_number != 255)
		{
			info->enemy = &info->ai_target;
			info->ai_target.object_number = ai->object_number;
			info->ai_target.room_number = ai->room_number;
			info->ai_target.pos.x_pos = ai->x;
			info->ai_target.pos.y_pos = ai->y;
			info->ai_target.pos.z_pos = ai->z;
			info->ai_target.pos.y_rot = ai->y_rot;
			info->ai_target.flags = ai->flags;
			info->ai_target.trigger_flags = ai->trigger_flags;
			info->ai_target.box_number = ai->box_number;

			if (!(info->ai_target.flags & IFL_TRIGGERED))
			{
				info->ai_target.pos.x_pos += (256 * phd_sin(info->ai_target.pos.y_rot)) >> 14;
				info->ai_target.pos.z_pos += (256 * phd_cos(info->ai_target.pos.y_rot)) >> 14;
			}
		}
	}
}

void DoVonCroyCutscene(ITEM_INFO* item, CREATURE_INFO* info)
{
	FLOOR_INFO* floor;
	long h;
	short ang, room_number;

	if (lara.locationPad != 8 && lara.locationPad != 15)
		item->item_flags[3] = lara.locationPad;

	if (lara.locationPad == 20 && lara_item->current_anim_state != AS_SURFTREAD)
		return;

	item->pos.z_rot = 0;

	if (item->item_flags[3] == 8 || item->item_flags[3] == 15)
	{
		if (lara_item->current_anim_state == AS_HANG || lara_item->current_anim_state == AS_HANGLEFT || lara_item->current_anim_state == AS_HANGRIGHT)
		{
			S_CDPlay(VonCroyCutTracks[item->item_flags[3]], 0);
			VonCroyCutFlags[item->item_flags[3]] = 1;
			item->item_flags[2] = 2;
		}

		AnimateItem(item);
		return;
	}

	switch (item->trigger_flags)
	{
	case 0:
		SetFadeClip(24, 1);
		ScreenFading = 1;
		dScreenFade = 255;
		ScreenFade = 255;
		ScreenFadeSpeed = 16;
		ScreenFadeBack = 0;
		ScreenFadedOut = 1;
		bDisableLaraControl = 1;
		item->trigger_flags++;
		input = 0;

		if (item->item_flags[3] == 14)
		{
			item->anim_number = objects[VON_CROY].anim_index + 43;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 27;
			item->goal_anim_state = 30;
		}
		else
		{
			item->anim_number = objects[VON_CROY].anim_index + 4;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 1;
			item->goal_anim_state = 1;
		}

		GetAIEnemy(info, lara.locationPad);
		item->pos.x_pos = info->enemy->pos.x_pos;
		item->pos.y_pos = info->enemy->pos.y_pos;
		item->pos.z_pos = info->enemy->pos.z_pos;
		ang = (short)phd_atan(lara_item->pos.z_pos - item->pos.z_pos, lara_item->pos.x_pos - item->pos.x_pos);

		if (item->item_flags[3] == 14 || item->item_flags[3] == 3)
		{
			item->pos.y_rot = info->enemy->pos.y_rot;
			info->LOT.is_jumping = 1;
			info->maximum_turn = 0;
		}
		else if (item->item_flags[3] == 43 || item->item_flags[3] == 53)
		{
			info->maximum_turn = 0;
			item->pos.y_rot = -0x6000;
		}
		else
			item->pos.y_rot = ang;

		IsRoomOutsideNo = -1;
		IsRoomOutside(item->pos.x_pos, item->pos.y_pos - 64, item->pos.z_pos);

		if (IsRoomOutsideNo != item->room_number && IsRoomOutsideNo != -1)
			ItemNewRoom(item - items, IsRoomOutsideNo);

		lara_item->pos.y_rot = ang + 0x8000;

		if (lara_item->current_anim_state != AS_SURFTREAD)
		{
			room_number = lara_item->room_number;
			floor = GetFloor(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, &room_number);
			h = GetHeight(floor, lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);
			lara_item->pos.y_pos = h;
			lara_item->anim_number = ANIM_STOP;
			lara_item->frame_number = anims[ANIM_STOP].frame_base;
			lara_item->current_anim_state = AS_STOP;
			lara_item->goal_anim_state = AS_STOP;
			lara_item->speed = 0;
			lara_item->fallspeed = 0;
			lara_item->gravity_status = 0;
		}

		break;

	case 1:

		if (ScreenFadedOut)
		{
			SetCutSceneCamera(item);
			ScreenFading = 0;
			SetScreenFadeIn(16);
			item->trigger_flags++;
			S_StartSyncedAudio(VonCroyCutTracks[item->item_flags[3]]);
		}

		break;

	case 2:

		if (input & IN_LOOK && item->item_flags[3] != 43 && item->item_flags[3] != 53)
		{
			item->meshswap_meshbits &= ~0x8000;
			item->trigger_flags = 3;

			if (item->item_flags[3] != 14)
				item->goal_anim_state = 1;
		}
		else
		{
			if (XATrack != VonCroyCutTracks[item->item_flags[3]])
				item->trigger_flags = 3;

			if (item->current_anim_state == 1)
			{
				if (info->enemy && info->enemy->flags == 36)
					item->goal_anim_state = 11;
				else
					item->goal_anim_state = GetRandomControl() % 3 + 8;
			}
			else if (item->current_anim_state != 27)
			{
				if (info->enemy && info->enemy->flags == 36)
					info->enemy->flags = 255;

				item->goal_anim_state = 1;
			}
		}

		break;

	case 3:
		ClearCutSceneCamera();
		S_CDStop();
		S_CDPlay(CurrentAtmosphere, 1);
		IsAtmospherePlaying = 1;
		bDisableLaraControl = 0;
		SetFadeClip(0, 1);
		item->trigger_flags = 0;
		VonCroyCutFlags[item->item_flags[3]] = 1;
		ang = info->enemy->pos.y_rot - item->pos.y_rot;

		if (ang > 1024)
			item->required_anim_state = 22;
		else if (ang < -1024)
			item->required_anim_state = 35;

		item->goal_anim_state = 1;
		item->item_flags[2] = 2;
		break;
	};

	AnimateItem(item);

	if (item->current_anim_state == 11)
	{
		if (item->frame_number == anims[item->anim_number].frame_base + 32)
			item->meshswap_meshbits |= 0x8000;
		else if (item->frame_number == anims[item->anim_number].frame_base + 216)
			item->meshswap_meshbits &= ~0x8000;
	}
}

void InitialiseVoncroy(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[VON_CROY].anim_index + 11;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = 6;
	item->goal_anim_state = 6;
	item->meshswap_meshbits = 0x40080;
	memset(VonCroyCutFlags, 0, 64);
}

void VoncroyRaceControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* oEnemy;
	CREATURE_INFO* VonCroy;
	FLOOR_INFO* floor;
	AI_INFO info;
	long Xoffset, Zoffset, x, y, z, nearheight, midheight, farheight, dx, dz, distance, ahead, iAngle, h, c;
	short angle, torso_x, torso_y, head_x, head_y, room_number, jump_ahead, long_jump_ahead, ifl3;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	VonCroy = (CREATURE_INFO*)item->data;
	angle = 0;
	head_x = 0;
	head_y = 0;
	torso_x = 0;
	torso_y = 0;
	ifl3 = 0;
	room_number = item->room_number;
	Xoffset = 808 * phd_sin(item->pos.y_rot) >> 14;
	Zoffset = 808 * phd_cos(item->pos.y_rot) >> 14;
	x = item->pos.x_pos + Xoffset;
	y = item->pos.y_pos;
	z = item->pos.z_pos + Zoffset;
	floor = GetFloor(x, y, z, &room_number);
	nearheight = GetHeight(floor, x, y, z);
	room_number = item->room_number;
	x += Xoffset;
	z += Zoffset;
	floor = GetFloor(x, y, z, &room_number);
	midheight = GetHeight(floor, x, y, z);
	room_number = item->room_number;
	x += Xoffset;
	z += Zoffset;
	floor = GetFloor(x, y, z, &room_number);
	farheight = GetHeight(floor, x, y, z);

	if (y >= nearheight - 384 || y >= midheight + 256 || y <= midheight - 256)
		jump_ahead = 0;
	else
		jump_ahead = 1;

	if (y >= nearheight - 384 || y >= midheight - 384 || y >= farheight + 256 || y <= farheight - 256)
		long_jump_ahead = 0;
	else
		long_jump_ahead = 1;

	GetAITarget(VonCroy);
	oEnemy = VonCroy->enemy;

	if (item->anim_number == objects[VON_CROY].anim_index + 36 || item->anim_number == objects[VON_CROY].anim_index + 52)
	{
		item->pos.x_pos += Xoffset;
		item->pos.z_pos += Zoffset;
		room_number = item->room_number;
		GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &item->room_number);

		if (item->frame_number == anims[item->anim_number].frame_base + 1)
			CreateZone(item);

		CreatureAIInfo(item, &info);
		item->room_number = room_number;
		item->pos.z_pos -= Zoffset;
		item->pos.x_pos -= Xoffset;
	}
	else
		CreatureAIInfo(item, &info);

	GetCreatureMood(item, &info, 1);
	CreatureMood(item, &info, 1);

	if (VonCroy->enemy == lara_item)
	{
		iAngle = info.angle;
		distance = info.distance;
	}
	else
	{
		dx = lara_item->pos.x_pos - item->pos.x_pos;
		dz = lara_item->pos.z_pos - item->pos.z_pos;
		iAngle = phd_atan(dz, dx) - item->pos.y_rot;

		if (iAngle > -0x4000 && iAngle < 0x4000)
			ahead = 1;
		else
			ahead = 0;

		distance = SQUARE(dx) + SQUARE(dz);
	}

	angle = CreatureTurn(item, VonCroy->maximum_turn);

	switch (item->current_anim_state)
	{
	case 1:
		VonCroy->LOT.is_jumping = 0;
		VonCroy->LOT.is_monkeying = 0;
		VonCroy->flags = 0;
		VonCroy->maximum_turn = 0;
		head_y = info.angle >> 1;

		if (info.ahead)
		{
			torso_x = info.x_angle >> 1;
			torso_y = info.angle >> 1;
		}

		if (lara.location < item->item_flags[3] || XATrack == 80 && (XAFlag == 6 || XAFlag == 5))
		{
			item->goal_anim_state = 1;
			break;
		}

		if (item->item_flags[2] != 6)
			item->item_flags[2] = 0;

		if (VonCroy->flags & 8)
		{
			switch (oEnemy->flags)
			{
			case 0:
			case 32:
				ifl3 = -1;

				if (oEnemy)
				{
					room_number = oEnemy->room_number;
					floor = GetFloor(oEnemy->pos.x_pos, oEnemy->pos.y_pos, oEnemy->pos.z_pos, &room_number);
					GetHeight(floor, oEnemy->pos.x_pos, oEnemy->pos.y_pos, oEnemy->pos.z_pos);
					TestTriggers(trigger_index, 1, 0);
					ifl3 = 1;
				}

				VonCroy->reached_goal = 0;
				VonCroy->enemy = 0;
				item->item_flags[3] += ifl3;
				item->ai_bits = 16;
				break;

			case 1:
			case 3:
			case 5:
			case 7:
			case 9:
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
			case 18:
			case 19:
			case 20:
			case 21:
			case 22:
			case 23:
			case 24:
			case 25:
			case 26:
			case 27:
			case 28:
			case 29:
			case 30:
			case 31:
			case 33:
			case 35:
			case 37:
			case 38:
			case 39:
				break;

			case 2:
				item->current_anim_state = 29;
				item->anim_number = objects[VON_CROY].anim_index + 37;
				item->frame_number = anims[item->anim_number].frame_base;
				item->pos.x_pos = oEnemy->pos.x_pos;
				item->pos.y_pos = oEnemy->pos.y_pos;
				item->pos.z_pos = oEnemy->pos.z_pos;
				item->pos.x_rot = oEnemy->pos.x_rot;
				item->pos.y_rot = oEnemy->pos.y_rot;
				item->pos.z_rot = oEnemy->pos.z_rot;
				ifl3 = 1;
				VonCroy->reached_goal = 0;
				VonCroy->enemy = 0;
				item->item_flags[3] += ifl3;
				item->ai_bits = 16;
				break;

			case 4:
				item->current_anim_state = 26;
				item->anim_number = objects[VON_CROY].anim_index + 36;
				item->frame_number = anims[item->anim_number].frame_base;
				VonCroy->LOT.is_jumping = 1;
				item->pos.x_pos = oEnemy->pos.x_pos;
				item->pos.y_pos = oEnemy->pos.y_pos;
				item->pos.z_pos = oEnemy->pos.z_pos;
				item->pos.x_rot = oEnemy->pos.x_rot;
				item->pos.y_rot = oEnemy->pos.y_rot;
				item->pos.z_rot = oEnemy->pos.z_rot;
				ifl3 = 1;
				VonCroy->reached_goal = 0;
				VonCroy->enemy = 0;
				item->item_flags[3] += ifl3;
				item->ai_bits = 16;
				break;

			case 6:

				if (lara.location > item->item_flags[3])
				{
					ifl3 = 1;
					VonCroy->reached_goal = 0;
					VonCroy->enemy = 0;
					item->item_flags[3] += ifl3;
					item->ai_bits = 16;
				}
				else if (item->meshswap_meshbits & 0x40080)
					item->goal_anim_state = 6;
				else
					item->goal_anim_state = 31;

				break;

			case 8:

				if (lara.location > item->item_flags[3])
				{
					ifl3 = 1;
					VonCroy->reached_goal = 0;
					VonCroy->enemy = 0;
					item->item_flags[3] += ifl3;
					item->ai_bits = 16;
				}

				item->goal_anim_state = 20;
				break;

			case 10:

				if (lara.location > item->item_flags[3])
				{
					ifl3 = 1;
					VonCroy->reached_goal = 0;
					VonCroy->enemy = 0;
					item->item_flags[3] += ifl3;
					item->ai_bits = 16;
				}

				item->goal_anim_state = 7;
				break;

			case 34:

				if (lara.location > item->item_flags[3])
				{
					ifl3 = 2;
					VonCroy->reached_goal = 0;
					VonCroy->enemy = 0;
					item->item_flags[3] += ifl3;
					item->ai_bits = 16;
				}

				item->goal_anim_state = 32;
				break;

			case 36:

				if (lara.location > item->item_flags[3])
				{
					ifl3 = 1;
					VonCroy->reached_goal = 0;
					VonCroy->enemy = 0;
					item->item_flags[3] += ifl3;
					item->ai_bits = 16;
				}

				item->goal_anim_state = 11;
				break;

			case 40:

				if (item->item_flags[2] == 6)
					item->goal_anim_state = 3;
				else
				{
					item->goal_anim_state = 34;
					item->pos.x_pos = oEnemy->pos.x_pos;
					item->pos.y_pos = oEnemy->pos.y_pos;
					item->pos.z_pos = oEnemy->pos.z_pos;
					item->pos.x_rot = oEnemy->pos.x_rot;
					item->pos.y_rot = oEnemy->pos.y_rot;
					item->pos.z_rot = oEnemy->pos.z_rot;
				}

				break;
			}
		}
		else if (jump_ahead || long_jump_ahead)
		{
			VonCroy->maximum_turn = 0;
			item->anim_number = objects[VON_CROY].anim_index + 22;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 15;

			if (long_jump_ahead)
				item->goal_anim_state = 16;
			else
				item->goal_anim_state = 15;

			VonCroy->LOT.is_jumping = 1;
		}
		else if (!(VonCroy->flags & 128))
		{
			if (oEnemy == lara_item && info.distance <= 0x64000)
				break;
		}
		else
		{
			floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
			h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
			c = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

			if (c == h - 1536)
				item->goal_anim_state = 4;
			else
				item->goal_anim_state = 2;
		}

		break;

	case 2:
		VonCroy->LOT.is_jumping = 0;
		VonCroy->LOT.is_monkeying = 0;
		VonCroy->maximum_turn = 1092;

		if (ahead)
			head_y = (short)iAngle;
		else if (info.ahead)
			head_y = info.angle;

		if (!savegame.Level.Timer)
			savegame.Level.Timer = 1;

		if (lara.location < item->item_flags[3])
			item->goal_anim_state = 1;
		else if (jump_ahead || long_jump_ahead)
			VonCroy->maximum_turn = 0;
		else if (VonCroy->flags & 128)
			item->goal_anim_state = 1;
		else if (!(VonCroy->flags & 8))
		{
			if (info.distance < 0x64000 && oEnemy->flags != 32)
				item->goal_anim_state = 1;
			else if (info.distance > 0x900000)
				item->goal_anim_state = 3;
		}
		else if (oEnemy->flags == 32)
		{
			ifl3 = -1;

			if (oEnemy)
			{
				room_number = oEnemy->room_number;
				floor = GetFloor(oEnemy->pos.x_pos, oEnemy->pos.y_pos, oEnemy->pos.z_pos, &room_number);
				GetHeight(floor, oEnemy->pos.x_pos, oEnemy->pos.y_pos, oEnemy->pos.z_pos);
				TestTriggers(trigger_index, 1, 0);
				ifl3 = 1;
			}

			VonCroy->reached_goal = 0;
			VonCroy->enemy = 0;
			item->item_flags[3] += ifl3;
			item->ai_bits = 16;
		}

		break;

	case 3:

		if (info.ahead)
			head_y = info.angle;

		if (item->frame_number == anims[item->anim_number].frame_base)
		{
			VonCroy->LOT.is_jumping = 0;
			VonCroy->maximum_turn = 1456;
		}

		angle >>= 1;

		if (item->item_flags[2] == 6)
		{
			VonCroy->maximum_turn = 0;
			item->goal_anim_state = 16;
		}
		else if (lara.location < item->item_flags[3] || jump_ahead)
			item->goal_anim_state = 1;
		else if (long_jump_ahead)
		{
			VonCroy->maximum_turn = 0;
			item->goal_anim_state = 16;
		}
		else if (VonCroy->flags & 128)
			item->goal_anim_state = 1;
		else if (VonCroy->flags & 8)
		{
			if (oEnemy->flags == 32)
			{
				ifl3 = -1;

				if (oEnemy)
				{
					room_number = oEnemy->room_number;
					floor = GetFloor(oEnemy->pos.x_pos, oEnemy->pos.y_pos, oEnemy->pos.z_pos, &room_number);
					GetHeight(floor, oEnemy->pos.x_pos, oEnemy->pos.y_pos, oEnemy->pos.z_pos);
					TestTriggers(trigger_index, 1, 0);
					ifl3 = 1;
				}

				VonCroy->reached_goal = 0;
				VonCroy->enemy = 0;
				item->item_flags[3] += ifl3;
				item->ai_bits = 16;
			}
			else if (info.distance < 0x4000)
			{
				if (oEnemy->flags == 40)
				{
					VonCroy->maximum_turn = 0;
					item->pos.y_rot = oEnemy->pos.y_rot;
					item->goal_anim_state = 16;
					item->item_flags[2] = 6;
				}
			}
			else
				item->goal_anim_state = 1;
		}
		else if (info.distance < 0x64000)
		{
			if (oEnemy->flags != 32 && oEnemy->flags != 40)
				item->goal_anim_state = 1;
		}

		break;

	case 4:
		VonCroy->maximum_turn = 0;

		if (item->box_number == VonCroy->LOT.target_box || VonCroy->monkey_ahead)
		{
			floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
			h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
			c = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

			if (c == h - 1536)
				item->goal_anim_state = 1;
		}
		else 
			item->goal_anim_state = 5;

		break;

	case 5:
		VonCroy->LOT.is_jumping = 1;
		VonCroy->LOT.is_monkeying = 1;
		VonCroy->maximum_turn = 1092;

		if (item->box_number == VonCroy->LOT.target_box || VonCroy->monkey_ahead)
		{
			floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
			h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
			c = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

			if (c == h - 1536)
				item->goal_anim_state = 4;
		}

		break;

	case 6:

		if (item->frame_number == anims[item->anim_number].frame_base + 28)
		{
			if (item->meshswap_meshbits & 0x40080)
				item->meshswap_meshbits &= ~0x40080;
			else
				item->meshswap_meshbits |= 0x40080;
		}

		break;

	case 15:

		if (item->anim_number == objects[39].anim_index + 23)
			item->goal_anim_state = 3;

		break;

	case 16:

		if (item->anim_number == objects[VON_CROY].anim_index + 25 || item->frame_number > anims[item->anim_number].frame_base + 7)
			VonCroy->LOT.is_jumping = 1;
		else if (jump_ahead)
			item->goal_anim_state = 15;
		else if (!objects[BAT].loaded)
			item->goal_anim_state = 3;

		if (item->item_flags[2] == 6)
			item->goal_anim_state = 33;

		break;

	case 20:

		if (item->frame_number == anims[item->anim_number].frame_base)
		{
			item->pos.x_pos = oEnemy->pos.x_pos;
			item->pos.y_pos = oEnemy->pos.y_pos;
			item->pos.z_pos = oEnemy->pos.z_pos;
			item->pos.x_rot = oEnemy->pos.x_rot;
			item->pos.y_rot = oEnemy->pos.y_rot;
			item->pos.z_rot = oEnemy->pos.z_rot;
		}
		else if (item->frame_number == anims[item->anim_number].frame_base + 120)
		{
			ifl3 = -1;

			if (oEnemy)
			{
				room_number = oEnemy->room_number;
				floor = GetFloor(oEnemy->pos.x_pos, oEnemy->pos.y_pos, oEnemy->pos.z_pos, &room_number);
				GetHeight(floor, oEnemy->pos.x_pos, oEnemy->pos.y_pos, oEnemy->pos.z_pos);
				TestTriggers(trigger_index, 1, 0);
				ifl3 = 1;
			}

			VonCroy->reached_goal = 0;
			VonCroy->enemy = 0;
			item->item_flags[3] += ifl3;
			item->ai_bits = 16;
		}

		break;

	case 22:
	case 35:
		VonCroy->maximum_turn = 0;

		if (item->item_flags[2])
			CreatureYRot(&item->pos, oEnemy->pos.y_rot - item->pos.y_rot, 512);
		else
			CreatureYRot(&item->pos, (short)iAngle, 512);

		break;

	case 31:

		if (info.ahead)
		{
			head_y = info.angle >> 1;
			torso_y = info.angle >> 1;
			torso_x = info.x_angle >> 1;
		}

		VonCroy->maximum_turn = 0;

		if (ABS(info.angle) < 1092)
			item->pos.y_rot += info.angle;
		else if (info.angle < 0)
			item->pos.y_rot -= 1092;
		else
			item->pos.y_rot += 1092;

		if (oEnemy && oEnemy->flags == 6 && item->frame_number > anims[item->anim_number].frame_base + 21)
		{
			ifl3 = -1;

			if (oEnemy)
			{
				room_number = oEnemy->room_number;
				floor = GetFloor(oEnemy->pos.x_pos, oEnemy->pos.y_pos, oEnemy->pos.z_pos, &room_number);
				GetHeight(floor, oEnemy->pos.x_pos, oEnemy->pos.y_pos, oEnemy->pos.z_pos);
				TestTriggers(trigger_index, 1, 0);
				ifl3 = 1;
			}

			VonCroy->reached_goal = 0;
			VonCroy->enemy = 0;
			item->item_flags[3] += ifl3;
			item->ai_bits = 16;
		}
		else if (!VonCroy->flags)
		{
			if (oEnemy)
			{
				if (item->frame_number > anims[item->anim_number].frame_base + 15 && item->frame_number < anims[item->anim_number].frame_base + 26)
				{
					if (ABS(oEnemy->pos.x_pos - item->pos.x_pos) < 512 &&
						ABS(oEnemy->pos.y_pos - item->pos.y_pos) <= 512 &&
						ABS(oEnemy->pos.z_pos - item->pos.z_pos) < 512)
					{
						oEnemy->hit_points -= 20;
						oEnemy->hit_status = 1;
						VonCroy->flags = 1;
						CreatureEffectT(item, &VonCroyBite, 8, -1, DoBloodSplat);
					}
				}
			}
		}

		break;

	case 33:

		if (item->anim_number != objects[VON_CROY].anim_index + 52 || item->frame_number != anims[item->anim_number].frame_base)
			ifl3 = 0;
		else
			ifl3 = 1;

		item->goal_anim_state = 2;
		item->item_flags[2] = 0;

		if (ifl3 == -1)
		{
			if (oEnemy)
			{
				room_number = oEnemy->room_number;
				floor = GetFloor(oEnemy->pos.x_pos, oEnemy->pos.y_pos, oEnemy->pos.z_pos, &room_number);
				GetHeight(floor, oEnemy->pos.x_pos, oEnemy->pos.y_pos, oEnemy->pos.z_pos);
				TestTriggers(trigger_index, 1, 0);
				ifl3 = 1;
			}
		}
		else if (!ifl3)
			break;

		VonCroy->reached_goal = 0;
		VonCroy->enemy = 0;
		item->item_flags[3] += ifl3;
		item->ai_bits = 16;
		break;

	case 34:
		item->item_flags[2] = 6;
		break;

	case 36:
	case 37:
		VonCroy->maximum_turn = 0;
		MoveCreature3DPos(&item->pos, &oEnemy->pos, 8, oEnemy->pos.y_rot - item->pos.y_rot, 512);
		break;
	}

	head_x = torso_x;
	CreatureTilt(item, angle);
	CreatureJoint(item, 0, torso_y);
	CreatureJoint(item, 1, torso_x);
	CreatureJoint(item, 2, head_y);
	CreatureJoint(item, 3, head_x);

	if (item->current_anim_state >= 15 || item->current_anim_state == 5)
		CreatureAnimation(item_number, angle, 0);
	else
	{
		switch (CreatureVault(item_number, angle, 2, 260))
		{
		case -4:
			VonCroy->maximum_turn = 0;
			item->anim_number = objects[VON_CROY].anim_index + 35;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 25;
			break;

		case -3:
			VonCroy->maximum_turn = 0;
			item->anim_number = objects[VON_CROY].anim_index + 41;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 24;
			break;

		case -2:
			VonCroy->maximum_turn = 0;
			item->anim_number = objects[VON_CROY].anim_index + 42;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 23;
			break;

		case 2:
			VonCroy->maximum_turn = 0;
			item->anim_number = objects[VON_CROY].anim_index + 29;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 19;
			break;

		case 3:
			VonCroy->maximum_turn = 0;
			item->anim_number = objects[VON_CROY].anim_index + 28;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 18;
			break;

		case 4:
			VonCroy->maximum_turn = 0;
			item->anim_number = objects[VON_CROY].anim_index + 27;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 17;
			break;
		}
	}
}

void inject_voncroy(bool replace)
{
	INJECT(0x00418D00, SetCutSceneCamera, replace);
	INJECT(0x00418F10, ClearCutSceneCamera, replace);
	INJECT(0x00418FA0, GetAIEnemy, replace);
	INJECT(0x00419070, DoVonCroyCutscene, replace);
	INJECT(0x00419560, InitialiseVoncroy, replace);
	INJECT(0x0041AF00, VoncroyRaceControl, replace);
}
