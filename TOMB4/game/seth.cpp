#include "../tomb4/pch.h"
#include "seth.h"
#include "effect2.h"
#include "../specific/function_stubs.h"
#include "items.h"
#include "objects.h"
#include "tomb4fx.h"
#include "sphere.h"
#include "../specific/3dmath.h"
#include "box.h"

void TriggerSethMissileFlame(short fx_number, long xv, long yv, long zv)
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
	sptr->dR = 0;
	sptr->dB = (GetRandomControl() & 0x7F) + 32;
	sptr->dG = sptr->dB + 64;
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

	if (effects[fx_number].flag1 == 1)
		sptr->Scalar = 3;
	else
		sptr->Scalar = 2;

	sptr->Size = (GetRandomControl() & 7) + 64;
	sptr->sSize = sptr->Size;
	sptr->dSize = sptr->Size >> 5;
}

void TriggerSethMissile(PHD_3DPOS* pos, short room_number, short type)
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
		fx->pos.y_rot = pos->y_rot;
		fx->pos.z_rot = 0;
		fx->room_number = room_number;
		fx->counter = short(2 * GetRandomControl() + 0x8000);
		fx->flag1 = type;
		fx->object_number = BUBBLES;
		fx->speed = (GetRandomControl() & 0x1F) - (type == 1 ? 64 : 0) + 96;
		fx->frame_number = objects[BUBBLES].mesh_index + 2 * type;
	}
}

void TriggerSethSparks(long x, long y, long z, short xv, short yv, short zv)
{
	SPARKS* sptr;
	long dx, dz;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.x_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 0;
	sptr->sG = 0;
	sptr->sB = 0;
	sptr->dR = 64;
	sptr->dG = (GetRandomControl() & 0x7F) + 64;
	sptr->dB = sptr->dG + 32;
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

void TriggerSethFlame(short item_number, uchar NodeNumber, short size)
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
	sptr->dR = 0;
	sptr->dG = (GetRandomControl() & 0x7F) + 32;
	sptr->dB = sptr->dG + 64;
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
	sptr->Size = uchar((GetRandomControl() & 0xF) + size);
	sptr->sSize = sptr->Size;
	sptr->dSize = sptr->Size >> 4;
}

void DoSethEffects(short item_number)
{
	ITEM_INFO* item;
	PHD_VECTOR r, l;
	PHD_VECTOR vec;
	PHD_3DPOS pos;
	short xv, yv, zv, size;
	short angles[2];

	item = &items[item_number];
	item->item_flags[0]++;

	r.x = NodeOffsets[3].x;
	r.y = NodeOffsets[3].y;
	r.z = NodeOffsets[3].z;
	GetJointAbsPosition(item, &r, NodeOffsets[3].mesh_num);

	l.x = NodeOffsets[2].x;
	l.y = NodeOffsets[2].y;
	l.z = NodeOffsets[2].z;
	GetJointAbsPosition(item, &l, NodeOffsets[2].mesh_num);

	switch (item->current_anim_state)
	{
	case 11:
	case 15:

		if (item->item_flags[0] < 78 && (GetRandomControl() & 0x1F) < item->item_flags[0])
		{
			for (int i = 0; i < 2; i++)
			{
				vec.x = (GetRandomControl() & 0x7FF) + r.x - 1024;
				vec.y = (GetRandomControl() & 0x7FF) + r.y - 1024;
				zv = (GetRandomControl() & 0x7FF);
				vec.z = zv + r.z - 1024;
				xv = short((r.x - vec.x) << 3);
				yv = short((r.y - vec.y) << 3);
				zv = (1024 - zv) << 3;
				TriggerSethSparks(vec.x, vec.y, vec.z, xv, yv, zv);

				vec.x = (GetRandomControl() & 0x7FF) + l.x - 1024;
				vec.y = (GetRandomControl() & 0x7FF) + l.y - 1024;
				zv = (GetRandomControl() & 0x7FF);
				vec.z = zv + l.z - 1024;
				xv = short((l.x - vec.x) << 3);
				yv = short((l.y - vec.y) << 3);
				zv = (1024 - zv) << 3;
				TriggerSethSparks(vec.x, vec.y, vec.z, xv, yv, zv);
			}
		}

		size = item->item_flags[0] * 2;

		if (size > 128)
			size = 128;

		if ((wibble & 0xF) == 8)
		{
			if (item->item_flags[0] < 127)
				TriggerSethFlame(item_number, 2, size);
		}
		else if (!(wibble & 0xF) && item->item_flags[0] < 103)
			TriggerSethFlame(item_number, 3, size);

		if (item->item_flags[0] >= 96 && item->item_flags[0] <= 99)
		{
			vec.x = NodeOffsets[3].x;
			vec.y = NodeOffsets[3].y << 1;
			vec.z = NodeOffsets[3].z;
			GetJointAbsPosition(item, &vec, NodeOffsets[3].mesh_num);
			pos.x_pos = r.x;
			pos.y_pos = r.y;
			pos.z_pos = r.z;
			phd_GetVectorAngles(vec.x - pos.x_pos, vec.y - pos.y_pos, vec.z - pos.z_pos, angles);
			pos.x_rot = angles[1];
			pos.y_rot = angles[0];
			TriggerSethMissile(&pos, item->room_number, 0);
		}

		if (item->item_flags[0] >= 122 && item->item_flags[0] <= 125)
		{
			vec.x = NodeOffsets[2].x;
			vec.y = NodeOffsets[2].y << 1;
			vec.z = NodeOffsets[2].z;
			GetJointAbsPosition(item, &vec, NodeOffsets[2].mesh_num);
			pos.x_pos = l.x;
			pos.y_pos = l.y;
			pos.z_pos = l.z;
			phd_GetVectorAngles(vec.x - pos.x_pos, vec.y - pos.y_pos, vec.z - pos.z_pos, angles);
			pos.x_rot = angles[1];
			pos.y_rot = angles[0];
			TriggerSethMissile(&pos, item->room_number, 0);
		}

		break;

	case 12:

		size = item->item_flags[0] * 4;

		if (size > 160)
			size = 160;

		if ((wibble & 0xF) == 8)
		{
			if (item->item_flags[0] < 132)
				TriggerSethFlame(item_number, 2, size);
		}
		else if (!(wibble & 0xF) && item->item_flags[0] < 132)
			TriggerSethFlame(item_number, 3, size);

		if ((item->item_flags[0] >= 60 && item->item_flags[0] <= 74 || item->item_flags[0] >= 112 && item->item_flags[0] <= 124) && wibble & 4)
		{
			vec.x = NodeOffsets[3].x;
			vec.y = NodeOffsets[3].y << 1;
			vec.z = NodeOffsets[3].z;
			GetJointAbsPosition(item, &vec, NodeOffsets[3].mesh_num);
			pos.x_pos = r.x;
			pos.y_pos = r.y;
			pos.z_pos = r.z;
			phd_GetVectorAngles(vec.x - pos.x_pos, vec.y - pos.y_pos, vec.z - pos.z_pos, angles);
			pos.x_rot = angles[1];
			pos.y_rot = angles[0];
			TriggerSethMissile(&pos, item->room_number, 0);

			vec.x = NodeOffsets[2].x;
			vec.y = NodeOffsets[2].y << 1;
			vec.z = NodeOffsets[2].z;
			GetJointAbsPosition(item, &vec, NodeOffsets[2].mesh_num);
			pos.x_pos = l.x;
			pos.y_pos = l.y;
			pos.z_pos = l.z;
			phd_GetVectorAngles(vec.x - pos.x_pos, vec.y - pos.y_pos, vec.z - pos.z_pos, angles);
			pos.x_rot = angles[1];
			pos.y_rot = angles[0];
			TriggerSethMissile(&pos, item->room_number, 0);
		}

		break;

	case 13:

		if (item->item_flags[0] > 40 && item->item_flags[0] < 100 && (GetRandomControl() & 7) < item->item_flags[0] - 40)
		{
			for (int i = 0; i < 2; i++)
			{
				vec.x = (GetRandomControl() & 0x7FF) + r.x - 1024;
				vec.y = (GetRandomControl() & 0x7FF) + r.y - 1024;
				zv = (GetRandomControl() & 0x7FF);
				vec.z = zv + r.z - 1024;
				xv = short((r.x - vec.x) << 3);
				yv = short((r.y - vec.y) << 3);
				zv = (1024 - zv) << 3;
				TriggerSethSparks(vec.x, vec.y, vec.z, xv, yv, zv);

				vec.x = (GetRandomControl() & 0x7FF) + l.x - 1024;
				vec.y = (GetRandomControl() & 0x7FF) + l.y - 1024;
				zv = (GetRandomControl() & 0x7FF);
				vec.z = zv + l.z - 1024;
				xv = short((l.x - vec.x) << 3);
				yv = short((l.y - vec.y) << 3);
				zv = (1024 - zv) << 3;
				TriggerSethSparks(vec.x, vec.y, vec.z, xv, yv, zv);
			}
		}

		size = item->item_flags[0] * 2;

		if (size > 128)
			size = 128;

		if ((wibble & 0xF) == 8)
		{
			if (item->item_flags[0] < 103)
				TriggerSethFlame(item_number, 2, size);
		}
		else if (!(wibble & 0xF) && item->item_flags[0] < 103)
			TriggerSethFlame(item_number, 3, size);

		if (item->item_flags[0] == 102)
		{
			vec.x = NodeOffsets[3].x;
			vec.y = NodeOffsets[3].y << 1;
			vec.z = NodeOffsets[3].z;
			GetJointAbsPosition(item, &vec, NodeOffsets[3].mesh_num);
			pos.x_pos = r.x;
			pos.y_pos = r.y;
			pos.z_pos = r.z;
			phd_GetVectorAngles(vec.x - pos.x_pos, vec.y - pos.y_pos, vec.z - pos.z_pos, angles);
			pos.x_rot = angles[1];
			pos.y_rot = angles[0];
			TriggerSethMissile(&pos, item->room_number, 1);
		}

		break;
	}
}

void InitialiseSeth(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[SETHA].anim_index + 4;
	item->frame_number = item->anim_number;
	item->current_anim_state = 12;
	item->goal_anim_state = 12;
}

void inject_seth(bool replace)
{
	INJECT(0x004103B0, TriggerSethMissileFlame, replace);
	INJECT(0x00410070, TriggerSethMissile, replace);
	INJECT(0x00410130, TriggerSethSparks, replace);
	INJECT(0x00410220, TriggerSethFlame, replace);
	INJECT(0x00410530, DoSethEffects, replace);
	INJECT(0x00410C30, InitialiseSeth, replace);
}
