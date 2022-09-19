#include "../tomb4/pch.h"
#include "demigod.h"
#include "items.h"
#include "../specific/function_stubs.h"
#include "objects.h"
#include "effect2.h"
#include "tomb4fx.h"
#include "sphere.h"
#include "../specific/3dmath.h"
#include "box.h"
#include "control.h"
#include "lara_states.h"
#include "people.h"
#include "camera.h"
#include "effects.h"
#include "lara.h"
#include "gameflow.h"

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

void TriggerDemigodMissileFlame(short fx_number, long xv, long yv, long zv)
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
		sptr->Xvel = short((off * phd_sin(angle)) >> W2V_SHIFT);
		sptr->Yvel = -32 - (GetRandomControl() & 0x3F);
		sptr->Zvel = short((off * phd_cos(angle)) >> W2V_SHIFT);
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

void DoDemigodEffects(short item_number)
{
	ITEM_INFO* item;
	PHD_3DPOS pos;
	PHD_VECTOR pos1;
	PHD_VECTOR pos2;
	short angles[2];
	short anim, frame;

	item = &items[item_number];
	anim = item->anim_number - objects[item->object_number].anim_index;

	if (anim == 8 || anim == 19)
	{
		if (item->frame_number == anims[item->anim_number].frame_base)
		{
			pos1.x = -544;
			pos1.y = 96;
			pos1.z = 0;
			GetJointAbsPosition(item, &pos1, 16);
			pos2.x = -900;
			pos2.y = 96;
			pos2.z = 0;
			GetJointAbsPosition(item, &pos2, 16);
			pos.z_pos = pos1.z;
			pos.y_pos = pos1.y;
			pos.x_pos = pos1.x;
			phd_GetVectorAngles(pos2.x - pos1.x, pos2.y - pos1.y, pos2.z - pos1.z, angles);
			pos.x_rot = angles[1];
			pos.y_rot = angles[0];

			if (item->object_number == DEMIGOD3)
				TriggerDemigodMissile(&pos, item->room_number, 3);
			else
				TriggerDemigodMissile(&pos, item->room_number, 5);
		}
	}
	else if (anim == 16)
	{
		frame = item->frame_number - anims[item->anim_number].frame_base;

		if (frame >= 8 && frame <= 64)
		{
			pos1.x = 0;
			pos1.y = 0;
			pos1.z = 192;
			pos2.x = 0;
			pos2.y = 0;
			pos2.z = 384;
			GetJointAbsPosition(item, &pos1, GlobalCounter & 1 ? 18 : 17);
			GetJointAbsPosition(item, &pos2, GlobalCounter & 1 ? 18 : 17);
			pos.z_pos = pos1.z;
			pos.y_pos = pos1.y;
			pos.x_pos = pos1.x;
			phd_GetVectorAngles(pos2.x - pos1.x, pos2.y - pos1.y, pos2.z - pos1.z, angles);
			pos.x_rot = angles[1];
			pos.y_rot = angles[0];
			TriggerDemigodMissile(&pos, item->room_number, 4);
		}
	}
}

void InitialiseDemigod(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* item2;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[item->object_number].anim_index;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = 0;
	item->goal_anim_state = 0;

	for (int i = 0; i < level_items; i++)
	{
		item2 = &items[i];

		if (item != item2 && item2->object_number == DEMIGOD3 && !item2->item_flags[0])
			item->item_flags[0] = i;
	}
}

void DemigodControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* item2;
	CREATURE_INFO* god;
	ROOM_INFO* r;
	FLOOR_INFO* floor;
	AI_INFO info;
	PHD_VECTOR pos;
	short* zone;
	long dx, dz, h;
	short objnum, angle, torso_x, torso_y, torso_z, head, iAngle, iAhead, room_number;

	item = &items[item_number];
	objnum = item->item_flags[0];

	if (objnum)
	{
		item2 = &items[objnum];

		if (item2->status == ITEM_ACTIVE && item2->active)
		{
			item->hit_points = objects[item->object_number].hit_points;
			return;
		}
	}

	if (!CreatureActive(item_number))
		return;

	god = (CREATURE_INFO*)item->data;
	objnum = item->object_number;
	angle = 0;
	head = 0;
	torso_x = 0;
	torso_y = 0;
	torso_z = 0;

	if (gfCurrentLevel == 24)	//Chambers of Tulun
	{
		r = &room[lara_item->room_number];
		zone = ground_zone[god->LOT.zone][flip_status];
		lara_item->box_number = r->floor[((lara_item->pos.z_pos - r->z) >> 10) + r->x_size * ((lara_item->pos.x_pos - r->x) >> 10)].box;

		if (zone[item->box_number] == zone[lara_item->box_number])
		{
			item->ai_bits = 0;
			god->enemy = lara_item;
		}
		else
		{
			item->ai_bits = FOLLOW;
			item->item_flags[3] = lara.location;
			god->enemy = 0;
		}
	}

	if (item->hit_points <= 0)
	{
		item->hit_points = 0;

		if (item->current_anim_state != 8 && item->current_anim_state != 15)
		{
			if (item->current_anim_state == 1 || item->current_anim_state == 2)
			{
				item->anim_number = objects[objnum].anim_index + 27;
				item->frame_number = anims[item->anim_number].frame_base;
				item->current_anim_state = 15;
			}
			else
			{
				item->anim_number = objects[objnum].anim_index + 12;
				item->frame_number = anims[item->anim_number].frame_base;
				item->current_anim_state = 8;
			}
		}
	}
	else
	{
		if (item->ai_bits)
			GetAITarget(god);

		CreatureAIInfo(item, &info);

		if (god->enemy == lara_item)
		{
			iAngle = (short)info.angle;
			iAhead = (short)info.ahead;
		}
		else
		{
			dx = lara_item->pos.x_pos - item->pos.x_pos;
			dz = lara_item->pos.z_pos - item->pos.z_pos;
			iAngle = (short)phd_atan(dz, dx) - item->pos.y_rot;
			iAhead = abs(iAngle) < 16384;
			dx = abs(dx);
			dz = abs(dz);

			if (dx > dz)
				info.x_angle = (short)phd_atan(dx + (dz >> 1), item->pos.y_pos - lara_item->pos.y_pos);
			else
				info.x_angle = (short)phd_atan(dz + (dx >> 1), item->pos.y_pos - lara_item->pos.y_pos);
		}

		GetCreatureMood(item, &info, 1);
		CreatureMood(item, &info, 1);
		angle = CreatureTurn(item, god->maximum_turn);
		torso_x = -info.x_angle;

		if (iAhead)
		{
			torso_y = iAngle >> 1;
			torso_z = iAngle >> 1;
			head = iAngle >> 1;
		}
		else if (info.ahead)
		{
			torso_y = info.angle >> 1;
			torso_z = info.angle >> 1;
			head = info.angle >> 1;
		}

		switch (item->current_anim_state)
		{
		case 0:
			god->maximum_turn = 0;

			if (info.ahead)
				torso_x = -info.x_angle;

			if (objnum == DEMIGOD1)
			{
				if (info.distance < 0x900000)
				{
					if (info.bite ||
					((lara_item->current_anim_state >= AS_CLIMBSTNC && lara_item->current_anim_state <= AS_CLIMBDOWN ||
						lara_item->current_anim_state == AS_HANG) && !lara.location && lara_item->room_number > 114))
					{
						item->goal_anim_state = 13;
						break;
					}
				}
			}
			else
			{
				if (Targetable(item, &info))
				{
					god->flags = 1;

					if (objnum == DEMIGOD2)
						item->goal_anim_state = 3;
					else
						item->goal_anim_state = 11;

					break;
				}

				if (objnum == DEMIGOD3)
				{
					if (info.distance > 0x400000 && info.distance < 0x1900000)
					{
						if (!(GetRandomControl() & 3))
						{
							item->goal_anim_state = 9;
							break;
						}
					}
				}
			}

			if (info.distance > 0x900000 && objnum == DEMIGOD2)
				item->goal_anim_state = 5;
			else
				item->goal_anim_state = 1;

			break;

		case 1:
			god->maximum_turn = 1274;

			if (info.distance < 0x400000)
			{
				item->goal_anim_state = 0;
				break;
			}

			if (objnum == DEMIGOD1)
			{
				if (info.distance < 0x900000)
				{
					item->goal_anim_state = 0;
					break;
				}
			}
			else if (Targetable(item, &info))
			{
				item->goal_anim_state = 0;
				break;
			}

			if (info.distance > 0x900000)
			{
				if (objnum == DEMIGOD2)
					item->goal_anim_state = 5;
				else
					item->goal_anim_state = 2;
			}

			break;

		case 2:
			god->maximum_turn = 1274;

			if (info.distance < 0x400000)
			{
				item->goal_anim_state = 0;
				break;
			}

			if (objnum == DEMIGOD1)
			{
				if (info.distance < 0x900000)
				{
					item->goal_anim_state = 0;
					break;
				}
			}
			else
			{
				if (Targetable(item, &info) || objnum == DEMIGOD3 && info.distance > 0x400000)
				{
					item->goal_anim_state = 0;
					break;
				}

				if (info.distance < 0x900000)
					item->goal_anim_state = 1;
			}

			break;

		case 3:

			if (info.ahead)
				torso_x = -info.x_angle;

			god->maximum_turn = 0;

			if (item->anim_number == objects[objnum].anim_index + 6)
			{
				if (abs(info.angle) < 1274)
					item->pos.y_rot += info.angle;
				else if (info.angle < 0)
					item->pos.y_rot -= 1274;
				else
					item->pos.y_rot += 1274;
			}

			if (Targetable(item, &info) || god->flags)
				item->goal_anim_state = 4;
			else
				item->goal_anim_state = 0;

			god->flags = 0;

			break;

		case 4:
		case 12:
			DoDemigodEffects(item_number);
			break;

		case 6:
			god->maximum_turn = 1274;

			if (Targetable(item, &info))
				item->goal_anim_state = 7;

			break;

		case 9:
			god->maximum_turn = 1274;

			if (!Targetable(item, &info) && info.distance < 0x1900000)
				item->goal_anim_state = 10;

			break;

		case 10:
			god->maximum_turn = 1274;
			DoDemigodEffects(item_number);

			if (!Targetable(item, &info) || info.distance < 0x1900000 || !(GetRandomControl() & 0xFF))
				item->goal_anim_state = 0;

			break;

		case 11:
			torso_y = iAngle;
			torso_z = 0;

			if (info.ahead)
				torso_x = -info.x_angle;

			god->maximum_turn = 0;

			if (item->anim_number == objects[objnum].anim_index + 6)
			{
				if (abs(info.angle) < 1274)
					item->pos.y_rot += info.angle;
				else if (info.angle < 0)
					item->pos.y_rot -= 1274;
				else
					item->pos.y_rot += 1274;
			}

			if (Targetable(item, &info) || god->flags)
				item->goal_anim_state = 12;
			else
				item->goal_anim_state = 0;

			god->flags = 0;
			break;

		case 13:
			god->maximum_turn = 0;
			torso_y = iAngle;
			torso_z = 0;

			if (abs(info.angle) < 1274)
				item->pos.y_rot += info.angle;
			else if (info.angle < 0)
				item->pos.y_rot -= 1274;
			else
				item->pos.y_rot += 1274;

			if (info.distance < 0x900000 && info.bite ||
			((lara_item->current_anim_state >= AS_CLIMBSTNC && lara_item->current_anim_state <= AS_CLIMBDOWN ||
				lara_item->current_anim_state == AS_HANG) && !lara.location && lara_item->room_number > 114))
				item->goal_anim_state = 14;
			else
				item->goal_anim_state = 0;

			break;

		case 14:

			if (item->frame_number - anims[item->anim_number].frame_base == 26)
			{
				pos.x = 80;
				pos.y = -8;
				pos.z = -40;
				GetJointAbsPosition(item, &pos, 17);
				room_number = item->room_number;
				floor = GetFloor(pos.x, pos.y, pos.z, &room_number);
				h = GetHeight(floor, pos.x, pos.y, pos.z);

				if (h == NO_HEIGHT)
					pos.y -= 128;
				else
					pos.y = h - 128;

				TriggerShockwave(&pos, 0x580018, 256, 0x20808080, 0x20000);
				TriggerHammerSmoke(pos.x, pos.y + 128, pos.z, 8);
				camera.bounce = -128;

				if ((lara_item->current_anim_state >= AS_CLIMBSTNC && lara_item->current_anim_state <= AS_CLIMBDOWN ||
					lara_item->current_anim_state == AS_HANG) && !lara.location && lara_item->room_number > 114)
				{
					lara.torso_x_rot = 0;
					lara.torso_y_rot = 0;
					lara.head_x_rot = 0;
					lara.head_y_rot = 0;
					lara_item->anim_number = ANIM_FALLDOWN;
					lara_item->frame_number = anims[ANIM_FALLDOWN].frame_base;
					lara_item->current_anim_state = AS_FORWARDJUMP;
					lara_item->goal_anim_state = AS_FORWARDJUMP;
					lara_item->pos.x_pos += -50 * phd_sin(lara_item->pos.y_rot) >> W2V_SHIFT;
					lara_item->gravity_status = 1;
					lara_item->speed = 2;
					lara_item->fallspeed = 1;
					lara.gun_status = LG_NO_ARMS;
				}
			}

			break;
		}
	}

	CreatureTilt(item, 0);
	CreatureJoint(item, 0, torso_z);
	CreatureJoint(item, 1, torso_x);
	CreatureJoint(item, 2, torso_y);
	CreatureJoint(item, 3, head);
	CreatureAnimation(item_number, angle, 0);
}
