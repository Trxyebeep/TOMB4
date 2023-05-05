#include "../tomb4/pch.h"
#include "tomb4.h"
#include "../specific/registry.h"
#include "libs/discordRPC/discord_rpc.h"
#include "../game/lara.h"
#include "../game/gameflow.h"

tomb4_options tomb4;

void init_tomb4_stuff()
{
	char buf[40];
	bool first;

	OpenRegistry("tomb4");
	first = REG_KeyWasCreated();

	if (first)	//key was created = no settings found, write defaults
	{
		sprintf(buf, "footprints");
		tomb4.footprints = 1;							//footprints on
		REG_WriteBool(buf, tomb4.footprints);

		sprintf(buf, "shadow");
		tomb4.shadow_mode = 3;							//PSX like shadow
		REG_WriteLong(buf, tomb4.shadow_mode);

		sprintf(buf, "crawltilt");
		tomb4.crawltilt = 1;							//crawl tilt on
		REG_WriteBool(buf, tomb4.crawltilt);

		sprintf(buf, "flex_crawl");
		tomb4.flexible_crawling = 1;					//on
		REG_WriteBool(buf, tomb4.flexible_crawling);

		sprintf(buf, "climbup");
		tomb4.fix_climb_up_delay = 1;					//no delay
		REG_WriteBool(buf, tomb4.fix_climb_up_delay);

		sprintf(buf, "gameover");
		tomb4.gameover = 1;								//on
		REG_WriteBool(buf, tomb4.gameover);

		sprintf(buf, "barMode");
		tomb4.bar_mode = 3;								//PSX
		REG_WriteLong(buf, tomb4.bar_mode);

		sprintf(buf, "bar_pos");
		tomb4.bars_pos = 1;								//improved
		REG_WriteLong(buf, tomb4.bars_pos);

		sprintf(buf, "enemy_bar");
		tomb4.enemy_bars = 0;							//off
		REG_WriteBool(buf, tomb4.enemy_bars);

		sprintf(buf, "cutseq_skipper");
		tomb4.cutseq_skipper = 0;						//off
		REG_WriteBool(buf, tomb4.cutseq_skipper);

		sprintf(buf, "cheats");
		tomb4.cheats = 0;								//off
		REG_WriteBool(buf, tomb4.cheats);

		sprintf(buf, "loadtxt");
		tomb4.loadingtxt = 1;							//on
		REG_WriteBool(buf, tomb4.loadingtxt);

		sprintf(buf, "inv_bgM");
		tomb4.inv_bg_mode = 1;							//original
		REG_WriteLong(buf, tomb4.inv_bg_mode);

		sprintf(buf, "tr5LB");
		tomb4.tr5_loadbar = 0;							//off
		REG_WriteBool(buf, tomb4.tr5_loadbar);

		sprintf(buf, "ltransparency");
		tomb4.look_transparency = 0;					//off
		REG_WriteBool(buf, tomb4.look_transparency);

		sprintf(buf, "ammo_counter");
		tomb4.ammo_counter = 0;							//off
		REG_WriteBool(buf, tomb4.ammo_counter);

		sprintf(buf, "ammotype_hotkeys");
		tomb4.ammotype_hotkeys = 1;						//on
		REG_WriteBool(buf, tomb4.ammotype_hotkeys);

		sprintf(buf, "combat_tilt");
		tomb4.combat_cam_tilt = 1;						//on
		REG_WriteBool(buf, tomb4.combat_cam_tilt);

		sprintf(buf, "inv_hpbar");
		tomb4.hpbar_inv = 0;							//off
		REG_WriteBool(buf, tomb4.hpbar_inv);

		sprintf(buf, "static_lighting");
		tomb4.static_lighting = 1;						//on
		REG_WriteBool(buf, tomb4.static_lighting);

		sprintf(buf, "reverb");
		tomb4.reverb = 2;								//Lara room
		REG_WriteLong(buf, tomb4.reverb);

		sprintf(buf, "distance_fog");
		tomb4.distance_fog = 12;						//default is 12
		REG_WriteLong(buf, tomb4.distance_fog);

		sprintf(buf, "UIScale");
		tomb4.GUI_Scale = 1.0F;							//default is 1.0F
		REG_WriteFloat(buf, tomb4.GUI_Scale);
	}
	else	//Key already exists, settings already written, read them. also falls back to default if any of them missing
	{
		sprintf(buf, "footprints");
		REG_ReadBool(buf, tomb4.footprints, 1);

		sprintf(buf, "shadow");
		REG_ReadLong(buf, tomb4.shadow_mode, 3);

		sprintf(buf, "crawltilt");
		REG_ReadBool(buf, tomb4.crawltilt, 1);

		sprintf(buf, "flex_crawl");
		REG_ReadBool(buf, tomb4.flexible_crawling, 1);

		sprintf(buf, "climbup");
		REG_ReadBool(buf, tomb4.fix_climb_up_delay, 1);

		sprintf(buf, "gameover");
		REG_ReadBool(buf, tomb4.gameover, 1);

		sprintf(buf, "barMode");
		REG_ReadLong(buf, tomb4.bar_mode, 3);

		sprintf(buf, "bar_pos");
		REG_ReadLong(buf, tomb4.bars_pos, 1);

		sprintf(buf, "enemy_bar");
		REG_ReadBool(buf, tomb4.enemy_bars, 0);

		sprintf(buf, "cutseq_skipper");
		REG_ReadBool(buf, tomb4.cutseq_skipper, 0);

		sprintf(buf, "cheats");
		REG_ReadBool(buf, tomb4.cheats, 0);

		sprintf(buf, "loadtxt");
		REG_ReadBool(buf, tomb4.loadingtxt, 1);

		sprintf(buf, "inv_bgM");
		REG_ReadLong(buf, tomb4.inv_bg_mode, 1);

		sprintf(buf, "tr5LB");
		REG_ReadBool(buf, tomb4.tr5_loadbar, 0);

		sprintf(buf, "ltransparency");
		REG_ReadBool(buf, tomb4.look_transparency, 1);

		sprintf(buf, "ammo_counter");
		REG_ReadBool(buf, tomb4.ammo_counter, 0);

		sprintf(buf, "ammotype_hotkeys");
		REG_ReadBool(buf, tomb4.ammotype_hotkeys, 1);

		sprintf(buf, "combat_tilt");
		REG_ReadBool(buf, tomb4.combat_cam_tilt, 1);

		sprintf(buf, "inv_hpbar");
		REG_ReadBool(buf, tomb4.hpbar_inv, 0);

		sprintf(buf, "static_lighting");
		REG_ReadBool(buf, tomb4.static_lighting, 1);

		sprintf(buf, "reverb");
		REG_ReadLong(buf, tomb4.reverb, 1);

		sprintf(buf, "distance_fog");
		REG_ReadLong(buf, tomb4.distance_fog, 12);

		sprintf(buf, "UIScale");
		REG_ReadFloat(buf, tomb4.GUI_Scale, 1.0F);
	}

	CloseRegistry();
}

void save_new_tomb4_settings()
{
	char buf[40];

	OpenRegistry("tomb4");

	sprintf(buf, "footprints");
	REG_WriteBool(buf, tomb4.footprints);

	sprintf(buf, "shadow");
	REG_WriteLong(buf, tomb4.shadow_mode);

	sprintf(buf, "crawltilt");
	REG_WriteLong(buf, tomb4.crawltilt);

	sprintf(buf, "flex_crawl");
	REG_WriteBool(buf, tomb4.flexible_crawling);

	sprintf(buf, "climbup");
	REG_WriteBool(buf, tomb4.fix_climb_up_delay);

	sprintf(buf, "gameover");
	REG_WriteBool(buf, tomb4.gameover);

	sprintf(buf, "barMode");
	REG_WriteLong(buf, tomb4.bar_mode);

	sprintf(buf, "bar_pos");
	REG_WriteLong(buf, tomb4.bars_pos);

	sprintf(buf, "enemy_bar");
	REG_WriteBool(buf, tomb4.enemy_bars);

	sprintf(buf, "cutseq_skipper");
	REG_WriteBool(buf, tomb4.cutseq_skipper);

	sprintf(buf, "cheats");
	REG_WriteBool(buf, tomb4.cheats);

	sprintf(buf, "loadtxt");
	REG_WriteBool(buf, tomb4.loadingtxt);

	sprintf(buf, "inv_bgM");
	REG_WriteLong(buf, tomb4.inv_bg_mode);

	sprintf(buf, "tr5LB");
	REG_WriteBool(buf, tomb4.tr5_loadbar);

	sprintf(buf, "ltransparency");
	REG_WriteBool(buf, tomb4.look_transparency);

	sprintf(buf, "ammo_counter");
	REG_WriteBool(buf, tomb4.ammo_counter);

	sprintf(buf, "ammotype_hotkeys");
	REG_WriteBool(buf, tomb4.ammotype_hotkeys);

	sprintf(buf, "combat_tilt");
	REG_WriteBool(buf, tomb4.combat_cam_tilt);

	sprintf(buf, "inv_hpbar");
	REG_WriteBool(buf, tomb4.hpbar_inv);

	sprintf(buf, "static_lighting");
	REG_WriteBool(buf, tomb4.static_lighting);

	sprintf(buf, "reverb");
	REG_WriteLong(buf, tomb4.reverb);

	sprintf(buf, "distance_fog");
	REG_WriteLong(buf, tomb4.distance_fog);

	sprintf(buf, "UIScale");
	REG_WriteFloat(buf, tomb4.GUI_Scale);

	CloseRegistry();
}

void RPC_Init()
{
	DiscordEventHandlers handlers;

	memset(&handlers, 0, sizeof(handlers));
	Discord_Initialize("946240885866262598", &handlers, 1, 0);
}

const char* RPC_GetLevelName()
{
	if (!gfCurrentLevel)
	{
		if (bDoCredits)
			return "In Credits";
		else
			return "In Title";
	}
	else
		return SCRIPT_TEXT(gfLevelNames[gfCurrentLevel]);
}

const char* RPC_GetTimer()
{
	long sec, days, hours, min;
	static char buf[64];

	sec = GameTimer / 30;
	days = sec / 86400;
	hours = (sec % 86400) / 3600;
	min = (sec / 60) % 60;
	sec = (sec % 60);
	sprintf(buf, "Time Taken: %02d:%02d:%02d", (days * 24) + hours, min, sec);
	return buf;
}

const char* RPC_GetLevelPic()
{
	switch (gfCurrentLevel)
	{
	case 1:
		return "angkor";
		
	case 2:
		return "iris";

	case 3:
		return "seth";

	case 4:
		return "chamber";

	case 5:
		return "valley";

	case 6:
		return "kv5";

	case 7:
		return "karnak";

	case 8:
		return "hypostyle";

	case 9:
		return "lake";

		//10 doesnt exist

	case 11:
		return "senet";

	case 12:
		return "guardian";

	case 13:
		return "train";

	case 14:
		return "alexandria";

	case 15:
		return "coastal";

	case 16:
		return "isis";

	case 17:
		return "cleopetra";

	case 18:
		return "catacomb";

	case 19:
		return "poseidon";

	case 20:
		return "library";

	case 21:
		return "demi";

	case 22:
		return "city";

	case 23:
		return "trenches";

	case 24:
		return "tulun";

	case 25:
		return "bazaar";

	case 26:
		return "gate";

	case 27:
		return "citadel";

	case 28:
		return "sphinx";

		//29 doesn't exist

	case 30:
		return "under";

	case 31:
		return "menkaure";

	case 32:
		return "inmenkaure";

	case 33:
		return "mastabas";

	case 34:
		return "great";

	case 35:
		return "khufu";

	case 36:
		return "ingreat";

	case 37:
		return "horusa";

	case 38:
		return "horusb";

	default:
		return "default";
	}
}

const char* RPC_GetHealthPic()
{
	if (lara_item->hit_points > 666)
		return "green";
	
	if (lara_item->hit_points > 333)
		return "yellow";

	return "red";
}

const char* RPC_GetHealthPercentage()
{
	static char buf[32];

	sprintf(buf, "Health: %i%%", lara_item->hit_points / 10);
	return buf;
}

void RPC_Update()
{
	DiscordRichPresence RPC;

	memset(&RPC, 0, sizeof(RPC));

	RPC.details = RPC_GetLevelName();
	RPC.largeImageKey = RPC_GetLevelPic();
	RPC.largeImageText = RPC.details;

	RPC.smallImageKey = RPC_GetHealthPic();
	RPC.smallImageText = RPC_GetHealthPercentage();

	RPC.state = RPC_GetTimer();

	RPC.instance = 1;
	Discord_UpdatePresence(&RPC);
}

void RPC_close()
{
	Discord_Shutdown();
}
