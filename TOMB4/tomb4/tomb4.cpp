#include "../tomb4/pch.h"
#include "tomb4.h"
#include "../specific/registry.h"
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

		sprintf(buf, "mmode");
		tomb4.mirrorMode = 0;							//off
		REG_WriteBool(buf, tomb4.mirrorMode);
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

		sprintf(buf, "mmode");
		REG_ReadBool(buf, tomb4.mirrorMode, 0);
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

	sprintf(buf, "mmode");
	REG_WriteBool(buf, tomb4.mirrorMode);

	CloseRegistry();
}