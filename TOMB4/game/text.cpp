#include "../tomb4/pch.h"
#include "text.h"
#include "../specific/polyinsert.h"

#ifdef GENERAL_FIXES
long stash_font_height;
long smol_font_height;
#endif
#include "../specific/specificfx.h"

char AccentTable[46][2] =
{
	{'{', ' '},
	{'u', '^'},
	{'e', '\\'},
	{'a', ']'},
	{'a', '^'},
	{'a', '['},
	{'a', '\\'},
	{'{', ' '},
	{'e', ']'},
	{'e', '^'},
	{'e', '['},
	{'|', '^'},
	{'|', ']'},
	{'|', '['},
	{'A', '^'},
	{'A', ']'},
	{'E', '\\'},
	{' ', ' '},
	{' ', ' '},
	{'o', ']'},
	{'o', '^'},
	{'o', '['},
	{'u', ']'},
	{'u', '['},
	{'y', '^'},
	{'O', '^'},
	{'U', '^'},
	{' ', ' '},
	{'O', '\\'},
	{' ', ' '},
	{' ', ' '},
	{' ', ' '},
	{'a', '\\'},
	{'|', '\\'},
	{'o', '\\'},
	{'u', '\\'},
	{'n', '_'},
	{'N', '_'},
	{' ', ' '},
	{' ', ' '},
	{'}', ' '},
	{' ', ' '},
	{' ', ' '},
	{' ', ' '},
	{' ', ' '},
	{'~', ' '}
};

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

long GetStringLength(char* string, short* top, short* bottom)
{
	CHARDEF* def;
	long s, accent, length;
	short lowest, highest, y;

	s = *string++;
	length = 0;
	accent = 0;
	lowest = -1024;
	highest = 1024;

	while (s)
	{
		if (s == '\n')
			break;

		if (s == ' ')
			length += long((float(phd_winxmax + 1) / 640.0F) * 8.0F);
		else if (s == '\t')
		{
			length += 40;

			if (top)
			{
				if (highest > -12)
					highest = -12;
			}

			if (bottom)
			{
				if (lowest < 2)
					lowest = 2;
			}
		}
		else if (s >= 20)
		{
			if (s < ' ')
				def = &CharDef[s + 74];
			else
			{
				if (s >= 128 && s <= 173)
				{
					accent = 1;
					s = AccentTable[s - 128][0];
				}

				def = &CharDef[s - '!'];
			}

			if (ScaleFlag)
				length += def->w - def->w / 4;
			else
				length += def->w;

			y = def->YOffset;

			if (top)
			{
				if (y < highest)
					highest = def->YOffset;
			}

			if (bottom)
			{
				if (def->h + y > lowest)
					lowest = def->h + y;
			}
		}

		s = *string++;
	}

	if (top)
	{
		if (accent)
			highest -= 4;

		*top = highest;
	}

	if (bottom)
		*bottom = lowest;

	return length;
}

void DrawChar(short x, short y, ushort col, CHARDEF* def)
{
	D3DTLVERTEX v[4];
	TEXTURESTRUCT tex;
	float u1, v1, u2, v2;
	long x1, y1, x2, y2, top, bottom;

	y1 = short(y + phd_winymin) + def->YOffset;
	y2 = short(y + phd_winymin) + def->h + def->YOffset;

	if (small_font)
	{
		y1 = long((float)y1 * 0.75F);
		y2 = long((float)y2 * 0.75F);
	}

	x1 = short(x + phd_winxmin);
	x2 = x1 + def->w;
	setXY4(v, x1, y1, x2, y1, x2, y2, x1, y2, (long)f_mznear, clipflags);

	top = *(long*)&FontShades[col][2 * def->TopShade];
	bottom = *(long*)&FontShades[col][2 * def->BottomShade];
	v[0].color = top;
	v[1].color = top;
	v[2].color = bottom;
	v[3].color = bottom;

	top = *(long*)&FontShades[col][(2 * def->TopShade) + 1];
	bottom = *(long*)&FontShades[col][(2 * def->BottomShade) + 1];
	v[0].specular = top;
	v[1].specular = top;
	v[2].specular = bottom;
	v[3].specular = bottom;

	u1 = def->u + (1.0F / 512.0F);
	v1 = def->v + (1.0F / 512.0F);
	u2 = 512.0F / float(phd_winxmax + 1) * (float)def->w * (1.0F / 256.0F) + def->u - (1.0F / 512.0F);
	v2 = 240.0F / float(phd_winymax + 1) * (float)def->h * (1.0F / 256.0F) + def->v - (1.0F / 512.0F);
	tex.u1 = u1;
	tex.v1 = v1;
	tex.u2 = u2;
	tex.v2 = v1;
	tex.u3 = u2;
	tex.v3 = v2;
	tex.u4 = u1;
	tex.v4 = v2;

	tex.drawtype = 1;
	tex.tpage = ushort(nTextures - 2);
	tex.flag = 0;
	nPolyType = 4;
	AddQuadClippedSorted(v, 0, 1, 2, 3, &tex, 0);
}

void inject_text(bool replace)
{
	INJECT(0x00463650, InitFont, replace);
	INJECT(0x00463930, UpdatePulseColour, replace);
	INJECT(0x004639E0, GetStringLength, replace);
	INJECT(0x00463DE0, DrawChar, replace);
}
