#include "../tomb4/pch.h"
#include "control.h"
#include "pickup.h"
#include "../specific/function_stubs.h"
#include "../specific/input.h"
#include "newinv.h"
#include "../specific/LoadSave.h"
#include "../specific/3dmath.h"
#include "effect2.h"
#include "tomb4fx.h"
#include "items.h"
#include "box.h"
#include "laramisc.h"
#include "sound.h"
#include "hair.h"
#include "debris.h"
#include "text.h"
#include "effects.h"
#include "spotcam.h"
#include "camera.h"
#include "objects.h"
#include "scarab.h"
#include "croc.h"

long ControlPhase(long nframes, long demo_mode)
{
	ITEM_INFO* item;
	FX_INFO* fx;
	FLOOR_INFO* floor;
	MESH_INFO* mesh;
	short item_num, nex, fx_num;

	RegeneratePickups();

	if (nframes > 10)
		nframes = 10;

	if (bTrackCamInit)
		bUseSpotCam = 0;

	SetDebounce = 1;

	for (framecount += nframes; framecount > 0; framecount -= 2)
	{
		GlobalCounter++;
		UpdateSky();

		if (S_UpdateInput() == IN_ALL)
			return 0;

		if (bDisableLaraControl)
		{
			if (gfCurrentLevel)
				dbinput = 0;

			input &= IN_LOOK;
		}

		if (cutseq_trig)
		{
#ifdef CUTSEQ_SKIPPER
			if (keymap[DIK_ESCAPE] && !ScreenFading && !bDoCredits)
				cutseq_trig = 3;
#endif

			input = 0;

			if (cutseq_num == 27)
				input = IN_ACTION;
		}

		SetDebounce = 0;

		if (gfCurrentLevel && (dbinput & IN_OPTION || GLOBAL_enterinventory != -1) && !cutseq_trig && lara_item->hit_points > 0)
		{
			if (S_CallInventory2())
				return 2;
		}

		if (gfLevelComplete)
			return 3;
		
		if (reset_flag || lara.death_count > 300 || lara.death_count > 60 && input)
		{
			if (Gameflow->DemoDisc && reset_flag)
			{
				reset_flag = 0;
				return 4;
			}

			reset_flag = 0;
			return 1;
		}

		if (demo_mode && input == IN_ALL)
			input = 0;

		if (!FadeScreenHeight)
		{
			if (input & IN_SAVE)
				S_LoadSave(IN_SAVE, 0);
			else if (input & IN_LOAD)
			{
				if (S_LoadSave(IN_LOAD, 0) >= 0)
					return 2;
			}

			if (input & IN_PAUSE && gfGameMode == 0)
			{
				if (S_PauseMenu() == 8)
					return 1;
			}
		}

		if (MainThread.ended)
			return 4;

		if (input & IN_LOOK && (lara_item->current_anim_state == 2 && lara_item->anim_number == 103 ||
			(lara.IsDucked && input & IN_DUCK && lara_item->anim_number == 222 && lara_item->goal_anim_state == 71)))
		{
			if (!BinocularRange)
			{
				if (lara.gun_type == WEAPON_REVOLVER && lara.sixshooter_type_carried & 4 && lara.gun_status == LG_READY)
				{
					BinocularRange = 128;
					BinocularOldCamera = camera.old_type;
					lara.Busy = 1;
					LaserSight = 1;
				}
				else if (lara.gun_type == WEAPON_CROSSBOW && lara.crossbow_type_carried & 4 && lara.gun_status == LG_READY)
				{
					BinocularRange = 128;
					BinocularOldCamera = camera.old_type;
					lara.Busy = 1;
					LaserSight = 1;
				}
			}
		}
		else if (BinocularRange)
		{
			if (LaserSight)
			{
				BinocularRange = 0;
				LaserSight = 0;
				AlterFOV(14560);
				lara_item->mesh_bits = -1;
				lara.Busy = 0;
				camera.type = BinocularOldCamera;
				lara.move_angle = 0;
				lara.head_y_rot = 0;
				lara.head_z_rot = 0;
				lara.torso_y_rot = 0;
				BinocularOn = -8;
			}
			else
				input |= IN_LOOK;
		}

		ClearDynamics();
		ClearFires();
		GotLaraSpheres = 0;
		InItemControlLoop = 1;
		item_num = next_item_active;

		while (item_num != -1)
		{
			item = &items[item_num];
			nex = item->next_active;

			if (item->after_death < 128)
			{
				if (objects[item->object_number].control)
					objects[item->object_number].control(item_num);
			}
			else
				KillItem(item_num);

			item_num = nex;
		}

		InItemControlLoop = 0;
		KillMoveItems();
		InItemControlLoop = 1;
		fx_num = next_fx_active;

		while (fx_num != -1)
		{
			fx = &effects[fx_num];
			nex = fx->next_active;

			if (objects[fx->object_number].control)
				objects[fx->object_number].control(fx_num);

			fx_num = nex;
		}

		InItemControlLoop = 0;
		KillMoveEffects();

		if (KillEverythingFlag)
			KillEverything();

		if (SmokeCountL)
			SmokeCountL--;

		if (SmokeCountR)
			SmokeCountR--;

		if (SplashCount)
			SplashCount--;

		if (WeaponDelay)
			WeaponDelay--;

		if (lara.blindTimer)
			lara.blindTimer--;

		if (lara.has_fired && !(wibble & 0x7F))
		{
			AlertNearbyGuards(lara_item);
			lara.has_fired = 0;
		}

		XSoff1 += 150;
		YSoff1 += 230;
		ZSoff1 += 660;
		XSoff2 += 270;
		YSoff2 += 440;
		ZSoff2 += 160;

		if (lara.poisoned && !GLOBAL_playing_cutseq)
		{
			if (lara.poisoned > 4096)
				lara.poisoned = 4096;
			else if (lara.dpoisoned)
				lara.dpoisoned++;

			if (lara.poisoned >= 256 && !(wibble & 0xFF))
			{
				lara_item->hit_points -= lara.poisoned >> 8;
				PoisonFlag = 16;
			}
		}

		InItemControlLoop = 1;

		if (!GLOBAL_playing_cutseq && !gfGameMode)
			LaraControl(0);

		InItemControlLoop = 0;

		while (SmashedMeshCount)
		{
			SmashedMeshCount--;
			mesh = SmashedMesh[SmashedMeshCount];
			floor = GetFloor(mesh->x, mesh->y, mesh->z, &SmashedMeshRoom[SmashedMeshCount]);
			GetHeight(floor, mesh->x, mesh->y, mesh->z);
			TestTriggers(trigger_index, 1, 0);
			floor->stopper = 0;
			SmashedMesh[SmashedMeshCount] = 0;
		}

		KillMoveItems();

		if (GLOBAL_inventoryitemchosen != -1)
		{
			SayNo();
			GLOBAL_inventoryitemchosen = -1;
		}

		if (GLOBAL_playing_cutseq)
		{
			camera.type = CINEMATIC_CAMERA;
			CalculateCamera();
		}
		else
		{
			HairControl(0, 0, 0);

			if (gfLevelFlags & GF_YOUNGLARA)
				HairControl(0, 1, 0);

			if (bUseSpotCam)
				CalculateSpotCams();
			else if (!VC_InCut)
				CalculateCamera();
		}

		CamRot.y = (mGetAngle(camera.pos.z, camera.pos.x, camera.target.z, camera.target.x) >> 4) & 0xFFF;
		wibble = (wibble + 4) & 0xFC;
		TriggerLaraDrips();
		UpdateSparks();
		UpdateFireSparks();
		UpdateSmokeSparks();
		UpdateBubbles();
		UpdateSplashes();
		UpdateDebris();
		UpdateBlood();
		UpdateDrips();
		UpdateGunShells();
		UpdateScarabs();
		UpdateLocusts();
		UpdateShockwaves();
		UpdateLightning();
		AnimateWaterfalls();
		UpdatePulseColour();
		SoundEffects();
		health_bar_timer--;
		
		if (!gfGameMode)
		{
			GameTimer++;

			if (savegame.Level.Timer)
			{
				if (!GLOBAL_playing_cutseq)
					savegame.Level.Timer++;
			}
		}

		UpdateFadeClip();
	}

	return 0;
}

void inject_control(bool replace)
{
	INJECT(0x00449410, ControlPhase, replace);
}
