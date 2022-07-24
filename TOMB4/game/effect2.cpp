#include "../tomb4/pch.h"
#include "effect2.h"
#include "control.h"
#include "objects.h"
#include "../specific/function_stubs.h"
#include "tomb4fx.h"
#include "../specific/3dmath.h"
#include "missile.h"
#include "items.h"
#include "effects.h"
#include "traps.h"
#include "seth.h"
#include "demigod.h"
#include "harpy.h"
#include "croc.h"

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

void ControlEnemyMissile(short fx_number)
{
	FX_INFO* fx;
	FLOOR_INFO* floor;
	long speed, ox, oy, oz, h, c;
	short room_number, max_speed, max_turn;
	short angles[2];

	fx = &effects[fx_number];
	phd_GetVectorAngles(lara_item->pos.x_pos - fx->pos.x_pos, lara_item->pos.y_pos - fx->pos.y_pos - 256, lara_item->pos.z_pos - fx->pos.z_pos, angles);

	if (fx->flag1 == 1)
	{
		max_turn = 512;
		max_speed = 256;
	}
	else if (fx->flag1 == 6)
	{
		if (fx->counter)
			fx->counter--;

		max_turn = 768;
		max_speed = 192;
	}
	else
	{
		max_turn = 768;
		max_speed = 192;
	}

	if (fx->speed < max_speed)
	{
		if (fx->flag1 == 6)
			fx->speed++;
		else
			fx->speed += 3;

		oy = (ushort)angles[0] - (ushort)fx->pos.y_rot;

		if (ABS(oy) > 0x8000)
			oy = (ushort)fx->pos.y_rot - (ushort)angles[0];

		ox = (ushort)angles[1] - (ushort)fx->pos.x_rot;

		if (ABS(ox) > 0x8000)
			ox = (ushort)fx->pos.x_rot - (ushort)angles[1];

		oy >>= 3;
		ox >>= 3;

		if (oy > max_turn)
			oy = max_turn;
		else if (oy < -max_turn)
			oy = -max_turn;

		if (ox > max_turn)
			ox = max_turn;
		else if (ox < -max_turn)
			ox = -max_turn;

		fx->pos.x_rot += (short)ox;

		if (fx->flag1 != 4 && (fx->flag1 != 6 || !fx->counter))
			fx->pos.y_rot += (short)oy;
	}

	fx->pos.z_rot += fx->speed << 4;

	if (fx->flag1 == 6)
		fx->pos.z_rot += fx->speed << 4;

	ox = fx->pos.x_pos;
	oy = fx->pos.y_pos;
	oz = fx->pos.z_pos;
	speed = fx->speed * phd_cos(fx->pos.x_rot) >> W2V_SHIFT;
	fx->pos.x_pos += speed * phd_sin(fx->pos.y_rot) >> W2V_SHIFT;
	fx->pos.y_pos += fx->speed * phd_sin(-fx->pos.x_rot) >> W2V_SHIFT;
	fx->pos.z_pos += speed * phd_cos(fx->pos.y_rot) >> W2V_SHIFT;
	room_number = fx->room_number;
	floor = GetFloor(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, &room_number);
	h = GetHeight(floor, fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);
	c = GetCeiling(floor, fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);

	if (fx->pos.y_pos >= h || fx->pos.y_pos <= c)
	{
		fx->pos.x_pos = ox;
		fx->pos.y_pos = oy;
		fx->pos.z_pos = oz;

		if (fx->flag1 != 6)
			ExplodeFX(fx, 0, -32);

		if (fx->flag1 == 1)
		{
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xA00020, 64, 0x18008040, (((~room[fx->room_number].flags & 0xFF) >> 4) & 2) << 16);	//decipher me
			TriggerExplosionSparks(ox, oy, oz, 3, -2, 2, fx->room_number);
		}
		else if (fx->flag1 == 0)
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xA00020, 64, 0x10008040, 0);
		else if (fx->flag1 == 3 || fx->flag1 == 4)
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xA00020, 64, 0x10004080, 0);
		else if (fx->flag1 == 5)
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xA00020, 64, 0x10806000, 0);
		else if (fx->flag1 == 2)
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xA00020, 64, 0x10808000, 0);
		else if (fx->flag1 == 6)
		{
			TriggerExplosionSparks(ox, oy, oz, 3, -2, 0, fx->room_number);
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xF00030, 64, 0x18806000, 0x20000);
			fx->pos.y_pos -= 128;
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xF00030, 48, 0x10807000, 0x20000);
			fx->pos.y_pos += 256;
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xF00030, 48, 0x10807000, 0x20000);
		}

		KillEffect(fx_number);
		return;
	}

	if (ItemNearLara(&fx->pos, 200))
	{
		lara_item->hit_status = 1;

		if (fx->flag1 != 6)
			ExplodeFX(fx, 0, -32);

		KillEffect(fx_number);

		if (fx->flag1 == 1)
		{
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xF00030, 64, 0x18008040, 0);
			TriggerExplosionSparks(ox, oy, oz, 3, -2, 2, fx->room_number);
			LaraBurn();
			lara.BurnGreen = 1;
		}
		else if (fx->flag1 == 0)
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0x580018, 48, 0x10008040, (((~room[fx->room_number].flags & 0xFF) >> 4) & 2) << 16);
		else if (fx->flag1 == 3 || fx->flag1 == 4)
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xA00020, 64, 0x10004080, 0x10000);
		else if (fx->flag1 == 5)
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xA00020, 64, 0x10806000, 0x20000);
		else if (fx->flag1 == 2)
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xA00020, 64, 0x10808000, 0x20000);
		else if (fx->flag1 == 6)
		{
			TriggerExplosionSparks(ox, oy, oz, 3, -2, 0, fx->room_number);
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xF00030, 64, 0x18806000, 0);
			fx->pos.y_pos -= 128;
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xF00030, 48, 0x10807000, 0);
			fx->pos.y_pos += 256;
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xF00030, 48, 0x10807000, 0);
			LaraBurn();
		}
	}
	else
	{
		if (room_number != fx->room_number)
			EffectNewRoom(fx_number, room_number);

		ox -= fx->pos.x_pos;
		oy -= fx->pos.y_pos;
		oz -= fx->pos.z_pos;

		if (wibble & 4 || fx->flag1 == 1 || fx->flag1 == 5 || fx->flag1 == 2)
		{
			if (fx->flag1 == 0)
				TriggerSethMissileFlame(fx_number, ox << 4, oy << 4, oz << 4);
			else if (fx->flag1 == 1)
				TriggerSethMissileFlame(fx_number, ox << 5, oy << 5, oz << 5);
			else if (fx->flag1 == 3 || fx->flag1 == 4 || fx->flag1 == 5)
				TriggerDemigodMissileFlame(fx_number, ox << 4, oy << 4, oz << 4);
			else if (fx->flag1 == 2)
				TriggerHarpyMissileFlame(fx_number, ox << 4, oy << 4, oz << 4);
			else if (fx->flag1 == 6)
				TriggerCrocgodMissileFlame(fx_number, ox << 4, oy << 4, oz << 4);
		}
	}
}

void SetupRipple(long x, long y, long z, long size, long flags)
{
	RIPPLE_STRUCT* ripple;
	long num;

	num = 0;

	while (ripples[num].flags & 1)
	{
		num++;

		if (num >= 16)
			return;
	}

	ripple = &ripples[num];
	ripple->flags = (char)flags | 1;
	ripple->size = (uchar)size;
	ripple->life = (GetRandomControl() & 0xF) + 48;
	ripple->init = 1;
	ripple->x = x;
	ripple->y = y;
	ripple->z = z;

	if (flags & 0x40)
	{
		ripple->x += (GetRandomControl() & 0x7F) - 64;
		ripple->z += (GetRandomControl() & 0x7F) - 64;
	}
}

void TriggerUnderwaterBlood(long x, long y, long z, long size)
{
	RIPPLE_STRUCT* ripple;
	long n;

	ripple = ripples;
	n = 0;

	while (ripple->flags & 1)
	{
		ripple++;
		n++;

		if (n >= 16)
			return;
	}

	ripple->flags = 49;
	ripple->init = 1;
	ripple->life = (GetRandomControl() & 7) - 16;
	ripple->size = (uchar)size;
	ripple->x = x + (GetRandomControl() & 0x3F) - 32;
	ripple->y = y;
	ripple->z = z + (GetRandomControl() & 0x3F) - 32;
}

void TriggerWaterfallMist(long x, long y, long z, long ang)
{
	SPARKS* sptr;
	long offsets[4];
	long ang2, ps, pc, rad;
	short vs, vc;

	offsets[0] = 576;
	offsets[1] = 203;
	offsets[2] = -203;
	offsets[3] = -576;
	ang2 = (ang + 1024) & 0xFFF;
	ps = rcossin_tbl[ang2 << 2];
	pc = rcossin_tbl[(ang2 << 2) + 1];
	vs = rcossin_tbl[ang << 2];
	vc = rcossin_tbl[(ang << 2) + 1];

	for (int i = 0; i < 4; i++)
	{
		sptr = &spark[GetFreeSpark()];
		sptr->On = 1;
		sptr->sR = 128;
		sptr->sG = 128;
		sptr->sB = 128;
		sptr->dR = 192;
		sptr->dG = 192;
		sptr->dB = 192;
		sptr->ColFadeSpeed = 2;
		sptr->FadeToBlack = 4;
		sptr->TransType = 2;
		sptr->Life = (GetRandomControl() & 3) + 6;
		sptr->sLife = sptr->Life;
		rad = (GetRandomControl() & 0x1F) + offsets[i] - 16;
		sptr->x = ((rad * ps) >> 12) + x + (GetRandomControl() & 0xF) - 8;
		sptr->y = (GetRandomControl() & 0xF) + y - 8;
		sptr->z = ((rad * pc) >> 12) + z + (GetRandomControl() & 0xF) - 8;
		sptr->Xvel = vs >> 12;
		sptr->Yvel = 0;
		sptr->Zvel = vc >> 12;
		sptr->Friction = 3;
		if (GetRandomControl() & 1)
		{
			sptr->Flags = 538;
			sptr->RotAng = GetRandomControl() & 0xFFF;

			if (GetRandomControl() & 1)
				sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
			else
				sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
		}
		else
			sptr->Flags = 522;

		sptr->Scalar = 6;
		sptr->Gravity = 0;
		sptr->MaxYvel = 0;
		sptr->dSize = (GetRandomControl() & 7) + 12;
		sptr->sSize = sptr->dSize >> 1;
		sptr->Size = sptr->dSize >> 1;
	}
}

void TriggerDartSmoke(long x, long y, long z, long xv, long zv, long hit)
{
	SPARKS* sptr;
	long dx, dz, rand;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 16;
	sptr->sG = 8;
	sptr->sB = 4;
	sptr->dR = 64;
	sptr->dG = 48;
	sptr->dB = 32;
	sptr->ColFadeSpeed = 8;
	sptr->FadeToBlack = 4;
	sptr->Life = (GetRandomControl() & 3) + 32;
	sptr->sLife = sptr->Life;
	sptr->TransType = 2;
	sptr->x = (GetRandomControl() & 0x1F) + x - 16;
	sptr->y = (GetRandomControl() & 0x1F) + y - 16;
	sptr->z = (GetRandomControl() & 0x1F) + z - 16;

	if (hit)
	{
		sptr->Xvel = short((GetRandomControl() & 0xFF) - xv - 128);
		sptr->Yvel = -4 - (GetRandomControl() & 3);
		sptr->Zvel = short((GetRandomControl() & 0xFF) - zv - 128);
	}
	else
	{
		if (xv)
			sptr->Xvel = (short)-xv;
		else
			sptr->Xvel = (GetRandomControl() & 0xFF) - 128;

		sptr->Yvel = -4 - (GetRandomControl() & 3);

		if (zv)
			sptr->Zvel = (short)-zv;
		else
			sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	}

	sptr->Friction = 3;

	if (GetRandomControl() & 1)
	{
		sptr->Flags = 538;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else
		sptr->Flags = 522;

	sptr->Scalar = 1;
	rand = (GetRandomControl() & 0x3F) + 72;

	if (hit)
	{
		sptr->MaxYvel = 0;
		sptr->Gravity = 0;
		sptr->Size = uchar(rand >> 3);
		sptr->sSize = sptr->Size;
		sptr->dSize = uchar(rand >> 1);
	}
	else
	{
		sptr->MaxYvel = -4 - (GetRandomControl() & 3);
		sptr->Gravity = -4 - (GetRandomControl() & 3);
		sptr->Size = uchar(rand >> 4);
		sptr->sSize = sptr->Size;
		sptr->dSize = (uchar)rand;
	}
}

void KillAllCurrentItems(short item_number)
{
	KillEverythingFlag = 1;
}

void KillEverything()
{
	KillEverythingFlag = 0;
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
	INJECT(0x004369B0, ControlEnemyMissile, replace);
	INJECT(0x00435B70, SetupRipple, replace);
	INJECT(0x00435BF0, TriggerUnderwaterBlood, replace);
	INJECT(0x00435C60, TriggerWaterfallMist, replace);
	INJECT(0x00435E30, TriggerDartSmoke, replace);
	INJECT(0x00436040, KillAllCurrentItems, replace);
	INJECT(0x00436050, KillEverything, replace);
}
