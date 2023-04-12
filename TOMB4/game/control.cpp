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
#include "../specific/output.h"
#include "gameflow.h"
#include "../tomb4/tomb4.h"
#include "lara1gun.h"
#include "sphere.h"
#include "draw.h"
#include "larafire.h"
#include "rope.h"
#include "voncroy.h"
#include "../specific/gamemain.h"
#include "lara.h"
#include "deltapak.h"
#include "health.h"
#include "../specific/dxshell.h"
#include "savegame.h"
#include "../specific/file.h"

ITEM_INFO* items;
ANIM_STRUCT* anims;
ROOM_INFO* room;
short** meshes;
long* bones;
long level_items;
short number_rooms;

short* OutsideRoomOffsets;
char* OutsideRoomTable;
short IsRoomOutsideNo;

MESH_INFO* SmashedMesh[16];
short SmashedMeshRoom[16];
short SmashedMeshCount = 0;

long flipmap[10];
long flip_stats[10];
long flip_status;
long flipeffect = -1;
long fliptimer = 0;

short* trigger_index;
long tiltxoff;
long tiltyoff;
long OnObject;
long height_type;

long InItemControlLoop = 0;
short ItemNewRooms[256][2];
short ItemNewRoomNo = 0;

uchar CurrentAtmosphere;
uchar IsAtmospherePlaying;
char cd_flags[128];

ulong FmvSceneTriggered;
ulong CutSceneTriggered;
long SetDebounce;
long framecount = 0;
long reset_flag = 0;
long WeaponDelay = 0;
long LaserSightX;
long LaserSightY;
long LaserSightZ;
ushort GlobalCounter = 0;
short XSoff1;
short XSoff2;
short YSoff1;
short YSoff2;
short ZSoff1;
short ZSoff2;
short FXType;
char PoisonFlag;
char TriggerTimer = 0;
char LaserSightActive = 0;
char DeathMenuActive;

static PHD_VECTOR ClosestCoord;
static long ClosestItem;
static long ClosestDist;

static long number_los_rooms = 0;
static short los_rooms[20];

static short cdtrack = -1;

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
				PrintString(phd_centerx, phd_centery, 3, SCRIPT_TEXT(TXT_GAME_OVER), FF_CENTER);
				PrintString(phd_centerx, phd_centery + 2 * font_height, !selection ? 1 : 2, SCRIPT_TEXT(TXT_Load_Game), FF_CENTER);
				PrintString(phd_centerx, phd_centery + 3 * font_height, selection == 1 ? 1 : 2, SCRIPT_TEXT(TXT_Exit_to_Title), FF_CENTER);

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
			PrintString(phd_centerx, phd_centery, 3, SCRIPT_TEXT(TXT_GAME_OVER), FF_CENTER);

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
		RPC_Update();

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
			if (keymap[DIK_ESCAPE] && !ScreenFading && !bDoCredits && tomb4.cutseq_skipper)
				cutseq_trig = 3;

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
			if (input & IN_SAVE && lara_item->hit_points > 0)
				S_LoadSave(IN_SAVE, 0, 0);

			else if (input & IN_LOAD)
			{
				if (S_LoadSave(IN_LOAD, 0, 0) >= 0)
					return 2;
			}

			if (input & IN_PAUSE && gfGameMode == 0 && lara_item->hit_points > 0)
			{
				if (S_PauseMenu() == 8)
					return 1;
			}
		}

		if (MainThread.ended)
			return 4;

		if (input & IN_LOOK && (lara_item->current_anim_state == AS_STOP && lara_item->anim_number == ANIM_BREATH ||
			(lara.IsDucked && !(input & IN_DUCK) && lara_item->anim_number == ANIM_DUCKBREATHE && lara_item->goal_anim_state == AS_DUCK)))
		{
			if (!BinocularRange)
			{
				if (lara.gun_type == WEAPON_REVOLVER && lara.sixshooter_type_carried & W_LASERSIGHT && lara.gun_status == LG_READY)
				{
					BinocularRange = 128;
					BinocularOldCamera = camera.old_type;
					lara.Busy = 1;
					LaserSight = 1;
				}
				else if (lara.gun_type == WEAPON_CROSSBOW && lara.crossbow_type_carried & W_LASERSIGHT && lara.gun_status == LG_READY)
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
				lara.head_x_rot = 0;
				lara.head_y_rot = 0;
				lara.torso_x_rot = 0;
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

		if (!GLOBAL_playing_cutseq)
		{
			HairControl(0, 0, 0);

			if (gfLevelFlags & GF_YOUNGLARA)
				HairControl(0, 1, 0);
		}

		if (GLOBAL_playing_cutseq)
		{
			camera.type = CINEMATIC_CAMERA;
			CalculateCamera();
		}
		else if (bUseSpotCam)
			CalculateSpotCams();
		else if (!bVoncroyCutScene)
			CalculateCamera();

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
			for (int j = r->item_number; j != NO_ITEM; j = items[j].next_item)
				items[j].il.room_number = 255;

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
	static uchar HeavyTriggered;
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

	camera_item = 0;

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

	if (y < GetMaximumFloor(floor, x, z))
	{
		if (y < GetMinimumCeiling(floor, x, z) && floor->sky_room != 255)
		{
			do
			{
				if (CheckNoColCeilingTriangle(floor, x, z) == 1)
					break;

				*room_number = floor->sky_room;
				r = &room[floor->sky_room];
				floor = &r->floor[((z - r->z) >> 10) + r->x_size * ((x - r->x) >> 10)];

				if (y >= GetMinimumCeiling(floor, x, z))
					break;

			} while (floor->sky_room != 255);
		}
	}
	else if (floor->pit_room != 255)
	{
		while (1)
		{
			if (CheckNoColFloorTriangle(floor, x, z) == 1)
				break;

			*room_number = floor->pit_room;
			r = &room[floor->pit_room];
			floor = &r->floor[((z - r->z) >> 10) + r->x_size * ((x - r->x) >> 10)];

			if (y < GetMaximumFloor(floor, x, z))
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
				break;

			floor = &r->floor[((z - r->z) >> 10) + r->x_size * ((x - r->x) >> 10)];
		}

		return GetMinimumCeiling(floor, x, z);
	}
	else
	{
		while (floor->pit_room != 255)
		{
			if (CheckNoColFloorTriangle(floor, x, z) == 1)
				break;

			r = &room[floor->pit_room];

			if (r->flags & ROOM_UNDERWATER)
				return GetMaximumFloor(floor, x, z);

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

			if (abs(xoff) > 2 || abs(yoff) > 2)
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

			if (abs(xoff) > 2 || abs(yoff) > 2)
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
			Log(0, "GetHeight(): Unknown type");
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
				Log(0, "GetCeiling(): Unknown type");
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

void KillMoveItems()
{
	for (int i = 0; i < ItemNewRoomNo; i++)
	{
		if (ItemNewRooms[i][0] & 0x8000)
			KillItem(ItemNewRooms[i][0] & 0x7FFF);
		else
			ItemNewRoom(ItemNewRooms[i][0], ItemNewRooms[i][1]);
	}

	ItemNewRoomNo = 0;
}

void RefreshCamera(short type, short* data)
{
	short trigger, value, target_ok;

	target_ok = 2;

	do
	{
		trigger = *data++;
		value = trigger & 0x3FF;

		if (((trigger >> 10) & 0xF) == TO_CAMERA)
		{
			data++;

			if (value == camera.last)
			{
				camera.number = trigger & 0x3FF;

				if (camera.timer >= 0 && (camera.type != LOOK_CAMERA && camera.type != COMBAT_CAMERA || camera.fixed[camera.number].flags & 3))
				{
					camera.type = FIXED_CAMERA;
					target_ok = 1;
					continue;
				}

				camera.timer = -1;
			}

			target_ok = 0;
		}
		else if (((trigger >> 10) & 0xF) == TO_TARGET)
		{
			if (camera.type != LOOK_CAMERA && camera.type != COMBAT_CAMERA || camera.number == NO_ITEM || camera.fixed[camera.number].flags & 1)
				camera.item = &items[value];
		}

	} while (!(trigger & 0x8000));

	if (camera.item)
	{
		if (!target_ok || target_ok == 2 && camera.item->looked_at && camera.item != camera.last_item)
			camera.item = 0;
	}

	if (camera.number == -1 && camera.timer > 0)
		camera.timer = -1;
}

long TriggerActive(ITEM_INFO* item)
{
	long reverse;

	reverse = (item->flags & IFL_REVERSE) ? 1 : 0;

	if ((item->flags & IFL_CODEBITS) != IFL_CODEBITS)
		return reverse;

	if (!item->timer)
		return !reverse;

	if (item->timer > 0)
	{
		item->timer--;

		if (!item->timer)
			item->timer = -1;
	}
	else if (item->timer < -1)
	{
		item->timer++;

		if (item->timer == -1)
			item->timer = 0;
	}

	if (item->timer <= -1)
		return reverse;

	return !reverse;
}

void TriggerNormalCDTrack(short value, short flags, short type)
{
	long code;

	if (value >= 105 && value <= 111 || value == 102 || value == 97)
	{
		if (CurrentAtmosphere != value)
		{
			CurrentAtmosphere = (uchar)value;

			if (IsAtmospherePlaying)
				S_CDPlay(value, 1);
		}
	}
	else
	{
		code = (flags >> 8) & 0x3F;	//(IFL_CODEBITS | IFL_INVISIBLE)= 0x3F00, then >> 8, 3F

		if ((cd_flags[value] & code) != code)
		{
			cd_flags[value] |= code;
			S_CDPlay(value, 0);
			IsAtmospherePlaying = 0;
		}
	}
}

void TriggerCDTrack(short value, short flags, short type)
{
	if (value < 128)
		TriggerNormalCDTrack(value, flags, type);
}

long ClipTarget(GAME_VECTOR* start, GAME_VECTOR* target)
{
	GAME_VECTOR src;
	long dx, dy, dz;
	short room_no;

	room_no = target->room_number;

	if (target->y > GetHeight(GetFloor(target->x, target->y, target->z, &room_no), target->x, target->y, target->z))
	{
		src.x = (7 * (target->x - start->x) >> 3) + start->x;
		src.y = (7 * (target->y - start->y) >> 3) + start->y;
		src.z = (7 * (target->z - start->z) >> 3) + start->z;

		for (int i = 3; i > 0; i--)
		{
			dx = ((target->x - src.x) * i >> 2) + src.x;
			dy = ((target->y - src.y) * i >> 2) + src.y;
			dz = ((target->z - src.z) * i >> 2) + src.z;

			if (dy < GetHeight(GetFloor(dx, dy, dz, &room_no), dx, dy, dz))
				break;
		}

		target->x = dx;
		target->y = dy;
		target->z = dz;
		target->room_number = room_no;
		return 0;
	}

	room_no = target->room_number;

	if (target->y < GetCeiling(GetFloor(target->x, target->y, target->z, &room_no), target->x, target->y, target->z))
	{
		src.x = (7 * (target->x - start->x) >> 3) + start->x;
		src.y = (7 * (target->y - start->y) >> 3) + start->y;
		src.z = (7 * (target->z - start->z) >> 3) + start->z;

		for (int i = 3; i > 0; i--)
		{
			dx = ((target->x - src.x) * i >> 2) + src.x;
			dy = ((target->y - src.y) * i >> 2) + src.y;
			dz = ((target->z - src.z) * i >> 2) + src.z;

			if (dy > GetCeiling(GetFloor(dx, dy, dz, &room_no), dx, dy, dz))
				break;
		}

		target->x = dx;
		target->y = dy;
		target->z = dz;
		target->room_number = room_no;
		return 0;
	}

	return 1;
}

long xLOS(GAME_VECTOR* start, GAME_VECTOR* target)
{
	FLOOR_INFO* floor;
	long dx, dy, dz, x, y, z;
	short room_number, last_room;

	dx = target->x - start->x;

	if (!dx)
		return 1;

	dy = 1024 * (target->y - start->y) / dx;
	dz = 1024 * (target->z - start->z) / dx;
	number_los_rooms = 1;
	los_rooms[0] = start->room_number;
	room_number = start->room_number;
	last_room = start->room_number;

	if (dx < 0)
	{
		x = start->x & ~1023;
		y = ((x - start->x) * dy >> 10) + start->y;
		z = ((x - start->x) * dz >> 10) + start->z;

		while (x > target->x)
		{
			floor = GetFloor(x, y, z, &room_number);

			if (room_number != last_room)
			{
				last_room = room_number;
				los_rooms[number_los_rooms] = room_number;
				number_los_rooms++;
			}

			if (y > GetHeight(floor, x, y, z) || y < GetCeiling(floor, x, y, z))
			{
				target->x = x;
				target->y = y;
				target->z = z;
				target->room_number = room_number;
				return -1;
			}

			floor = GetFloor(x - 1, y, z, &room_number);

			if (room_number != last_room)
			{
				last_room = room_number;
				los_rooms[number_los_rooms] = room_number;
				number_los_rooms++;
			}

			if (y > GetHeight(floor, x - 1, y, z) || y < GetCeiling(floor, x - 1, y, z))
			{
				target->x = x;
				target->y = y;
				target->z = z;
				target->room_number = last_room;
				return 0;
			}

			x -= 1024;
			y -= dy;
			z -= dz;
		}
	}
	else
	{
		x = start->x | 1023;
		y = ((x - start->x) * dy >> 10) + start->y;
		z = ((x - start->x) * dz >> 10) + start->z;

		while (x < target->x)
		{
			floor = GetFloor(x, y, z, &room_number);

			if (room_number != last_room)
			{
				last_room = room_number;
				los_rooms[number_los_rooms] = room_number;
				number_los_rooms++;
			}

			if (y > GetHeight(floor, x, y, z) || y < GetCeiling(floor, x, y, z))
			{
				target->x = x;
				target->y = y;
				target->z = z;
				target->room_number = room_number;
				return -1;
			}

			floor = GetFloor(x + 1, y, z, &room_number);

			if (room_number != last_room)
			{
				last_room = room_number;
				los_rooms[number_los_rooms] = room_number;
				number_los_rooms++;
			}

			if (y > GetHeight(floor, x + 1, y, z) || y < GetCeiling(floor, x + 1, y, z))
			{
				target->x = x;
				target->y = y;
				target->z = z;
				target->room_number = last_room;
				return 0;
			}

			x += 1024;
			y += dy;
			z += dz;
		}
	}

	target->room_number = room_number;
	return 1;
}

long zLOS(GAME_VECTOR* start, GAME_VECTOR* target)
{
	FLOOR_INFO* floor;
	long dx, dy, dz, x, y, z;
	short room_number, last_room;

	dz = target->z - start->z;

	if (!dz)
		return 1;

	dx = 1024 * (target->x - start->x) / dz;
	dy = 1024 * (target->y - start->y) / dz;
	number_los_rooms = 1;
	los_rooms[0] = start->room_number;
	room_number = start->room_number;
	last_room = start->room_number;

	if (dz < 0)
	{
		z = start->z & ~1023;
		x = ((z - start->z) * dx >> 10) + start->x;
		y = ((z - start->z) * dy >> 10) + start->y;

		while (z > target->z)
		{
			floor = GetFloor(x, y, z, &room_number);

			if (room_number != last_room)
			{
				last_room = room_number;
				los_rooms[number_los_rooms] = room_number;
				number_los_rooms++;
			}

			if (y > GetHeight(floor, x, y, z) || y < GetCeiling(floor, x, y, z))
			{
				target->x = x;
				target->y = y;
				target->z = z;
				target->room_number = room_number;
				return -1;
			}

			floor = GetFloor(x, y, z - 1, &room_number);

			if (room_number != last_room)
			{
				last_room = room_number;
				los_rooms[number_los_rooms] = room_number;
				number_los_rooms++;
			}

			if (y > GetHeight(floor, x, y, z - 1) || y < GetCeiling(floor, x, y, z - 1))
			{
				target->x = x;
				target->y = y;
				target->z = z;
				target->room_number = last_room;
				return 0;
			}

			z -= 1024;
			x -= dx;
			y -= dy;
		}
	}
	else
	{
		z = start->z | 1023;
		x = ((z - start->z) * dx >> 10) + start->x;
		y = ((z - start->z) * dy >> 10) + start->y;

		while (z < target->z)
		{
			floor = GetFloor(x, y, z, &room_number);

			if (room_number != last_room)
			{
				last_room = room_number;
				los_rooms[number_los_rooms] = room_number;
				number_los_rooms++;
			}

			if (y > GetHeight(floor, x, y, z) || y < GetCeiling(floor, x, y, z))
			{
				target->x = x;
				target->y = y;
				target->z = z;
				target->room_number = room_number;
				return -1;
			}

			floor = GetFloor(x, y, z + 1, &room_number);

			if (room_number != last_room)
			{
				last_room = room_number;
				los_rooms[number_los_rooms] = room_number;
				number_los_rooms++;
			}

			if (y > GetHeight(floor, x, y, z + 1) || y < GetCeiling(floor, x, y, z + 1))
			{
				target->x = x;
				target->y = y;
				target->z = z;
				target->room_number = last_room;
				return 0;
			}

			z += 1024;
			x += dx;
			y += dy;
		}
	}

	target->room_number = room_number;
	return 1;
}

long LOS(GAME_VECTOR* start, GAME_VECTOR* target)
{
	long los1, los2;

	target->room_number = start->room_number;

	if (abs(target->z - start->z) > abs(target->x - start->x))
	{
		los1 = xLOS(start, target);
		los2 = zLOS(start, target);
	}
	else
	{
		los1 = zLOS(start, target);
		los2 = xLOS(start, target);
	}

	if (los2)
	{
		GetFloor(target->x, target->y, target->z, &target->room_number);

		if (ClipTarget(start, target) && los1 == 1 && los2 == 1)
			return 1;
	}

	return 0;
}

void FireCrossBowFromLaserSight(GAME_VECTOR* start, GAME_VECTOR* target)
{
	PHD_3DPOS pos;
	short angles[2];

	phd_GetVectorAngles(target->x - start->x, target->y - start->y, target->z - start->z, angles);
	pos.x_pos = start->x;
	pos.y_pos = start->y;
	pos.z_pos = start->z;
	pos.x_rot = angles[1];
	pos.y_rot = angles[0];
	pos.z_rot = 0;
	FireCrossbow(&pos);
}

long ExplodeItemNode(ITEM_INFO* item, long Node, long NoXZVel, long bits)
{
	OBJECT_INFO* object;
	short** meshpp;

	if (!(item->mesh_bits & (1 << Node)))
		return 0;

	if (bits == 256)
		bits = -64;
	else
		SoundEffect(SFX_HIT_ROCK, &item->pos, SFX_DEFAULT);

	GetSpheres(item, Slist, 3);
	object = &objects[item->object_number];
	meshpp = &meshes[object->mesh_index + Node * 2];
	ShatterItem.Bit = 1 << Node;
	ShatterItem.meshp = *meshpp;
	ShatterItem.Sphere.x = Slist[Node].x;
	ShatterItem.Sphere.y = Slist[Node].y;
	ShatterItem.Sphere.z = Slist[Node].z;
	ShatterItem.YRot = item->pos.y_rot;
	ShatterItem.il = &item->il;
	ShatterItem.Flags = item->object_number != CROSSBOW_BOLT ? 0 : 1024;
	ShatterObject(&ShatterItem, 0, (short)bits, item->room_number, NoXZVel);
	item->mesh_bits &= ~ShatterItem.Bit;
	return 1;
}

long IsRoomOutside(long x, long y, long z)
{
	ROOM_INFO* r;
	FLOOR_INFO* floor;
	uchar* pTable;
	long h, c;
	short offset, room_no;

	if (x < 0 || z < 0)
		return -2;

	offset = OutsideRoomOffsets[27 * (x >> 12) + (z >> 12)];

	if (offset == -1)
		return -2;

	if (offset < 0)
	{
		r = &room[offset & 0x7FFF];

		if (y >= r->maxceiling && y <= r->minfloor &&
			z >= r->z + 1024 && z <= ((r->x_size - 1) << 10) + r->z &&
			x >= r->x + 1024 && x <= ((r->y_size - 1) << 10) + r->x)
		{
			IsRoomOutsideNo = offset & 0x7FFF;
			room_no = IsRoomOutsideNo;
			floor = GetFloor(x, y, z, &room_no);
			h = GetHeight(floor, x, y, z);
			c = GetCeiling(floor, x, y, z);

			if (h == NO_HEIGHT || y > h || y < c)
				return -2;

			if (r->flags & (ROOM_UNDERWATER | ROOM_NOT_INSIDE))
				return 1;
			else
				return -3;
		}
	}
	else
	{
		pTable = (uchar*)&OutsideRoomTable[offset];

		while (*pTable != 255)
		{
			r = &room[*pTable];

			if (y >= r->maxceiling && y <= r->minfloor &&
				z >= r->z + 1024 && z <= ((r->x_size - 1) << 10) + r->z &&
				x >= r->x + 1024 && x <= ((r->y_size - 1) << 10) + r->x)
			{
				IsRoomOutsideNo = *pTable;
				room_no = IsRoomOutsideNo;
				floor = GetFloor(x, y, z, &room_no);
				h = GetHeight(floor, x, y, z);
				c = GetCeiling(floor, x, y, z);

				if (h == NO_HEIGHT || y > h || y < c)
					return -2;

				if (r->flags & (ROOM_UNDERWATER | ROOM_NOT_INSIDE))
					return 1;
				else
					return -3;
			}

			pTable++;
		}
	}

	return -2;
}

long ObjectOnLOS2(GAME_VECTOR* start, GAME_VECTOR* target, PHD_VECTOR* Coord, MESH_INFO** StaticMesh)
{
	ITEM_INFO* item;
	MESH_INFO* mesh;
	ROOM_INFO* r;
	PHD_3DPOS ItemPos;
	short* bounds;
	long dx, dy, dz;
	short item_number;

	dx = target->x - start->x;
	dy = target->y - start->y;
	dz = target->z - start->z;
	ClosestItem = 999;
	ClosestDist = SQUARE(dx) + SQUARE(dy) + SQUARE(dz);

	for (int i = 0; i < number_los_rooms; i++)
	{
		r = &room[los_rooms[i]];

		for (item_number = r->item_number; item_number != NO_ITEM; item_number = item->next_item)
		{
			item = &items[item_number];

			if (item->status != ITEM_DEACTIVATED && item->status != ITEM_INVISIBLE && item->object_number != LARA)
			{
				bounds = GetBoundsAccurate(item);
				ItemPos.x_pos = item->pos.x_pos;
				ItemPos.y_pos = item->pos.y_pos;
				ItemPos.z_pos = item->pos.z_pos;
				ItemPos.y_rot = item->pos.y_rot;

				if (DoRayBox(start, target, bounds, &ItemPos, Coord, item_number))
					target->room_number = los_rooms[i];
			}
		}

		for (int j = 0; j < r->num_meshes; j++)
		{
			mesh = &r->mesh[j];

			if (mesh->Flags & 1)
			{
				ItemPos.x_pos = mesh->x;
				ItemPos.y_pos = mesh->y;
				ItemPos.z_pos = mesh->z;
				ItemPos.y_rot = mesh->y_rot;

				if (DoRayBox(start, target, &static_objects[mesh->static_number].x_minc, &ItemPos, Coord, -1 - mesh->static_number))
				{
					*StaticMesh = mesh;
					target->room_number = los_rooms[i];
				}
			}
		}
	}

	Coord->x = ClosestCoord.x;
	Coord->y = ClosestCoord.y;
	Coord->z = ClosestCoord.z;
	return ClosestItem;
}

long GetTargetOnLOS(GAME_VECTOR* src, GAME_VECTOR* dest, long DrawTarget, long firing)
{
	ITEM_INFO* shotitem;
	MESH_INFO* Mesh;
	GAME_VECTOR target;
	PHD_VECTOR v;
	short item_no, hit, ricochet, room_number, TriggerItems[8], NumTrigs;

	hit = 0;
	target.x = dest->x;
	target.y = dest->y;
	target.z = dest->z;
	ricochet = (short)LOS(src, &target);
	GetFloor(target.x, target.y, target.z, &target.room_number);

	if (firing && LaserSight)
	{
		if (lara.gun_type == WEAPON_REVOLVER)
			SoundEffect(SFX_DESERT_EAGLE_FIRE, 0, SFX_DEFAULT);
	}

	item_no = (short)ObjectOnLOS2(src, dest, &v, &Mesh);

	if (item_no != 999)
	{
		target.x = v.x - ((v.x - src->x) >> 5);
		target.y = v.y - ((v.y - src->y) >> 5);
		target.z = v.z - ((v.z - src->z) >> 5);

		if (item_no >= 0 && DrawTarget)
			lara.target = &items[item_no];

		if (firing)
		{
			if (lara.gun_type != WEAPON_CROSSBOW)
			{
				if (item_no < 0)
				{
					if (Mesh->static_number >= SHATTER0 && Mesh->static_number < SHATTER8)
					{
						ShatterObject(0, Mesh, 128, target.room_number, 0);
						SmashedMeshRoom[SmashedMeshCount] = target.room_number;
						SmashedMesh[SmashedMeshCount] = Mesh;
						SmashedMeshCount++;
						Mesh->Flags &= ~1;
						SoundEffect(SFX_HIT_ROCK, (PHD_3DPOS*)Mesh, SFX_DEFAULT);
					}

					TriggerRicochetSpark(&target, lara_item->pos.y_rot, 3, 0);
					TriggerRicochetSpark(&target, lara_item->pos.y_rot, 3, 0);
				}
				else
				{
					shotitem = &items[item_no];

					if (shotitem->object_number != SWITCH_TYPE7 && shotitem->object_number != SWITCH_TYPE8)
					{
						if (objects[shotitem->object_number].explodable_meshbits & ShatterItem.Bit && LaserSight)
						{
							ShatterObject(&ShatterItem, 0, 128, target.room_number, 0);
							shotitem->mesh_bits &= ~ShatterItem.Bit;
							TriggerRicochetSpark(&target, lara_item->pos.y_rot, 3, 0);
						}
						else if (DrawTarget && lara.gun_type == WEAPON_REVOLVER)
						{
							if (objects[shotitem->object_number].intelligent)
								HitTarget(shotitem, &target, weapons[lara.gun_type].damage, 0);
						}
						else
						{
							if (objects[shotitem->object_number].HitEffect == 1)
								DoBloodSplat(target.x, target.y, target.z, (GetRandomControl() & 3) + 3, shotitem->pos.y_rot, shotitem->room_number);
							else if (objects[shotitem->object_number].HitEffect == 2)
								TriggerRicochetSpark(&target, lara_item->pos.y_rot, 3, -5);
							else if (objects[shotitem->object_number].HitEffect == 3)
								TriggerRicochetSpark(&target, lara_item->pos.y_rot, 3, 0);

							shotitem->hit_status = 1;

							if (!objects[shotitem->object_number].undead)
								shotitem->hit_points -= weapons[lara.gun_type].damage;
						}
					}
					else
					{
						if (ShatterItem.Bit == 1 << (objects[shotitem->object_number].nmeshes - 1) && !(shotitem->flags & IFL_SWITCH_ONESHOT))
						{
							if (shotitem->object_number == SWITCH_TYPE7)
								ExplodeItemNode(shotitem, objects[shotitem->object_number].nmeshes - 1, 0, 64);

							if (shotitem->flags & IFL_CODEBITS && (shotitem->flags & IFL_CODEBITS) != IFL_CODEBITS)
							{
								room_number = shotitem->room_number;
								GetHeight(GetFloor(shotitem->pos.x_pos, shotitem->pos.y_pos - 256, shotitem->pos.z_pos, &room_number), shotitem->pos.x_pos, shotitem->pos.y_pos - 256, shotitem->pos.z_pos);
								TestTriggers(trigger_index, 1, shotitem->flags & IFL_CODEBITS);
							}
							else
							{
								NumTrigs = (short)GetSwitchTrigger(shotitem, TriggerItems, 1);

								for (int i = NumTrigs - 1; i >= 0; i--)
								{
									AddActiveItem(TriggerItems[i]);
									items[TriggerItems[i]].status = ITEM_ACTIVE;
									items[TriggerItems[i]].flags |= IFL_CODEBITS;
								}
							}

							AddActiveItem(item_no);
							shotitem->status = ITEM_ACTIVE;
							shotitem->flags |= IFL_SWITCH_ONESHOT | IFL_CODEBITS;
						}

						TriggerRicochetSpark(&target, lara_item->pos.y_rot, 3, 0);
					}
				}
			}
			else if (LaserSight)
				FireCrossBowFromLaserSight(src, &target);
		}

		hit = 1;
	}
	else if (lara.gun_type != WEAPON_CROSSBOW)
	{
		target.x -= (target.x - src->x) >> 5;
		target.y -= (target.y - src->y) >> 5;
		target.z -= (target.z - src->z) >> 5;

		if (firing && !ricochet)
			TriggerRicochetSpark(&target, lara_item->pos.y_rot, 8, 0);
	}
	else if (firing && LaserSight)
		FireCrossBowFromLaserSight(src, &target);

	if (DrawTarget && (hit || !ricochet))
	{
		LaserSightActive = 1;
		LaserSightX = target.x;
		LaserSightY = target.y;
		LaserSightZ = target.z;
		TriggerDynamic(target.x, target.y, target.z, 16, 32, 0, 0);
	}

	return hit;
}

void AnimateItem(ITEM_INFO* item)
{
	ANIM_STRUCT* anim;
	short* cmd;
	long speed, speed2;
	ushort type, num;

	anim = &anims[item->anim_number];
	item->touch_bits = 0;
	item->hit_status = 0;
	item->frame_number++;

	if (anim->number_changes > 0)
	{
		if (GetChange(item, anim))
		{
			anim = &anims[item->anim_number];
			item->current_anim_state = anim->current_anim_state;

			if (item->required_anim_state == item->current_anim_state)
				item->required_anim_state = 0;
		}
	}

	if (item->frame_number > anim->frame_end)
	{
		if (anim->number_commands > 0)
		{
			cmd = &commands[anim->command_index];

			for (int i = anim->number_commands; i > 0; i--)
			{
				switch (*cmd++)
				{
				case ACMD_SETPOS:
					TranslateItem(item, cmd[0], cmd[1], cmd[2]);
					cmd += 3;
					break;

				case ACMD_JUMPVEL:
					item->fallspeed = cmd[0];
					item->speed = cmd[1];
					item->gravity_status = 1;
					cmd += 2;
					break;

				case ACMD_KILL:

					if (objects[item->object_number].intelligent)
						item->after_death = 1;

					item->status = ITEM_DEACTIVATED;
					break;

				case ACMD_PLAYSFX:
				case ACMD_FLIPEFFECT:
					cmd += 2;
					break;
				}
			}
		}

		item->anim_number = anim->jump_anim_num;
		item->frame_number = anim->jump_frame_num;
		anim = &anims[item->anim_number];

		if (item->current_anim_state != anim->current_anim_state)
		{
			item->current_anim_state = anim->current_anim_state;
			item->goal_anim_state = anim->current_anim_state;
		}

		if (item->required_anim_state == item->current_anim_state)
			item->required_anim_state = 0;
	}

	if (anim->number_commands > 0)
	{
		cmd = &commands[anim->command_index];

		for (int i = anim->number_commands; i > 0; i--)
		{
			switch (*cmd++)
			{
			case ACMD_SETPOS:
				cmd += 3;
				break;

			case ACMD_JUMPVEL:
				cmd += 2;
				break;

			case ACMD_PLAYSFX:

				if (item->frame_number == *cmd)
				{
					num = cmd[1] & 0x3FFF;
					type = cmd[1] & 0xC000;

					if (objects[item->object_number].water_creature)
					{
						if (room[item->room_number].flags & ROOM_UNDERWATER)
							SoundEffect(num, &item->pos, SFX_WATER);
						else
							SoundEffect(num, &item->pos, SFX_DEFAULT);
					}
					else if (item->room_number == 255)
					{
						item->pos.x_pos = lara_item->pos.x_pos;
						item->pos.y_pos = lara_item->pos.y_pos - 762;
						item->pos.z_pos = lara_item->pos.z_pos;
						SoundEffect(num, &item->pos, SFX_DEFAULT);
					}
					else if (room[item->room_number].flags & ROOM_UNDERWATER)
					{
						if (type == SFX_LANDANDWATER || type == SFX_WATERONLY)
							SoundEffect(num, &item->pos, SFX_DEFAULT);
					}
					else if (type == SFX_LANDANDWATER || type == SFX_LANDONLY)
						SoundEffect(num, &item->pos, SFX_DEFAULT);
				}

				cmd += 2;
				break;

			case ACMD_FLIPEFFECT:

				if (item->frame_number == *cmd)
				{
					FXType = cmd[1] & 0xC000;
					effect_routines[cmd[1] & 0x3FFF](item);
				}

				cmd += 2;
				break;
			}
		}
	}

	speed2 = 0;

	if (item->gravity_status)
	{
		if (item->fallspeed < 128)
			item->fallspeed += 6;
		else
			item->fallspeed++;

		item->pos.y_pos += item->fallspeed;
	}
	else
	{
		speed = anim->velocity;

		if (anim->acceleration)
			speed += anim->acceleration * (item->frame_number - anim->frame_base);

		item->speed = speed >> 16;

		speed2 = anim->Xvelocity;

		if (anim->Xacceleration)
			speed2 += anim->Xacceleration * (item->frame_number - anim->frame_base);

		speed2 >>= 16;
	}

	item->pos.x_pos += (item->speed * phd_sin(item->pos.y_rot)) >> W2V_SHIFT;
	item->pos.z_pos += (item->speed * phd_cos(item->pos.y_rot)) >> W2V_SHIFT;
	item->pos.x_pos += (speed2 * phd_sin(item->pos.y_rot + 0x4000)) >> W2V_SHIFT;
	item->pos.z_pos += (speed2 * phd_cos(item->pos.y_rot + 0x4000)) >> W2V_SHIFT;
}

long RayBoxIntersect(PHD_VECTOR* min, PHD_VECTOR* max, PHD_VECTOR* origin, PHD_VECTOR* dir, PHD_VECTOR* Coord)
{
	long planes[3];
	long dists[3];
	long plane;
	char quad[3];
	char inside;

	inside = 1;

	if (origin->x < min->x)
	{
		quad[0] = 1;
		planes[0] = min->x;
		inside = 0;
	}
	else if (origin->x > max->x)
	{
		quad[0] = 0;
		planes[0] = max->x;
		inside = 0;
	}
	else
		quad[0] = 2;

	if (origin->y < min->y)
	{
		quad[1] = 1;
		planes[1] = min->y;
		inside = 0;
	}
	else if (origin->y > max->y)
	{
		quad[1] = 0;
		planes[1] = max->y;
		inside = 0;
	}
	else
		quad[1] = 2;

	if (origin->z < min->z)
	{
		quad[2] = 1;
		planes[2] = min->z;
		inside = 0;
	}
	else if (origin->z > max->z)
	{
		quad[2] = 0;
		planes[2] = max->z;
		inside = 0;
	}
	else
		quad[2] = 2;

	if (inside)
	{
		Coord->x = origin->x >> 16;
		Coord->y = origin->y >> 16;
		Coord->z = origin->z >> 16;
		return 1;
	}

	if (quad[0] == 2 || !dir->x)
		dists[0] = -65536;
	else
		dists[0] = ((planes[0] - origin->x) / (dir->x >> 8)) << 8;

	if (quad[1] == 2 || !dir->y)
		dists[1] = -65536;
	else
		dists[1] = ((planes[1] - origin->y) / (dir->y >> 8)) << 8;

	if (quad[2] == 2 || !dir->z)
		dists[2] = -65536;
	else
		dists[2] = ((planes[2] - origin->z) / (dir->z >> 8)) << 8;

	plane = 0;

	for (int i = 1; i < 3; i++)
	{
		if (dists[plane] < dists[i])
			plane = i;
	}

	if (dists[plane] < 0)
		return 0;

	if (!plane)
		Coord->x = planes[0];
	else
	{
		Coord->x = origin->x + (((__int64)dir->x * (__int64)dists[plane]) >> 16);

		if ((!quad[0] && Coord->x < min->x) || (quad[0] == 1 && Coord->x > max->x))
			return 0;
	}

	if (plane == 1)
		Coord->y = planes[1];
	else
	{
		Coord->y = origin->y + (((__int64)dir->y * (__int64)dists[plane]) >> 16);

		if ((!quad[1] && Coord->y < min->y) || (quad[1] == 1 && Coord->y > max->y))
			return 0;
	}

	if (plane == 2)
		Coord->z = planes[2];
	else
	{
		Coord->z = origin->z + (((__int64)dir->z * (__int64)dists[plane]) >> 16);

		if ((!quad[2] && Coord->z < min->z) || (quad[2] == 1 && Coord->z > max->z))
			return 0;
	}

	Coord->x >>= 16;
	Coord->y >>= 16;
	Coord->z >>= 16;
	return 1;
}

long DoRayBox(GAME_VECTOR* start, GAME_VECTOR* target, short* bounds, PHD_3DPOS* ItemPos, PHD_VECTOR* Coord, short item_number)
{
	ITEM_INFO* item;
	OBJECT_INFO* obj;
	SPHERE* sphere;
	PHD_VECTOR min, max, origin, dir;
	PHD_VECTOR spos, tpos, s, t, sp, pos;
	short** meshpp;
	short* ClosestMesh;
	long x, y, z, ClosestNode, ClosestBit, bit, r, r0, r1, dist, max_dist;

	min.x = bounds[0] << 16;
	min.y = bounds[2] << 16;
	min.z = bounds[4] << 16;

	max.x = bounds[1] << 16;
	max.y = bounds[3] << 16;
	max.z = bounds[5] << 16;

	phd_PushUnitMatrix();
	phd_RotY(-ItemPos->y_rot);

	x = target->x - ItemPos->x_pos;
	y = target->y - ItemPos->y_pos;
	z = target->z - ItemPos->z_pos;
	tpos.x = long(mMXPtr[M00] * x + mMXPtr[M01] * y + mMXPtr[M02] * z);
	tpos.y = long(mMXPtr[M10] * x + mMXPtr[M11] * y + mMXPtr[M12] * z);
	tpos.z = long(mMXPtr[M20] * x + mMXPtr[M21] * y + mMXPtr[M22] * z);

	x = start->x - ItemPos->x_pos;
	y = start->y - ItemPos->y_pos;
	z = start->z - ItemPos->z_pos;
	spos.x = long(mMXPtr[M00] * x + mMXPtr[M01] * y + mMXPtr[M02] * z);
	spos.y = long(mMXPtr[M10] * x + mMXPtr[M11] * y + mMXPtr[M12] * z);
	spos.z = long(mMXPtr[M20] * x + mMXPtr[M21] * y + mMXPtr[M22] * z);

	phd_PopMatrix();

	dir.x = (tpos.x - spos.x) << 16;
	dir.y = (tpos.y - spos.y) << 16;
	dir.z = (tpos.z - spos.z) << 16;
	origin.x = spos.x << 16;
	origin.y = spos.y << 16;
	origin.z = spos.z << 16;
	Normalise(&dir);
	dir.x <<= 8;
	dir.y <<= 8;
	dir.z <<= 8;

	if (!RayBoxIntersect(&min, &max, &origin, &dir, Coord))
		return 0;

	if (Coord->x < bounds[0] || Coord->x > bounds[1] ||
		Coord->y < bounds[2] || Coord->y > bounds[3] ||
		Coord->z < bounds[4] || Coord->z > bounds[5])
		return 0;

	phd_PushUnitMatrix();
	phd_RotY(ItemPos->y_rot);

	x = long(mMXPtr[M00] * Coord->x + mMXPtr[M01] * Coord->y + mMXPtr[M02] * Coord->z);
	y = long(mMXPtr[M10] * Coord->x + mMXPtr[M11] * Coord->y + mMXPtr[M12] * Coord->z);
	z = long(mMXPtr[M20] * Coord->x + mMXPtr[M21] * Coord->y + mMXPtr[M22] * Coord->z);
	Coord->x = x;
	Coord->y = y;
	Coord->z = z;

	phd_PopMatrix();
	max_dist = 0x7FFFFFFF;
	ClosestMesh = 0;
	ClosestBit = 0;
	item = 0;

	if (item_number < 0)
		ClosestNode = -1;
	else
	{
		item = &items[item_number];
		obj = &objects[item->object_number];
		meshpp = &meshes[obj->mesh_index];

		GetSpheres(item, Slist, 1);
		bit = 1;
		ClosestNode = -2;

		for (int i = 0; i < obj->nmeshes; i++, meshpp += 2, bit <<= 1)
		{
			if (!(item->mesh_bits & bit))
				continue;

			sphere = &Slist[i];
			s.x = start->x;
			s.y = start->y;
			s.z = start->z;

			t.x = target->x;
			t.y = target->y;
			t.z = target->z;

			sp.x = sphere->x;
			sp.y = sphere->y;
			sp.z = sphere->z;

			r0 = ((sp.x - s.x) * (t.x - s.x)) + ((sp.y - s.y) * (t.y - s.y)) + ((sp.z - s.z) * (t.z - s.z));
			r1 = SQUARE(t.x - s.x) + SQUARE(t.y - s.y) + SQUARE(t.z - s.z);

			if ((r0 >= 0 || r1 >= 0) && (r1 <= 0 || r0 <= 0) || (abs(r0) > abs(r1)))
				continue;

			r1 >>= 16;

			if (r1)
				r = r0 / r1;
			else
				r = 0;

			pos.x = s.x + ((r * (t.x - s.x)) >> 16);
			pos.y = s.y + ((r * (t.y - s.y)) >> 16);
			pos.z = s.z + ((r * (t.z - s.z)) >> 16);

			if (SQUARE(pos.x - sp.x) + SQUARE(pos.y - sp.y) + SQUARE(pos.z - sp.z) < SQUARE(sphere->r))
			{
				dist = SQUARE(sphere->x - start->x) + SQUARE(sphere->y - start->y) + SQUARE(sphere->z - start->z);

				if (dist < max_dist)
				{
					max_dist = dist;
					ClosestMesh = *meshpp;
					ClosestBit = bit;
					ClosestNode = i;
				}
			}
		}
	}

	if (ClosestNode >= -1)
	{
		dist = SQUARE(Coord->x + ItemPos->x_pos - start->x) +
			SQUARE(Coord->y + ItemPos->y_pos - start->y) +
			SQUARE(Coord->z + ItemPos->z_pos - start->z);

		if (dist < ClosestDist)
		{
			ClosestCoord.x = Coord->x + ItemPos->x_pos;
			ClosestCoord.y = Coord->y + ItemPos->y_pos;
			ClosestCoord.z = Coord->z + ItemPos->z_pos;
			ClosestItem = item_number;
			ClosestDist = dist;

			if (ClosestNode >= 0)
			{
				GetSpheres(item, Slist, 3);
				ShatterItem.YRot = item->pos.y_rot;
				ShatterItem.meshp = ClosestMesh;
				ShatterItem.Sphere.x = Slist[ClosestNode].x;
				ShatterItem.Sphere.y = Slist[ClosestNode].y;
				ShatterItem.Sphere.z = Slist[ClosestNode].z;
				ShatterItem.Bit = ClosestBit;
				ShatterItem.il = &item->il;
				ShatterItem.Flags = 0;
			}

			return 1;
		}
	}

	return 0;
}

long GetMaximumFloor(FLOOR_INFO* floor, long x, long z)
{
	long height, h1, h2;
	short* data, type, dx, dz, t0, t1, t2, t3, hadj;

	height = floor->floor << 8;

	if (height != NO_HEIGHT && floor->index)
	{
		data = &floor_data[floor->index];
		type = *data++;
		h1 = 0;
		h2 = 0;

		if ((type & 0x1F) != TILT_TYPE)
		{
			if ((type & 0x1F) == SPLIT1 || (type & 0x1F) == SPLIT2 || (type & 0x1F) == NOCOLF1T || (type & 0x1F) == NOCOLF1B || (type & 0x1F) == NOCOLF2T || (type & 0x1F) == NOCOLF2B)
			{
				dx = x & 0x3FF;
				dz = z & 0x3FF;
				t0 = *data & 0xF;
				t1 = *data >> 4 & 0xF;
				t2 = *data >> 8 & 0xF;
				t3 = *data >> 12 & 0xF;

				if ((type & 0x1F) == SPLIT1 || (type & 0x1F) == NOCOLF1T || (type & 0x1F) == NOCOLF1B)
				{
					if (dx <= 1024 - dz)
					{
						hadj = type >> 10 & 0x1F;
						h1 = t2 - t1;
						h2 = t0 - t1;
					}
					else
					{
						hadj = type >> 5 & 0x1F;
						h1 = t3 - t0;
						h2 = t3 - t2;
					}
				}
				else
				{
					if (dx <= dz)
					{
						hadj = type >> 10 & 0x1F;
						h1 = t2 - t1;
						h2 = t3 - t2;
					}
					else
					{
						hadj = type >> 5 & 0x1F;
						h1 = t3 - t0;
						h2 = t0 - t1;
					}
				}

				if (hadj & 0x10)
					hadj |= 0xFFF0;

				height += hadj << 8;
			}
		}
		else
		{
			h1 = *data >> 8;
			h2 = *(char*)data;
		}

		height += 256 * (abs(h1) + abs(h2));
	}

	return height;
}

long GetMinimumCeiling(FLOOR_INFO* floor, long x, long z)
{
	long height, h1, h2;
	short* data, type, dx, dz, t0, t1, t2, t3, hadj, ended;

	height = floor->ceiling << 8;

	if (height != NO_HEIGHT && floor->index)
	{
		data = &floor_data[floor->index];
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
							h1 = t2 - t1;
							h2 = t3 - t2;
						}
						else
						{
							hadj = type >> 5 & 0x1F;
							h1 = t3 - t0;
							h2 = t0 - t1;
						}
					}
					else
					{
						if (dx <= dz)
						{
							hadj = type >> 10 & 0x1F;
							h1 = t2 - t1;
							h2 = t0 - t1;
						}
						else
						{
							hadj = type >> 5 & 0x1F;
							h1 = t3 - t0;
							h2 = t3 - t2;
						}
					}

					if (hadj & 0x10)
						hadj |= 0xFFF0;

					height += hadj << 8;
				}
			}
			else
			{
				h1 = *data >> 8;
				h2 = *(char*)data;
			}

			height -= 256 * (abs(h1) + abs(h2));
		}
	}

	return height;
}
