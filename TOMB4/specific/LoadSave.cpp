#include "../tomb4/pch.h"
#include "LoadSave.h"
#include "../game/text.h"
#include "../game/sound.h"
#include "audio.h"
#include "dxsound.h"
#include "input.h"
#include "function_table.h"
#include "drawroom.h"
#include "polyinsert.h"
#include "winmain.h"
#include "output.h"
#include "../game/gameflow.h"
#ifdef GENERAL_FIXES
#include "../tomb4/tomb4.h"
#endif

void S_DrawHealthBar(long pos)
{
	if (gfCurrentLevel)
	{
		if (lara.poisoned)
			DoBar(font_height >> 2, font_height >> 2, 150, 12, pos, 0xFF000000, 0xFFFFFF00);	//yellow
		else
			DoBar(font_height >> 2, font_height >> 2, 150, 12, pos, 0xFF000000, 0xFFFF0000);	//red
	}
}

void S_DrawAirBar(long pos)
{
	if (gfCurrentLevel)
		DoBar(490 - (font_height >> 2), (font_height >> 2) + (font_height >> 1), 150, 12, pos, 0xFF000000, 0xFF0000FF);	//blue
}

void S_DrawDashBar(long pos)
{
	if (gfCurrentLevel)
		DoBar(490 - (font_height >> 2), font_height >> 2, 150, 12, pos, 0xFF000000, 0xFF00FF00);	//green
}

void S_InitLoadBar(long maxpos)
{
	loadbar_pos = 0;
	loadbar_maxpos = maxpos;
}

void S_LoadBar()
{
	if (gfCurrentLevel || App.dx.Flags & 0x80)
	{
		_BeginScene();
		InitBuckets();
		InitialiseSortList();
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
		App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);
		loadbar_pos += 100 / loadbar_maxpos;
		DoBar(20, phd_winymax - font_height, 600, 15, (long)loadbar_pos, 0xFF000000, 0xFF9F1F80);
		SortPolyList(SortCount, SortList);
		RestoreFPCW(FPCW);
		DrawSortList();
		MungeFPCW(&FPCW);
		S_DumpScreen();
	}
}

void DoBar(long x, long y, long width, long height, long pos, long clr1, long clr2)
{
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	static float wat = 0;
	long x2, sx, sy;

	nPolyType = 4;
	wat += 0.0099999998F;

	if (wat > 0.99000001F)
		wat = 0;

	clipflags[0] = 0;
	clipflags[1] = 0;
	clipflags[2] = 0;
	clipflags[3] = 0;

	x2 = (long)(x * phd_winxmax * 0.0015625F);
	sx = (long(width * phd_winxmax * 0.0015625F) * pos) / 100;
	sy = (long)((height >> 1) * phd_winymax * 0.0020833334F);

	v[0].sx = (float)x2;
	v[0].sy = (float)y;
	v[0].color = clr1;

	v[1].sx = (float)(x2 + sx);
	v[1].sy = (float)y;
	v[1].color = clr1;

	v[2].sx = (float)(x2 + sx);
	v[2].sy = (float)(y + sy);
	v[2].color = clr2;

	v[3].sx = (float)x2;
	v[3].sy = (float)(y + sy);
	v[3].color = clr2;

	for (int i = 0; i < 4; i++)
	{
		v[i].specular = 0xFF000000;
		v[i].sz = f_mznear - 6;
		v[i].rhw = f_moneoznear - 2;
		v[i].tu = 0;
		v[i].tv = 0;
	}

	tex.drawtype = 0;
	tex.flag = 0;
	tex.tpage = 0;
	AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);

	v[0].sx = (float)x2;
	v[0].sy = (float)(y + sy);
	v[0].color = clr2;

	v[1].sx = (float)(x2 + sx);
	v[1].sy = (float)(y + sy);
	v[1].color = clr2;

	v[2].sx = (float)(x2 + sx);
	v[2].sy = (float)(y + 2 * sy);
	v[2].color = clr1;

	v[3].sx = (float)x2;
	v[3].sy = (float)(y + 2 * sy);
	v[3].color = clr1;

	for (int i = 0; i < 4; i++)
	{
		v[i].specular = 0xFF000000;
		v[i].sz = f_mznear - 6;
		v[i].rhw = f_moneoznear - 2;
		v[i].tu = 0;
		v[i].tv = 0;
	}

	AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);

	x2 = (long)(x * phd_winxmax * 0.0015625F);
	sx = (long)(width * phd_winxmax * 0.0015625F);
	sy = (long)((height >> 1) * phd_winymax * 0.0020833334F);

	v[0].sx = (float)(x2 - 1);
	v[0].sy = (float)(y - 1);

	v[1].sx = (float)(x2 + sx + 1);
	v[1].sy = (float)(y - 1);

	v[2].sx = (float)(x2 + sx + 1);
	v[2].sy = (float)(y + 2 * sy + 1);

	v[3].sx = (float)(x2 - 1);
	v[3].sy = (float)(y + 2 * sy + 1);

	for (int i = 0; i < 4; i++)
	{
		v[i].color = 0xFFFFFFFF;
		v[i].specular = 0xFF000000;
		v[i].sz = f_mznear - 3;
		v[i].rhw = f_moneoznear - 4;
		v[i].tu = 0;
		v[i].tv = 0;
	}

	AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);

	v[0].sx = (float)x2;
	v[0].sy = (float)y;

	v[1].sx = (float)(x2 + sx);
	v[1].sy = (float)y;

	v[2].sx = (float)(x2 + sx);
	v[2].sy = (float)(y + 2 * sy);

	v[3].sx = (float)x2;
	v[3].sy = (float)(y + 2 * sy);

	for (int i = 0; i < 4; i++)
	{
		v[i].color = 0xFF000000;
		v[i].specular = 0xFF000000;
		v[i].sz = f_mznear - 4;
		v[i].rhw = f_moneoznear - 3;
		v[i].tu = 0;
		v[i].tv = 0;
	}

	AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
}

#pragma warning(push)
#pragma warning(disable : 4244)

#ifdef GENERAL_FIXES
static void TroyeMenu(long textY, long& menu, ulong& selection)
{
	long num;
	char buffer[80];
	bool changed;

	num = 3;
	PrintString(phd_centerx, 2 * font_height, 6, "New tomb4 options", FF_CENTER);
	PrintString(phd_centerx >> 2, textY + 2 * font_height, selection & 1 ? 1 : 2, "FootPrints", 0);
	PrintString(phd_centerx >> 2, textY + 3 * font_height, selection & 2 ? 1 : 2, "Shadow mode", 0);
	PrintString(phd_centerx >> 2, textY + 4 * font_height, selection & 4 ? 1 : 2, "Crawl Tilting", 0);

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

	if (selection > (ulong)(1 << (num - 1)))
		selection = 1 << (num - 1);

	if (dbinput & IN_DESELECT)
	{
		SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
		menu = 0;
		dbinput &= ~IN_DESELECT;
		selection = 0x20;
		return;
	}

	strcpy(buffer, tomb4.footprints ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 1), textY + 2 * font_height, selection & 1 ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb4.shadow_mode == 1 ? "original" : tomb4.shadow_mode == 2 ? "circle" : "PSX");
	PrintString(phd_centerx + (phd_centerx >> 1), textY + 3 * font_height, selection & 2 ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb4.crawltilt ? "on" : "off");
	PrintString(phd_centerx + (phd_centerx >> 1), textY + 4 * font_height, selection & 4 ? 1 : 6, buffer, 0);

	changed = 0;

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

			if (tomb4.shadow_mode > 3)
				tomb4.shadow_mode = 1;

			changed = 1;
		}

		if (dbinput & IN_LEFT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			tomb4.shadow_mode--;

			if (tomb4.shadow_mode < 1)
				tomb4.shadow_mode = 3;

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
	}

	if (changed)
		save_new_tomb4_settings();
}
#endif

void DoOptions()
{
	JOYINFOEX joy;
	char** keyboard_buttons;
	char* txt;
	static long menu;
	static ulong selection = 1;	//selection
	static ulong controls_selection;	//selection for when mapping keys
	static long music_volume_bar_shade = 0xFF3F3F3F;
	static long sfx_volume_bar_shade = 0xFF3F3F3F;
	static long sfx_bak;	//backup sfx volume
	static long sfx_quality_bak;	//backup sfx quality
	static long sfx_breath_db = -1;
	long textY, joystick, joystick_x, joystick_y, joy1, joy2, joy3;
	uchar num, num2;
	char quality_text[80];
	static char sfx_backup_flag;	//have we backed sfx stuff up?
	static bool waiting_for_key = 0;

	if (!(sfx_backup_flag & 1))
	{
		sfx_backup_flag |= 1;
		sfx_bak = SFXVolume;
	}

	if (!(sfx_backup_flag & 2))
	{
		sfx_backup_flag |= 2;
		sfx_quality_bak = SoundQuality;
	}

	textY = font_height - 4;

	if (menu)	//controls menu
	{
#ifdef GENERAL_FIXES	//excuse me let me just 
		if (menu == 200)
		{
			TroyeMenu(textY, menu, selection);
			return;
		}
#endif

		if (Gameflow->Language == 2)
			keyboard_buttons = GermanKeyboard;
		else
			keyboard_buttons = KeyboardButtons;

		small_font = 1;
		PrintString(phd_centerx >> 2, textY, selection & 1 ? 1 : 2, SCRIPT_TEXT(TXT_Control_Method), 0);
		PrintString(phd_centerx >> 2, textY + font_height, selection & 2 ? 1 : 2, "\x18", 0);
		PrintString(phd_centerx >> 2, textY + 2 * font_height, selection & 4 ? 1 : 2, "\x1A", 0);
		PrintString(phd_centerx >> 2, textY + 3 * font_height, selection & 8 ? 1 : 2, "\x19", 0);
		PrintString(phd_centerx >> 2, textY + 4 * font_height, selection & 0x10 ? 1 : 2, "\x1B", 0);
		PrintString(phd_centerx >> 2, textY + 5 * font_height, selection & 0x20 ? 1 : 2, SCRIPT_TEXT(TXT_Duck), 0);
		PrintString(phd_centerx >> 2, textY + 6 * font_height, selection & 0x40 ? 1 : 2, SCRIPT_TEXT(TXT_Dash), 0);
		PrintString(phd_centerx >> 2, textY + 7 * font_height, selection & 0x80 ? 1 : 2, SCRIPT_TEXT(TXT_Walk), 0);
		PrintString(phd_centerx >> 2, textY + 8 * font_height, selection & 0x100 ? 1 : 2, SCRIPT_TEXT(TXT_Jump), 0);
		PrintString(phd_centerx >> 2, textY + 9 * font_height, selection & 0x200 ? 1 : 2, SCRIPT_TEXT(TXT_Action), 0);
		PrintString(phd_centerx >> 2, textY + 10 * font_height, selection & 0x400 ? 1 : 2, SCRIPT_TEXT(TXT_Draw_Weapon), 0);
		PrintString(phd_centerx >> 2, textY + 11 * font_height, selection & 0x800 ? 1 : 2, SCRIPT_TEXT(TXT_Use_Flare), 0);
		PrintString(phd_centerx >> 2, textY + 12 * font_height, selection & 0x1000 ? 1 : 2, SCRIPT_TEXT(TXT_Look), 0);
		PrintString(phd_centerx >> 2, textY + 13 * font_height, selection & 0x2000 ? 1 : 2, SCRIPT_TEXT(TXT_Roll), 0);
		PrintString(phd_centerx >> 2, textY + 14 * font_height, selection & 0x4000 ? 1 : 2, SCRIPT_TEXT(TXT_Inventory), 0);
		PrintString(phd_centerx >> 2, textY + 15 * font_height, selection & 0x8000 ? 1 : 2, SCRIPT_TEXT(TXT_Step_Left), 0);
		PrintString(phd_centerx >> 2, textY + 16 * font_height, selection & 0x10000 ? 1 : 2, SCRIPT_TEXT(TXT_Step_Right), 0);

		if (!ControlMethod)
			PrintString(phd_centerx + (phd_centerx >> 2), textY, controls_selection & 1 ? 1 : 6, SCRIPT_TEXT(TXT_Keyboard), 0);
		else if (ControlMethod == 1)
			PrintString(phd_centerx + (phd_centerx >> 2), textY, controls_selection & 1 ? 1 : 6, SCRIPT_TEXT(TXT_Joystick), 0);
		else if (ControlMethod == 2)
			PrintString(phd_centerx + (phd_centerx >> 2), textY, controls_selection & 1 ? 1 : 6, SCRIPT_TEXT(TXT_Reset), 0);

		if (waiting_for_key && controls_selection & 2)
			txt = SCRIPT_TEXT(TXT_Waiting);
		else
			txt = keyboard_buttons[layout[1][0]];

		PrintString(phd_centerx + (phd_centerx >> 2), textY + font_height, controls_selection & 2 ? 1 : 6, txt, 0);

		if (waiting_for_key && controls_selection & 4)
			txt = SCRIPT_TEXT(TXT_Waiting);
		else
			txt = keyboard_buttons[layout[1][1]];

		PrintString(phd_centerx + (phd_centerx >> 2), textY + 2 * font_height, controls_selection & 4 ? 1 : 6, txt, 0);

		if (waiting_for_key && controls_selection & 8)
			txt = SCRIPT_TEXT(TXT_Waiting);
		else
			txt = keyboard_buttons[layout[1][2]];

		PrintString(phd_centerx + (phd_centerx >> 2), textY + 3 * font_height, controls_selection & 8 ? 1 : 6, txt, 0);

		if (waiting_for_key && controls_selection & 0x10)
			txt = SCRIPT_TEXT(TXT_Waiting);
		else
			txt = keyboard_buttons[layout[1][3]];

		PrintString(phd_centerx + (phd_centerx >> 2), textY + 4 * font_height, controls_selection & 0x10 ? 1 : 6, txt, 0);

		if (waiting_for_key && controls_selection & 0x20)
			txt = SCRIPT_TEXT(TXT_Waiting);
		else
			txt = keyboard_buttons[layout[1][4]];

		PrintString(phd_centerx + (phd_centerx >> 2), textY + 5 * font_height, controls_selection & 0x20 ? 1 : 6, txt, 0);

		if (waiting_for_key && controls_selection & 0x40)
			txt = SCRIPT_TEXT(TXT_Waiting);
		else
			txt = keyboard_buttons[layout[1][5]];

		PrintString(phd_centerx + (phd_centerx >> 2), textY + 6 * font_height, controls_selection & 0x40 ? 1 : 6, txt, 0);

		if (waiting_for_key && controls_selection & 0x80)
			txt = SCRIPT_TEXT(TXT_Waiting);
		else
			txt = keyboard_buttons[layout[1][6]];

		PrintString(phd_centerx + (phd_centerx >> 2), textY + 7 * font_height, controls_selection & 0x80 ? 1 : 6, txt, 0);

		if (waiting_for_key && controls_selection & 0x100)
			txt = SCRIPT_TEXT(TXT_Waiting);
		else
			txt = keyboard_buttons[layout[1][7]];

		PrintString(phd_centerx + (phd_centerx >> 2), textY + 8 * font_height, controls_selection & 0x100 ? 1 : 6, txt, 0);

		if (waiting_for_key && controls_selection & 0x200)
			txt = SCRIPT_TEXT(TXT_Waiting);
		else
			txt = keyboard_buttons[layout[1][8]];

		PrintString(phd_centerx + (phd_centerx >> 2), textY + 9 * font_height, controls_selection & 0x200 ? 1 : 6, txt, 0);

		if (waiting_for_key && controls_selection & 0x400)
			txt = SCRIPT_TEXT(TXT_Waiting);
		else
			txt = keyboard_buttons[layout[1][9]];

		PrintString(phd_centerx + (phd_centerx >> 2), textY + 10 * font_height, controls_selection & 0x400 ? 1 : 6, txt, 0);

		if (waiting_for_key && controls_selection & 0x800)
			txt = SCRIPT_TEXT(TXT_Waiting);
		else
			txt = keyboard_buttons[layout[1][10]];

		PrintString(phd_centerx + (phd_centerx >> 2), textY + 11 * font_height, controls_selection & 0x800 ? 1 : 6, txt, 0);

		if (waiting_for_key && controls_selection & 0x1000)
			txt = SCRIPT_TEXT(TXT_Waiting);
		else
			txt = keyboard_buttons[layout[1][11]];

		PrintString(phd_centerx + (phd_centerx >> 2), textY + 12 * font_height, controls_selection & 0x1000 ? 1 : 6, txt, 0);

		if (waiting_for_key && controls_selection & 0x2000)
			txt = SCRIPT_TEXT(TXT_Waiting);
		else
			txt = keyboard_buttons[layout[1][12]];

		PrintString(phd_centerx + (phd_centerx >> 2), textY + 13 * font_height, controls_selection & 0x2000 ? 1 : 6, txt, 0);

		if (waiting_for_key && controls_selection & 0x4000)
			txt = SCRIPT_TEXT(TXT_Waiting);
		else
			txt = keyboard_buttons[layout[1][13]];

		PrintString(phd_centerx + (phd_centerx >> 2), textY + 14 * font_height, controls_selection & 0x4000 ? 1 : 6, txt, 0);

		if (waiting_for_key && controls_selection & 0x8000)
			txt = SCRIPT_TEXT(TXT_Waiting);
		else
			txt = keyboard_buttons[layout[1][14]];

		PrintString(phd_centerx + (phd_centerx >> 2), textY + 15 * font_height, controls_selection & 0x8000 ? 1 : 6, txt, 0);

		if (waiting_for_key && controls_selection & 0x10000)
			txt = SCRIPT_TEXT(TXT_Waiting);
		else
			txt = keyboard_buttons[layout[1][15]];

		PrintString(phd_centerx + (phd_centerx >> 2), textY + 16 * font_height, controls_selection & 0x10000 ? 1 : 6, txt, 0);

		small_font = 0;

		if (ControlMethod < 2 && !waiting_for_key)
		{
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
		}

		if (waiting_for_key)
		{
			num2 = 0;

			if (keymap[DIK_ESCAPE])
			{
				SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
				controls_selection = 0;
				dbinput = 0;
				waiting_for_key = 0;
				return;
			}

			for (int i = 0; i < 255; i++)
			{
				if (keymap[i] && keyboard_buttons[i])
				{
					if (i != DIK_RETURN && i != DIK_LEFT && i != DIK_RIGHT && i != DIK_UP && i != DIK_DOWN)
					{
						waiting_for_key = 0;

						for (int j = controls_selection >> 2; j; num2++)
							j >>= 1;

						controls_selection = 0;
						layout[1][num2] = i;
					}
				}
			}

			if (ControlMethod == 1)
			{
				joystick = ReadJoystick(joystick_x, joystick_y);

				if (joystick)
				{
					joy1 = controls_selection >> 2;
					joy2 = 0;

					while (joy1)
					{
						joy1 >>= 1;
						joy2++;
					}

					controls_selection = 0;
					joy1 = joystick >> 1;
					joy3 = 0;

					while (joy1)
					{
						joy1 >>= 1;
						joy3++;
					}

					layout[1][joy2] = joy3 + 255;
					waiting_for_key = 0;
				}
			}

			CheckKeyConflicts();
			dbinput = 0;
		}

		if (dbinput & IN_SELECT && selection > 1 && ControlMethod < 2)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			controls_selection = selection;
			waiting_for_key = 1;
			memset(keymap, 0, sizeof(keymap));
		}

		if (dbinput & IN_SELECT && ControlMethod == 2)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			ControlMethod = 0;
			memcpy(layout[1], layout, 72);
		}

		if (selection & 1)
		{
			if (dbinput & IN_LEFT)
			{
				SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
				ControlMethod--;
			}

			if (dbinput & IN_RIGHT)
			{
				SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
				ControlMethod++;
			}

			if (ControlMethod > 2)
				ControlMethod = 2;

			if (ControlMethod < 0)
				ControlMethod = 0;

			if (ControlMethod == 1)
			{
				joy.dwSize = sizeof(JOYINFOEX);

				if (joyGetPosEx(0, &joy) == JOYERR_UNPLUGGED)
				{
					if (dbinput & IN_LEFT)
						ControlMethod = 0;

					if (dbinput & IN_RIGHT)
						ControlMethod = 2;
				}
			}
		}

		if (!selection)
			selection = 1;

		if (selection > (ulong)(1 << (17 - 1)))
			selection = 1 << (17 - 1);

		if (dbinput & IN_DESELECT)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);

			if (ControlMethod < 2)
				menu = 0;

			dbinput = 0;
			selection = 1;
		}
	}
	else	//'main' menu
	{
#ifdef GENERAL_FIXES	//1 more option
		num = 6;
#else
		num = 5;
#endif
		textY = 3 * font_height;
		PrintString(phd_centerx, textY, 6, SCRIPT_TEXT(TXT_Options), FF_CENTER);
		PrintString(phd_centerx, textY + font_height + (font_height >> 1), selection & 1 ? 1 : 2, SCRIPT_TEXT(TXT_Control_Configuration), FF_CENTER);
		PrintString(phd_centerx >> 2, textY + 3 * font_height, selection & 0x2 ? 1 : 2, SCRIPT_TEXT(TXT_Music_Volume), 0);
		PrintString(phd_centerx >> 2, textY + 4 * font_height, selection & 0x4 ? 1 : 2, SCRIPT_TEXT(TXT_SFX_Volume), 0);
		PrintString(phd_centerx >> 2, textY + 5 * font_height, selection & 0x8 ? 1 : 2, SCRIPT_TEXT(TXT_Sound_Quality), 0);
		PrintString(phd_centerx >> 2, textY + 6 * font_height, selection & 0x10 ? 1 : 2, SCRIPT_TEXT(TXT_Targeting), 0);
		DoSlider(400, 3 * font_height - (font_height >> 1) + textY + 4, 200, 16, MusicVolume, 0xFF1F1F1F, 0xFF3F3FFF, music_volume_bar_shade);
		DoSlider(400, textY + 4 * font_height + 4 - (font_height >> 1), 200, 16, SFXVolume, 0xFF1F1F1F, 0xFF3F3FFF, sfx_volume_bar_shade);

		switch (SoundQuality)
		{
		case 0:
			strcpy(quality_text, SCRIPT_TEXT(TXT_Low));
			break;

		case 1:
			strcpy(quality_text, SCRIPT_TEXT(TXT_Medium));
			break;

		case 2:
			strcpy(quality_text, SCRIPT_TEXT(TXT_High));
			break;
		}

		PrintString(phd_centerx + (phd_centerx >> 2), textY + 5 * font_height, selection & 8 ? 1 : 6, quality_text, 0);
		
		if (App.AutoTarget)
			strcpy(quality_text, SCRIPT_TEXT(TXT_Automatic));
		else
			strcpy(quality_text, SCRIPT_TEXT(TXT_Manual));

		PrintString(phd_centerx + (phd_centerx >> 2), textY + 6 * font_height, selection & 0x10 ? 1 : 6, quality_text, 0);

#ifdef GENERAL_FIXES	//yay!
		PrintString(phd_centerx, (font_height >> 1) + textY + 7 * font_height, selection & 0x20 ? 1 : 2, "tomb4 options", FF_CENTER);
#endif
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

		if (dbinput & IN_SELECT && selection & 1)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			menu = 1;
		}

#ifdef GENERAL_FIXES
		if (dbinput & IN_SELECT && selection & 0x20)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			selection = 1;
			menu = 200;
		}
#endif

		if (!selection)
			selection = 1;

		if (selection > (ulong)(1 << (num - 1)))
			selection = 1 << (num - 1);

		music_volume_bar_shade = 0xFF3F3F3F;
		sfx_volume_bar_shade = 0xFF3F3F3F;

		if (selection & 2)
		{
			sfx_bak = SFXVolume;

			if (linput & IN_LEFT)
				MusicVolume--;

			if (linput & IN_RIGHT)
				MusicVolume++;

			if (MusicVolume > 100)
				MusicVolume = 100;
			else if (MusicVolume < 0)
				MusicVolume = 0;

			sfx_volume_bar_shade = 0xFF3F3F3F;
			music_volume_bar_shade = 0xFF7F7F7F;
			ACMSetVolume();
		}
		else if (selection & 4)
		{
			if (linput & IN_LEFT)
				SFXVolume--;

			if (linput & IN_RIGHT)
				SFXVolume++;

			if (SFXVolume > 100)
				SFXVolume = 100;
			else if (SFXVolume < 0)
				SFXVolume = 0;

			if (SFXVolume != sfx_bak)
			{
				if (sfx_breath_db == -1 || !DSIsChannelPlaying(0))
				{
					S_SoundStopAllSamples();
					sfx_bak = SFXVolume;
					sfx_breath_db = SoundEffect(SFX_LARA_BREATH, 0, SFX_DEFAULT);
					DSChangeVolume(0, -100 * ((100 - SFXVolume) >> 1));
				}
				else if (sfx_breath_db != -1 && DSIsChannelPlaying(0))
					DSChangeVolume(0, -100 * ((100 - SFXVolume) >> 1));
			}

			music_volume_bar_shade = 0xFF3F3F3F;
			sfx_volume_bar_shade = 0xFF7F7F7F;
		}
		else if (selection & 8)
		{
			sfx_bak = SFXVolume;

			if (dbinput & IN_LEFT)
				SoundQuality--;

			if (dbinput & IN_RIGHT)
				SoundQuality++;

			if (SoundQuality > 2)
				SoundQuality = 2;
			else if (SoundQuality < 0)
				SoundQuality = 0;

			if (SoundQuality != sfx_quality_bak)
			{
				S_SoundStopAllSamples();
				DXChangeOutputFormat(sfx_frequencies[SoundQuality], 0);
				sfx_quality_bak = SoundQuality;
				SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			}
		}
		else if (selection & 16)
		{
			if (dbinput & IN_LEFT)
			{
				if (App.AutoTarget)
					App.AutoTarget = 0;

				SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			}

			if (dbinput & IN_RIGHT)
			{
				if (!App.AutoTarget)
					App.AutoTarget = 1;

				SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			}

			savegame.AutoTarget = App.AutoTarget;
		}
	}
}
#pragma warning(pop)

void inject_loadsave(bool replace)
{
	INJECT(0x0047D460, S_DrawHealthBar, replace);
	INJECT(0x0047D420, S_DrawAirBar, replace);
	INJECT(0x0047D3E0, S_DrawDashBar, replace);
	INJECT(0x0047D4B0, S_InitLoadBar, replace);
	INJECT(0x0047D4D0, S_LoadBar, replace);
	INJECT(0x0047CE10, DoBar, replace);
	INJECT(0x0047B170, DoOptions, replace);
}
