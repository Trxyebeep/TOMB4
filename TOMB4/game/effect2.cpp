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

			sptr->Xvel = (short)((size * phd_sin(item->pos.y_rot - 32768)) >> W2V_SHIFT);
			sptr->Yvel = -16 - (GetRandomControl() & 0xF);
			sptr->Zvel = (short)((size * phd_cos(item->pos.y_rot - 32768)) >> W2V_SHIFT);
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
	if (!uw)
	{
		sptr->sR = 196;
		sptr->sG = 196;
		sptr->sB = 196;
		sptr->dR = 128;
		sptr->dG = 128;
		sptr->dB = 128;
	}
	else
#endif
	{
		sptr->sR = 144;
		sptr->sG = 144;
		sptr->sB = 144;
		sptr->dR = 64;
		sptr->dG = 64;
		sptr->dB = 64;
	}

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

void TriggerFlareSparks(long x, long y, long z, long xvel, long yvel, long zvel, long smoke)
{
	SPARKS* sptr;
	SPARKS* smokeSpark;
	long dx, dz, rnd;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	rnd = GetRandomDraw();
	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->dR = -1;
	sptr->dG = (rnd & 0x7F) + 64;
	sptr->dB = -64 - sptr->dG;
	sptr->sR = -1;
	sptr->sG = -1;
	sptr->sB = -1;
	sptr->Life = 10;
	sptr->sLife = 10;
	sptr->ColFadeSpeed = 3;
	sptr->FadeToBlack = 5;
	sptr->x = (rnd & 7) + x - 3;
	sptr->y = ((rnd >> 3) & 7) + y - 3;
	sptr->z = ((rnd >> 6) & 7) + z - 3;
	sptr->Xvel = short(((rnd >> 2) & 0xFF) + xvel - 128);
	sptr->Yvel = short(((rnd >> 4) & 0xFF) + yvel - 128);
	sptr->Zvel = short(((rnd >> 6) & 0xFF) + zvel - 128);
	sptr->TransType = 2;
	sptr->Friction = 34;
	sptr->Scalar = 1;
	sptr->Flags = 2;
	sptr->sSize = ((rnd >> 9) & 3) + 4;
	sptr->Size = sptr->sSize;
	sptr->dSize = ((rnd >> 12) & 1) + 1;
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;

	if (smoke)
	{
		rnd = GetRandomDraw();
		smokeSpark = &spark[GetFreeSpark()];
		smokeSpark->On = 1;
		smokeSpark->sR = sptr->dR >> 1;
		smokeSpark->sG = sptr->dG >> 1;
		smokeSpark->sB = sptr->dB >> 1;
		smokeSpark->dR = 32;
		smokeSpark->dG = 32;
		smokeSpark->dB = 32;
		smokeSpark->FadeToBlack = 4;
		smokeSpark->TransType = 2;
		smokeSpark->ColFadeSpeed = (rnd & 3) + 8;
		smokeSpark->Life = ((rnd >> 3) & 7) + 13;
		smokeSpark->sLife = smokeSpark->Life;
		smokeSpark->Friction = 4;
		smokeSpark->x = x + (xvel >> 5);
		smokeSpark->y = y + (yvel >> 5);
		smokeSpark->z = z + (zvel >> 5);
		smokeSpark->Xvel = short((rnd & 0x3F) + xvel - 32);
		smokeSpark->Yvel = (short)yvel;
		smokeSpark->Zvel = short(((rnd >> 6) & 0x3F) + zvel - 32);

		if (rnd & 1)
		{
			smokeSpark->Flags = 538;
			smokeSpark->RotAng = short(rnd >> 3);

			if (rnd & 2)
				smokeSpark->RotAdd = -16 - (rnd & 0xF);
			else
				smokeSpark->RotAdd = (rnd & 0xF) + 16;
		}
		else
			smokeSpark->Flags = 522;

		smokeSpark->Gravity = -8 - ((rnd >> 3) & 3);
		smokeSpark->Scalar = 2;
		smokeSpark->MaxYvel = -4 - ((rnd >> 6) & 3);
		smokeSpark->dSize = ((rnd >> 8) & 0xF) + 24;
		smokeSpark->sSize = smokeSpark->dSize >> 3;
		smokeSpark->Size = smokeSpark->dSize >> 3;
	}
}

void TriggerDynamic(long x, long y, long z, long falloff, long r, long g, long b)
{
	DYNAMIC* dl;

	if (number_dynamics == MAX_DYNAMICS || !falloff)
		return;

	dl = &dynamics[number_dynamics];
	dl->on = 1;
	dl->x = x;
	dl->y = y;
	dl->z = z;
	dl->falloff = ushort(falloff << 8);

	if (falloff < 8)
	{
		dl->r = uchar((r * falloff) >> 3);
		dl->g = uchar((g * falloff) >> 3);
		dl->b = uchar((b * falloff) >> 3);
	}
	else
	{
		dl->r = (uchar)r;
		dl->g = (uchar)g;
		dl->b = (uchar)b;
	}

	dl->FalloffScale = 0x200000 / (falloff << 8);
	number_dynamics++;
}

void TriggerDynamic_MIRROR(long x, long y, long z, long falloff, long r, long g, long b)
{
	DYNAMIC* dl;

	for (int i = 0; i < 2; i++)
	{
		if (number_dynamics == MAX_DYNAMICS || !falloff)
			return;

		dl = &dynamics[number_dynamics];
		dl->on = 1;
		dl->x = x;
		dl->y = y;
		dl->z = z;
		dl->falloff = ushort(falloff << 8);

		if (falloff < 8)
		{
			dl->r = uchar((r * falloff) >> 3);
			dl->g = uchar((g * falloff) >> 3);
			dl->b = uchar((b * falloff) >> 3);
		}
		else
		{
			dl->r = (uchar)r;
			dl->g = (uchar)g;
			dl->b = (uchar)b;
		}

		dl->FalloffScale = 0x200000 / (falloff << 8);
		number_dynamics++;
		z = 2 * gfMirrorZPlane - z;
	}
}

void ClearDynamics()
{
	number_dynamics = 0;

	for (int i = 0; i < MAX_DYNAMICS; i++)
		dynamics[i].on = 0;
}

void inject_effect2(bool replace)
{
	INJECT(0x00436340, ControlSmokeEmitter, replace);
	INJECT(0x00434DC0, TriggerExplosionSmokeEnd, replace);
	INJECT(0x00434FA0, TriggerExplosionSmoke, replace);
	INJECT(0x00434770, TriggerFlareSparks, replace);
	INJECT(0x004361A0, TriggerDynamic, replace);
	INJECT(0x00436250, TriggerDynamic_MIRROR, replace);
	INJECT(0x00436320, ClearDynamics, replace);
}
