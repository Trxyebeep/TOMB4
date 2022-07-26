#include "../tomb4/pch.h"
#include "sentrygun.h"
#include "effect2.h"
#include "../specific/function_stubs.h"
#include "sphere.h"
#include "box.h"

void TriggerAutogunFlamethrower(ITEM_INFO* item)
{
	SPARKS* sptr;
	PHD_VECTOR pos;
	PHD_VECTOR vel;
	long v;

	for (int i = 0; i < 3; i++)
	{
		sptr = &spark[GetFreeSpark()];
		sptr->On = 1;
		sptr->sR = (GetRandomControl() & 0x1F) + 48;
		sptr->sG = 48;
		sptr->sB = 255;
		sptr->dR = (GetRandomControl() & 0x3F) + 192;
		sptr->dG = (GetRandomControl() & 0x3F) + 128;
		sptr->dB = 32;
		sptr->ColFadeSpeed = 12;
		sptr->FadeToBlack = 8;
		sptr->TransType = 2;
		sptr->Life = (GetRandomControl() & 0x1F) + 16;
		sptr->sLife = sptr->Life;

		pos.x = -140;
		pos.y = -30;
		pos.z = -4;
		GetJointAbsPosition(item, &pos, 7);
		sptr->x = (GetRandomControl() & 0x1F) + pos.x - 16;
		sptr->y = (GetRandomControl() & 0x1F) + pos.y - 16;
		sptr->z = (GetRandomControl() & 0x1F) + pos.z - 16;

		vel.x = -280;
		vel.y = -30;
		vel.z = -4;
		GetJointAbsPosition(item, &vel, 7);
		v = (GetRandomControl() & 0x3F) + 192;
		sptr->Xvel = short(v * (vel.x - pos.x)) / 10;
		sptr->Yvel = short(v * (vel.y - pos.y)) / 10;
		sptr->Zvel = short(v * (vel.z - pos.z) / 10);

		sptr->Friction = 85;
		sptr->MaxYvel = 0;
		sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
		sptr->Flags = 538;

		if (GlobalCounter & 1)
		{
			v = 255;
			sptr->Flags = 539;
		}

		sptr->Scalar = 3;
		v *= (GetRandomControl() & 7) + 60;
		sptr->dSize = uchar(v >> 8);
		sptr->Size = uchar(v >> 12);
		sptr->sSize = sptr->Size;
	}
}

void InitialiseAutogun(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->item_flags[0] = 0;
	item->item_flags[1] = 768;
	item->item_flags[2] = 0;
}

void inject_sentry(bool replace)
{
	INJECT(0x0043F5C0, TriggerAutogunFlamethrower, replace);
	INJECT(0x0043F7A0, InitialiseAutogun, replace);
}
