#include "../tomb4/pch.h"
#include "wraith.h"
#include "../specific/function_stubs.h"
#include "effect2.h"
#include "objects.h"

void InitialiseWraith(short item_number)
{
	ITEM_INFO* item;
	WRAITH_STRUCT* data;

	item = &items[item_number];
	data = (WRAITH_STRUCT*)game_malloc(8 * sizeof(WRAITH_STRUCT));
	item->data = data;
	item->item_flags[0] = 0;
	item->hit_points = 0;
	item->speed = WraithSpeed;

	for (int i = 0; i < 8; i++)
	{
		data->pos.x = item->pos.x_pos;
		data->pos.y = item->pos.y_pos;
		data->pos.z = item->pos.z_pos;
		data->zv = 0;
		data->yv = 0;
		data->xv = 0;
		data->b = 0;
		data->g = 0;
		data->r = 0;
		data++;
	}
}

void TriggerWraithFlame(long x, long y, long z, long xv, long yv, long zv, long objnum)
{
	SPARKS* sptr;
	uchar col;
	
	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;

	if (objnum == WRAITH1)
	{
		sptr->sR = (GetRandomControl() & 0x1F) + 128;
		sptr->sG = (GetRandomControl() & 0x1F) + 48;
		sptr->sB = 24;
		sptr->dR = (GetRandomControl() & 0x1F) + 128;
		sptr->dG = (GetRandomControl() & 0x1F) + 64;
		sptr->dB = 24;
	}
	else if (objnum == WRAITH2)
	{
		sptr->sR = 24;
		sptr->sG = (GetRandomControl() & 0x1F) + 48;
		sptr->sB = (GetRandomControl() & 0x1F) + 128;
		sptr->dR = 24;
		sptr->dG = (GetRandomControl() & 0x1F) + 64;
		sptr->dB = (GetRandomControl() & 0x1F) + 128;
	}
	else
	{
		col = (GetRandomControl() & 0xF) + 64;
		sptr->sR = col;
		sptr->sG = col;
		sptr->sB = col;
		sptr->dR = col;
		sptr->dG = col;
		sptr->dB = sptr->sB + (GetRandomControl() & 0xF);
	}

	sptr->ColFadeSpeed = 4;
	sptr->FadeToBlack = 7;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 7) + 12;
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0x1F) + x - 16;
	sptr->y = y;
	sptr->z = (GetRandomControl() & 0x1F) + z - 16;
	sptr->Friction = 85;
	sptr->Flags = 522;
	sptr->Xvel = xv;
	sptr->Yvel = yv;
	sptr->Zvel = zv;
	sptr->Gravity = 0;
	sptr->MaxYvel = 0;
	sptr->Scalar = 2;
	sptr->dSize = 2;
	sptr->Size = (GetRandomControl() & 0x1F) + 48;
	sptr->sSize = sptr->Size;
}

void TriggerWraithEffect(long x, long y, long z, short vel, long objnum)
{
	SPARKS* sptr;
	ulong col, scol, dcol;
	long rad, ang;
	uchar sr, sg, sb, dr, dg, db;

	if (objnum == WRAITH1)
	{
		sr = (GetRandomControl() & 0x1F) + 128;
		sg = (GetRandomControl() & 0x1F) + 48;
		sb = 24;
		dr = (GetRandomControl() & 0x1F) + 128;
		dg = (GetRandomControl() & 0x1F) + 48;
		db = 24;
	}
	else if (objnum == WRAITH2)
	{
		sr = 24;
		sg = (GetRandomControl() & 0x1F) + 48;
		sb = (GetRandomControl() & 0x1F) + 128;
		dr = 24;
		dg = (GetRandomControl() & 0x1F) + 48;
		db = (GetRandomControl() & 0x1F) + 128;
	}
	else
	{
		col = (GetRandomControl() & 0xF) + 64;
		sr = col;
		sg = col;
		sb = col;
		dr = col;
		dg = col;
		db = col;
	}

	scol = (sb << 24) | (sg << 16) | (sr << 8) | 1;			//sptr->On = 1; sptr->sR = sr; sptr->sG = sg; sptr->sB = sb;
	dcol = (0xFF << 24) | (db << 16) | (dg << 8) | dr;		//sptr->dR = dr; sptr->dG = dg; sptr->dB = sb; sptr->R = 0xFF;

	for (int i = 0; i < 15; i++)
	{
		sptr = &spark[GetFreeSpark()];
		*(ulong*)&sptr->On = scol;	//just.. why
		*(ulong*)&sptr->dR = dcol;
		sptr->ColFadeSpeed = 4;
		sptr->FadeToBlack = 7;
		sptr->TransType = 2;
		sptr->Life = (GetRandomControl() & 7) + 32;
		sptr->sLife = sptr->Life;
		sptr->x = (GetRandomControl() & 0x1F) + x - 16;
		sptr->y = (GetRandomControl() & 0x1F) + y - 16;
		sptr->z = (GetRandomControl() & 0x1F) + z - 16;
		rad = (GetRandomControl() & 0x3FF) + 1024;
		ang = vel + GetRandomControl() - 0x4000;
		sptr->Xvel = (rad * phd_sin(ang)) >> 14;
		sptr->Yvel = (GetRandomControl() & 0x7F) - 64;
		sptr->Zvel = (rad * phd_cos(ang)) >> 14;
		sptr->Friction = 4;
		sptr->Flags = 522;
		sptr->MaxYvel = 0;
		sptr->Scalar = 3;
		sptr->Gravity = (GetRandomControl() & 0x7F) - 64;
		sptr->Size = (GetRandomControl() & 0x1F) + 48;
		sptr->sSize = sptr->Size;
		sptr->dSize = sptr->Size >> 2;
	}
}

void inject_wraith(bool replace)
{
	INJECT(0x0041C2E0, InitialiseWraith, replace);
	INJECT(0x0041C360, TriggerWraithFlame, replace);
	INJECT(0x0041C4C0, TriggerWraithEffect, replace);
}
