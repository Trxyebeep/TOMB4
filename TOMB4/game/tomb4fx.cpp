#include "../tomb4/pch.h"
#include "tomb4fx.h"
#include "../specific/function_stubs.h"
#include "draw.h"
#include "../specific/3dmath.h"
#include "items.h"
#include "objects.h"

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

void inject_tomb4fx(bool replace)
{
	INJECT(0x0043AE50, TriggerLightning, replace);
	INJECT(0x0043A690, ExplodingDeath2, replace);
}
