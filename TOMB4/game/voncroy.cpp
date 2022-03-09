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

void inject_voncroy(bool replace)
{
	INJECT(0x00418D00, SetCutSceneCamera, replace);
	INJECT(0x00418F10, ClearCutSceneCamera, replace);
	INJECT(0x00418FA0, GetAIEnemy, replace);
	INJECT(0x00419070, DoVonCroyCutscene, replace);
	INJECT(0x00419560, InitialiseVoncroy, replace);
}
