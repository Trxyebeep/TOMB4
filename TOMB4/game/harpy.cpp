#include "../tomb4/pch.h"
#include "harpy.h"
#include "effect2.h"
#include "../specific/function_stubs.h"
#include "items.h"
#include "objects.h"

void TriggerHarpyMissileFlame(short fx_number, long xv, long yv, long zv)
{
	SPARKS* sptr;
	long dx, dz;

	dx = lara_item->pos.x_pos - effects[fx_number].pos.x_pos;
	dz = lara_item->pos.z_pos - effects[fx_number].pos.z_pos;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 0;
	sptr->sG = (GetRandomControl() & 0x7F) + 32;
	sptr->sB = sptr->dG + 64;	//uhm
	sptr->dR = (GetRandomControl() & 0x7F) + 32;
	sptr->dG = sptr->dR;
	sptr->dB = 0;
	sptr->FadeToBlack = 8;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 4;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 3) + 16;
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0xF) - 8;
	sptr->y = 0;
	sptr->z = (GetRandomControl() & 0xF) - 8;
	sptr->Xvel = (short)xv;
	sptr->Yvel = (short)yv;
	sptr->Zvel = (short)zv;
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

void TriggerHarpyMissile(PHD_3DPOS* pos, short room_number, short mesh)
{
	FX_INFO* fx;
	short fx_num;

	fx_num = CreateEffect(room_number);

	if (fx_num != NO_ITEM)
	{
		fx = &effects[fx_num];
		fx->pos.x_pos = pos->x_pos;
		fx->pos.y_pos = pos->y_pos - (GetRandomControl() & 0x3F) - 32;
		fx->pos.z_pos = pos->z_pos;
		fx->pos.x_rot = pos->x_rot;
		fx->pos.y_rot = pos->y_rot;
		fx->pos.z_rot = 0;
		fx->room_number = room_number;
		fx->counter = short(2 * GetRandomControl() + 0x8000);
		fx->object_number = BUBBLES;
		fx->speed = (GetRandomControl() & 0x1F) + 96;
		fx->flag1 = mesh;
		fx->frame_number = objects[BUBBLES].mesh_index + mesh * 2;
	}
}

void TriggerHarpySparks(long x, long y, long z, short xv, short yv, short zv)
{
	SPARKS* sptr;
	long dx, dz;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 0;
	sptr->sG = 0;
	sptr->sB = 0;
	sptr->dR = (GetRandomControl() & 0x7F) + 64;
	sptr->dG = sptr->dR;
	sptr->dB = 0;
	sptr->Life = 16;
	sptr->sLife = 16;
	sptr->ColFadeSpeed = 4;
	sptr->TransType = 2;
	sptr->FadeToBlack = 4;
	sptr->x = x;
	sptr->y = y;
	sptr->z = z;
	sptr->Xvel = xv;
	sptr->Yvel = yv;
	sptr->Zvel = zv;
	sptr->Friction = 34;
	sptr->Scalar = 1;
	sptr->Size = (GetRandomControl() & 3) + 4;
	sptr->sSize = sptr->Size;
	sptr->dSize = (GetRandomControl() & 1) + 1;
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;
	sptr->Flags = 0;
}

void TriggerHarpyFlame(short item_number, uchar NodeNumber, uchar size)
{
	SPARKS* sptr;
	long dx, dz;

	dx = lara_item->pos.x_pos - items[item_number].pos.x_pos;
	dz = lara_item->pos.z_pos - items[item_number].pos.z_pos;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 0;
	sptr->sG = 0;
	sptr->sB = 0;
	sptr->dR = (GetRandomControl() & 0x7F) + 32;
	sptr->dG = sptr->dR;
	sptr->dB = 0;
	sptr->FadeToBlack = 8;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 4;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 7) + 20;
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0xF) - 8;
	sptr->y = 0;
	sptr->z = (GetRandomControl() & 0xF) - 8;
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Yvel = 0;
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Friction = 5;
	sptr->Flags = 4762;
	sptr->RotAng = GetRandomControl() & 0xFFF;

	if (GetRandomControl() & 1)
		sptr->RotAdd = -32 - (GetRandomControl() & 0x1F);
	else
		sptr->RotAdd = (GetRandomControl() & 0x1F) + 32;

	sptr->MaxYvel = 0;
	sptr->Gravity = (GetRandomControl() & 0x1F) + 16;
	sptr->FxObj = (uchar)item_number;
	sptr->NodeNumber = NodeNumber;
	sptr->Scalar = 2;
	sptr->Size = (GetRandomControl() & 0xF) + size;
	sptr->sSize = sptr->Size;
	sptr->dSize = sptr->Size >> 4;
}

void inject_harpy(bool replace)
{
	INJECT(0x00407290, TriggerHarpyMissileFlame, replace);
	INJECT(0x00407700, TriggerHarpyMissile, replace);
	INJECT(0x004077B0, TriggerHarpySparks, replace);
	INJECT(0x004078A0, TriggerHarpyFlame, replace);
}
