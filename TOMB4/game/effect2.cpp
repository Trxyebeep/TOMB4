#include "../tomb4/pch.h"
#include "effect2.h"
#include "control.h"
#include "objects.h"
#include "../specific/function_stubs.h"
#include "tomb4fx.h"

void ControlSmokeEmitter(short item_number)
{
	ITEM_INFO* item;
	SPARKS* sptr;
	PHD_3DPOS pos;
	long size, dx, dz, normal;

	item = &items[item_number];
	normal = 0;

	if (!TriggerActive(item))
		return;

	if (item->object_number == STEAM_EMITTER && room[item->room_number].flags & ROOM_UNDERWATER)
	{
		if (item->item_flags[0] || !(GetRandomControl() & 0x1F) || item->trigger_flags == 1)
		{
			if (!(GetRandomControl() & 3) || item->item_flags[1])
			{
				pos.x_pos = (GetRandomControl() & 0x3F) + item->pos.x_pos - 32;
				pos.y_pos = item->pos.y_pos - (GetRandomControl() & 0x1F) - 16;
				pos.z_pos = (GetRandomControl() & 0x3F) + item->pos.z_pos - 32;

				if (item->trigger_flags == 1)
					CreateBubble(&pos, item->room_number, 15, 15, 0, 0, 0, 0);
				else
					CreateBubble(&pos, item->room_number, 8, 7, 0, 0, 0, 0);

				if (item->item_flags[0])
				{
					item->item_flags[0]--;

					if (!item->item_flags[0])
						item->item_flags[1] = 0;
				}
			}
		}
		else if (!(GetRandomControl() & 0x1F))
			item->item_flags[0] = (GetRandomControl() & 3) + 4;

		return;
	}

	if (item->object_number == STEAM_EMITTER && item->trigger_flags & 8)
	{
		if (item->item_flags[0])
		{
			item->item_flags[0]--;

			if (!item->item_flags[0])
				item->item_flags[1] = (GetRandomControl() & 0x3F) + 30;

			normal = 1;

			if (item->item_flags[2])
				item->item_flags[2] -= 256;
		}
		else if (item->item_flags[2] < 4096)
			item->item_flags[2] += 256;

		if (item->item_flags[2])
		{
			dx = lara_item->pos.x_pos - item->pos.x_pos;
			dz = lara_item->pos.z_pos - item->pos.z_pos;

			if (dx < -16384 || dx > 16384 || dz < -16384 || dz > 16384)
				return;

			sptr = &spark[GetFreeSpark()];
			sptr->On = 1;
			sptr->sR = 96;
			sptr->sG = 96;
			sptr->sB = 96;
			sptr->dR = 48;
			sptr->dG = 48;
			sptr->dB = 48;
			sptr->FadeToBlack = 6;
			sptr->ColFadeSpeed = (GetRandomControl() & 3) + 6;
			sptr->TransType = 2;
			sptr->Life = (GetRandomControl() & 7) + 16;
			sptr->sLife = sptr->Life;
			sptr->x = (GetRandomControl() & 0x3F) + item->pos.x_pos - 32;
			sptr->y = (GetRandomControl() & 0x3F) + item->pos.y_pos - 32;
			sptr->z = (GetRandomControl() & 0x3F) + item->pos.z_pos - 32;
			size = item->item_flags[2];

			if (item->item_flags[2] == 4096)
				size = (GetRandomControl() & 0x7FF) + 2048;

			sptr->Xvel = (short)((size * phd_sin(item->pos.y_rot - 32768)) >> 14);
			sptr->Yvel = -16 - (GetRandomControl() & 0xF);
			sptr->Zvel = (short)((size * phd_cos(item->pos.y_rot - 32768)) >> 14);
			sptr->Friction = 4;
			sptr->Flags = 538;

			if (!(GlobalCounter & 3))
				sptr->Flags = 1562;

			sptr->RotAng = GetRandomControl() & 0xFFF;

			if (GetRandomControl() & 1)
				sptr->RotAdd = -8 - (GetRandomControl() & 7);
			else
				sptr->RotAdd = (GetRandomControl() & 7) + 8;

			sptr->Scalar = 2;
			sptr->Gravity = -8 - (GetRandomControl() & 0xF);
			sptr->MaxYvel = -8 - (GetRandomControl() & 7);
			size = (GetRandomControl() & 0x1F) + 128;
			sptr->dSize = (uchar)size;
			sptr->sSize = sptr->dSize >> 1;
			sptr->Size = sptr->sSize;

			if (item->item_flags[1])
				item->item_flags[1]--;
			else
				item->item_flags[0] = item->trigger_flags >> 4;
		}

		if (!normal)
			return;
	}

	if (!(wibble & 0xF) && (item->object_number != STEAM_EMITTER || !(wibble & 0x1F)))
	{
		dx = lara_item->pos.x_pos - item->pos.x_pos;
		dz = lara_item->pos.z_pos - item->pos.z_pos;

		if (dx < -16384 || dx > 16384 || dz < -16384 || dz > 16384)
			return;

		sptr = &spark[GetFreeSpark()];
		sptr->On = 1;
		sptr->sR = 0;
		sptr->sG = 0;
		sptr->sB = 0;
		sptr->dR = 64;
		sptr->dG = 64;
		sptr->dB = 64;

#ifdef GENERAL_FIXES
		if (item->object_number == SMOKE_EMITTER_BLACK)
		{
			sptr->dR = 96;
			sptr->dG = 96;
			sptr->dB = 96;
		}
#endif
		sptr->FadeToBlack = 16;
		sptr->ColFadeSpeed = (GetRandomControl() & 3) + 8;
		sptr->Life = (GetRandomControl() & 7) + 28;
		sptr->sLife = sptr->Life;

		if (item->object_number == SMOKE_EMITTER_BLACK)
			sptr->TransType = 3;
		else
			sptr->TransType = 2;

		sptr->x = (GetRandomControl() & 0x3F) + item->pos.x_pos - 32;
		sptr->y = (GetRandomControl() & 0x3F) + item->pos.y_pos - 32;
		sptr->z = (GetRandomControl() & 0x3F) + item->pos.z_pos - 32;
		sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
		sptr->Yvel = -16 - (GetRandomControl() & 0xF);
		sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
		sptr->Friction = 3;
		sptr->Flags = 538;

		if (room[item->room_number].flags & ROOM_OUTSIDE)
			sptr->Flags = 794;

		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -8 - (GetRandomControl() & 7);
		else
			sptr->RotAdd = (GetRandomControl() & 7) + 8;

		sptr->Scalar = 2;
		sptr->Gravity = -8 - (GetRandomControl() & 0xF);
		sptr->MaxYvel = -8 - (GetRandomControl() & 7);
		size = (GetRandomControl() & 0x1F) + 128;
		sptr->dSize = (uchar)size;
		sptr->sSize = (uchar)(size >> 2);
		sptr->Size = (uchar)(size >> 2);

		if (item->object_number == STEAM_EMITTER)
		{
			sptr->Gravity >>= 1;
			sptr->Yvel >>= 1;
			sptr->MaxYvel >>= 1;
			sptr->Life += 16;
			sptr->sLife += 16;
			sptr->dR = 32;
			sptr->dG = 32;
			sptr->dB = 32;
		}
	}
}

void TriggerExplosionSmokeEnd(long x, long y, long z, long uw)
{
	SPARKS* sptr;
	long dx, dz;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -16384 || dx > 16384 || dz < -16384 || dz > 16384)
		return;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;

	if (uw)
	{
		sptr->sR = 0;
		sptr->sG = 0;
		sptr->sB = 0;
		sptr->dR = 192;
		sptr->dG = 192;
		sptr->dB = 208;
	}
	else
	{
#ifdef GENERAL_FIXES
		sptr->sR = 196;
		sptr->sG = 196;
		sptr->sB = 196;
		sptr->dR = 96;
		sptr->dG = 96;
		sptr->dB = 96;
#else
		sptr->sR = 144;
		sptr->sG = 144;
		sptr->sB = 144;
		sptr->dR = 64;
		sptr->dG = 64;
		sptr->dB = 64;
#endif
	}

	sptr->ColFadeSpeed = 8;
	sptr->FadeToBlack = 64;
	sptr->Life = (GetRandomControl() & 0x1F) + 96;
	sptr->sLife = sptr->Life;

	if (uw)
		sptr->TransType = 2;
	else
		sptr->TransType = 3;

	sptr->x = (GetRandomControl() & 0x1F) + x - 16;
	sptr->y = (GetRandomControl() & 0x1F) + y - 16;
	sptr->z = (GetRandomControl() & 0x1F) + z - 16;
	sptr->Xvel = ((GetRandomControl() & 0xFFF) - 2048) >> 2;
	sptr->Yvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Zvel = ((GetRandomControl() & 0xFFF) - 2048) >> 2;

	if (uw)
	{
		sptr->Friction = 20;
		sptr->Yvel >>= 4;
		sptr->y += 32;
	}
	else
		sptr->Friction = 6;

	sptr->Flags = 538;
	sptr->RotAng = GetRandomControl() & 0xFFF;

	if (GetRandomControl() & 1)
		sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
	else
		sptr->RotAdd = (GetRandomControl() & 0xF) + 16;

	sptr->Scalar = 3;

	if (uw)
	{
		sptr->MaxYvel = 0;
		sptr->Gravity = 0;
	}
	else
	{
		sptr->Gravity = -3 - (GetRandomControl() & 3);
		sptr->MaxYvel = -4 - (GetRandomControl() & 3);
	}

	sptr->dSize = (GetRandomControl() & 0x1F) + 128;
	sptr->sSize = sptr->dSize >> 2;
	sptr->Size = sptr->sSize;
}

void TriggerExplosionSmoke(long x, long y, long z, long uw)
{
	SPARKS* sptr;
	long dx, dz;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -16384 || dx > 16384 || dz < -16384 || dz > 16384)
		return;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;

#ifdef GENERAL_FIXES
	sptr->sR = 196;
	sptr->sG = 196;
	sptr->sB = 196;
	sptr->dR = 128;
	sptr->dG = 128;
	sptr->dB = 128;
#else
	sptr->sR = 144;
	sptr->sG = 144;
	sptr->sB = 144;
	sptr->dR = 64;
	sptr->dG = 64;
	sptr->dB = 64;
#endif

	sptr->ColFadeSpeed = 2;
	sptr->FadeToBlack = 8;
	sptr->TransType = 3;
	sptr->Life = (GetRandomControl() & 3) + 10;
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0x1FF) + x - 256;
	sptr->y = (GetRandomControl() & 0x1FF) + y - 256;
	sptr->z = (GetRandomControl() & 0x1FF) + z - 256;
	sptr->Xvel = ((GetRandomControl() & 0xFFF) - 2048) >> 2;
	sptr->Yvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Zvel = ((GetRandomControl() & 0xFFF) - 2048) >> 2;

	if (uw)
		sptr->Friction = 2;
	else
		sptr->Friction = 6;

	sptr->Flags = 538;
	sptr->RotAng = GetRandomControl() & 0xFFF;
	sptr->Scalar = 1;
	sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	sptr->Gravity = -3 - (GetRandomControl() & 3);
	sptr->MaxYvel = -4 - (GetRandomControl() & 3);
	sptr->dSize = (GetRandomControl() & 0x1F) + 128;
	sptr->sSize = sptr->dSize >> 2;
	sptr->Size = sptr->sSize >> 2;
	GetRandomControl();	//cool
}

void inject_effect2(bool replace)
{
	INJECT(0x00436340, ControlSmokeEmitter, replace);
	INJECT(0x00434DC0, TriggerExplosionSmokeEnd, replace);
	INJECT(0x00434FA0, TriggerExplosionSmoke, replace);
}
