#include "../tomb4/pch.h"
#include "text.h"
#include "../specific/polyinsert.h"

#ifdef GENERAL_FIXES
long stash_font_height;
long smol_font_height;
#endif

void InitFont()
{
	D3DTLVERTEX v;
	static CHARDEF copy[106];
	static long init = 1;
	ushort r, g, b;
	short h, w, yoff;
	uchar fr, fg, fb, tr, tg, tb;

	for (int i = 0; i < 10; i++)
	{
		fr = ShadeFromTo[i][0].r;
		fg = ShadeFromTo[i][0].g;
		fb = ShadeFromTo[i][0].b;
		tr = ShadeFromTo[i][1].r;
		tg = ShadeFromTo[i][1].g;
		tb = ShadeFromTo[i][1].b;

		for (int j = 0; j < 16; j++)
		{
			r = ((tr * j) >> 4) + ((fr * (16 - j)) >> 4);
			g = ((tg * j) >> 4) + ((fg * (16 - j)) >> 4);
			b = ((tb * j) >> 4) + ((fb * (16 - j)) >> 4);

			if (r > 255)
				r = 255;

			if (g > 255)
				g = 255;

			if (b > 255)
				b = 255;

			CalcColorSplit(RGBONLY(b, g, r), &v.color);

			r = CLRR(v.color);
			g = CLRG(v.color);
			b = CLRB(v.color);
			FontShades[i][j << 1].r = (uchar)r;
			FontShades[i][j << 1].g = (uchar)g;
			FontShades[i][j << 1].b = (uchar)b;
			FontShades[i][j << 1].a = (uchar)0xFF;

			r = CLRR(v.specular);
			g = CLRG(v.specular);
			b = CLRB(v.specular);
			FontShades[i][(j << 1) + 1].r = (uchar)r;
			FontShades[i][(j << 1) + 1].g = (uchar)g;
			FontShades[i][(j << 1) + 1].b = (uchar)b;
			FontShades[i][(j << 1) + 1].a = (uchar)0xFF;
		}
	}

	if (init)
	{
		for (int i = 0; i < 106; i++)
		{
			copy[i].h = CharDef[i].h;
			copy[i].w = CharDef[i].w;
			copy[i].YOffset = CharDef[i].YOffset;
		}

		init = 0;
	}

	for (int i = 0; i < 106; i++)
	{
		h = short((float)copy[i].h * float(phd_winymax / 240.0F));
		w = short((float)copy[i].w * float(phd_winxmax / 512.0F));
		yoff = short((float)copy[i].YOffset * float(phd_winymax / 240.0F));
		CharDef[i].h = h;
		CharDef[i].w = w;
		CharDef[i].YOffset = yoff;
	}

	font_height = short(float(3.0F * phd_winymax / 40.0F));
#ifdef GENERAL_FIXES
	stash_font_height = font_height;
	smol_font_height = short(float(7.0F * phd_winymax / 120.0F));
#endif
}

void UpdatePulseColour()
{
	D3DTLVERTEX v;
	static uchar PulseCnt = 0;
	uchar c, r, g, b;

	PulseCnt = (PulseCnt + 1) & 0x1F;

	if (PulseCnt > 16)
		c = -PulseCnt;
	else
		c = PulseCnt;

	c <<= 3;
	CalcColorSplit(RGBONLY(c, c, c), &v.color);

	for (int i = 0; i < 16; i++)
	{
		r = CLRR(v.color);
		g = CLRG(v.color);
		b = CLRB(v.color);
		FontShades[1][i << 1].r = r;
		FontShades[1][i << 1].g = g;
		FontShades[1][i << 1].b = b;

		r = CLRR(v.specular);
		g = CLRG(v.specular);
		b = CLRB(v.specular);
		FontShades[1][(i << 1) + 1].r = r;
		FontShades[1][(i << 1) + 1].g = g;
		FontShades[1][(i << 1) + 1].b = b;
	}
}

void inject_text(bool replace)
{
	INJECT(0x00463650, InitFont, replace);
	INJECT(0x00463930, UpdatePulseColour, replace);
}
