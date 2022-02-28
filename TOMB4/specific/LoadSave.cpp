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
#include "../game/savegame.h"
#include "gamemain.h"
#include "specificfx.h"
#include "time.h"
#ifdef GENERAL_FIXES
#include "../tomb4/tomb4.h"
#include "../game/control.h"
#endif

#ifdef IMPROVED_BARS
static GouraudBarColourSet healthBarColourSet =
{
	{ 64, 96, 128, 96, 64 },
	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 },
	{ 128, 192, 255, 192, 128 },
	{ 0, 0, 0, 0, 0 }
};

static GouraudBarColourSet poisonBarColourSet =
{
	{ 96, 176, 240, 176, 96 },
	{ 96, 176, 240, 176, 96 },
	{ 0, 0, 0, 0, 0 },
	{ 96, 176, 240, 176, 96 },
	{ 96, 176, 240, 176, 96 },
	{ 0, 0, 0, 0, 0 }
};

static GouraudBarColourSet airBarColourSet =
{
	{ 0, 0, 0, 0, 0 },
	{ 113, 146, 113, 93, 74 },
	{ 123, 154, 123, 107, 91 },
	{ 0, 0, 0, 0, 0 },
	{ 113, 146, 113, 93, 74 },
	{ 0, 0, 0, 0, 0 }
};

static GouraudBarColourSet dashBarColourSet =
{
	{ 144, 192, 240, 192, 144 },
	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 },
	{ 144, 192, 240, 192, 144 },
	{ 144, 192, 240, 192, 144 },
	{ 0, 0, 0, 0, 0 }
};

static GouraudBarColourSet loadBarColourSet =
{
	{ 48, 96, 127, 80, 32 },
	{ 0, 0, 0, 0, 0 },
	{ 48, 96, 127, 80, 32 },
	{ 0, 0, 0, 0, 0 },
	{ 48, 96, 127, 80, 32 },
	{ 48, 96, 127, 80, 32 }
};

static GouraudBarColourSet enemyBarColourSet =
{
	{ 128, 192, 255, 192, 128 },
	{ 64, 96, 128, 96, 64 },
	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 },
	{ 123, 154, 123, 107, 91 },
	{ 0, 0, 0, 0, 0 }
};

static void S_DrawGouraudBar(int x, int y, int width, int height, int value, GouraudBarColourSet* colour)
{
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	float fx, fx2, fy, fy2, fvalue;
	long r, g, b;

	clipflags[0] = 0;
	clipflags[1] = 0;
	clipflags[2] = 0;
	clipflags[3] = 0;
	nPolyType = 4;
	tex.drawtype = 0;
	tex.tpage = 0;
	fx = phd_winxmax * 0.0015625F;
	fy = phd_winymax * 0.0020833334F;
	fvalue = 0.0099999998F * value;
	fx2 = width * fvalue;
	fy2 = height * 0.1666666716F;
	v[0].specular = 0xFF000000;
	v[1].specular = 0xFF000000;
	v[2].specular = 0xFF000000;
	v[3].specular = 0xFF000000;
	v[0].sx = x * fx;
	v[1].sx = x * fx + fx2 * fx;
	v[2].sx = x * fx;
	v[3].sx = x * fx + fx2 * fx;
	v[0].sy = y * fy - fy2 * fy;
	v[1].sy = y * fy - fy2 * fy;
	v[2].sy = y * fy;
	v[3].sy = y * fy;
	v[0].sz = f_mznear;
	v[1].sz = f_mznear;
	v[2].sz = f_mznear;
	v[3].sz = f_mznear;
	v[0].rhw = f_mpersp / f_mznear * f_moneopersp;
	v[1].rhw = f_mpersp / f_mznear * f_moneopersp;
	v[2].rhw = f_mpersp / f_mznear * f_moneopersp;
	v[3].rhw = f_mpersp / f_mznear * f_moneopersp;

	v[0].sy += fy2 * fy;
	v[1].sy += fy2 * fy;
	v[2].sy += fy2 * fy;
	v[3].sy += fy2 * fy;

	v[0].color = 0xFF000000;
	v[1].color = 0xFF000000;

	r = colour->abLeftRed[0];
	g = colour->abLeftGreen[0];
	b = colour->abLeftBlue[0];
	r -= r >> 2;
	g -= g >> 2;
	b -= b >> 2;
	v[2].color = RGBONLY(r, g, b);

	r = (long)((1 - fvalue) * colour->abLeftRed[0] + fvalue * colour->abRightRed[0]);
	g = (long)((1 - fvalue) * colour->abLeftGreen[0] + fvalue * colour->abRightGreen[0]);
	b = (long)((1 - fvalue) * colour->abLeftBlue[0] + fvalue * colour->abRightBlue[0]);
	r -= r >> 2;
	g -= g >> 2;
	b -= b >> 2;
	v[3].color = RGBONLY(r, g, b);

	AddQuadSorted(v, 0, 1, 3, 2, &tex, 1);

	for (int i = 0; i < 4; i++)
	{
		v[0].sy += fy2 * fy;
		v[1].sy += fy2 * fy;
		v[2].sy += fy2 * fy;
		v[3].sy += fy2 * fy;
		v[0].color = RGBONLY(colour->abLeftRed[i], colour->abLeftGreen[i], colour->abLeftBlue[i]);
		r = (long)((1 - fvalue) * colour->abLeftRed[i] + fvalue * colour->abRightRed[i]);
		g = (long)((1 - fvalue) * colour->abLeftGreen[i] + fvalue * colour->abRightGreen[i]);
		b = (long)((1 - fvalue) * colour->abLeftBlue[i] + fvalue * colour->abRightBlue[i]);
		v[1].color = RGBONLY(r, g, b);
		v[2].color = RGBONLY(colour->abLeftRed[i + 1], colour->abLeftGreen[i + 1], colour->abLeftBlue[i + 1]);
		r = (long)((1 - fvalue) * colour->abLeftRed[i + 1] + fvalue * colour->abRightRed[i + 1]);
		g = (long)((1 - fvalue) * colour->abLeftGreen[i + 1] + fvalue * colour->abRightGreen[i + 1]);
		b = (long)((1 - fvalue) * colour->abLeftBlue[i + 1] + fvalue * colour->abRightBlue[i + 1]);
		v[3].color = RGBONLY(r, g, b);
		AddQuadSorted(v, 0, 1, 3, 2, &tex, 1);
	}

	v[0].sy += fy2 * fy;
	v[1].sy += fy2 * fy;
	v[2].sy += fy2 * fy;
	v[3].sy += fy2 * fy;

	r = colour->abLeftRed[4];
	g = colour->abLeftGreen[4];
	b = colour->abLeftBlue[4];
	r -= r >> 2;
	g -= g >> 2;
	b -= b >> 2;
	v[0].color = RGBONLY(r, g, b);

	r = (long)((1 - fvalue) * colour->abLeftRed[4] + fvalue * colour->abRightRed[4]);
	g = (long)((1 - fvalue) * colour->abLeftGreen[4] + fvalue * colour->abRightGreen[4]);
	b = (long)((1 - fvalue) * colour->abLeftBlue[4] + fvalue * colour->abRightBlue[4]);
	r -= r >> 2;
	g -= g >> 2;
	b -= b >> 2;
	v[1].color = RGBONLY(r, g, b);

	v[2].color = 0xFF000000;
	v[3].color = 0xFF000000;
	AddQuadSorted(v, 0, 1, 3, 2, &tex, 1);

	v[0].sx = x * fx - 2;
	v[1].sx = x * fx + width * fx + 2;
	v[2].sx = x * fx - 2;
	v[3].sx = x * fx + width * fx + 2;
	v[0].sy = y * fy;
	v[1].sy = y * fy;
	v[2].sy = y * fy + height * fy;
	v[3].sy = y * fy + height * fy;
	v[0].sz = f_mznear + 1;
	v[1].sz = f_mznear + 1;
	v[2].sz = f_mznear + 1;
	v[3].sz = f_mznear + 1;
	v[0].rhw = f_mpersp / (f_mznear + 1) * f_moneopersp;
	v[1].rhw = f_mpersp / (f_mznear + 1) * f_moneopersp;
	v[2].rhw = f_mpersp / (f_mznear + 1) * f_moneopersp;
	v[3].rhw = f_mpersp / (f_mznear + 1) * f_moneopersp;
	v[0].color = 0;
	v[1].color = 0;
	v[2].color = 0;
	v[3].color = 0;
	AddQuadSorted(v, 0, 1, 3, 2, &tex, 1);	//black background

	v[0].sx = x * fx - 3;
	v[1].sx = x * fx + width * fx + 3;
	v[2].sx = x * fx - 3;
	v[3].sx = x * fx + width * fx + 3;
	v[0].sy = y * fy - 1;
	v[1].sy = y * fy - 1;
	v[2].sy = y * fy + height * fy + 1;
	v[3].sy = y * fy + height * fy + 1;
	v[0].sz = f_mznear + 2;
	v[1].sz = f_mznear + 2;
	v[2].sz = f_mznear + 2;
	v[3].sz = f_mznear + 2;
	v[0].rhw = f_mpersp / (f_mznear + 2) * f_moneopersp;
	v[1].rhw = f_mpersp / (f_mznear + 2) * f_moneopersp;
	v[2].rhw = f_mpersp / (f_mznear + 2) * f_moneopersp;
	v[3].rhw = f_mpersp / (f_mznear + 2) * f_moneopersp;
	v[0].color = 0xFFFFFFFF;
	v[1].color = 0xFFFFFFFF;
	v[2].color = 0xFFFFFFFF;
	v[3].color = 0xFFFFFFFF;
	AddQuadSorted(v, 0, 1, 3, 2, &tex, 1);	//white border
}

static void S_DoTR5Bar(long x, long y, long width, long height, long pos, long clr1, long clr2)
{
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	float fx, fx2, fy, fw, fh, r1, g1, b1, r2, g2, b2, r, g, b, mul;
	long lr, lg, lb, clr_11, clr_12, clr_21, clr_22;

	clipflags[0] = 0;
	clipflags[1] = 0;
	clipflags[2] = 0;
	clipflags[3] = 0;
	nPolyType = 4;
	tex.drawtype = 0;
	tex.tpage = 0;
	fx = (float)phd_winxmax * 0.0015625F;
	fy = (float)phd_winymax * 0.0020833334F;
	fw = (float)width;
	fh = (float)(height >> 1);
	fx2 = (fw * fx) * 0.0099999998F * (float)pos;
	v[0].specular = 0xFF000000;
	v[1].specular = 0xFF000000;
	v[2].specular = 0xFF000000;
	v[3].specular = 0xFF000000;
	v[0].sx = (float)x * fx;
	v[1].sx = ((float)x * fx) + fx2;
	v[2].sx = (float)x * fx;
	v[3].sx = ((float)x * fx) + fx2;
	v[0].sy = (float)y * fy;
	v[1].sy = (float)y * fy;
	v[2].sy = ((float)y * fy) + (fh * fy);
	v[3].sy = ((float)y * fy) + (fh * fy);
	v[0].sz = f_mznear;
	v[1].sz = f_mznear;
	v[2].sz = f_mznear;
	v[3].sz = f_mznear;
	v[0].rhw = f_mpersp / f_mznear * f_moneopersp;
	v[1].rhw = f_mpersp / f_mznear * f_moneopersp;
	v[2].rhw = f_mpersp / f_mznear * f_moneopersp;
	v[3].rhw = f_mpersp / f_mznear * f_moneopersp;

	r1 = (float)CLRR(clr1);		//get rgbs
	g1 = (float)CLRG(clr1);
	b1 = (float)CLRB(clr1);
	r2 = (float)CLRR(clr2);
	g2 = (float)CLRG(clr2);
	b2 = (float)CLRB(clr2);

	mul = fx2 / (fw * fx);		//mix
	r = r1 + ((r2 - r1) * mul);
	g = g1 + ((g2 - g1) * mul);
	b = b1 + ((b2 - b1) * mul);

	lr = (long)r1;
	lg = (long)g1;
	lb = (long)b1;
	clr_11 = RGBONLY(lr >> 1, lg >> 1, lb >> 1);	//clr1 is taken as is
	clr_12 = RGBONLY(lr, lg, lb);

	lr = (long)r;
	lg = (long)g;
	lb = (long)b;
	clr_21 = RGBONLY(lr >> 1, lg >> 1, lb >> 1);	//clr2 is the mix
	clr_22 = RGBONLY(lr, lg, lb);

	v[0].color = clr_11;
	v[1].color = clr_21;
	v[2].color = clr_12;
	v[3].color = clr_22;
	AddQuadSorted(v, 0, 1, 3, 2, &tex, 1);	//top half

	v[0].color = clr_12;
	v[1].color = clr_22;
	v[2].color = clr_11;
	v[3].color = clr_21;
	v[0].sy = ((float)y * fy) + (fh * fy);
	v[1].sy = ((float)y * fy) + (fh * fy);
	v[2].sy = (fh * fy) + (fh * fy) + ((float)y * fy);
	v[3].sy = (fh * fy) + (fh * fy) + ((float)y * fy);
	AddQuadSorted(v, 0, 1, 3, 2, &tex, 1);		//bottom half

	v[0].sx = (float)x * fx;
	v[1].sx = (fw * fx) + ((float)x * fx);
	v[2].sx = (float)x * fx;
	v[3].sx = (fw * fx) + ((float)x * fx);
	v[0].sy = (float)y * fy;
	v[1].sy = (float)y * fy;
	v[2].sy = (fh * fy) + (fh * fy) + ((float)y * fy);
	v[3].sy = (fh * fy) + (fh * fy) + ((float)y * fy);
	v[0].sz = f_mznear + 1;
	v[1].sz = f_mznear + 1;
	v[2].sz = f_mznear + 1;
	v[3].sz = f_mznear + 1;
	v[0].rhw = f_mpersp / (f_mznear + 1) * f_moneopersp;
	v[1].rhw = f_mpersp / (f_mznear + 1) * f_moneopersp;
	v[2].rhw = f_mpersp / (f_mznear + 1) * f_moneopersp;
	v[3].rhw = f_mpersp / (f_mznear + 1) * f_moneopersp;
	v[0].color = 0;
	v[1].color = 0;
	v[2].color = 0;
	v[3].color = 0;
	AddQuadSorted(v, 0, 1, 3, 2, &tex, 1);	//black background

	v[0].sx = ((float)x * fx) - 1;
	v[1].sx = (fw * fx) + ((float)x * fx) + 1;
	v[2].sx = ((float)x * fx) - 1;
	v[3].sx = (fw * fx) + ((float)x * fx) + 1;
	v[0].sy = ((float)y * fy) - 1;
	v[1].sy = ((float)y * fy) - 1;
	v[2].sy = (fh * fy) + (fh * fy) + ((float)y * fy) + 1;
	v[3].sy = (fh * fy) + (fh * fy) + ((float)y * fy) + 1;
	v[0].sz = f_mznear + 2;
	v[1].sz = f_mznear + 2;
	v[2].sz = f_mznear + 2;
	v[3].sz = f_mznear + 2;
	v[0].rhw = f_mpersp / (f_mznear + 2) * f_moneopersp;
	v[1].rhw = f_mpersp / (f_mznear + 2) * f_moneopersp;
	v[2].rhw = f_mpersp / (f_mznear + 2) * f_moneopersp;
	v[3].rhw = f_mpersp / (f_mznear + 2) * f_moneopersp;
	v[0].color = 0xFFFFFFFF;
	v[1].color = 0xFFFFFFFF;
	v[2].color = 0xFFFFFFFF;
	v[3].color = 0xFFFFFFFF;
	AddQuadSorted(v, 0, 1, 3, 2, &tex, 1);	//white border
}

void S_DrawEnemyBar(long pos)
{
	long x, y;

	if (tomb4.bars_pos == 1)//original
	{
		x = font_height >> 2;
		y = (font_height >> 2) + (font_height >> 1);
	}
	else if (tomb4.bars_pos == 2)//improved
	{
		x = font_height >> 2;
		y = (font_height >> 2) + (font_height >> 1);
	}
	else//PSX
	{
		x = 470 - (font_height >> 2);

		if (tomb4.bar_mode == 2 || tomb4.bar_mode == 3)
			y = (font_height >> 1) + (font_height >> 2) + (5 * font_height / 9);
		else
			y = (font_height >> 2) + (font_height >> 1) + font_height;
	}

	if (tomb4.bar_mode == 3)
		S_DrawGouraudBar(x, y, 150, 12, pos, &enemyBarColourSet);
	else if (tomb4.bar_mode == 2)
		S_DoTR5Bar(x, y, 150, 12, pos, 0xA00000, 0xA0A000);
	else
		DoBar(x, y, 150, 12, pos, 0xFF000000, 0xFFFFA000);
}

static void S_DrawHealthBar2(long pos)
{
	if (!gfCurrentLevel)
		return;

	if (tomb4.bar_mode == 3)
		S_DrawGouraudBar(245, (font_height >> 1) + 32, 150, 12, pos, lara.poisoned ? &poisonBarColourSet : &healthBarColourSet);
	else if (tomb4.bar_mode == 2)
		S_DoTR5Bar(245, (font_height >> 1) + 32, 150, 12, pos, 0xA00000, lara.poisoned ? 0xA0A000 : 0xA000);
	else
		DoBar(245, font_height + 48, 150, 12, pos, 0xFF000000, lara.poisoned ? 0xFFFFFF00 : 0xFFFF0000);
}
#endif

void S_DrawHealthBar(long pos)
{
	if (!gfCurrentLevel)
		return;

#ifdef GENERAL_FIXES
	long x, y;

	if (BinocularRange)
		S_DrawHealthBar2(pos);

	if (tomb4.bars_pos == 1 || tomb4.bars_pos == 2)//original or improved
	{
		x = font_height >> 2;
		y = font_height >> 2;
	}
	else//PSX
	{
		x = 470 - (font_height >> 2);
		y = font_height >> 2;
	}

#ifdef IMPROVED_BARS
	if (tomb4.bar_mode == 2)
	{
		if (lara.poisoned)
			S_DoTR5Bar(x, y, 150, 12, pos, 0xA00000, 0xA0A000);
		else
			S_DoTR5Bar(x, y, 150, 12, pos, 0xA00000, 0x00A000);
	}
	else if (tomb4.bar_mode == 3)
		S_DrawGouraudBar(x, y, 150, 12, pos, lara.poisoned ? &poisonBarColourSet : &healthBarColourSet);
	else
#endif
	{
		if (lara.poisoned)
			DoBar(x, y, 150, 12, pos, 0xFF000000, 0xFFFFFF00);	//yellow
		else
			DoBar(x, y, 150, 12, pos, 0xFF000000, 0xFFFF0000);	//red
	}
#else

	if (lara.poisoned)
		DoBar(font_height >> 2, font_height >> 2, 150, 12, pos, 0xFF000000, 0xFFFFFF00);	//yellow
	else
		DoBar(font_height >> 2, font_height >> 2, 150, 12, pos, 0xFF000000, 0xFFFF0000);	//red
#endif
}

void S_DrawAirBar(long pos)
{
	if (!gfCurrentLevel)
		return;

#ifdef GENERAL_FIXES
	long x, y;

	if (tomb4.bars_pos == 1)//original
	{
		x = 490 - (font_height >> 2);
		y = (font_height >> 2) + (font_height >> 1);
	}
	else if (tomb4.bars_pos == 2)//improved
	{
		x = 490 - (font_height >> 2);
		y = font_height >> 2;
	}
	else//PSX
	{
		x = 470 - (font_height >> 2);

		if (tomb4.bar_mode == 1)
			y = (font_height >> 1) + (font_height >> 2);
		else
			y = (font_height >> 2) + (font_height / 3);
	}

#ifdef IMPROVED_BARS
	if (tomb4.bar_mode == 2)
		S_DoTR5Bar(x, y, 150, 12, pos, 0x0000A0, 0x0050A0);
	else if (tomb4.bar_mode == 3)
		S_DrawGouraudBar(x, y, 150, 12, pos, &airBarColourSet);
	else
#endif
		DoBar(x, y, 150, 12, pos, 0xFF000000, 0xFF0000FF);	//blue
#else
	DoBar(490 - (font_height >> 2), (font_height >> 2) + (font_height >> 1), 150, 12, pos, 0xFF000000, 0xFF0000FF);	//blue
#endif
}

void S_DrawDashBar(long pos)
{
	if (!gfCurrentLevel)
		return;

#ifdef GENERAL_FIXES
	long x, y;

	if (tomb4.bars_pos == 1)//original
	{
		x = 490 - (font_height >> 2);
		y = font_height >> 2;
	}
	else if (tomb4.bars_pos == 2)//improved
	{
		x = 490 - (font_height >> 2);
		y = (font_height >> 2) + (font_height >> 1);
	}
	else//PSX
	{
		x = 470 - (font_height >> 2);

		if (tomb4.bar_mode == 1)
			y = (font_height >> 2) + font_height;
		else
			y = (font_height >> 2) + (font_height / 3) + (font_height / 3);
	}

#ifdef IMPROVED_BARS
	if (tomb4.bar_mode == 2)
		S_DoTR5Bar(x, y, 150, 12, pos, 0xA0A000, 0x00A000);
	else if (tomb4.bar_mode == 3)
		S_DrawGouraudBar(x, y, 150, 12, pos, &dashBarColourSet);
	else
#endif
		DoBar(x, y, 150, 12, pos, 0xFF000000, 0xFF00FF00);	//green
#else
	DoBar(490 - (font_height >> 2), font_height >> 2, 150, 12, pos, 0xFF000000, 0xFF00FF00);
#endif
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
		DoBar(20, phd_winymax - font_height, 600, 15, (long)loadbar_pos, 0xFF000000, 0xFF9F1F80);	//hmmm other bar funcs don't draw anything
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

	num = 9;
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
	PrintString(phd_centerx + (phd_centerx >> 1), textY + 2 * font_height, selection & 0x1 ? 1 : 6, buffer, 0);

	strcpy(buffer, tomb4.shadow_mode == 1 ? "original" : tomb4.shadow_mode == 2 ? "circle" : "PSX");
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

long S_LoadSave(long load_or_save, long mono)
{
	long fade, ret;

	fade = 0;

	if (!mono)
		CreateMonoScreen();

	GetSaveLoadFiles();
	InventoryActive = 1;

	while (1)
	{
		S_InitialisePolyList();

		if (fade)
			dbinput = 0;
		else
			S_UpdateInput();

		SetDebounce = 1;
		S_DisplayMonoScreen();
		ret = DoLoadSave(load_or_save);
		UpdatePulseColour();
		S_OutputPolyList();
		S_DumpScreen();

		if (ret >= 0)
		{
			if (load_or_save & IN_SAVE)
			{
				sgSaveGame();
				S_SaveGame(ret);
				GetSaveLoadFiles();
				break;
			}

			fade = ret + 1;
			S_LoadGame(ret);

#ifdef GENERAL_FIXES
			if (!DeathMenuActive)
				SetFade(0, 255);
#endif

			ret = -1;
		}

		if (fade && DoFade == 2)
		{
			ret = fade - 1;
			break;
		}

		if (input & IN_OPTION)
		{
			ret = -1;
			break;
		}

		if (MainThread.ended)
			break;
	}

	TIME_Init();

	if (!mono)
		FreeMonoScreen();

	InventoryActive = 0;

	return ret;
}

void inject_loadsave(bool replace)
{
	INJECT(0x0047D460, S_DrawHealthBar, replace);
	INJECT(0x0047D420, S_DrawAirBar, replace);
	INJECT(0x0047D3E0, S_DrawDashBar, replace);
	INJECT(0x0047D4B0, S_InitLoadBar, replace);
	INJECT(0x0047D4D0, S_LoadBar, replace);
	INJECT(0x0047CE10, DoBar, replace);
	INJECT(0x0047B170, DoOptions, replace);
	INJECT(0x0047CD20, S_LoadSave, replace);
}
