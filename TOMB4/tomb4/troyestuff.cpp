#include "../tomb4/pch.h"
#include "troyestuff.h"
#include "../game/sound.h"
#include "../game/text.h"
#include "tomb4.h"

#define PAGE0_NUM	10
#define PAGE1_NUM	2

#pragma warning(push)
#pragma warning(disable : 4244)
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

	switch (page)
	{
	case 0:
		changed = Page0(num, textY, selection);
		break;

	case 1:
		changed = Page1(num, textY, selection);
		break;
	}

	PrintString(phd_centerx - (phd_centerx >> 3), (ushort)(textY + (num + 2) * font_height), selection & (1 << num) ? 1 : 6, "\x19", 0);
	PrintString(phd_centerx + (phd_centerx >> 3), (ushort)(textY + (num + 2) * font_height), selection & (1 << num) ? 1 : 6, "\x1B", 0);

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
	bool changed;

	changed = 0;
	num = PAGE0_NUM;
	PrintString(phd_centerx, 2 * font_height, 6, "New tomb4 options", FF_CENTER);
	PrintString(phd_centerx >> 2, textY + 2 * font_height, selection & 0x1 ? 1 : 2, "FootPrints", 0);
	PrintString(phd_centerx >> 2, textY + 3 * font_height, selection & 0x2 ? 1 : 2, "Shadow mode", 0);
	PrintString(phd_centerx >> 2, textY + 4 * font_height, selection & 0x4 ? 1 : 2, "Crawl Tilting", 0);
	PrintString(phd_centerx >> 2, textY + 5 * font_height, selection & 0x8 ? 1 : 2, "Flexible crawling", 0);
	PrintString(phd_centerx >> 2, textY + 6 * font_height, selection & 0x10 ? 1 : 2, "Fix climb up delay", 0);
	PrintString(phd_centerx >> 2, textY + 7 * font_height, selection & 0x20 ? 1 : 2, "Gameover menu", 0);
	PrintString(phd_centerx >> 2, textY + 8 * font_height, selection & 0x40 ? 1 : 2, "Bar mode", 0);
	PrintString(phd_centerx >> 2, textY + 9 * font_height, selection & 0x80 ? 1 : 2, "Bar positions", 0);
	PrintString(phd_centerx >> 2, textY + 10 * font_height, selection & 0x100 ? 1 : 2, "Enemy bars", 0);
	PrintString(phd_centerx >> 2, textY + 11 * font_height, selection & 0x200 ? 1 : 2, "Cutscene skipper", 0);

	strcpy(buffer, tomb4.footprints ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 1), textY + 2 * font_height, selection & 0x1 ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb4.shadow_mode == 1 ? "original" : tomb4.shadow_mode == 2 ? "circle" : tomb4.shadow_mode == 3 ? "improved PSX" : "PSX");
	PrintString(phd_centerx + (phd_centerx >> 1), textY + 3 * font_height, selection & 0x2 ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb4.crawltilt ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 1), textY + 4 * font_height, selection & 0x4 ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb4.flexible_crawling ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 1), textY + 5 * font_height, selection & 0x8 ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb4.fix_climb_up_delay ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 1), textY + 6 * font_height, selection & 0x10 ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb4.gameover ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 1), textY + 7 * font_height, selection & 0x20 ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb4.bar_mode == 1 ? "original" : tomb4.bar_mode == 2 ? "TR5" : "PSX");
	PrintString(phd_centerx + (phd_centerx >> 1), textY + 8 * font_height, selection & 0x40 ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb4.bars_pos == 1 ? "original" : tomb4.bars_pos == 2 ? "improved" : "PSX");
	PrintString(phd_centerx + (phd_centerx >> 1), textY + 9 * font_height, selection & 0x80 ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb4.enemy_bars ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 1), textY + 10 * font_height, selection & 0x100 ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb4.cutseq_skipper ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 1), textY + 11 * font_height, selection & 0x200 ? 1 : 6, buffer, 0);

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
	}

	return changed;
}

bool Page1(long& num, long textY, ulong selection)
{
	char buffer[80];
	bool changed;

	changed = 0;
	num = PAGE1_NUM;
	PrintString(phd_centerx, 2 * font_height, 6, "New tomb4 options", FF_CENTER);
	PrintString(phd_centerx >> 2, textY + 2 * font_height, selection & 1 ? 1 : 2, "Cheats", 0);
	PrintString(phd_centerx >> 2, textY + 3 * font_height, selection & 2 ? 1 : 2, "Loading text", 0);

	strcpy(buffer, tomb4.cheats ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 1), textY + 2 * font_height, selection & 1 ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb4.loadingtxt ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 1), textY + 3 * font_height, selection & 2 ? 1 : 6, buffer, 0);

	switch (selection)
	{
	case 1 << 0:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.cheats = !tomb4.cheats;
			changed = 1;
		}

		break;

	case 1 << 1:

		if (dbinput & IN_LEFT || dbinput & IN_RIGHT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.loadingtxt = !tomb4.loadingtxt;
			changed = 1;
		}

		break;
	}

	return changed;
}
#pragma warning(pop)