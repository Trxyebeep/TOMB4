#include "../tomb4/pch.h"
#include "troyestuff.h"
#include "../game/sound.h"
#include "../game/text.h"
#include "tomb4.h"
#include "../specific/input.h"
#include "../specific/3dmath.h"

#define PAGE0_NUM	14
#define PAGE1_NUM	9
#define YPOS	textY + y++ * font_height
#define CHECK_SEL(c)	selection & (1 << s++) ? 1 : c

void TroyeMenu(long textY, long& menu, ulong& selection)
{
	long num;
	static long page = 0;
	bool changed;

	num = 0;
	changed = 0;

	if (dbinput & IN_DESELECT)
	{
		SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
		menu = 0;
		dbinput &= ~IN_DESELECT;
		page = 0;
		return;
	}

	font_height = smol_font_height;

	switch (page)
	{
	case 0:
		changed = Page0(num, textY, selection);
		break;

	case 1:
		changed = Page1(num, textY, selection);
		break;
	}

	PrintString(phd_centerx - (phd_centerx >> 3), textY + (num + 2) * font_height, selection & (1 << num) ? 1 : 6, "\x19", 0);
	PrintString(phd_centerx + (phd_centerx >> 3), textY + (num + 2) * font_height, selection & (1 << num) ? 1 : 6, "\x1B", 0);

	font_height = stash_font_height;

	if (dbinput & IN_FORWARD)
	{
		SoundEffect(SFX_MENU_CHOOSE, 0, SFX_ALWAYS);
		selection >>= 1;
	}

	if (dbinput & IN_BACK)
	{
		SoundEffect(SFX_MENU_CHOOSE, 0, SFX_ALWAYS);
		selection <<= 1;
	}

	if (!selection)
		selection = 1;

	if (selection > ulong(1 << num))
		selection = 1 << num;

	if (selection & (1 << num))
	{
		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);

			if (page)
			{
				page = 0;
				num = PAGE0_NUM;
			}
			else
			{
				page = 1;
				num = PAGE1_NUM;
			}

			selection = 1 << num;
		}
	}

	if (changed)
		save_new_tomb4_settings();
}

bool Page0(long& num, long textY, ulong selection)
{
	char buffer[80];
	long y, s;
	bool changed;

	changed = 0;
	num = PAGE0_NUM;
	y = 2;
	s = 0;

	PrintString(phd_centerx, 2 * font_height, 6, "New tomb4 options", FF_CENTER);
	PrintString(phd_centerx >> 2, YPOS, CHECK_SEL(2), "FootPrints", 0);
	PrintString(phd_centerx >> 2, YPOS, CHECK_SEL(2), "Shadow mode", 0);
	PrintString(phd_centerx >> 2, YPOS, CHECK_SEL(2), "Crawl Tilting", 0);
	PrintString(phd_centerx >> 2, YPOS, CHECK_SEL(2), "Flexible crawling", 0);
	PrintString(phd_centerx >> 2, YPOS, CHECK_SEL(2), "Fix climb up delay", 0);
	PrintString(phd_centerx >> 2, YPOS, CHECK_SEL(2), "Gameover menu", 0);
	PrintString(phd_centerx >> 2, YPOS, CHECK_SEL(2), "Bar mode", 0);
	PrintString(phd_centerx >> 2, YPOS, CHECK_SEL(2), "Bar positions", 0);
	PrintString(phd_centerx >> 2, YPOS, CHECK_SEL(2), "Enemy bars", 0);
	PrintString(phd_centerx >> 2, YPOS, CHECK_SEL(2), "Cutscene skipper", 0);
	PrintString(phd_centerx >> 2, YPOS, CHECK_SEL(2), "Cheats", 0);
	PrintString(phd_centerx >> 2, YPOS, CHECK_SEL(2), "Loading text", 0);
	PrintString(phd_centerx >> 2, YPOS, CHECK_SEL(2), "mono screen style", 0);
	PrintString(phd_centerx >> 2, YPOS, CHECK_SEL(2), "loadbar style", 0);

	y = 2;
	s = 0;

	strcpy(buffer, tomb4.footprints ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 1), YPOS, CHECK_SEL(6), buffer, 0);

	strcpy(buffer, tomb4.shadow_mode == 1 ? "original" : tomb4.shadow_mode == 2 ? "circle" : tomb4.shadow_mode == 3 ? "faded circle" : "PSX");
	PrintString(phd_centerx + (phd_centerx >> 1), YPOS, CHECK_SEL(6), buffer, 0);

	strcpy(buffer, tomb4.crawltilt ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 1), YPOS, CHECK_SEL(6), buffer, 0);

	strcpy(buffer, tomb4.flexible_crawling ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 1), YPOS, CHECK_SEL(6), buffer, 0);

	strcpy(buffer, tomb4.fix_climb_up_delay ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 1), YPOS, CHECK_SEL(6), buffer, 0);

	strcpy(buffer, tomb4.gameover ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 1), YPOS, CHECK_SEL(6), buffer, 0);

	strcpy(buffer, tomb4.bar_mode == 1 ? "original" : tomb4.bar_mode == 2 ? "TR5" : "PSX");
	PrintString(phd_centerx + (phd_centerx >> 1), YPOS, CHECK_SEL(6), buffer, 0);

	strcpy(buffer, tomb4.bars_pos == 1 ? "original" : tomb4.bars_pos == 2 ? "improved" : "PSX");
	PrintString(phd_centerx + (phd_centerx >> 1), YPOS, CHECK_SEL(6), buffer, 0);

	strcpy(buffer, tomb4.enemy_bars ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 1), YPOS, CHECK_SEL(6), buffer, 0);

	strcpy(buffer, tomb4.cutseq_skipper ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 1), YPOS, CHECK_SEL(6), buffer, 0);

	strcpy(buffer, tomb4.cheats ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 1), YPOS, CHECK_SEL(6), buffer, 0);

	strcpy(buffer, tomb4.loadingtxt ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 1), YPOS, CHECK_SEL(6), buffer, 0);

	strcpy(buffer, tomb4.inv_bg_mode == 1 ? "original" : tomb4.inv_bg_mode == 2 ? "TR5" : "clear");
	PrintString(phd_centerx + (phd_centerx >> 1), YPOS, CHECK_SEL(6), buffer, 0);

	strcpy(buffer, tomb4.tr5_loadbar ? "TR5" : "TR4");
	PrintString(phd_centerx + (phd_centerx >> 1), YPOS, CHECK_SEL(6), buffer, 0);

	switch (selection)
	{
	case 1 << 0:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.footprints = !tomb4.footprints;
			changed = 1;
		}

		break;

	case 1 << 1:

		if (dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.shadow_mode++;

			if (tomb4.shadow_mode > 4)
				tomb4.shadow_mode = 1;

			changed = 1;
		}

		if (dbinput & IN_LEFT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.shadow_mode--;

			if (tomb4.shadow_mode < 1)
				tomb4.shadow_mode = 4;

			changed = 1;
		}

		break;

	case 1 << 2:

		if (dbinput & IN_RIGHT || dbinput & IN_LEFT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.crawltilt = !tomb4.crawltilt;
			changed = 1;
		}

		break;

	case 1 << 3:

		if (dbinput & IN_RIGHT || dbinput & IN_LEFT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.flexible_crawling = !tomb4.flexible_crawling;
			changed = 1;
		}

		break;

	case 1 << 4:

		if (dbinput & IN_RIGHT || dbinput & IN_LEFT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.fix_climb_up_delay = !tomb4.fix_climb_up_delay;
			changed = 1;
		}

		break;

	case 1 << 5:

		if (dbinput & IN_RIGHT || dbinput & IN_LEFT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.gameover = !tomb4.gameover;
			changed = 1;
		}

		break;

	case 1 << 6:

		if (dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.bar_mode++;

			if (tomb4.bar_mode > 3)
				tomb4.bar_mode = 1;

			changed = 1;
		}

		if (dbinput & IN_LEFT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.bar_mode--;

			if (tomb4.bar_mode < 1)
				tomb4.bar_mode = 3;

			changed = 1;
		}

		break;

	case 1 << 7:

		if (dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.bars_pos++;

			if (tomb4.bars_pos > 3)
				tomb4.bars_pos = 1;

			changed = 1;
		}

		if (dbinput & IN_LEFT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.bars_pos--;

			if (tomb4.bars_pos < 1)
				tomb4.bars_pos = 3;

			changed = 1;
		}

		break;

	case 1 << 8:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.enemy_bars = !tomb4.enemy_bars;
			changed = 1;
		}

		break;

	case 1 << 9:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.cutseq_skipper = !tomb4.cutseq_skipper;
			changed = 1;
		}

		break;

	case 1 << 10:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.cheats = !tomb4.cheats;
			changed = 1;
		}

		break;

	case 1 << 11:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.loadingtxt = !tomb4.loadingtxt;
			changed = 1;
		}

		break;

	case 1 << 12:

		if (dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.inv_bg_mode++;

			if (tomb4.inv_bg_mode > 3)
				tomb4.inv_bg_mode = 1;

			changed = 1;
		}

		if (dbinput & IN_LEFT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.inv_bg_mode--;

			if (tomb4.inv_bg_mode < 1)
				tomb4.inv_bg_mode = 3;

			changed = 1;
		}

		break;

	case 1 << 13:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.tr5_loadbar = !tomb4.tr5_loadbar;
			changed = 1;
		}

		break;
	}

	return changed;
}

bool Page1(long& num, long textY, ulong selection)
{
	char buffer[80];
	long y, s;
	bool changed;

	changed = 0;
	num = PAGE1_NUM;
	y = 2;
	s = 0;

	PrintString(phd_centerx, 2 * font_height, 6, "New tomb4 options", FF_CENTER);
	PrintString(phd_centerx >> 2, YPOS, CHECK_SEL(2), "look transparency", 0);
	PrintString(phd_centerx >> 2, YPOS, CHECK_SEL(2), "ammo counter", 0);
	PrintString(phd_centerx >> 2, YPOS, CHECK_SEL(2), "ammotype hotkeys", 0);
	PrintString(phd_centerx >> 2, YPOS, CHECK_SEL(2), "combat cam tilt", 0);
	PrintString(phd_centerx >> 2, YPOS, CHECK_SEL(2), "Inv healthbar", 0);
	PrintString(phd_centerx >> 2, YPOS, CHECK_SEL(2), "static lighting", 0);
	PrintString(phd_centerx >> 2, YPOS, CHECK_SEL(2), "Reverb", 0);
	PrintString(phd_centerx >> 2, YPOS, CHECK_SEL(2), "distance fog", 0);
	PrintString(phd_centerx >> 2, YPOS, CHECK_SEL(2), "Bars scale", 0);

	y = 2;
	s = 0;

	strcpy(buffer, tomb4.look_transparency ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 1), YPOS, CHECK_SEL(6), buffer, 0);

	strcpy(buffer, tomb4.ammo_counter ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 1), YPOS, CHECK_SEL(6), buffer, 0);

	strcpy(buffer, tomb4.ammotype_hotkeys ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 1), YPOS, CHECK_SEL(6), buffer, 0);

	strcpy(buffer, tomb4.combat_cam_tilt ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 1), YPOS, CHECK_SEL(6), buffer, 0);

	strcpy(buffer, tomb4.hpbar_inv ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 1), YPOS, CHECK_SEL(6), buffer, 0);

	strcpy(buffer, tomb4.static_lighting ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 1), YPOS, CHECK_SEL(6), buffer, 0);

	strcpy(buffer, tomb4.reverb == 1 ? "off" : tomb4.reverb == 2 ? "Lara room" : "Camera room");
	PrintString(phd_centerx + (phd_centerx >> 1), YPOS, CHECK_SEL(6), buffer, 0);

	sprintf(buffer, "%i", tomb4.distance_fog);
	PrintString(phd_centerx + (phd_centerx >> 1), YPOS, CHECK_SEL(6), buffer, 0);

	sprintf(buffer, "%.1f", tomb4.GUI_Scale);
	PrintString(phd_centerx + (phd_centerx >> 1), YPOS, CHECK_SEL(6), buffer, 0);

	switch (selection)
	{
	case 1 << 0:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.look_transparency = !tomb4.look_transparency;
			changed = 1;
		}

		break;

	case 1 << 1:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.ammo_counter = !tomb4.ammo_counter;
			changed = 1;
		}

		break;

	case 1 << 2:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.ammotype_hotkeys = !tomb4.ammotype_hotkeys;
			changed = 1;
		}

		break;

	case 1 << 3:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.combat_cam_tilt = !tomb4.combat_cam_tilt;
			changed = 1;
		}

		break;

	case 1 << 4:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.hpbar_inv = !tomb4.hpbar_inv;
			changed = 1;
		}

		break;

	case 1 << 5:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.static_lighting = !tomb4.static_lighting;
			changed = 1;
		}

		break;

	case 1 << 6:

		if (dbinput & IN_LEFT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.reverb--;

			if (tomb4.reverb < 1)
				tomb4.reverb = 3;

			changed = 1;
		}

		if (dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.reverb++;

			if (tomb4.reverb > 3)
				tomb4.reverb = 1;

			changed = 1;
		}

		break;

	case 1 << 7:

		if (dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.distance_fog++;

			if (tomb4.distance_fog > 30)
				tomb4.distance_fog = 30;

			changed = 1;
		}

		if (dbinput & IN_LEFT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.distance_fog--;

			if (tomb4.distance_fog < 3)
				tomb4.distance_fog = 3;

			changed = 1;
		}

		break;

	case 1 << 8:

		if (dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.GUI_Scale += 0.1F;

			if (tomb4.GUI_Scale > 1.9F)
				tomb4.GUI_Scale = 1.9F;

			changed = 1;
		}

		if (dbinput & IN_LEFT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.GUI_Scale -= 0.1F;

			if (tomb4.GUI_Scale < 1.0F)
				tomb4.GUI_Scale = 1.0F;

			changed = 1;
		}

		break;
	}

	return changed;
}

#undef PAGE0_NUM
#undef PAGE1_NUM
#undef YPOS
#undef CHECK_SEL
