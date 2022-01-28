#include "../tomb4/pch.h"
#include "gameflow.h"
#include "../specific/function_stubs.h"
#include "../specific/output.h"
#include "savegame.h"
#ifdef GENERAL_FIXES
#include "../tomb4/tomb4.h"
#endif
#include "../specific/specificfx.h"
#include "../specific/file.h"
#include "../specific/function_table.h"
#include "items.h"
#include "lot.h"
#include "../specific/polyinsert.h"
#include "spotcam.h"
#include "health.h"
#include "sound.h"
#include "../specific/audio.h"
#include "camera.h"
#include "control.h"
#include "tomb4fx.h"
#include "../specific/dxsound.h"
#include "text.h"
#include "deltapak.h"
#include "draw.h"

void DoGameflow()
{
	uchar* gf;
	uchar n;

	PlayFmvNow(0);
	do_boot_screen(Gameflow->Language);
	num_fmvs = 0;
	fmv_to_play[0] = 0;
	fmv_to_play[1] = 0;
	gfCurrentLevel = Gameflow->TitleEnabled ? 0 : 1;
	gf = &gfScriptWad[gfScriptOffset[gfCurrentLevel]];

#ifdef GENERAL_FIXES
	init_tomb4_stuff();	//This is only called once at the beginning of the game, and these settings can technically be considered 'gameflow' so this is fine here
#endif

	while (1)
	{
		switch (n = *gf++)
		{
		case CMD_FMV:
			fmv_to_play[num_fmvs] = gf[0];
			num_fmvs++;
			gf++;
			break;

		case CMD_LEVEL:
			gfLevelFlags = gf[1] | (gf[2] << 8);

			if (!(gfLevelFlags & GF_NOLEVEL))
				DoLevel(gf[3], gf[4]);
			else
			{
				gfStatus = 999;
				gfCurrentLevel++;
			}

			gfLegendTime = 0;
			LaserSight = 0;
			BinocularRange = 0;
			gfResidentCut[0] = 0;
			gfResidentCut[1] = 0;
			gfResidentCut[2] = 0;
			gfResidentCut[3] = 0;
			gfUVRotate = 0;
			gfNumMips = 0;
			gfMirrorRoom = -1;
			gfFog.b = 0;
			gfFog.g = 0;
			gfFog.r = 0;
			gfFog.a = 0;

			switch (gfStatus)
			{
			case 1:
				gfInitialiseGame = 1;
				gfCurrentLevel = Gameflow->TitleEnabled ? 0 : 1;
				break;

			case 2:
				gfGameMode = 4;
				gfCurrentLevel = savegame.CurrentLevel & 0x7F;
				break;

			case 3:

				if (gfLevelFlags & GF_RESETHUB && gfLevelComplete == gfResetHubDest || skipped_level)
				{
					sgInitialiseHub(0);
					skipped_level = 0;
				}
				else
					sgSaveLevel();

				if (Gameflow->DemoDisc || Gameflow->nLevels == 2)
					gfCurrentLevel = 0;
				else
				{
					if (gfLevelComplete > Gameflow->nLevels)
						gfCurrentLevel = 0;
					else
						gfCurrentLevel = gfLevelComplete;
				}

				break;

			case 4:
				return;
			}

			gf = &gfScriptWad[gfScriptOffset[gfCurrentLevel]];
			break;

		case CMD_TITLE:
			gfLevelFlags = gf[0] | (gf[1] << 8);
			DoTitle(gf[2], gf[3]);
			gfResidentCut[0] = 0;
			gfResidentCut[1] = 0;
			gfResidentCut[2] = 0;
			gfResidentCut[3] = 0;
			gfUVRotate = 0;
			gfNumMips = 0;
			gfMirrorRoom = -1;

			switch (gfStatus)
			{
			case 2:
				gfGameMode = 4;
				gfCurrentLevel = savegame.CurrentLevel & 0x7F;
				break;

			case 3:
				gfGameMode = 0;
				gfCurrentLevel = gfLevelComplete;
				gfInitialiseGame = 1;
				break;

			case 4:
				return;
			}

			gf = &gfScriptWad[gfScriptOffset[gfCurrentLevel]];
			break;

		case CMD_ENDSEQ:
			continue;

		case CMD_PLAYCUT:
			gfCutNumber = gf[0];
			gf++;
			break;

		case CMD_CUT1:
			gfResidentCut[0] = gf[0];
			gf++;
			break;

		case CMD_CUT2:
			gfResidentCut[1] = gf[0];
			gf++;
			break;

		case CMD_CUT3:
			gfResidentCut[2] = gf[0];
			gf++;
			break;

		case CMD_CUT4:
			gfResidentCut[3] = gf[0];
			gf++;
			break;

		case CMD_LAYER1:
			LightningRGB[0] = gf[0];
			LightningRGBs[0] = gf[0];
			gfLayer1Col.r = gf[0];

			LightningRGB[1] = gf[1];
			LightningRGBs[1] = gf[1];
			gfLayer1Col.g = gf[1];

			LightningRGB[2] = gf[2];
			LightningRGBs[2] = gf[2];
			gfLayer1Col.b = gf[2];

			gfLayer1Vel = gf[3];
			gf += 4;
			break;

		case CMD_LAYER2:
			LightningRGB[0] = gf[0];
			LightningRGBs[0] = gf[0];
			gfLayer2Col.r = gf[0];

			LightningRGB[1] = gf[1];
			LightningRGBs[1] = gf[1];
			gfLayer2Col.g = gf[1];

			LightningRGB[2] = gf[2];
			LightningRGBs[2] = gf[2];
			gfLayer2Col.b = gf[2];

			gfLayer2Vel = gf[3];
			gf += 4;
			break;

		case CMD_UVROT:
			gfUVRotate = gf[0];
			gf++;
			break;

		case CMD_LEGEND:
			gfLegend = gf[0];
			gf++;

			if (gfGameMode != 4)
				gfLegendTime = 150;

			break;

		case CMD_LENSFLARE:
			gfLensFlare.x = ((gf[1] << 8) | gf[0]) << 8;
			gfLensFlare.y = ((gf[3] << 8) | gf[2]) << 8;
			gfLensFlare.z = ((gf[5] << 8) | gf[4]) << 8;
			gfLensFlareColour.r = gf[6];
			gfLensFlareColour.g = gf[7];
			gfLensFlareColour.b = gf[8];
			gf += 9;
			break;

		case CMD_MIRROR:
			gfMirrorRoom = gf[0];
			gfMirrorZPlane = (gf[4] << 24) | (gf[3] << 16) | (gf[2] << 8) | gf[1];
			gf += 5;
			break;

		case CMD_FOG:
			gfFog.r = gf[0];
			gfFog.g = gf[1];
			gfFog.b = gf[2];
			gf += 3;
			break;

		case CMD_ANIMATINGMIP:
			gfMips[gfNumMips] = gf[0];
			gfNumMips++;
			gf++;
			break;

		case CMD_CAMERA:
			load_cam.x = (gf[3] << 24) | (gf[2] << 16) | (gf[1] << 8) | gf[0];
			load_cam.y = (gf[7] << 24) | (gf[6] << 16) | (gf[5] << 8) | gf[4];
			load_cam.z = (gf[11] << 24) | (gf[10] << 16) | (gf[9] << 8) | gf[8];
			load_target.x = (gf[15] << 24) | (gf[14] << 16) | (gf[13] << 8) | gf[12];
			load_target.y = (gf[19] << 24) | (gf[18] << 16) | (gf[17] << 8) | gf[16];
			load_target.z = (gf[23] << 24) | (gf[22] << 16) | (gf[21] << 8) | gf[20];
			load_roomnum = gf[24];
			gf += 25;
			break;

		case CMD_RESETHUB:
			gfResetHubDest = gf[0];
			gf++;
			break;

		default:
			if (n >= CMD_KEY1 && n <= CMD_KEY12)
				n -= 82;
			else if (n >= CMD_PUZZLE1 && n <= CMD_PUZZLE12)
				n -= 122;
			else if (n >= CMD_PICKUP1 && n <= CMD_PICKUP4)
				n -= 78;
			else if (n >= CMD_EXAMINE1 && n <= CMD_EXAMINE3)
				n -= 59;
			else if (n >= CMD_KEYCOMBO1_1 && n <= CMD_KEYCOMBO8_2)
				n -= 101;
			else if (n >= CMD_PUZZLECOMBO1_1 && n <= CMD_PUZZLECOMBO8_2)
				n += 111;
			else if (n >= CMD_PICKUPCOMBO1_1 && n <= CMD_PICKUPCOMBO4_2)
				n -= 113;

			inventry_objects_list[n].objname = gf[0] | (gf[1] << 8);
			inventry_objects_list[n].yoff = gf[2] | (gf[3] << 8);
			inventry_objects_list[n].scale1 = gf[4] | (gf[5] << 8);
			inventry_objects_list[n].yrot = gf[6] | (gf[7] << 8);
			inventry_objects_list[n].xrot = gf[8] | (gf[9] << 8);
			inventry_objects_list[n].zrot = gf[10] | (gf[11] << 8);
			inventry_objects_list[n].flags = gf[12] | (gf[13] << 8);
			gf += 14;
			break;
		}
	}
}

void DoLevel(uchar Name, uchar Audio)
{
	long gamestatus;

	gamestatus = 0;
	SetFade(255, 0);

	if (gfGameMode != 4)
	{
		savegame.Level.Timer = 0;
		savegame.Level.Distance = 0;
		savegame.Level.AmmoUsed = 0;
		savegame.Level.AmmoHits = 0;
		savegame.Level.Kills = 0;
		savegame.Level.Secrets = 0;
		savegame.Level.HealthUsed = 0;
	}

	S_LoadLevelFile(Name);
	SetFogColor(gfFog.r, gfFog.g, gfFog.b);
	InitialiseFXArray(1);
	InitialiseLOTarray(1);
	ClearFXFogBulbs();
	InitSpotCamSequences();
	InitialisePickUpDisplay();
	SOUND_Stop();
	bDisableLaraControl = 0;

	if (gfGameMode == 4)
	{
		sgRestoreGame();
		gfRequiredStartPos = 0;
		gfInitialiseGame = 0;
		gfFog.r = savegame.fog_colour.r;
		gfFog.g = savegame.fog_colour.g;
		gfFog.b = savegame.fog_colour.b;

		if (IsVolumetric())
			SetFogColor(gfFog.r, gfFog.g, gfFog.b);
	}
	else
	{
		if (gfInitialiseGame)
		{
			GameTimer = 0;
			gfRequiredStartPos = 0;
			gfInitialiseGame = 0;
			CutSceneTriggered = 0;
			FmvSceneTriggered = 0;
		}
		else
			sgRestoreLevel();

		if (gfLevelFlags & GF_REMOVEAMULET)
			lara.questitems &= ~1;

		savegame.Level.Timer = 0;
		CurrentAtmosphere = Audio;
	}

	S_CDPlay(CurrentAtmosphere, 1);
	IsAtmospherePlaying = 1;
	ScreenFadedOut = 0;
	ScreenFading = 0;
	ScreenFadeBack = 0;
	dScreenFade = 255;
	ScreenFade = 255;

	if (!gfCutNumber || CheckCutPlayed(gfCutNumber))
	{
		cutseq_num = 0;
		gfCutNumber = 0;
		SetScreenFadeIn(16);
	}
	else
	{
		cutseq_num = gfCutNumber;
		gfCutNumber = 0;
		ScreenFadedOut = 1;
	}

	InitialiseCamera();
	bUseSpotCam = 0;
	gfGameMode = 0;
	gfLevelComplete = 0;
	nFrames = 2;
	framecount = 0;
	gfStatus = ControlPhase(2, 0);
	dbinput = 0;

	while (!gfStatus)
	{
		S_InitialisePolyList();

		if (gfLegendTime && !DestFadeScreenHeight && !FadeScreenHeight && !cutseq_num)
		{
			PrintString((ushort)(phd_winwidth >> 1), (ushort)(phd_winymax - font_height), 2, SCRIPT_TEXT(gfLegend), FF_CENTER);
			gfLegendTime--;
		}

		nFrames = DrawPhaseGame();
		handle_cutseq_triggering(Name);

		if (DEL_playingamefmv)
		{
			DEL_playingamefmv = 0;
			S_CDStop();
			PlayFmvNow(7);
			DelsHandyTeleportLara(54179, -8192, 50899, -32703);
		}

		if (gfLevelComplete)
		{
			gfStatus = 3;
			break;
		}

		gfStatus = ControlPhase(nFrames, 0);

		if (gfStatus && !gamestatus)
		{
			if (lara_item->hit_points < 0)
			{
				gamestatus = gfStatus;
				SetFade(0, 255);
				gfStatus = 0;
			}
			else
				break;
		}

		if (gamestatus)
		{
			gfStatus = 0;

			if (DoFade == 2)
				gfStatus = gamestatus;
		}
	}

	S_SoundStopAllSamples();
	S_CDStop();

	if (gfStatus == 3)
	{
		if (fmv_to_play[0] & 0x80)
		{
			if ((fmv_to_play[0] & 0x7F) == 9 && gfLevelComplete != 10)
				fmv_to_play[0] = 0;

			if ((fmv_to_play[0] & 0x7F) == 8 && gfLevelComplete != 22)
				fmv_to_play[0] = 0;
		}

		if (fmv_to_play[0] && PlayFmvNow(fmv_to_play[0] & 0x7F) == 2)
		{
			if (fmv_to_play[1])
				PlayFmvNow(fmv_to_play[1] & 0x7F);
		}
	}

	num_fmvs = 0;
	fmv_to_play[1] = 0;
	fmv_to_play[0] = 0;
	lara.examine3 = 0;
	lara.examine2 = 0;
	lara.examine1 = 0;
	RenderLoadPic(0);

	if (gfStatus == 3)
	{
		if (gfLevelComplete == 39)
		{
			input = 0;
			reset_flag = 0;
			gfStatus = 1;
			bDoCredits = 1;
			return;
		}

		if (gfLevelComplete == 25 && skipped_level)
			lara.vehicle = -1;
	}

	input = 0;
	reset_flag = 0;
}

void inject_gameflow(bool replace)
{
	INJECT(0x00451770, DoGameflow, replace);
	INJECT(0x00452710, DoLevel, replace);
}
