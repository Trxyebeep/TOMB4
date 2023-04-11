#include "../tomb4/pch.h"
#include "text.h"
#include "../specific/polyinsert.h"
#include "../specific/specificfx.h"
#include "../specific/texture.h"
#include "../specific/3dmath.h"
#include "../specific/function_stubs.h"
#include "../specific/gamemain.h"
#include "../specific/function_table.h"

long stash_font_height;
long smol_font_height;
long small_font;
long font_height;
long GnFrameCounter;

static CVECTOR FontShades[10][32];
static uchar ScaleFlag;

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

#pragma warning(push)
#pragma warning(disable : 4838)
#pragma warning(disable : 4309)
static CVECTOR ShadeFromTo[10][2] =
{
	{ {128, 128, 128, 0}, {128, 128, 128, 0} },
	{ {128, 128, 128, 0}, {128, 128, 128, 0} },
	{ {128, 128, 128, 0}, {128, 128, 128, 0} },
	{ {128, 0, 0, 0}, {64, 0, 0, 0} },
	{ {0, 0, 160, 0}, {0, 0, 80, 0} },
	{ {128, 128, 128, 0}, {16, 16, 16, 0} },
	{ {192, 128, 64, 0}, {64, 16, 0, 0} },
	{ {16, 16, 16, 0}, {128, 128, 128, 0} },
	{ {224, 192, 0, 0}, {64, 32, 0, 0} },
	{ {128, 0, 0, 0}, {64, 0, 0, 0} },
};
#pragma warning(pop)

static CHARDEF CharDef[106] =
{
	{0.68235302F, 0.203922F, 4, 13, -11, 0, 11},
	{0.384314F, 0.227451F, 7, 5, -10, 1, 5},
	{0.321569F, 0.101961F, 14, 12, -10, 1, 11},
	{0.30588201F, 0.14902F, 10, 14, -10, 1, 13},
	{0.83921599F, 0.050980002F, 15, 12, -9, 2, 12},
	{0.156863F, 0.101961F, 14, 12, -10, 1, 11},
	{0.61960799F, 0.223529F, 5, 5, -11, 0, 4},
	{0.80000001F, 0.152941F, 6, 16, -12, 0, 13},
	{0.133333F, 0.156863F, 6, 16, -11, 0, 14},
	{0.721569F, 0.231373F, 5, 5, -11, 0, 4},
	{0.086274996F, 0.156863F, 11, 11, -9, 2, 11},
	{0.698039F, 0.231373F, 5, 5, -2, 8, 12},
	{0.41568601F, 0.235294F, 8, 3, -4, 6, 9},
	{0.44705901F, 0.235294F, 5, 4, -2, 8, 11},
	{0.83137298F, 0.14902F, 9, 15, -12, 0, 12},
	{0.34509799F, 0.192157F, 10, 10, -8, 3, 11},
	{0.78431398F, 0.21568599F, 6, 10, -8, 3, 11},
	{0.180392F, 0.203922F, 9, 10, -8, 3, 11},
	{0.34509799F, 0.14902F, 8, 11, -8, 3, 12},
	{0.243137F, 0.156863F, 11, 11, -8, 3, 12},
	{0.55686301F, 0.188235F, 9, 12, -9, 2, 12},
	{0.90980399F, 0.196078F, 9, 11, -9, 2, 11},
	{0.470588F, 0.184314F, 9, 12, -9, 2, 12},
	{0.086274996F, 0.2F, 9, 11, -9, 2, 11},
	{0.431373F, 0.192157F, 9, 11, -8, 3, 12},
	{0.59607798F, 0.223529F, 5, 9, -7, 4, 11},
	{0.533333F, 0.223529F, 5, 10, -7, 4, 12},
	{0.698039F, 0.156863F, 12, 10, -8, 3, 11},
	{0.82352901F, 0.20784301F, 11, 7, -7, 4, 9},
	{0.941176F, 0.156863F, 12, 10, -8, 3, 11},
	{0.047059F, 0.152941F, 10, 13, -11, 0, 11},
	{0.25882399F, 0.050980002F, 16, 14, -10, 1, 13},
	{0.50980401F, 0.050980002F, 14, 13, -11, 0, 11},
	{0.83921599F, 0.098039001F, 13, 13, -11, 0, 11},
	{0.51764703F, 0.137255F, 11, 13, -11, 0, 11},
	{0, 0.101961F, 13, 13, -11, 0, 11},
	{0.054901998F, 0.101961F, 13, 13, -11, 0, 11},
	{0.25882399F, 0.105882F, 12, 13, -11, 0, 11},
	{0.71372497F, 0.105882F, 12, 13, -11, 0, 11},
	{0.78431398F, 0.050980002F, 14, 13, -11, 0, 11},
	{0.870588F, 0.21176501F, 5, 13, -11, 0, 11},
	{0.21960799F, 0.203922F, 5, 16, -11, 0, 14},
	{0.90196103F, 0.058823999F, 13, 13, -11, 0, 11},
	{0.56470603F, 0.137255F, 11, 13, -11, 0, 11},
	{0.188235F, 0.050980002F, 18, 13, -11, 0, 11},
	{0.56470603F, 0.050980002F, 14, 13, -11, 0, 11},
	{0.21176501F, 0.101961F, 12, 13, -11, 0, 11},
	{0.78431398F, 0.101961F, 12, 13, -11, 0, 11},
	{0.941176F, 0, 14, 15, -11, 0, 13},
	{0.61960799F, 0.050980002F, 14, 13, -11, 0, 11},
	{0.61176503F, 0.137255F, 11, 13, -11, 0, 11},
	{0.67451F, 0.050980002F, 14, 13, -11, 0, 11},
	{0.384314F, 0.050980002F, 15, 13, -11, 0, 11},
	{0.321569F, 0.050980002F, 15, 13, -11, 0, 11},
	{0.094117999F, 0.050980002F, 23, 13, -11, 0, 11},
	{0.72941202F, 0.050980002F, 13, 14, -11, 0, 12},
	{0.44705901F, 0.050980002F, 15, 13, -11, 0, 11},
	{0.89411801F, 0.109804F, 12, 13, -11, 0, 11},
	{0.243137F, 0.235294F, 6, 4, -4, 6, 9},
	{0.97254902F, 0.231373F, 6, 4, -4, 6, 9},
	{0.34509799F, 0.231373F, 8, 4, -4, 6, 9},
	{0.55686301F, 0.235294F, 7, 3, -3, 7, 9},
	{0.470588F, 0.231373F, 8, 4, -4, 6, 9},
	{0.94902003F, 0.231373F, 5, 5, -11, 0, 4},
	{0.384314F, 0.192157F, 11, 9, -7, 4, 11},
	{0.37647101F, 0.137255F, 11, 14, -12, 0, 11},
	{0.28235301F, 0.203922F, 9, 9, -7, 4, 11},
	{0, 0.152941F, 11, 12, -10, 1, 11},
	{0.64313698F, 0.203922F, 9, 9, -7, 4, 11},
	{0.65882403F, 0.14902F, 10, 14, -12, 0, 11},
	{0.470588F, 0.137255F, 12, 12, -7, 4, 14},
	{0.423529F, 0.137255F, 11, 14, -12, 0, 11},
	{0.76078397F, 0.105882F, 6, 12, -10, 1, 11},
	{0.156863F, 0.2F, 6, 16, -10, 1, 14},
	{0.109804F, 0.101961F, 12, 14, -12, 0, 11},
	{0.321569F, 0.203922F, 6, 13, -11, 0, 11},
	{0.37647101F, 0.101961F, 18, 9, -7, 4, 11},
	{0.59607798F, 0.188235F, 12, 9, -7, 4, 11},
	{0.243137F, 0.2F, 10, 9, -7, 4, 11},
	{0.95686299F, 0.058823999F, 11, 13, -7, 4, 14},
	{0.203922F, 0.152941F, 10, 13, -7, 4, 14},
	{0.039216001F, 0.203922F, 10, 9, -7, 4, 11},
	{0.74509799F, 0.203922F, 9, 9, -7, 4, 11},
	{0, 0.2F, 9, 11, -9, 2, 11},
	{0.698039F, 0.196078F, 11, 9, -7, 4, 11},
	{0.50980401F, 0.188235F, 12, 9, -7, 4, 11},
	{0.51764703F, 0.101961F, 18, 9, -7, 4, 11},
	{0.94902003F, 0.196078F, 11, 9, -7, 4, 11},
	{0.156863F, 0.14902F, 11, 13, -7, 4, 14},
	{0.90980399F, 0.16078401F, 8, 9, -7, 4, 11},
	{0.870588F, 0.16078401F, 9, 13, -7, 4, 14},
	{0.50980401F, 0.223529F, 6, 9, -7, 4, 11},
	{0.76078397F, 0.152941F, 10, 13, -10, 1, 12},
	{0.12548999F, 0.21960799F, 5, 12, -10, 1, 11},
	{0, 0.050980002F, 24, 13, -10, 6, 11},
	{0.75294101F, 0, 24, 13, -10, 6, 11},
	{0.65882403F, 0, 24, 13, -10, 6, 11},
	{0.84705901F, 0, 24, 13, -10, 6, 11},
	{0.58823502F, 0.101961F, 18, 9, -8, 6, 11},
	{0.65882403F, 0.101961F, 13, 12, -9, 6, 11},
	{0.44705901F, 0.101961F, 18, 9, -8, 6, 11},
	{0.941176F, 0.109804F, 13, 12, -9, 6, 11},
	{0, 0, 41, 13, -10, 6, 11},
	{0.32941201F, 0, 41, 13, -10, 6, 11},
	{0.16470601F, 0, 41, 13, -10, 6, 11},
	{0.49411801F, 0, 41, 13, -10, 6, 11}
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

	font_height = long(float(3.0F * phd_winymax / 40.0F));
	stash_font_height = font_height;
	smol_font_height = long(float(7.0F * phd_winymax / 120.0F));
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

long GetStringLength(const char* string, long* top, long* bottom)
{
	CHARDEF* def;
	long s, accent, length, lowest, highest, y;

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

void DrawChar(long x, long y, ushort col, CHARDEF* def)
{
	D3DTLVERTEX* v;
	TEXTURESTRUCT tex;
	float u1, v1, u2, v2;
	long x1, y1, x2, y2, top, bottom;

	v = MyVertexBuffer;

	y1 = y + phd_winymin + def->YOffset;
	y2 = y + phd_winymin + def->h + def->YOffset;

	if (small_font)
	{
		y1 = long((float)y1 * 0.75F);
		y2 = long((float)y2 * 0.75F);
	}

	x1 = x + phd_winxmin;
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

void PrintString(long x, long y, uchar col, const char* string, ushort flags)
{
	CHARDEF* def;
	CHARDEF* accent;
	long x2, bottom, l, top, bottom2;
	uchar s;

	if (flags & FF_BLINK && GnFrameCounter & 0x10)
		return;

	ScaleFlag = (flags & FF_SMALL) != 0;
	x2 = GetStringLength(string, 0, &bottom);

	if (flags & FF_CENTER)
		x2 = x - (x2 >> 1);
	else if (flags & FF_RJUSTIFY)
		x2 = x - x2;
	else
		x2 = x;

	s = *string++;

	while (s)
	{
		if (s == '\n')
		{
			if (*string == '\n')
			{
				bottom = 0;
				y += 16;
			}
			else
			{
				l = GetStringLength(string, &top, &bottom2);

				if (flags & FF_CENTER)
					x2 = x - (l >> 1);
				else if (flags & FF_RJUSTIFY)
					x2 = x - l;
				else
					x2 = x;

				y += bottom - top + 2;
				bottom = bottom2;
			}

			s = *string++;
			continue;
		}

		if (s == ' ')
		{
			if (ScaleFlag)
				x2 += 6;
			else
				x2 += long(float(phd_winxmax + 1) / 640.0F * 8.0F);

			s = *string++;
			continue;
		}

		if (s == '\t')
		{
			x2 += 40;
			s = *string++;
			continue;
		}

		if (s < 20)
		{
			col = s - 1;
			s = *string++;
			continue;
		}

		if (s >= 128 && s <= 173)
		{
			def = &CharDef[AccentTable[s - 128][0] - '!'];
			accent = &CharDef[AccentTable[s - 128][1] - '!'];
			DrawChar(x2, y, col, def);

			if (AccentTable[s - 128][1] != ' ')
				DrawChar(def->w / 2 + x2 - 3, y + def->YOffset, col, accent);
		}
		else
		{
			if (s < ' ')
				def = &CharDef[s + 74];
			else
				def = &CharDef[s - '!'];

			DrawChar(x2, y, col, def);
		}

		if (ScaleFlag)
			x2 += def->w - def->w / 4;
		else
			x2 += def->w;

		s = *string++;
	}

	ScaleFlag = 0;
}
