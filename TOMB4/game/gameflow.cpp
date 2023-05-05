#include "../tomb4/pch.h"
#include "gameflow.h"
#include "../specific/fmv.h"
#include "../specific/output.h"
#include "savegame.h"
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
#include "../specific/LoadSave.h"
#include "../specific/gamemain.h"
#include "newinv.h"
#include "../specific/dxshell.h"
#include "../specific/input.h"
#include "../specific/3dmath.h"
#include "lara.h"
#include "../tomb4/tomb4.h"

short CreditGroups[18] =
{
	0,
	TXT_PC_Programmer,
	TXT_PSX_Programmers,
	TXT_Additional_Programming,
	TXT_Additional_Programmers,
	TXT_AI_Programming,
	TXT_Animators,
	TXT_Level_Designers,
	TXT_FMV_Sequences,
	TXT_Additional_Artwork,
	TXT_Music_Sound_FX,
	TXT_Original_Story,
	TXT_Script,
	TXT_Producer,
	TXT_QA,
	TXT_Executive_Producers,
	TXT_Additional_Sound_FX,
	0
};

const char* CreditsTable[]
{
	"%01",
	"Richard Flower", "0",

	"%05",
	"Tom Scutt", "0",

	"%02",
	"Chris Coupe", "Martin Gibbins", "Derek Leigh-Gilchrist", "0",

	"%03",
	"Martin Jensen", "0",

	"%06",
	"Phil Chapman", "Jerr O'Carroll", "0",

	"%07",
	"Andrea Cordella", "Pete Duncan", "Jamie Morton", "Richard Morton", "Andy Sandham", "Joby Wood", "0",

	"%08",
	"Peter Barnard", "David Reading", "Matt Furniss", "Simeon Furniss", "John Lilley", "0",

	"%09",
	"Damon Godley", "Mark Hazelton", "Steve Huckle", "Steve Hawkes", "Darren Wakeman", "0",

	"%10",
	"Peter Connelly", "0",

	"%16",
	"Martin Iveson", "0",

	"%11",
	"Pete Duncan", "Dr Kieron O'Hara", "Richard Morton", "Andy Sandham", "0",

	"%12",
	"Hope Canton", "Andy Sandham", "0",

	"%13",
	"Troy Horton", "0",

	"%14",
	"Tiziano Cirillo", "Nick Conelly", "Hayos Fatunmbi", "Paul Field", "Steve Wakeman", "Dave Ward", "Jason Churchman", "0",

	"%15",
	"Jeremy H. Smith", "Adrian Smith",

	"0",
	"Tomb Raider IV Community Edition",
	"Troye", "ChocolateFan"
};

GAMEFLOW* Gameflow;
PHD_VECTOR gfLoadCam;
PHD_VECTOR gfLoadTarget;
uchar gfLoadRoom = 255;
PHD_VECTOR gfLensFlare;
CVECTOR gfLensFlareColour;
CVECTOR gfFog = { 0, 0, 0, 0 };
CVECTOR gfLayer1Col;
CVECTOR gfLayer2Col;
ushort* gfStringOffset;
ushort* gfFilenameOffset;
uchar* gfScriptFile;
uchar* gfLanguageFile;
char* gfStringWad;
char* gfFilenameWad;
long gfMirrorZPlane;
long gfStatus = 0;
ushort gfLevelFlags;
uchar gfCurrentLevel;
uchar gfLevelComplete;
uchar gfGameMode = 1;
uchar gfMirrorRoom;
uchar gfNumMips = 0;
uchar gfRequiredStartPos;
uchar gfMips[8];
uchar gfLevelNames[40];
char gfUVRotate;
char gfLayer1Vel;
char gfLayer2Vel;

ulong GameTimer;
uchar bDoCredits = 0;
char DEL_playingamefmv = 0;
char skipped_level = 0;
char Chris_Menu = 0;
char title_controls_locked_out;

static ushort* gfScriptOffset;
static uchar* gfScriptWad = 0;
static char* gfExtensions = 0;
static long nFrames = 1;
static uchar gfLegend;
static uchar gfLegendTime = 0;
static uchar gfInitialiseGame = 1;
static uchar gfResetHubDest;
static uchar gfCutNumber = 0;
static uchar gfResidentCut[4];

static char fmv_to_play[2] = { 0, 0 };
static char num_fmvs = 0;

void DoGameflow()
{
	uchar* gf;
	uchar n;

#ifndef TIMES_LEVEL
	PlayFmvNow(0);
#endif
	do_boot_screen(Gameflow->Language);
	num_fmvs = 0;
	fmv_to_play[0] = 0;
	fmv_to_play[1] = 0;
	gfCurrentLevel = Gameflow->TitleEnabled ? 0 : 1;
	gf = &gfScriptWad[gfScriptOffset[gfCurrentLevel]];

	while (1)
	{
		switch (n = *gf++)
		{
		case CMD_FMV:
#ifndef TIMES_LEVEL
			fmv_to_play[num_fmvs] = gf[0];
			num_fmvs++;
#endif
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
			gfLensFlare.y = short((gf[3] << 8) | gf[2]) << 8;
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
			savegame.fog_colour.r = gfFog.r;
			savegame.fog_colour.g = gfFog.g;
			savegame.fog_colour.b = gfFog.b;
			gf += 3;
			break;

		case CMD_ANIMATINGMIP:
			gfMips[gfNumMips] = gf[0];
			gfNumMips++;
			gf++;
			break;

		case CMD_CAMERA:
			gfLoadCam.x = (gf[3] << 24) | (gf[2] << 16) | (gf[1] << 8) | gf[0];
			gfLoadCam.y = (gf[7] << 24) | (gf[6] << 16) | (gf[5] << 8) | gf[4];
			gfLoadCam.z = (gf[11] << 24) | (gf[10] << 16) | (gf[9] << 8) | gf[8];
			gfLoadTarget.x = (gf[15] << 24) | (gf[14] << 16) | (gf[13] << 8) | gf[12];
			gfLoadTarget.y = (gf[19] << 24) | (gf[18] << 16) | (gf[17] << 8) | gf[16];
			gfLoadTarget.z = (gf[23] << 24) | (gf[22] << 16) | (gf[21] << 8) | gf[20];
			gfLoadRoom = gf[24];
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
	//empty func call here
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
			PrintString(phd_winwidth >> 1, phd_winymax - font_height, 2, SCRIPT_TEXT(gfLegend), FF_CENTER);
			gfLegendTime--;
		}

		nFrames = DrawPhaseGame();
		handle_cutseq_triggering(Name);

		if (DEL_playingamefmv)
		{
			DEL_playingamefmv = 0;
#ifndef TIMES_LEVEL
			S_CDStop();
			PlayFmvNow(7);
			DelsHandyTeleportLara(54179, -8192, 50899, -32703);
#endif
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

#ifndef TIMES_LEVEL
	if (gfStatus == 3)
	{
		if (fmv_to_play[0] & 0x80)
		{
			if ((fmv_to_play[0] & 0x7F) == 9 && gfLevelComplete != 10)
				fmv_to_play[0] = 0;

			if ((fmv_to_play[0] & 0x7F) == 8 && gfLevelComplete != 22)
				fmv_to_play[0] = 0;
		}

		if (!fmv_to_play[0] || PlayFmvNow(fmv_to_play[0] & 0x7F) != 2)
		{
			if (fmv_to_play[1])
				PlayFmvNow(fmv_to_play[1] & 0x7F);
		}
	}

	num_fmvs = 0;
	fmv_to_play[0] = 0;
	fmv_to_play[1] = 0;
#endif
	lara.examine1 = 0;
	lara.examine2 = 0;
	lara.examine3 = 0;
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

long TitleOptions()
{
	static __int64 selection = 1;
	static __int64 selection_bak = 0;
	__int64 flag, sel;
	long nLevels, nFirst, lp;
	long ret, n, load, y;
	static long load_or_new;
	static long goto_level;
	static long menu = 0;	//0 main menu, 1 level select, 2 the reload menu, 3 the options menu

	ret = 0;

	if (load_or_new)
	{
		if (DoFade == 2)
		{
			ret = load_or_new;
			gfLevelComplete = (uchar)goto_level;
			goto_level = 0;
			load_or_new = 0;
			return ret;
		}

		input = 0;
		dbinput = 0;
	}

	if (bDoCredits)
	{
		if (DoCredits())
			return 0;

		bDoCredits = 0;
	}

	switch (menu)
	{
	case 1:
		PrintString(phd_centerx, font_height + phd_winymin, 6, SCRIPT_TEXT(TXT_Select_Level), FF_CENTER);

		if (Gameflow->nLevels < 10)
		{
			nFirst = 1;
			nLevels = Gameflow->nLevels - 1;
		}
		else
		{
			sel = selection;
			n = 0;
			nLevels = 10;

			while (sel)
			{
				sel >>= 1;
				n++;
			}

			nFirst = n - 9;

			if (nFirst < 1)
				nFirst = 1;
			else if (nFirst > 1)
			{
				PrintString(32, 3 * font_height + phd_winymin, 6, "\x18", 0);
				PrintString(phd_winxmax - 48, 3 * font_height + phd_winymin, 6, "\x18", 0);
			}

			if (n != Gameflow->nLevels - 1)
			{
				PrintString(32, 12 * font_height + phd_winymin, 6, "\x1a", 0);
				PrintString(phd_winxmax - 48, 12 * font_height + phd_winymin, 6, "\x1a", 0);
			}
		}

		y = 2 * font_height;

		for (lp = nFirst; lp < nLevels + nFirst; lp++)
		{
			y += font_height;
			PrintString(phd_centerx, y, selection & (1i64 << (lp - 1)) ? 1 : 2, SCRIPT_TEXT(gfLevelNames[lp]), FF_CENTER);
		}

		flag = 1i64 << (Gameflow->nLevels - 2);
		break;

	case 2:

		if (Gameflow->LoadSaveEnabled)
		{
			load = DoLoadSave(IN_LOAD);

			if (load >= 0)
			{
				S_LoadGame(load);
				ret = 2;
			}

			break;
		}

		SoundEffect(SFX_LARA_NO, 0, SFX_ALWAYS);
		menu = 0;

	case 0:
		ShowTitle();
		Chris_Menu = 0;
		PrintString(phd_centerx, phd_winymax - 4 * font_height, (selection & 1) ? 1 : 2, SCRIPT_TEXT(TXT_New_Game), FF_CENTER);
		PrintString(phd_centerx, phd_winymax - 3 * font_height, (selection & 2) ? 1 : 2, SCRIPT_TEXT(TXT_Load_Game), FF_CENTER);
		PrintString(phd_centerx, phd_winymax - 2 * font_height, (selection & 4) ? 1 : 2, SCRIPT_TEXT(TXT_Options), FF_CENTER);
		PrintString(phd_centerx, phd_winymax - 1 * font_height, (selection & 8) ? 1 : 2, SCRIPT_TEXT(TXT_Exit), FF_CENTER);
		flag = 8;
		break;

	case 3:
		DoOptions();
		break;
	}

	if (menu < 2)
	{
		if (dbinput & IN_FORWARD)
		{
			if (selection > 1)
				selection >>= 1;

			SoundEffect(SFX_MENU_CHOOSE, 0, SFX_ALWAYS);
		}

		if (dbinput & IN_BACK)
		{
			if (selection < flag)
				selection <<= 1;

			SoundEffect(SFX_MENU_CHOOSE, 0, SFX_ALWAYS);
		}
	}

	if (dbinput & IN_DESELECT && menu > 0)
	{
		menu = 0;
		selection = selection_bak;
		S_SoundStopAllSamples();
		SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
	}

	if (dbinput & IN_SELECT && !keymap[DIK_LALT] && menu < 2)
	{
		SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);

		if (!menu)
		{
			if (selection > 0 && selection <= 8)
			{
				switch (selection)
				{
				case 1:

					if (Gameflow->PlayAnyLevel)
					{
						selection_bak = selection;
						menu = 1;
					}
					else
					{
						gfLevelComplete = 1;
						ret = 3;
					}

					break;

				case 2:
					GetSaveLoadFiles();
					selection_bak = selection;
					menu = 2;
					break;

				case 3:
				case 5:
				case 6:
				case 7:
					break;

				case 4:
					selection_bak = selection;
					menu = 3;
					break;

				case 8:
					ret = 4;
					break;
				}
			}
		}
		else if (menu == 1)
		{
			gfLevelComplete = 0;
			sel = selection;

			while (sel)
			{
				sel >>= 1;
				gfLevelComplete++;
			}

			ret = 3;
		}
	}

	if (MainThread.ended)
		return 4;

	if (ret)
	{
		load_or_new = ret;
		goto_level = gfLevelComplete;
		gfLevelComplete = 0;
		ret = 0;
		SetFade(0, 255);
	}

	return ret;
}

void DoTitle(uchar Name, uchar Audio)
{
	SetFade(255, 0);
	num_fmvs = 0;
	fmv_to_play[1] = 0;
	fmv_to_play[0] = 0;
	sgInitialiseHub(1);
	savegame.Level.Timer = 0;
	savegame.Level.Distance = 0;
	savegame.Level.AmmoUsed = 0;
	savegame.Level.AmmoHits = 0;
	savegame.Level.Kills = 0;
	savegame.Level.Secrets = 0;
	savegame.Level.HealthUsed = 0;
	savegame.Game.AmmoHits = 0;
	savegame.Game.Timer = 0;
	savegame.Game.Distance = 0;
	savegame.Game.AmmoUsed = 0;
	savegame.Game.Kills = 0;
	savegame.Game.Secrets = 0;
	savegame.Game.HealthUsed = 0;
	savegame.HaveBikeBooster = 0;
	S_LoadLevelFile(Name);
	GLOBAL_lastinvitem = -1;
	InitSpotCamSequences();
	title_controls_locked_out = 0;
	InitialiseFXArray(1);
	InitialiseLOTarray(1);
	SetFogColor(gfFog.r, gfFog.g, gfFog.b);
	ClearFXFogBulbs();
	InitialisePickUpDisplay();
	//empty func call here
	SOUND_Stop();
	S_CDPlay(Audio, 1);
	IsAtmospherePlaying = 0;
	S_SetReverbType(1);
	InitialiseCamera();

	if (bDoCredits)
	{
		cutseq_num = 28;
		SetFadeClip(32, 1);
		ScreenFadedOut = 1;
		ScreenFade = 255;
		dScreenFade = 255;
		S_CDPlay(98, 1);
	}
	else
	{
		InitialiseSpotCam(1);
		ScreenFadedOut = 0;
		ScreenFade = 0;
		dScreenFade = 0;
		ScreenFadeBack = 0;
		ScreenFadeSpeed = 8;
		ScreenFading = 0;
	}

	bUseSpotCam = 1;
	lara_item->mesh_bits = 0;
	gfGameMode = 1;
	gfLevelComplete = 0;
	nFrames = 2;
	gfStatus = ControlPhase(2, 0);

	while (!gfStatus)
	{
		S_InitialisePolyList();
		gfStatus = TitleOptions();	//originally inlined

		if (gfStatus)
			break;

		handle_cutseq_triggering(Name);
		nFrames = DrawPhaseGame();
		gfStatus = ControlPhase(nFrames, 0);
	}

	S_SoundStopAllSamples();
	S_CDStop();
	bUseSpotCam = 0;
	bDisableLaraControl = 0;

#ifndef TIMES_LEVEL
	if (gfLevelComplete == 1 && gfStatus != 2)
		PlayFmvNow(12);
#endif

	if (gfStatus != 4)
		RenderLoadPic(0);

	input = 0;
}

void LoadGameflow()
{
	STRINGHEADER sh;
	uchar* n;
	char* s;
	char* d;
	long l, end;

	s = 0;
	LoadFile("SCRIPT.DAT", &s);

	gfScriptFile = (uchar*)s;

	Gameflow = (GAMEFLOW*)s;
	s += sizeof(GAMEFLOW);

	gfExtensions = s;	//"[PCExtensions]"
	s += 40;

	gfFilenameOffset = (ushort*)s;
	s += sizeof(ushort) * Gameflow->nFileNames;

	gfFilenameWad = s;
	s += Gameflow->FileNameLen;

	gfScriptOffset = (ushort*)s;
	s += sizeof(ushort) * Gameflow->nLevels;

	gfScriptWad = (uchar*)s;
	s += Gameflow->ScriptLen;

	for (l = 0;; l++)
	{
		d = 0;

		if (LoadFile(s, &d))
			break;

		s += strlen(s) + 1;
	}

	gfStringOffset = (ushort*)d;
	gfLanguageFile = (uchar*)d;
	Gameflow->Language = l;

	memcpy(&sh, gfStringOffset, sizeof(STRINGHEADER));
	memcpy(gfStringOffset, gfStringOffset + (sizeof(STRINGHEADER) / sizeof(ushort)), TXT_NUM_STRINGS * sizeof(ushort));
	gfStringWad = (char*)(gfStringOffset + TXT_NUM_STRINGS);
	memcpy(gfStringOffset + TXT_NUM_STRINGS,
		gfStringOffset + TXT_NUM_STRINGS + (sizeof(STRINGHEADER) / sizeof(ushort)),
		sh.StringWadLen + sh.PCStringWadLen + sh.PSXStringWadLen);

	for (int i = 0; i < TXT_NUM_STRINGS - 1; i++)
	{
		s = &gfStringWad[gfStringOffset[i]];
		d = &gfStringWad[gfStringOffset[i + 1]];
		l = d - s - 1;

		for (int j = 0; j < l; j++)
			s[j] ^= 0xA5;
	}

	for (int i = 0; i < Gameflow->nLevels; i++)
	{
		end = 0;
		n = &gfScriptWad[gfScriptOffset[i]];

		while (!end)
		{
			switch (*n++)
			{
			case CMD_FMV:
			case CMD_PLAYCUT:
			case CMD_CUT1:
			case CMD_CUT2:
			case CMD_CUT3:
			case CMD_CUT4:
			case CMD_UVROT:
			case CMD_LEGEND:
			case CMD_ANIMATINGMIP:
			case CMD_RESETHUB:
				n++;
				break;

			case CMD_FOG:
				n += 3;
				break;

			case CMD_TITLE:
			case CMD_LAYER1:
			case CMD_LAYER2:
				n += 4;
				break;

			case CMD_MIRROR:
				n += 5;
				break;

			case CMD_LENSFLARE:
				n += 9;
				break;

			case CMD_CAMERA:
				n += 25;
				break;

			case CMD_LEVEL:
				gfLevelNames[i] = *n;
				n += 5;
				break;

			case CMD_ENDSEQ:
				end = 1;
				break;

			default:
				n += 2;
				break;
			}
		}
	}
}

long DoCredits()
{
	const char* s;
	static ulong StartPos = 0;
	static long init = 0;
	long y, num_drawn;

	num_drawn = 0;

	if (!init)
	{
		StartPos = font_height + phd_winheight;
		init = 1;
	}

	y = StartPos;

	for (int i = 0; i < sizeof(CreditsTable) / 4; i++)
	{
		s = CreditsTable[i];

		if (y < font_height + phd_winheight + 1 && y > -font_height)
		{
			if (*s == '%')
				PrintString(phd_winwidth >> 1, y, 6, SCRIPT_TEXT(CreditGroups[atoi(s + 1)]), FF_CENTER);
			else if (*s != '0')
				PrintString(phd_winwidth >> 1, y, 2 + (i == 72 ? 4 : 0), s, FF_CENTER);

			num_drawn++;
		}

		y += font_height;
	}

	StartPos--;

	if (!num_drawn)
		init = 0;

	return num_drawn;
}
