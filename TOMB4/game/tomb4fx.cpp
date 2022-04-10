#include "../tomb4/pch.h"
#include "tomb4fx.h"
#include "../specific/function_stubs.h"
#include "draw.h"
#include "../specific/3dmath.h"
#include "items.h"
#include "objects.h"
#include "../specific/output.h"
#include "sound.h"
#include "delstuff.h"
#include "control.h"

LIGHTNING_STRUCT* TriggerLightning(PHD_VECTOR* s, PHD_VECTOR* d, char variation, long rgb, uchar flags, uchar size, uchar segments)
{
	LIGHTNING_STRUCT* lptr;
	char* vptr;

	for (int i = 0; i < 16; i++)
	{
		lptr = &Lightning[i];

		if (!lptr->Life)
		{
			lptr->Point[0].x = s->x;
			lptr->Point[0].y = s->y;
			lptr->Point[0].z = s->z;
			lptr->Point[1].x = ((s->x * 3) + d->x) >> 2;
			lptr->Point[1].y = ((s->y * 3) + d->y) >> 2;
			lptr->Point[1].z = ((s->z * 3) + d->z) >> 2;
			lptr->Point[2].x = ((d->x * 3) + s->x) >> 2;
			lptr->Point[2].y = ((d->y * 3) + s->y) >> 2;
			lptr->Point[2].z = ((d->z * 3) + s->z) >> 2;
			lptr->Point[3].x = d->x;
			lptr->Point[3].y = d->y;
			lptr->Point[3].z = d->z;
			vptr = &lptr->Xvel1;

			for (int j = 0; j < 6; j++)
				*vptr++ = (GetRandomControl() % variation) - (variation >> 1);

			for (int j = 0; j < 3; j++)
			{
				if (flags & 2)
					*vptr++ = (GetRandomControl() % variation) - (variation >> 1);
				else
					*vptr++ = 0;
			}

			lptr->Flags = flags;
			*(long*)&lptr->r = rgb;
			lptr->Segments = segments;
			lptr->Rand = variation;
			lptr->Size = size;
			return lptr;
		}
	}

	return 0;
}

long ExplodingDeath2(short item_number, long mesh_bits, short Flags)
{
	ITEM_INFO* item;
	OBJECT_INFO* obj;
	FX_INFO* fx;
	long* bone;
	short* rotation;
	short* frame;
	short* extra_rotation;
	long bit, poppush;
	short fx_number;

	item = &items[item_number];
	obj = &objects[item->object_number];
	frame = GetBestFrame(item);
	phd_PushUnitMatrix();
	phd_mxptr[M03] = 0;
	phd_mxptr[M13] = 0;
	phd_mxptr[M23] = 0;
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	phd_TranslateRel(frame[6], frame[7], frame[8]);
	rotation = frame + 9;
	gar_RotYXZsuperpack(&rotation, 0);

#ifdef GENERAL_FIXES	//fix crash if exploding inactive enemies.. eg: mummies before they wake up
	if (!item->data)
		extra_rotation = no_rotation;
	else
#endif
		extra_rotation = (short*)item->data;

	bone = &bones[obj->bone_index];
	bit = 1;

	if (mesh_bits & 1 && item->mesh_bits & 1)
	{
		if (Flags & 0x100 || !(GetRandomControl() & 3))
		{
			fx_number = CreateEffect(item->room_number);

			if (fx_number != NO_ITEM)
			{
				fx = &effects[fx_number];
				fx->pos.x_pos = item->pos.x_pos + (phd_mxptr[M03] >> 14);
				fx->pos.y_pos = item->pos.y_pos + (phd_mxptr[M13] >> 14);
				fx->pos.z_pos = item->pos.z_pos + (phd_mxptr[M23] >> 14);
				fx->room_number = item->room_number;
				fx->pos.y_rot = (short)(GetRandomControl() << 1);
				fx->pos.x_rot = 0;

				if (Flags & 0x10)
					fx->speed = 0;
				else if (Flags & 0x20)
					fx->speed = (short)(GetRandomControl() >> 12);
				else
					fx->speed = (short)(GetRandomControl() >> 8);

				if (Flags & 0x40)
					fx->fallspeed = 0;
				else if (Flags & 0x80)
					fx->fallspeed = (short)(-(GetRandomControl() >> 12));
				else
					fx->fallspeed = (short)(-(GetRandomControl() >> 8));

				fx->frame_number = obj->mesh_index;
				fx->object_number = BODY_PART;
				fx->shade = 0x4210;
				fx->flag2 = Flags;
			}

			item->mesh_bits--;
		}
	}

	for (int i = 1; i < obj->nmeshes; i++, bone += 4)
	{
		poppush = bone[0];

		if (poppush & 1)
			phd_PopMatrix();

		if (poppush & 2)
			phd_PushMatrix();

		phd_TranslateRel(bone[1], bone[2], bone[3]);
		gar_RotYXZsuperpack(&rotation, 0);

		if (poppush & 28)
		{
			if (poppush & 8)
			{
				phd_RotY(*extra_rotation);
				extra_rotation++;
			}

			if (poppush & 4)
			{
				phd_RotX(*extra_rotation);
				extra_rotation++;
			}

			if (poppush & 16)
			{
				phd_RotZ(*extra_rotation);
				extra_rotation++;
			}
		}

		bit <<= 1;

		if (bit & mesh_bits && bit & item->mesh_bits && (Flags & 0x100 || !(GetRandomControl() & 3)))
		{
			fx_number = CreateEffect(item->room_number);

			if (fx_number != NO_ITEM)
			{
				fx = &effects[fx_number];
				fx->pos.x_pos = item->pos.x_pos + (phd_mxptr[M03] >> 14);
				fx->pos.y_pos = item->pos.y_pos + (phd_mxptr[M13] >> 14);
				fx->pos.z_pos = item->pos.z_pos + (phd_mxptr[M23] >> 14);
				fx->room_number = item->room_number;
				fx->pos.y_rot = (short)(GetRandomControl() << 1);
				fx->pos.x_rot = 0;

				if (Flags & 0x10)
					fx->speed = 0;
				else if (Flags & 0x20)
					fx->speed = (short)(GetRandomControl() >> 12);
				else
					fx->speed = (short)(GetRandomControl() >> 8);

				if (Flags & 0x40)
					fx->fallspeed = 0;
				else if (Flags & 0x80)
					fx->fallspeed = (short)(-(GetRandomControl() >> 12));
				else
					fx->fallspeed = (short)(-(GetRandomControl() >> 8));

				fx->frame_number = obj->mesh_index + 2 * i;
				fx->object_number = BODY_PART;
				fx->shade = 0x4210;
				fx->flag2 = Flags;
			}

			item->mesh_bits -= bit;
		}
	}

	phd_PopMatrix();
	return !item->mesh_bits;
}

void DrawGunshells()
{
	GUNSHELL_STRUCT* p;
	OBJECT_INFO* obj;

	phd_left = 0;
	phd_right = phd_winwidth;
	phd_top = 0;
	phd_bottom = phd_winheight;

	for (int i = 0; i < 24; i++)
	{
		p = &Gunshells[i];

		if (p->counter)
		{
			obj = &objects[p->object_number];
			phd_PushMatrix();
			phd_TranslateAbs(p->pos.x_pos, p->pos.y_pos, p->pos.z_pos);
			phd_RotYXZ(p->pos.y_rot, p->pos.x_rot, p->pos.z_rot);
			phd_PutPolygons(meshes[obj->mesh_index], -1);
			phd_PopMatrix();
		}
	}
}

void TriggerGunSmoke(long x, long y, long z, long xVel, long yVel, long zVel, long notLara, long weaponType, long shade)
{
	SMOKE_SPARKS* sptr;
	uchar size;

	sptr = &smoke_spark[GetFreeSmokeSpark()];
	sptr->On = 1;
	sptr->sShade = 0;
	sptr->dShade = uchar(4 * shade);
	sptr->ColFadeSpeed = 4;
	sptr->FadeToBlack = uchar(32 - 16 * notLara);
	sptr->Life = (GetRandomControl() & 3) + 40;
	sptr->sLife = sptr->Life;

	if ((weaponType == WEAPON_PISTOLS || weaponType == WEAPON_REVOLVER || weaponType == WEAPON_UZI) && sptr->dShade > 64u)
		sptr->dShade = 64;

	sptr->TransType = 2;
	sptr->x = (GetRandomControl() & 0x1F) + x - 16;
	sptr->y = (GetRandomControl() & 0x1F) + y - 16;
	sptr->z = (GetRandomControl() & 0x1F) + z - 16;

	if (notLara)
	{
		sptr->Xvel = short((GetRandomControl() & 0x3FF) + xVel - 512);
		sptr->Yvel = short((GetRandomControl() & 0x3FF) + yVel - 512);
		sptr->Zvel = short((GetRandomControl() & 0x3FF) + zVel - 512);
	}
	else
	{
		sptr->Xvel = ((GetRandomControl() & 0x1FF) - 256) >> 1;
		sptr->Yvel = ((GetRandomControl() & 0x1FF) - 256) >> 1;
		sptr->Zvel = ((GetRandomControl() & 0x1FF) - 256) >> 1;
	}

	sptr->Friction = 4;

	if (GetRandomControl() & 1)
	{
		if (room[lara_item->room_number].flags & ROOM_NOT_INSIDE)
			sptr->Flags = 272;

		else
			sptr->Flags = 16;

		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else if (room[lara_item->room_number].flags & ROOM_NOT_INSIDE)
		sptr->Flags = 256;
	else
		sptr->Flags = 0;

	sptr->Gravity = -2 - (GetRandomControl() & 1);
	sptr->MaxYvel = -2 - (GetRandomControl() & 1);
	size = (GetRandomControl() & 0xF) - (weaponType != WEAPON_GRENADE ? 24 : 0) + 48;

	if (notLara)
	{
		sptr->Size = size >> 1;
		sptr->sSize = size >> 1;
		sptr->dSize = (size + 4)<<1;
	}
	else
	{
		sptr->sSize = size >> 2;
		sptr->Size = size >> 2;
		sptr->dSize = size;
	}

	sptr->mirror = gfLevelFlags & GF_MIRROR && lara_item->room_number == gfMirrorRoom;
}

void LaraBubbles(ITEM_INFO* item)
{
	PHD_VECTOR pos;

	SoundEffect(SFX_LARA_BUBBLES, &item->pos, SFX_WATER);
	pos.x = 0;
	pos.y = -4;
	pos.z = 64;
	GetLaraJointPos(&pos, 8);

	for (int i = (GetRandomControl() & 1) + 2; i > 0; i--)
	{
		CreateBubble((PHD_3DPOS*)&pos, item->room_number, 8, 7, 0, 0, 0, 0);

		if (gfLevelFlags & GF_MIRROR  && item->room_number == gfMirrorRoom)
		{
			pos.z = 2 * gfMirrorZPlane - pos.z;
			CreateBubble((PHD_3DPOS*)&pos, item->room_number, 8, 7, 0, 0, 0, 0);
			pos.z = 2 * gfMirrorZPlane - pos.z;
		}
	}
}

void UpdateDrips()
{
	DRIP_STRUCT* drip;
	FLOOR_INFO* floor;
	long h;

	for (int i = 0; i < 32; i++)
	{
		drip = &Drips[i];

		if (!drip->On)
			continue;

		drip->Life--;

		if (!drip->Life)
		{
			drip->On = 0;
			continue;
		}

		if (drip->Life < 16)
		{
			drip->R -= drip->R >> 3;
			drip->G -= drip->G >> 3;
			drip->B -= drip->B >> 3;
		}

		drip->Yvel += drip->Gravity;

		if (room[drip->RoomNumber].flags & ROOM_NOT_INSIDE)
		{
			drip->x += SmokeWindX >> 1;
			drip->z += SmokeWindZ >> 1;
		}

		drip->y += drip->Yvel >> 5;
		floor = GetFloor(drip->x, drip->y, drip->z, &drip->RoomNumber);
		h = GetHeight(floor, drip->x, drip->y, drip->z);

		if (room[drip->RoomNumber].flags & ROOM_UNDERWATER || drip->y > h)
			drip->On = 0;
	}
}

void inject_tomb4fx(bool replace)
{
	INJECT(0x0043AE50, TriggerLightning, replace);
	INJECT(0x0043A690, ExplodingDeath2, replace);
	INJECT(0x004395B0, DrawGunshells, replace);
	INJECT(0x00438940, TriggerGunSmoke, replace);
	INJECT(0x004398B0, LaraBubbles, replace);
	INJECT(0x00439F80, UpdateDrips, replace);
}
