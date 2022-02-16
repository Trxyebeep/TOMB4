#include "../tomb4/pch.h"
#include "demigod.h"
#include "items.h"
#include "../specific/function_stubs.h"
#include "objects.h"
#include "effect2.h"

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

void inject_demigod(bool replace)
{
	INJECT(0x00404770, TriggerDemigodMissile, replace);
	INJECT(0x00404840, TriggerDemigodMissileFlame, replace);
}
