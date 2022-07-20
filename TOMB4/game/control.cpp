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
#include "traps.h"
#include "switch.h"
#include "lara_states.h"
#include "../specific/audio.h"
#include "lot.h"

#ifdef GENERAL_FIXES
#include "../specific/output.h"
#include "gameflow.h"
#include "../tomb4/tomb4.h"

char DeathMenuActive;

static long S_Death()
{
	long selection, menu, ret;

	CreateMonoScreen();
	selection = 0;
	menu = 0;
	ret = 0;
	DeathMenuActive = 1;

	while (!ret)
	{
		S_InitialisePolyList();
		SetDebounce = 1;
		S_UpdateInput();
		UpdatePulseColour();
		lara.death_count++;
		S_DisplayMonoScreen();

		if (MainThread.ended)
			return 4;

		if (Gameflow->LoadSaveEnabled)
		{
			if (!menu)	//"main" menu
			{
				PrintString((ushort)phd_centerx, (ushort)phd_centery, 3, SCRIPT_TEXT(TXT_GAME_OVER), FF_CENTER);
				PrintString((ushort)phd_centerx, ushort(phd_centery + 2 * font_height), !selection ? 1 : 2, SCRIPT_TEXT(TXT_Load_Game), FF_CENTER);
				PrintString((ushort)phd_centerx, ushort(phd_centery + 3 * font_height), selection == 1 ? 1 : 2, SCRIPT_TEXT(TXT_Exit_to_Title), FF_CENTER);

				if (selection)
				{
					if (dbinput & IN_FORWARD)
					{
						selection = 0;
						SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
					}

					if (dbinput & IN_SELECT)
					{
						lara.death_count = 0;
						ret = 1;
						SoundEffect(SFX_MENU_CHOOSE, 0, SFX_ALWAYS);
					}
				}
				else
				{
					if (dbinput & IN_BACK)
					{
						selection = 1;
						SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
					}

					if (dbinput & IN_SELECT)
					{
						menu = 1;
						SoundEffect(SFX_MENU_CHOOSE, 0, SFX_ALWAYS);
					}
				}
			}
			else if (menu == 1)	//reload
			{
				lara.death_count = 0;
				ret = go_and_load_game();

				if (ret)
				{
					if (ret > 0)
						ret = 2;
					else
					{
						menu = 0;
						ret = 0;
					}
				}
			}
		}
		else
		{
			PrintString((ushort)phd_centerx, (ushort)phd_centery, 3, SCRIPT_TEXT(TXT_GAME_OVER), FF_CENTER);

			if (lara.death_count > 300 || (lara.death_count > 150 && input != IN_NONE))
				return 1;
		}

		S_OutputPolyList();
		camera.number_frames = S_DumpScreen();
	}

	DeathMenuActive = 0;
	FreeMonoScreen();
	return ret;
}
#endif

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
#ifdef DISCORD_RPC
		RPC_Update();
#endif

	//	if (cdtrack > 0)
			//empty func call here

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
			if (keymap[DIK_ESCAPE] && !ScreenFading && !bDoCredits && tomb4.cutseq_skipper)
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
		
#ifdef GENERAL_FIXES
		if (tomb4.gameover)
		{
			if (reset_flag)
			{
				reset_flag = 0;
				return 1;
			}

			if (lara.death_count > 300 || lara.death_count > 90 && input)
			{
				reset_flag = 0;
				return S_Death();
			}
		}
		else
#endif
		{
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
		}

		if (demo_mode && input == IN_ALL)
			input = 0;

		if (!FadeScreenHeight)
		{
#ifdef GENERAL_FIXES
			if (input & IN_SAVE && lara_item->hit_points > 0)
				S_LoadSave(IN_SAVE, 0, 0);
#else
			if (input & IN_SAVE)
				S_LoadSave(IN_SAVE, 0);
#endif

			else if (input & IN_LOAD)
			{
#ifdef GENERAL_FIXES
				if (S_LoadSave(IN_LOAD, 0, 0) >= 0)
#else
				if (S_LoadSave(IN_LOAD, 0) >= 0)
#endif
					return 2;
			}

#ifdef GENERAL_FIXES
			if (input & IN_PAUSE && gfGameMode == 0 && lara_item->hit_points > 0)
#else
			if (input & IN_PAUSE && gfGameMode == 0)
#endif
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
			else if (!bVoncroyCutScene)
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

void FlipMap(long FlipNumber)
{
	ROOM_INFO* r;
	ROOM_INFO* flipped;
	CREATURE_INFO* cinfo;
	ROOM_INFO temp;

	for (int i = 0; i < number_rooms; i++)
	{
		r = &room[i];

		if (r->flipped_room >= 0 && r->FlipNumber == FlipNumber)
		{
#ifdef GENERAL_FIXES	//reset lighting room so objects take the new room's light...
			for (int j = r->item_number; j != NO_ITEM; j = items[j].next_item)
				items[j].il.room_number = 255;
#endif
			RemoveRoomFlipItems(r);
			flipped = &room[r->flipped_room];
			memcpy(&temp, r, sizeof(temp));
			memcpy(r, flipped, sizeof(ROOM_INFO));
			memcpy(flipped, &temp, sizeof(ROOM_INFO));
			r->flipped_room = flipped->flipped_room;
			flipped->flipped_room = -1;
			r->item_number = flipped->item_number;
			r->fx_number = flipped->fx_number;
			AddRoomFlipItems(r);
		}
	}

	flip_stats[FlipNumber] = !flip_stats[FlipNumber];
	flip_status = flip_stats[FlipNumber];

	for (short slot = 0; slot < 5; slot++)
	{
		cinfo = &baddie_slots[slot];
		cinfo->LOT.target_box = 2047;
	}
}

void RemoveRoomFlipItems(ROOM_INFO* r)
{
	ITEM_INFO* item;

	for (short item_num = r->item_number; item_num != NO_ITEM; item_num = item->next_item)
	{
		item = &items[item_num];

		if (item->flags & IFL_INVISIBLE && objects[item->object_number].intelligent)
		{
			if (item->hit_points <= 0 && item->hit_points != -16384)
				KillItem(item_num);
		}
	}
}

void AddRoomFlipItems(ROOM_INFO* r)
{
	ITEM_INFO* item;

	for (short item_num = r->item_number; item_num != NO_ITEM; item_num =item->next_item)
	{
		item = &items[item_num];

		if (items[item_num].object_number == RAISING_BLOCK1 && item->item_flags[1])
			AlterFloorHeight(item, -1024);

		if (item->object_number == RAISING_BLOCK2 && item->item_flags[1])
			AlterFloorHeight(item, -2048);
	}
}

void TestTriggers(short* data, long heavy, long HeavyFlags)
{
	ITEM_INFO* item;
	ITEM_INFO* camera_item;
	long switch_off, flip, flip_available, neweffect, key, quad;
	short camera_flags, camera_timer, type, trigger, value, flags, state;
	char timer;

	switch_off = 0;
	flip = -1;
	flip_available = 0;
	neweffect = -1;
	key = 0;
	HeavyTriggered = 0;

	if (!heavy)
	{
		lara.CanMonkeySwing = 0;
		lara.Unused2 = 0;
		lara.OnBeetleFloor = 0;
		lara.climb_status = 0;
	}

	if (!data)
		return;

	if ((*data & 0x1F) == LAVA_TYPE)
	{
		if (!heavy && (lara_item->pos.y_pos == lara_item->floor || lara.water_status != LW_ABOVE_WATER))
			LavaBurn(lara_item);

		if (*data & 0x8000)
			return;

		data++;
	}

	if ((*data & 0x1F) == CLIMB_TYPE)
	{
		if (!heavy)
		{
			quad = ushort(lara_item->pos.y_rot + 8192) >> 14;

			if ((1 << (quad + 8)) & *data)
				lara.climb_status = 1;
		}

		if (*data & 0x8000)
			return;

		data++;
	}

	if ((*data & 0x1F) == MONKEY_TYPE)
	{
		if (!heavy)
			lara.CanMonkeySwing = 1;

		if (*data & 0x8000)
			return;

		data++;
	}

	if ((*data & 0x1F) == TRIGTRIGGER_TYPE)
	{
		if (!(*data & 0x20) || *data & 0x8000)
			return;

		data++;
	}

	if ((*data & 0x1F) == MINER_TYPE)
	{
		if (!heavy)
			lara.OnBeetleFloor = 1;

		if (*data & 0x8000)
			return;

		data++;
	}

	type = (*data++ >> 8) & 0x3F;
	flags = *data++;
	timer = flags & 0xff;

	if (camera.type != HEAVY_CAMERA)
		RefreshCamera(type, data);

	if (heavy)
	{
		switch (type)
		{
		case HEAVY:
		case HEAVYANTITRIGGER:
			break;

		case HEAVYSWITCH:
			if (!HeavyFlags)
				return;

			if (HeavyFlags >= 0)
			{
				if ((flags & IFL_CODEBITS) != HeavyFlags)
					return;
			}
			else
			{
				flags |= IFL_CODEBITS;
				flags += (short)HeavyFlags;
			}

			break;

		default:
			return;
		}
	}
	else
	{
		switch (type)
		{
		case PAD:
		case ANTIPAD:

			if (lara_item->pos.y_pos != lara_item->floor)
				return;

			break;

		case SWITCH:
			value = *data++ & 0x3FF;

			if (flags & IFL_INVISIBLE)
				items[value].item_flags[0] = 1;

			if (!SwitchTrigger(value, timer))
				return;

			switch_off = items[value].current_anim_state == 1;
			break;

		case KEY:
			value = *data++ & 0x3FF;
			key = KeyTrigger(value);

			if (key != -1)
				break;

			return;

		case PICKUP:
			value = *data++ & 0x3FF;

			if (PickupTrigger(value))
				break;

			return;

		case HEAVY:
		case DUMMY:
		case HEAVYSWITCH:
		case HEAVYANTITRIGGER:
			return;

		case COMBAT:

			if (lara.gun_status == LG_READY)
				break;

			return;

		case MONKEY:
			state = lara_item->current_anim_state;

			if (state >= AS_HANG2 && (state <= AS_MONKEY180 || state == AS_HANGTURNL || state == AS_HANGTURNR))
				break;

			return;
		}
	}

	camera_item = NULL;

	do
	{
		trigger = *data++;
		value = trigger & 0x3FF;

		switch ((trigger & 0x3FFF) >> 10)
		{
		case TO_OBJECT:
			item = &items[value];

			if (key >= 2 || ((type == ANTIPAD || type == ANTITRIGGER || type == HEAVYANTITRIGGER) && item->flags & IFL_ANTITRIGGER_ONESHOT) ||
				(type == SWITCH && item->flags & IFL_SWITCH_ONESHOT) ||
				(type != SWITCH && type != ANTIPAD && type != ANTITRIGGER && type != HEAVYANTITRIGGER && item->flags & IFL_INVISIBLE) ||
				((type != ANTIPAD && type != ANTITRIGGER && type != HEAVYANTITRIGGER) && (item->object_number == DART_EMITTER && item->active)))
				break;

			item->timer = timer;

			if (timer != 1)
				item->timer *= 30;

			if (type == SWITCH || type == HEAVYSWITCH)
			{
				if (HeavyFlags >= 0)
				{
					item->flags ^= flags & IFL_CODEBITS;

					if (flags & IFL_INVISIBLE)
						item->flags |= IFL_SWITCH_ONESHOT;
				}
				else if (((item->flags ^ flags) & IFL_CODEBITS) == IFL_CODEBITS)
				{
					item->flags ^= (flags & IFL_CODEBITS);

					if (flags & IFL_INVISIBLE)
						item->flags |= IFL_SWITCH_ONESHOT;
				}
			}
			else if (type == ANTIPAD || type == ANTITRIGGER || type == HEAVYANTITRIGGER)
			{
				if (item->object_number == EARTHQUAKE)
				{
					item->item_flags[0] = 0;
					item->item_flags[1] = 100;
				}

				item->flags &= ~(IFL_CODEBITS | IFL_REVERSE);

				if (flags & IFL_INVISIBLE)
					item->flags |= IFL_ANTITRIGGER_ONESHOT;
			}
			else if (flags & IFL_CODEBITS)
				item->flags |= flags & IFL_CODEBITS;

			if ((item->flags & IFL_CODEBITS) != IFL_CODEBITS)
				break;

			item->flags |= IFL_TRIGGERED;

			if (flags & IFL_INVISIBLE)
				item->flags |= IFL_INVISIBLE;

			if (!item->active)
			{
				if (objects[item->object_number].intelligent)
				{
					if (item->status == ITEM_INACTIVE)
					{
						item->touch_bits = 0;
						item->status = ITEM_ACTIVE;
						AddActiveItem(value);
						EnableBaddieAI(value, 1);
					}
					else if (item->status == ITEM_INVISIBLE)
					{
						item->touch_bits = 0;

						if (EnableBaddieAI(value, 0))
							item->status = ITEM_ACTIVE;
						else
							item->status = ITEM_INVISIBLE;

						AddActiveItem(value);
					}
				}
				else
				{
					item->touch_bits = 0;
					AddActiveItem(value);
					item->status = ITEM_ACTIVE;
					HeavyTriggered = (uchar)heavy;
				}
			}

			break;

		case TO_CAMERA:
			trigger = *data++;
			camera_flags = trigger;
			camera_timer = trigger & 0xFF;

			if (key == 1 || camera.fixed[value].flags & IFL_INVISIBLE)
				break;

			camera.number = value;

			if ((camera.type == LOOK_CAMERA || camera.type == COMBAT_CAMERA) && !(camera.fixed[camera.number].flags & 1) ||
				type == COMBAT || type == SWITCH && timer && switch_off)
				break;

			if (camera.number != camera.last || type == SWITCH)
			{
				camera.timer = camera_timer * 30;

				if (camera_flags & IFL_INVISIBLE)
					camera.fixed[camera.number].flags |= IFL_INVISIBLE;

				camera.speed = ((camera_flags & IFL_CODEBITS) >> 6) + 1;

				if (heavy)
					camera.type = HEAVY_CAMERA;
				else
					camera.type = FIXED_CAMERA;
			}

			break;

		case TO_SINK:
			lara.current_active = value + 1;
			break;

		case TO_FLIPMAP:
			flip_available = 1;

			if (flipmap[value] & IFL_INVISIBLE)
				break;

			if (type == SWITCH)
				flipmap[value] ^= flags & IFL_CODEBITS;
			else if (flags & IFL_CODEBITS)
				flipmap[value] |= flags & IFL_CODEBITS;

			if ((flipmap[value] & IFL_CODEBITS) == IFL_CODEBITS)
			{
				if (flags & IFL_INVISIBLE)
					flipmap[value] |= IFL_INVISIBLE;

				if (!flip_stats[value])
					flip = value;
			}
			else if (flip_stats[value])
				flip = value;

			break;

		case TO_FLIPON:
			flip_available = 1;

			if ((flipmap[value] & IFL_CODEBITS) == IFL_CODEBITS && !flip_stats[value])
				flip = value;

			break;

		case TO_FLIPOFF:
			flip_available = 1;

			if ((flipmap[value] & IFL_CODEBITS) == IFL_CODEBITS && flip_stats[value])
				flip = value;

			break;

		case TO_TARGET:
			camera_item = &items[value];
			break;

		case TO_FINISH:
			gfLevelComplete = (char)value;
			gfRequiredStartPos = timer;
			break;

		case TO_CD:
			TriggerCDTrack(value, flags, type);
			break;

		case TO_FLIPEFFECT:
			TriggerTimer = timer;
			neweffect = value;
			break;

		case TO_SECRET:

			if (!(savegame.Level.Secrets & 1 << value))
			{
				S_CDPlay(5, 0);
				savegame.Level.Secrets |= 1 << value;
				savegame.Game.Secrets++;
			}

			break;

		case TO_FLYBY:
			trigger = *data++;
			camera_flags = trigger;
			camera_timer = trigger & 0xFF;

			if (key == 1)
				break;

			if (type == ANTIPAD || type == ANTITRIGGER || type == HEAVYANTITRIGGER)
			{
				bUseSpotCam = 0;
				break;
			}

			if (SpotCam[value].flags & SP_FLYBYONESHOT)
				break;

			if (camera_flags & IFL_INVISIBLE)
				SpotCam[value].flags |= SP_FLYBYONESHOT;

			if (bUseSpotCam)
				break;

			bUseSpotCam = 1;

			if (LastSequence != value)
				bTrackCamInit = 0;

			InitialiseSpotCam(value);
			break;

		case TO_CUTSCENE:

			if (value == 32)
			{
				if (!(FmvSceneTriggered & 0x80))
					DEL_playingamefmv = 1;
			}
			else if (!cutseq_trig && !CheckCutPlayed(value))
			{
				if (value == 8)
					SetCutPlayed(8);
				else if (value >= 17 && value <= 20)
				{
					if (lara.highest_location == value - 17)
						cutseq_num = value;
				}
				else if (value == 27)
				{
					if (lara_item->room_number == 55)
						cutseq_num = 27;
				}
				else if (value == 7)
				{
					if (CheckCutPlayed(8))
						cutseq_num = 8;
					else
					{
						cutseq_num = 7;
						SetCutPlayed(8);
					}
				}
				else
					cutseq_num = value;
			}

			break;

		default:
			break;
		}
	} while (!(trigger & 0x8000));

	if (camera_item && (camera.type == FIXED_CAMERA || camera.type == HEAVY_CAMERA))
		camera.item = camera_item;

	if (flip != -1)
		FlipMap(flip);

	if (neweffect != -1 && (flip || !flip_available))
	{
		flipeffect = neweffect;
		fliptimer = 0;
	}
}

short GetDoor(FLOOR_INFO* floor)
{
	short* data;
	short type;

	if (!floor->index)
		return 255;

	data = &floor_data[floor->index];
	type = *data++;

	if ((type & 0x1F) == TILT_TYPE || (type & 0x1F) == SPLIT1 || (type & 0x1F) == SPLIT2 || (type & 0x1F) == NOCOLF1B ||
		(type & 0x1F) == NOCOLF1T || (type & 0x1F) == NOCOLF2B || (type & 0x1F) == NOCOLF2T)
	{
		if (type & 0x8000)
			return 255;

		data++;
		type = *data++;
	}

	if ((type & 0x1F) == ROOF_TYPE || (type & 0x1F) == SPLIT3 || (type & 0x1F) == SPLIT4 || (type & 0x1F) == NOCOLC1B ||
		(type & 0x1F) == NOCOLC1T || (type & 0x1F) == NOCOLC2B || (type & 0x1F) == NOCOLC2T)
	{
		if (type & 0x8000)
			return 255;

		data++;
		type = *data++;
	}

	if ((type & 0x1F) == DOOR_TYPE)
		return *data;

	return 255;
}

long CheckNoColFloorTriangle(FLOOR_INFO* floor, long x, long z)
{
	short type;

	if (!floor->index)
		return 0;

	type = floor_data[floor->index] & 0x1F;

	if (type == NOCOLF1T || type == NOCOLF1B || type == NOCOLF2T || type == NOCOLF2B)
	{
		x &= 0x3FF;
		z &= 0x3FF;

		switch (type)
		{
		case NOCOLF1T:

			if (x <= 1024 - z)
				return -1;

			break;

		case NOCOLF1B:

			if (x > 1024 - z)
				return -1;

			break;

		case NOCOLF2T:

			if (x <= z)
				return -1;

			break;

		case NOCOLF2B:

			if (x > z)
				return -1;

			break;
		}

		return 1;
	}

	return 0;
}

long CheckNoColCeilingTriangle(FLOOR_INFO* floor, long x, long z)
{
	short* data;
	short type;

	if (!floor->index)
		return 0;

	data = &floor_data[floor->index];
	type = *data & 0x1F;

	if (type == TILT_TYPE || type == SPLIT1 || type == SPLIT2 || type == NOCOLF1T || type == NOCOLF1B || type == NOCOLF2T || type == NOCOLF2B)
	{
		if (*data & 0x8000)
			return 0;

		type = data[2] & 0x1F;
	}

	if (type == NOCOLC1T || type == NOCOLC1B || type == NOCOLC2T || type == NOCOLC2B)
	{
		x &= 0x3FF;
		z &= 0x3FF;

		switch (type)
		{
		case NOCOLC1T:

			if (x <= 1024 - z)
				return -1;

			break;

		case NOCOLC1B:

			if (x > 1024 - z)
				return -1;

			break;

		case NOCOLC2T:

			if (x <= z)
				return -1;

			break;

		case NOCOLC2B:

			if (x > z)
				return -1;

			break;
		}

		return 1;
	}

	return 0;
}

FLOOR_INFO* GetFloor(long x, long y, long z, short* room_number)
{
	ROOM_INFO* r;
	FLOOR_INFO* floor;
	long x_floor, y_floor;
	short door;

	r = &room[*room_number];

	do
	{
		x_floor = (z - r->z) >> 10;
		y_floor = (x - r->x) >> 10;

		if (x_floor <= 0)
		{
			x_floor = 0;

			if (y_floor < 1)
				y_floor = 1;
			else if (y_floor > r->y_size - 2)
				y_floor = r->y_size - 2;
		}
		else if (x_floor >= r->x_size - 1)
		{
			x_floor = r->x_size - 1;

			if (y_floor < 1)
				y_floor = 1;
			else if (y_floor > r->y_size - 2)
				y_floor = r->y_size - 2;
		}
		else if (y_floor < 0)
			y_floor = 0;
		else if (y_floor >= r->y_size)
			y_floor = r->y_size - 1;

		floor = &r->floor[x_floor + y_floor * r->x_size];
		door = GetDoor(floor);

		if (door == 255)
			break;

		*room_number = door;
		r = &room[door];

	} while (door != 255);

	if (y < floor->floor << 8)
	{
		if (y < floor->ceiling << 8 && floor->sky_room != 255)
		{
			do
			{
				if (CheckNoColCeilingTriangle(floor, x, z) == 1 || CheckNoColCeilingTriangle(floor, x, z) == -1 && y >= r->maxceiling)
					break;

				*room_number = floor->sky_room;
				r = &room[floor->sky_room];
				floor = &r->floor[((z - r->z) >> 10) + r->x_size * ((x - r->x) >> 10)];

				if (y >= floor->ceiling << 8)
					break;

			} while (floor->sky_room != 255);
		}
	}
	else if (floor->pit_room != 255)
	{
		while (1)
		{
			if (CheckNoColFloorTriangle(floor, x, z) == 1 || CheckNoColFloorTriangle(floor, x, z) == -1 && y < r->minfloor)
				break;

			*room_number = floor->pit_room;
			r = &room[floor->pit_room];
			floor = &r->floor[((z - r->z) >> 10) + r->x_size * ((x - r->x) >> 10)];

			if (y < floor->floor << 8)
				break;

			if (floor->pit_room == 255)
				return floor;
		}
	}

	return floor;
}

long GetWaterHeight(long x, long y, long z, short room_number)
{
	ROOM_INFO* r;
	FLOOR_INFO* floor;
	long x_floor, y_floor;
	short data;

	r = &room[room_number];

	do
	{
		x_floor = (z - r->z) >> 10;
		y_floor = (x - r->x) >> 10;

		if (x_floor <= 0)
		{
			x_floor = 0;

			if (y_floor < 1)
				y_floor = 1;
			else if (y_floor > r->y_size - 2)
				y_floor = r->y_size - 2;
		}
		else if (x_floor >= r->x_size - 1)
		{
			x_floor = r->x_size - 1;

			if (y_floor < 1)
				y_floor = 1;
			else if (y_floor > r->y_size - 2)
				y_floor = r->y_size - 2;
		}
		else if (y_floor < 0)
			y_floor = 0;
		else if (y_floor >= r->y_size)
			y_floor = r->y_size - 1;

		floor = &r->floor[x_floor + y_floor * r->x_size];
		data = GetDoor(floor);

		if (data != 255)
		{
			room_number = data;
			r = &room[data];
		}

	} while (data != 255);

	if (r->flags & ROOM_UNDERWATER)
	{
		while (floor->sky_room != 255)
		{
			if (CheckNoColCeilingTriangle(floor, x, z) == 1)
				break;

			r = &room[floor->sky_room];

			if (!(r->flags & ROOM_UNDERWATER))
				return r->minfloor;

			floor = &r->floor[((z - r->z) >> 10) + r->x_size * ((x - r->x) >> 10)];
		}

		return r->maxceiling;
	}
	else
	{
		while (floor->pit_room != 255)
		{
			if (CheckNoColFloorTriangle(floor, x, z) == 1)
				break;

			r = &room[floor->pit_room];

			if (r->flags & ROOM_UNDERWATER)
				return r->maxceiling;

			floor = &r->floor[((z - r->z) >> 10) + r->x_size * ((x - r->x) >> 10)];
		}
	}

	return NO_HEIGHT;
}

long GetHeight(FLOOR_INFO* floor, long x, long y, long z)
{
	ITEM_INFO* item;
	ROOM_INFO* r;
	short* data;
	long height;
	ushort trigger;
	short type, dx, dz, xoff, yoff, tilt, hadj, tilt0, tilt1, tilt2, tilt3;

	tiltxoff = 0;
	tiltyoff = 0;
	OnObject = 0;
	height_type = WALL;

	while (floor->pit_room != 255)
	{
		if (CheckNoColFloorTriangle(floor, x, z) == 1)
			break;

		r = &room[floor->pit_room];
		floor = &r->floor[((z - r->z) >> 10) + ((x - r->x) >> 10) * r->x_size];
	}

	height = floor->floor << 8;

	if (height == NO_HEIGHT)
		return height;

	trigger_index = 0;

	if (!floor->index)
		return height;

	data = &floor_data[floor->index];

	do
	{
		type = *data++;

		switch (type & 0x1F)
		{
		case DOOR_TYPE:
		case ROOF_TYPE:
		case SPLIT3:
		case SPLIT4:
		case NOCOLC1T:
		case NOCOLC1B:
		case NOCOLC2T:
		case NOCOLC2B:
			data++;
			break;

		case TILT_TYPE:
			xoff = *data >> 8;
			yoff = *(char*)data;
			tiltxoff = xoff;
			tiltyoff = yoff;

			if (ABS(xoff) > 2 || ABS(yoff) > 2)
				height_type = BIG_SLOPE;
			else
				height_type = SMALL_SLOPE;

			if (xoff < 0)
				height -= xoff * (z & 1023) >> 2;
			else
				height += xoff * ((-1 - z) & 1023) >> 2;

			if (yoff < 0)
				height -= yoff * (x & 1023) >> 2;
			else
				height += yoff * ((-1 - x) & 1023) >> 2;

			data++;
			break;

		case TRIGGER_TYPE:

			if (!trigger_index)
				trigger_index = data - 1;

			data++;

			do
			{
				trigger = *data++;

				if ((trigger & 0x3C00) != (TO_OBJECT << 10))
				{
					if ((trigger & 0x3C00) == (TO_CAMERA << 10) || (trigger & 0x3C00) == (TO_FLYBY << 10))
						trigger = *data++;

					continue;
				}

				item = &items[trigger & 0x3FF];

				if (objects[item->object_number].floor && !(item->flags & 0x8000))
					objects[item->object_number].floor(item, x, y, z, &height);

			} while (!(trigger & 0x8000));

			break;

		case LAVA_TYPE:
			trigger_index = data - 1;
			break;

		case CLIMB_TYPE:
		case MONKEY_TYPE:
		case TRIGTRIGGER_TYPE:
		case MINER_TYPE:

			if (!trigger_index)
				trigger_index = data - 1;

			break;

		case SPLIT1:
		case SPLIT2:
		case NOCOLF1T:
		case NOCOLF1B:
		case NOCOLF2T:
		case NOCOLF2B:
			tilt = *data;
			tilt0 = tilt & 0xF;
			tilt1 = (tilt >> 4) & 0xF;
			tilt2 = (tilt >> 8) & 0xF;
			tilt3 = (tilt >> 12) & 0xF;
			dx = x & 1023;
			dz = z & 1023;
			height_type = SPLIT_TRI;

			if ((type & 0x1F) == SPLIT1 || (type & 0x1F) == NOCOLF1T || (type & 0x1F) == NOCOLF1B)
			{
				if (dx > (1024 - dz))
				{
					hadj = (type >> 5) & 0x1F;

					if ((type >> 5) & 0x10)
						hadj |= 0xFFF0;

					height += hadj << 8;
					xoff = tilt3 - tilt0;
					yoff = tilt3 - tilt2;
				}
				else
				{
					hadj = (type >> 10) & 0x1F;

					if ((type >> 10) & 0x10)
						hadj |= 0xFFF0;

					height += hadj << 8;
					xoff = tilt2 - tilt1;
					yoff = tilt0 - tilt1;
				}
			}
			else
			{
				if (dx > dz)
				{
					hadj = (type >> 5) & 0x1F;

					if ((type >> 5) & 0x10)
						hadj |= 0xFFF0;

					height += hadj << 8;
					xoff = tilt3 - tilt0;
					yoff = tilt0 - tilt1;
				}
				else
				{

					hadj = (type >> 10) & 0x1F;

					if ((type >> 10) & 0x10)
						hadj |= 0xFFF0;

					height += hadj << 8;
					xoff = tilt2 - tilt1;
					yoff = tilt3 - tilt2;
				}
			}

			tiltxoff = xoff;
			tiltyoff = yoff;

			if (ABS(xoff) > 2 || ABS(yoff) > 2)
				height_type = DIAGONAL;

			if (xoff >= 0)
				height += xoff * ((-1 - z) & 1023) >> 2;
			else
				height -= xoff * (z & 1023) >> 2;

			if (yoff >= 0)
				height += yoff * ((-1 - x) & 1023) >> 2;
			else
				height -= yoff * (x & 1023) >> 2;

			data++;
			break;

		default:
			S_ExitSystem("GetHeight(): Unknown type");
			break;
		}

	} while (!(type & 0x8000));

	return height;
}

long GetCeiling(FLOOR_INFO* floor, long x, long y, long z)
{
	ITEM_INFO* item;
	ROOM_INFO* r;
	FLOOR_INFO* f;
	long xoff, yoff, height, h1, h2;
	short* data, type, trigger, dx, dz, t0, t1, t2, t3, hadj, ended;

	f = floor;

	while (f->sky_room != 255)
	{
		if (CheckNoColCeilingTriangle(floor, x, z) == 1)
			break;

		r = &room[f->sky_room];
		xoff = (z - r->z) >> 10;
		yoff = (x - r->x) >> 10;
		f = &r->floor[xoff + r->x_size * yoff];
	}

	height = f->ceiling << 8;

	if (height == NO_HEIGHT)
		return NO_HEIGHT;

	if (f->index)
	{
		data = &floor_data[f->index];
		type = *data++;
		ended = 0;

		if ((type & 0x1F) == TILT_TYPE || (type & 0x1F) == SPLIT1 || (type & 0x1F) == SPLIT2 || (type & 0x1F) == NOCOLF1T || (type & 0x1F) == NOCOLF1B || (type & 0x1F) == NOCOLF2T || (type & 0x1F) == NOCOLF2B)
		{
			data++;

			if (type & 0x8000)
				ended = 1;

			type = *data++;
		}

		if (!ended)
		{
			h1 = 0;
			h2 = 0;

			if ((type & 0x1F) != ROOF_TYPE)
			{
				if ((type & 0x1F) == SPLIT3 || (type & 0x1F) == SPLIT4 || (type & 0x1F) == NOCOLC1T || (type & 0x1F) == NOCOLC1B || (type & 0x1F) == NOCOLC2T || (type & 0x1F) == NOCOLC2B)
				{
					dx = x & 0x3FF;
					dz = z & 0x3FF;
					t0 = -(*data & 0xF);
					t1 = -(*data >> 4 & 0xF);
					t2 = -(*data >> 8 & 0xF);
					t3 = -(*data >> 12 & 0xF);

					if ((type & 0x1F) == SPLIT3 || (type & 0x1F) == NOCOLC1T || (type & 0x1F) == NOCOLC1B)
					{
						if (dx <= 1024 - dz)
						{
							hadj = type >> 10 & 0x1F;

							if (hadj & 0x10)
								hadj |= 0xFFF0;

							height += hadj << 8;
							h1 = t2 - t1;
							h2 = t3 - t2;
						}
						else
						{
							hadj = type >> 5 & 0x1F;

							if (hadj & 0x10)
								hadj |= 0xFFF0;

							height += hadj << 8;
							h1 = t3 - t0;
							h2 = t0 - t1;
						}
					}
					else
					{
						if (dx <= dz)
						{
							hadj = type >> 10 & 0x1F;

							if (hadj & 0x10)
								hadj |= 0xFFF0;

							height += hadj << 8;
							h1 = t2 - t1;
							h2 = t0 - t1;
						}
						else
						{
							hadj = type >> 5 & 0x1F;

							if (hadj & 0x10)
								hadj |= 0xFFF0;

							height += hadj << 8;
							h1 = t3 - t0;
							h2 = t3 - t2;
						}
					}
				}
			}
			else
			{
				h1 = *data >> 8;
				h2 = *(char*)data;
			}

			if (h1 < 0)
				height += (z & 0x3FF) * h1 >> 2;
			else
				height -= (-1 - z & 0x3FF) * h1 >> 2;

			if (h2 < 0)
				height += (-1 - x & 0x3FF) * h2 >> 2;
			else
				height -= (x & 0x3FF) * h2 >> 2;
		}
	}

	while (floor->pit_room != 255)
	{
		if (CheckNoColFloorTriangle(floor, x, z) == 1)
			break;

		r = &room[floor->pit_room];
		xoff = (z - r->z) >> 10;
		yoff = (x - r->x) >> 10;
		floor = &r->floor[xoff + r->x_size * yoff];
	}

	if (floor->index)
	{
		data = &floor_data[floor->index];

		do
		{
			type = *data++;

			switch (type & 0x1F)
			{
			case DOOR_TYPE:
			case TILT_TYPE:
			case ROOF_TYPE:
			case SPLIT1:
			case SPLIT2:
			case SPLIT3:
			case SPLIT4:
			case NOCOLF1T:
			case NOCOLF1B:
			case NOCOLF2T:
			case NOCOLF2B:
			case NOCOLC1T:
			case NOCOLC1B:
			case NOCOLC2T:
			case NOCOLC2B:
				data++;
				break;

			case TRIGGER_TYPE:
				data++;

				do
				{
					trigger = *data++;

					if ((trigger & 0x3C00) != (TO_OBJECT << 10))
					{
						if ((trigger & 0x3C00) == (TO_CAMERA << 10) || (trigger & 0x3C00) == (TO_FLYBY << 10))
							trigger = *data++;
					}
					else
					{
						item = &items[trigger & 0x3FF];

						if (objects[item->object_number].ceiling && !(item->flags & 0x8000))
							objects[item->object_number].ceiling(item, x, y, z, &height);
					}

				} while (!(trigger & 0x8000));
				break;

			case LAVA_TYPE:
			case CLIMB_TYPE:
			case MONKEY_TYPE:
			case TRIGTRIGGER_TYPE:
			case MINER_TYPE:
				break;

			default:
				S_ExitSystem("GetCeiling(): Unknown type");
				break;
			}
		} while (!(type & 0x8000));
	}

	return height;
}

void AlterFloorHeight(ITEM_INFO* item, long height)
{
	FLOOR_INFO* floor;
	FLOOR_INFO* ceiling;
	short room_num;

	room_num = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_num);
	ceiling = GetFloor(item->pos.x_pos, item->pos.y_pos + height - 1024, item->pos.z_pos, &room_num);

	if (floor->floor == -127)
		floor->floor = ceiling->ceiling + char(height >> 8);
	else
	{
		floor->floor += char(height >> 8);

		if (floor->floor == ceiling->ceiling)
			floor->floor = -127;
	}

	if (boxes[floor->box].overlap_index & 0x8000)
	{
		if (height >= 0)
			boxes[floor->box].overlap_index &= ~0x4000;
		else
			boxes[floor->box].overlap_index |= 0x4000;
	}
}

void TranslateItem(ITEM_INFO* item, short x, short y, short z)
{
	long s, c;

	c = phd_cos(item->pos.y_rot);
	s = phd_sin(item->pos.y_rot);
	item->pos.x_pos += (z * s + x * c) >> W2V_SHIFT;
	item->pos.y_pos += y;
	item->pos.z_pos += (z * c - x * s) >> W2V_SHIFT;
}

long GetChange(ITEM_INFO* item, ANIM_STRUCT* anim)
{
	CHANGE_STRUCT* change;
	RANGE_STRUCT* range;

	if (item->current_anim_state == item->goal_anim_state || anim->number_changes <= 0)
		return 0;

	change = &changes[anim->change_index];

	for (int i = 0; i < anim->number_changes; i++, change++)
	{
		if (change->goal_anim_state == item->goal_anim_state && change->number_ranges > 0)
		{
			range = &ranges[change->range_index];

			for (int j = 0; j < change->number_ranges; j++, range++)
			{
				if (item->frame_number >= range->start_frame && item->frame_number <= range->end_frame)
				{
					item->anim_number = range->link_anim_num;
					item->frame_number = range->link_frame_num;
					return 1;
				}
			}
		}
	}

	return 0;
}

void UpdateSky()
{
	if (gfLevelFlags & GF_LAYER1)
	{
		SkyPos += gfLayer1Vel;

		if (SkyPos > 0x2C00)
			SkyPos -= 0x2C00;
		else if (SkyPos < 0)
			SkyPos += 0x2C00;
	}

	if (gfLevelFlags & GF_LAYER2)
	{
		SkyPos2 += gfLayer2Vel;

		if (SkyPos2 > 0x2C00)
			SkyPos2 -= 0x2C00;
		else if (SkyPos2 < 0)
			SkyPos2 += 0x2C00;
	}
}

void KillMoveEffects()
{
	for (int i = 0; i < ItemNewRoomNo; i++)
	{
		if (ItemNewRooms[i][0] & 0x8000)
			KillEffect(ItemNewRooms[i][0] & 0x7FFF);
		else
			EffectNewRoom(ItemNewRooms[i][0], ItemNewRooms[i][1]);
	}

	ItemNewRoomNo = 0;
}

void inject_control(bool replace)
{
	INJECT(0x00449410, ControlPhase, replace);
	INJECT(0x0044C570, FlipMap, replace);
	INJECT(0x0044C670, RemoveRoomFlipItems, replace);
	INJECT(0x0044C6F0, AddRoomFlipItems, replace);
	INJECT(0x0044AB50, TestTriggers, replace);
	INJECT(0x0044BB20, GetDoor, replace);
	INJECT(0x0044C820, CheckNoColFloorTriangle, replace);
	INJECT(0x0044C8D0, CheckNoColCeilingTriangle, replace);
	INJECT(0x0044A1A0, GetFloor, replace);
	INJECT(0x0044A390, GetWaterHeight, replace);
	INJECT(0x0044A530, GetHeight, replace);
	INJECT(0x0044B690, GetCeiling, replace);
	INJECT(0x0044A0D0, AlterFloorHeight, replace);
	INJECT(0x0044A060, TranslateItem, replace);
	INJECT(0x00449FC0, GetChange, replace);
	INJECT(0x00449390, UpdateSky, replace);
	INJECT(0x00449330, KillMoveEffects, replace);
}
