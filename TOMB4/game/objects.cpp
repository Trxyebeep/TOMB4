#include "../tomb4/pch.h"
#include "objects.h"
#include "control.h"
#include "sound.h"
#include "sphere.h"
#include "effect2.h"
#include "../specific/function_stubs.h"
#include "items.h"
#include "tomb4fx.h"
#include "traps.h"

void ControlMapper(short item_number)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	SPARKS* sptr;
	PHD_VECTOR pos;
	long rg, h;
	short room_number;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	if (item->frame_number - anims[item->anim_number].frame_base >= 200)
	{
		SoundEffect(SFX_MAPPER_LAZER, &item->pos, SFX_DEFAULT);
		item->mesh_bits |= 2;
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;
		GetJointAbsPosition(item, &pos, 1);
		rg = (GetRandomControl() & 0x1F) + 192;
		TriggerDynamic(pos.x, pos.y, pos.z, (GetRandomControl() & 3) + 16, rg, rg, 0);
		room_number = item->room_number;
		floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
		h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

		for (int i = 0; i < 2; i++)
		{
			sptr = &spark[GetFreeSpark()];
			sptr->On = 1;
			sptr->sR = (GetRandomControl() & 0x7F) + 64;
			sptr->sG = sptr->sR;
			sptr->sB = 0;
			sptr->dR = (GetRandomControl() & 0x7F) + 64;
			sptr->dG = sptr->dR - (GetRandomControl() & 0x1F);
			sptr->dB = 0;
			sptr->ColFadeSpeed = 4;
			sptr->FadeToBlack = 4;
			sptr->TransType = 2;
			sptr->Life = 12;
			sptr->sLife = 12;
			sptr->x = pos.x;
			sptr->y = h;
			sptr->z = pos.z;
			sptr->Xvel = (GetRandomControl() & 0x3FF) - 512;
			sptr->Yvel = -256 - (GetRandomControl() & 0x7F);
			sptr->Zvel = (GetRandomControl() & 0x3FF) - 512;
			sptr->Friction = 4;
			sptr->Scalar = 2;
			sptr->sSize = (GetRandomControl() & 0xF) + 16;;
			sptr->Size = sptr->sSize;
			sptr->dSize = (GetRandomControl() & 1) + 3;
			sptr->MaxYvel = 0;
			sptr->Gravity = (GetRandomControl() & 0x1F) + 32;
			sptr->Flags = 10;
		}
	}

	AnimateItem(item);
}

void ControlLightningConductor(short item_number)
{
	ITEM_INFO* item;
	PHD_VECTOR pos;
	PHD_VECTOR pos2;
	short r, g, b, room_number;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	if (item->trigger_flags == 2)
	{
		if (!flip_stats[1])
			return;

		if (item->item_flags[3] < 120)
		{
			item->item_flags[0] = 0;
			item->item_flags[3]++;
			return;
		}

		if (!item->item_flags[0])
		{
			item->item_flags[0] = (GetRandomControl() & 3) + 4;
			item->item_flags[1] = (GetRandomControl() & 0x3FF) - 512;
		}
	}
	else if (item->trigger_flags == 1 && flip_stats[1])
	{
		KillItem(item_number);
		return;
	}

	if (item->item_flags[0])
	{
		SoundEffect(SFX_ELEC_ARCING_LOOP, &item->pos, SFX_DEFAULT);
		item->item_flags[0]--;
		b = (GetRandomControl() & 0x3F) + 192;
		g = b - (GetRandomControl() & 0x1F);
		r = g - (GetRandomControl() & 0xFF);

		if (r < 0)
			r = 0;

		pos.x = (GetRandomControl() & 0x1FF) + item->pos.x_pos - 256;
		pos.y = item->pos.y_pos;
		pos.z = (GetRandomControl() & 0x1FF) + item->pos.z_pos - 256;
		TriggerLightning((PHD_VECTOR*)&item->pos, &pos, (GetRandomControl() & 0xF) + 16, RGBA(r, g, b, 24), 3, 24, 3);
		pos2.x = item->pos.x_pos + item->item_flags[1];
		pos2.y = item->pos.y_pos - 4096;
		pos2.z = item->pos.z_pos + item->item_flags[1];

		if (GetRandomControl() & 7 && item->item_flags[0])
			TriggerLightning(&pos2, (PHD_VECTOR*)&item->pos, (GetRandomControl() & 0x1F) + 16, RGBA(r, g, b, 32), 0, 48, 3);
		else
			TriggerLightning(&pos2, (PHD_VECTOR*)&item->pos, (GetRandomControl() & 0x1F) + 48, RGBA(r, g, b, 32), 1, 48, 3);

		room_number = lara_item->room_number;

		if (room_number == 87 || room_number == 22 || room_number == 10 || room_number == 15)
		{
			FlashFadeR = r >> (GlobalCounter & 1 ? 1 : 2);
			FlashFadeG = g >> (GlobalCounter & 1 ? 1 : 2);
			FlashFadeB = b >> (GlobalCounter & 1 ? 1 : 2);
			FlashFader = 32;
		}

		TriggerLightningGlow(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, RGBA(r, g, b, 64));

		if (item->trigger_flags == 2 && !item->item_flags[0])
		{
			ExplodeItemNode(&items[item->item_flags[2] & 0xFF], 0, 0, -128);
			KillItem(item->item_flags[2] & 0xFF);
			ExplodeItemNode(&items[item->item_flags[2] >> 8], 0, 0, -128);
			KillItem(item->item_flags[2] >> 8);
			KillItem(item_number);
		}
		else
		{
			if (item->trigger_flags == 1 && !lara.burn && !((item->pos.x_pos ^ lara_item->pos.x_pos) & -1024) &&
				!((item->pos.z_pos ^ lara_item->pos.z_pos) & -1024) && lara_item->pos.y_pos <= item->pos.y_pos)
			{
				LaraBurn();
				lara_item->hit_points = 0;
			}
		}
	}
	else if (!(GetRandomControl() & 0x3F))
	{
		SoundEffect(SFX_THUNDER_CRACK, &item->pos, SFX_DEFAULT);
		item->item_flags[0] = (GetRandomControl() & 3) + 4;
		item->item_flags[1] = (GetRandomControl() & 0x3FF) - 512;
	}
}

void inject_objects(bool replace)
{
	INJECT(0x00456580, ControlMapper, replace);
	INJECT(0x00456CD0, ControlLightningConductor, replace);
}
