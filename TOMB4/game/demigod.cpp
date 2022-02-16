#include "../tomb4/pch.h"
#include "demigod.h"
#include "items.h"
#include "../specific/function_stubs.h"
#include "objects.h"
#include "effect2.h"
#include "tomb4fx.h"

void TriggerDemigodMissile(PHD_3DPOS* pos, short room_number, short type)
{
	FX_INFO* fx;
	short fx_number;

	fx_number = CreateEffect(room_number);

	if (fx_number != NO_ITEM)
	{
		fx = &effects[fx_number];
		fx->pos.x_pos = pos->x_pos;
		fx->pos.y_pos = pos->y_pos - (GetRandomControl() & 0x3F) - 32;
		fx->pos.z_pos = pos->z_pos;
		fx->pos.x_rot = pos->x_rot;

		if (type < 4)
			fx->pos.y_rot = pos->y_rot;
		else
			fx->pos.y_rot = pos->y_rot + (GetRandomControl() & 0x7FF) - 1024;

		fx->pos.z_rot = 0;

		fx->room_number = room_number;
		fx->counter = short(2 * GetRandomControl() + 0x8000);
		fx->flag1 = type;
		fx->speed = (GetRandomControl() & 0x1F) + 96;
		fx->object_number = BUBBLES;

		if (type >= 4)
			type--;

		fx->frame_number = objects[BUBBLES].mesh_index + type * 2;
	}
}

void TriggerDemigodMissileFlame(short fx_number, short xv, short yv, short zv)
{
	FX_INFO* fx;
	SPARKS* sptr;
	long dx, dz;

	fx = &effects[fx_number];
	dx = lara_item->pos.x_pos - fx->pos.x_pos;
	dz = lara_item->pos.z_pos - fx->pos.z_pos;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;

	if (fx->flag1 == 3 || fx->flag1 == 4)
	{
		sptr->sR = 0;
		sptr->sB = (GetRandomControl() & 0x7F) + 32;
		sptr->sG = sptr->sB + 64;
		sptr->dR = 0;
		sptr->dG = (GetRandomControl() & 0x7F) + 32;
		sptr->dB = sptr->dG + 64;
	}
	else
	{
		sptr->sR = (GetRandomControl() & 0x7F) + 32;
		sptr->sG = sptr->sR - (GetRandomControl() & 0x1F);
		sptr->sB = 0;
		sptr->dR = (GetRandomControl() & 0x7F) + 32;
		sptr->dG = sptr->dR - (GetRandomControl() & 0x1F);
		sptr->dB = 0;
	}

	sptr->FadeToBlack = 8;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 4;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 3) + 16;
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0xF) - 8;
	sptr->y = 0;
	sptr->z = (GetRandomControl() & 0xF) - 8;
	sptr->Xvel = xv;
	sptr->Yvel = yv;
	sptr->Zvel = zv;
	sptr->Friction = 68;
	sptr->Flags = 602;
	sptr->RotAng = GetRandomControl() & 0xFFF;

	if (GetRandomControl() & 1)
		sptr->RotAdd = -32 - (GetRandomControl() & 0x1F);
	else
		sptr->RotAdd = (GetRandomControl() & 0x1F) + 32;

	sptr->Gravity = 0;
	sptr->MaxYvel = 0;
	sptr->FxObj = (uchar)fx_number;
	sptr->Scalar = 2;
	sptr->Size = (GetRandomControl() & 7) + 64;
	sptr->sSize = sptr->Size;
	sptr->dSize = sptr->Size >> 5;
}

void TriggerHammerSmoke(long x, long y, long z, long num)
{
	SMOKE_SPARKS* sptr;
	long angle, step, off;

	angle = GetRandomControl() << 1;
	step = 0x10000 / num;

	for (; num > 0; num--)
	{
		sptr = &smoke_spark[GetFreeSmokeSpark()];
		sptr->On = 1;
		sptr->sShade = 0;
		sptr->ColFadeSpeed = 4;
		sptr->dShade = (GetRandomControl() & 0x1F) + 96;
		sptr->FadeToBlack = 24 - (GetRandomControl() & 7);
		sptr->TransType = 2;
		sptr->Life = (GetRandomControl() & 7) + 48;
		sptr->sLife = sptr->Life;
		sptr->x = (GetRandomControl() & 0x1F) + x - 16;
		sptr->y = (GetRandomControl() & 0x1F) + y - 16;
		sptr->z = (GetRandomControl() & 0x1F) + z - 16;
		off = (GetRandomControl() & 0xFF) + 255;
		sptr->Xvel = short((off * phd_sin(angle)) >> 14);
		sptr->Yvel = -32 - (GetRandomControl() & 0x3F);
		sptr->Zvel = short((off * phd_cos(angle)) >> 14);
		sptr->Friction = 9;

		if (GetRandomControl() & 1)
		{
			sptr->Flags = 16;
			sptr->RotAng = GetRandomControl() & 0xFFF;

			if (GetRandomControl() & 1)
				sptr->RotAdd = -64 - (GetRandomControl() & 0x3F);
			else
				sptr->RotAdd = (GetRandomControl() & 0x3F) + 64;
		}
		else if (room[lara_item->room_number].flags & ROOM_NOT_INSIDE)
			sptr->Flags = 256;
		else
			sptr->Flags = 0;

		sptr->Gravity = -4 - (GetRandomControl() & 3);
		sptr->MaxYvel = -4 - (GetRandomControl() & 3);
		sptr->dSize = (GetRandomControl() & 0x3F) + 64;
		sptr->sSize = sptr->dSize >> 3;
		sptr->Size = sptr->dSize >> 3;
		angle += step;
	}
}

void inject_demigod(bool replace)
{
	INJECT(0x00404770, TriggerDemigodMissile, replace);
	INJECT(0x00404840, TriggerDemigodMissileFlame, replace);
	INJECT(0x00404A00, TriggerHammerSmoke, replace);
}
