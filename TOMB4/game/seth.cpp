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
#include "control.h"
#include "people.h"
#include "effects.h"
#include "lara_states.h"
#include "lara.h"

static BITE_INFO left_hand = { 0, 220, 50, 17 };
static BITE_INFO right_hand = { 0, 220, 50, 13 };

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

void SethControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* seth;
	FLOOR_INFO* floor;
	AI_INFO info;
	long x, y, z, Xoffset, Zoffset, c, h, nearheight, midheight, farheight, can_jump;
	short angle, room_number, hp;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	seth = (CREATURE_INFO*)item->data;

	angle = 0;
	Xoffset = 870 * phd_sin(item->pos.y_rot) >> W2V_SHIFT;
	Zoffset = 870 * phd_cos(item->pos.y_rot) >> W2V_SHIFT;

	room_number = item->room_number;
	x = item->pos.x_pos;
	y = item->pos.y_pos;
	z = item->pos.z_pos;
	floor = GetFloor(x, y, z, &room_number);
	c = GetHeight(floor, x, y, z);

	x = item->pos.x_pos + Xoffset;
	y = item->pos.y_pos;
	z = item->pos.z_pos + Zoffset;
	floor = GetFloor(x, y, z, &room_number);
	nearheight = GetHeight(floor, x, y, z);

	room_number = item->room_number;
	x += Xoffset;
	z += Zoffset;
	floor = GetFloor(x, y, z, &room_number);
	midheight = GetHeight(floor, x, y, z);

	room_number = item->room_number;
	x += Xoffset;
	z += Zoffset;
	floor = GetFloor(x, y, z, &room_number);
	farheight = GetHeight(floor, x, y, z);
	can_jump = (y < nearheight - 384 || y < midheight - 384) && (y < farheight + 256 && y > farheight - 256 || farheight == NO_HEIGHT);

	x = item->pos.x_pos - Xoffset;
	y = item->pos.y_pos;
	z = item->pos.z_pos - Zoffset;
	floor = GetFloor(x, y, z, &room_number);
	h = GetHeight(floor, x, y, z);
	CreatureAIInfo(item, &info);

	if (item->hit_points <= 0)
		item->hit_points = 0;
	else
	{
		if (item->ai_bits)
			GetAITarget(seth);
		else
			seth->enemy = lara_item;

		CreatureAIInfo(item, &info);
		GetCreatureMood(item, &info, 1);
		CreatureMood(item, &info, 1);
		angle = CreatureTurn(item, seth->maximum_turn);

		switch (item->current_anim_state)
		{
		case 1:
			seth->LOT.is_jumping = 0;
			seth->flags = 0;

			if (item->required_anim_state)
				item->goal_anim_state = item->required_anim_state;
			else if (info.distance < 0x100000 && info.bite)
				item->goal_anim_state = 8;
			else if (lara_item->pos.y_pos < item->pos.y_pos - 1024)
			{
				if (seth->reached_goal)
					item->goal_anim_state = 14;
				else
				{
					item->ai_bits = AMBUSH;
					seth->hurt_by_lara = 1;	
					item->goal_anim_state = 2;
				}
			}
			else if (info.distance < 0x640000 && info.ahead && GetRandomControl() & 1 && Targetable(item, &info))
			{
				item->item_flags[0] = 0;
				item->goal_anim_state = 11;
			}
			else if (c != NO_HEIGHT && c < y - 1792 && h != NO_HEIGHT && h > y - 1024 && GetRandomControl() & 1)
			{
				item->pos.y_pos -= 1536;

				if (Targetable(item, &info))
				{
					item->item_flags[0] = 0;
					item->goal_anim_state = 12;
				}
				else
					item->goal_anim_state = 2;

				item->pos.y_pos += 1536;
			}
			else if (info.distance < 0x900000 && info.angle < 0x1800 && info.angle > -0x1800 && info.bite)
			{
				if (Targetable(item, &info))
					item->goal_anim_state = 4;
				else
					item->goal_anim_state = 2;
			}
			else if (info.distance < 0x1000000 && info.angle < 0x2000 && info.angle > -0x2000 && h != NO_HEIGHT && h >= y - 256 && Targetable(item, &info))
			{
				item->item_flags[0] = 0;
				item->goal_anim_state = 13;
			}
			else if (can_jump)
				item->goal_anim_state = 5;
			else
				item->goal_anim_state = 2;

			break;

		case 2:
			seth->maximum_turn = 1274;

			if (info.ahead && info.distance < 0x1000000 || can_jump || seth->reached_goal)
				item->goal_anim_state = 1; 
			else if (info.distance > 0x900000)
				item->goal_anim_state = 3;

			break;

		case 3:
			seth->maximum_turn = 2002;

			if (info.ahead && info.distance < 0x1000000 || can_jump || seth->reached_goal)
				item->goal_anim_state = 1;
			else if (info.distance < 0x900000)
				item->goal_anim_state = 2;

			break;

		case 4:

			if (can_jump)
			{
				if (item->anim_number == objects[45].anim_index + 15 && item->frame_number == anims[item->anim_number].frame_base)
				{
					seth->LOT.is_jumping = 1;
					seth->maximum_turn = 0;
				}
			}

			if (!seth->flags && item->touch_bits && item->anim_number == objects[SETHA].anim_index + 16)
			{
				if (item->touch_bits & 0xE000)
				{
					lara_item->hit_points -= 200;
					lara_item->hit_status = 1;
					seth->flags = 1;
					CreatureEffectT(item, &right_hand, 25, -1, DoBloodSplat);
				}

				if (item->touch_bits & 0xE0000)
				{
					lara_item->hit_points -= 200;
					lara_item->hit_status = 1;
					seth->flags = 1;
					CreatureEffectT(item, &left_hand, 25, -1, DoBloodSplat);
				}
			}

			break;

		case 5:
			seth->LOT.is_jumping = 1;
			seth->maximum_turn = 0;
			break;

		case 7:

			if (item->anim_number == objects[SETHA].anim_index + 17 && item->frame_number == anims[item->anim_number].frame_end && GetRandomControl() & 1)
				item->required_anim_state = 10;

			break;

		case 8:
			hp = lara_item->hit_points;
			seth->maximum_turn = 0;

			if (abs(info.angle) < 546)
				item->pos.y_rot += info.angle;
			else if (info.angle < 0)
				item->pos.y_rot -= 546;
			else
				item->pos.y_rot += 546;

			if (!seth->flags && item->touch_bits)
			{
				if (item->frame_number > anims[item->anim_number].frame_base + 15 && item->frame_number < anims[item->anim_number].frame_base + 26)
				{
					lara_item->hit_points -= 250;
					lara_item->hit_status = 1;
					seth->flags = 1;
					CreatureEffectT(item, &right_hand, 25, -1, DoBloodSplat);
				}
			}

			if (hp && lara_item->hit_points <= 0)	//this hit killed her
			{
				CreatureKill(item, 14, 9, ANIM_SETHDEATH);
				seth->maximum_turn = 0;
				return;
			}

			break;

		case 15:
			seth->target.y = lara_item->pos.y_pos;
			
		case 11:
		case 12:
		case 13:
			seth->maximum_turn = 0;

			if (abs(info.angle) < 546)
				item->pos.y_rot += info.angle;
			else if (info.angle < 0)
				item->pos.y_rot -= 546;
			else
				item->pos.y_rot += 546;

			DoSethEffects(item_number);
			break;

		case 14:

			if (item->anim_number != objects[45].anim_index + 26)
			{
				seth->LOT.fly = 16;
				item->gravity_status = 0;
				seth->maximum_turn = 0;
				seth->target.y = lara_item->pos.y_pos;

				if (abs(info.angle) < 546)
					item->pos.y_rot += info.angle;
				else if (info.angle < 0)
					item->pos.y_rot -= 546;
				else
					item->pos.y_rot += 546;
			}

			if (lara_item->pos.y_pos > item->floor - 512)
			{
				seth->LOT.fly = 0;
				item->gravity_status = 1;

				if (item->pos.y_pos >= item->floor)
					item->goal_anim_state = 1;
			}
			else if (Targetable(item, &info))
			{
				item->item_flags[0] = 0;
				item->goal_anim_state = 15;
			}

			break;
		}
	}

	if (item->hit_status && (lara.gun_type == WEAPON_SHOTGUN || lara.gun_type == WEAPON_REVOLVER)
		&& info.distance < 0x400000 && !seth->LOT.is_jumping)
	{
		if (item->current_anim_state != 12)
		{
			if (item->current_anim_state > 13)
			{
				item->anim_number = objects[SETHA].anim_index + 25;
				item->current_anim_state = 16;
				item->goal_anim_state = 16;
			}
			else if (abs(h - y) < 512)
			{
				item->anim_number = objects[SETHA].anim_index + 17;
				item->current_anim_state = 7;
				item->goal_anim_state = 7;
			}
			else
			{
				item->anim_number = objects[SETHA].anim_index + 11;
				item->current_anim_state = 6;
				item->goal_anim_state = 6;
			}

			item->frame_number = anims[item->anim_number].frame_base;
		}
	}

	CreatureAnimation(item_number, angle, 0);
}
