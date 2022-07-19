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
#include "../specific/specificfx.h"
#include "effect2.h"

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
				fx->pos.x_pos = item->pos.x_pos + (phd_mxptr[M03] >> W2V_SHIFT);
				fx->pos.y_pos = item->pos.y_pos + (phd_mxptr[M13] >> W2V_SHIFT);
				fx->pos.z_pos = item->pos.z_pos + (phd_mxptr[M23] >> W2V_SHIFT);
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
				fx->pos.x_pos = item->pos.x_pos + (phd_mxptr[M03] >> W2V_SHIFT);
				fx->pos.y_pos = item->pos.y_pos + (phd_mxptr[M13] >> W2V_SHIFT);
				fx->pos.z_pos = item->pos.z_pos + (phd_mxptr[M23] >> W2V_SHIFT);
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

long GetFreeFireSpark()
{
	FIRE_SPARKS* sptr;
	long min_life, min_life_num;

	sptr = &fire_spark[next_fire_spark];
	min_life = 4095;
	min_life_num = 0;

	for (int free = next_fire_spark, i = 0; i < 20; i++)
	{
		if (sptr->On)
		{
			if (sptr->Life < min_life)
			{
				min_life_num = free;
				min_life = sptr->Life;
			}

			if (free == 19)
			{
				sptr = &fire_spark[1];
				free = 1;
			}
			else
			{
				free++;
				sptr++;
			}
		}
		else
		{
			next_fire_spark = free + 1;

			if (next_fire_spark >= 20)
				next_fire_spark = 1;

			return free;
		}
	}

	next_fire_spark = min_life_num + 1;

	if (next_fire_spark >= 20)
		next_fire_spark = 1;

	return min_life_num;
}

void TriggerGlobalStaticFlame()
{
	FIRE_SPARKS* sptr;

	sptr = &fire_spark[0];
	sptr->On = 1;
	sptr->dR = (GetRandomControl() & 0x3F) - 64;
	sptr->dG = (GetRandomControl() & 0x3F) + 96;
	sptr->dB = 64;
	sptr->sR = sptr->dR;
	sptr->sG = sptr->dG;
	sptr->sB = sptr->dB;
	sptr->ColFadeSpeed = 1;
	sptr->FadeToBlack = 0;
	sptr->Life = 8;
	sptr->sLife = 8;
	sptr->x = (GetRandomControl() & 7) - 4;
	sptr->y = 0;
	sptr->z = (GetRandomControl() & 7) - 4;
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;
	sptr->Friction = 0;
	sptr->Xvel = 0;
	sptr->Yvel = 0;
	sptr->Zvel = 0;
	sptr->Flags = 0;
	sptr->dSize = (GetRandomControl() & 0x1F) + 0x80;
	sptr->sSize = sptr->dSize;
	sptr->Size = sptr->dSize;
}

void TriggerGlobalFireFlame()
{
	FIRE_SPARKS* sptr;

	sptr = &fire_spark[GetFreeFireSpark()];
	sptr->On = 1;
	sptr->sR = 255;
	sptr->sG = (GetRandomControl() & 0x1F) + 48;
	sptr->sB = 48;
	sptr->dR = (GetRandomControl() & 0x3F) + 192;
	sptr->dG = (GetRandomControl() & 0x3F) + 128;
	sptr->dB = 32;
	sptr->FadeToBlack = 8;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 8;
	sptr->Life = (GetRandomControl() & 7) + 32;
	sptr->sLife = sptr->Life;
	sptr->x = 4 * (GetRandomControl() & 0x1F) - 64;
	sptr->y = 0;
	sptr->z = 4 * (GetRandomControl() & 0x1F) - 64;
	sptr->Xvel = 2 * (GetRandomControl() & 0xFF) - 256;
	sptr->Yvel = -16 - (GetRandomControl() & 0xF);
	sptr->Zvel = 2 * (GetRandomControl() & 0xFF) - 256;
	sptr->Friction = 5;
	sptr->Gravity = -32 - (GetRandomControl() & 0x1F);
	sptr->MaxYvel = -16 - (GetRandomControl() & 7);

	if (GetRandomControl() & 1)
	{
		sptr->Flags = 16;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else
		sptr->Flags = 0;

	sptr->Size = (GetRandomControl() & 0x1F) + 128;
	sptr->sSize = sptr->Size;
	sptr->dSize = sptr->Size >> 4;
}

void keep_those_fires_burning()
{
	FIRE_SPARKS* sptr;

	TriggerGlobalStaticFlame();

	if (!(wibble & 0xF))
	{
		TriggerGlobalFireFlame();

		if (!(wibble & 0x1F))
		{
			sptr = &fire_spark[GetFreeFireSpark()];
			sptr->On = 1;
			sptr->sR = 0;
			sptr->sG = 0;
			sptr->sB = 0;
			sptr->dR = 32;
			sptr->dG = 32;
			sptr->dB = 32;
			sptr->FadeToBlack = 16;
			sptr->ColFadeSpeed = (GetRandomControl() & 7) + 32;
			sptr->Life = (GetRandomControl() & 0xF) + 57;
			sptr->sLife = sptr->Life;
			sptr->x = (GetRandomControl() & 0xF) - 8;
			sptr->y = -256 - (GetRandomControl() & 0x7F);
			sptr->z = (GetRandomControl() & 0xF) - 8;
			sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
			sptr->Yvel = -16 - (GetRandomControl() & 0xF);
			sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
			sptr->Friction = 4;
			
			if (GetRandomControl() & 1)
			{
				sptr->Flags = 16;
				sptr->RotAng = GetRandomControl() & 0xFFF;

				if (GetRandomControl() & 1)
					sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
				else
					sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
			}
			else
				sptr->Flags = 0;

			sptr->Gravity = -16 - (GetRandomControl() & 0xF);
			sptr->MaxYvel = -8 - (GetRandomControl() & 7);
			sptr->dSize = (GetRandomControl() & 0x7F) + 128;
			sptr->sSize = sptr->dSize >> 2;
			sptr->Size = sptr->dSize >> 2;
		}
	}
}

void UpdateFireSparks()
{
	FIRE_SPARKS* sptr;
	long fade;

	keep_those_fires_burning();

	for (int i = 0; i < 20; i++)
	{
		sptr = &fire_spark[i];

		if (!sptr->On)
			continue;

		sptr->Life--;

		if (!sptr->Life)
		{
			sptr->On = 0;
			continue;
		}

		if (sptr->sLife - sptr->Life < sptr->ColFadeSpeed)
		{
			fade = ((sptr->sLife - sptr->Life) << 16) / sptr->ColFadeSpeed;
			sptr->R = sptr->sR + uchar((fade * (sptr->dR - sptr->sR)) >> 16);
			sptr->G = sptr->sG + uchar((fade * (sptr->dG - sptr->sG)) >> 16);
			sptr->B = sptr->sB + uchar((fade * (sptr->dB - sptr->sB)) >> 16);
		}
		else if (sptr->Life < sptr->FadeToBlack)
		{
			fade = ((sptr->Life - sptr->FadeToBlack) << 16) / sptr->FadeToBlack + 0x10000;
			sptr->R = uchar((fade * sptr->dR) >> 16);
			sptr->G = uchar((fade * sptr->dG) >> 16);
			sptr->B = uchar((fade * sptr->dB) >> 16);

			if (sptr->R < 8 && sptr->G < 8 && sptr->B < 8)
			{
				sptr->On = 0;
				continue;
			}
		}
		else
		{
			sptr->R = sptr->dR;
			sptr->G = sptr->dG;
			sptr->B = sptr->dB;
		}

		if (sptr->Flags & 0x10)
			sptr->RotAng = (sptr->RotAng + sptr->RotAdd) & 0xFFF;

		if (sptr->R < 24 && sptr->G < 24 && sptr->B < 24)
			sptr->Def = uchar(objects[DEFAULT_SPRITES].mesh_index + 2);
		else if (sptr->R < 80 && sptr->G < 80 && sptr->B < 80)
			sptr->Def = uchar(objects[DEFAULT_SPRITES].mesh_index + 1);
		else
			sptr->Def = (uchar)objects[DEFAULT_SPRITES].mesh_index;

		fade = ((sptr->sLife - sptr->Life) << 16) / sptr->sLife;
		sptr->Yvel += sptr->Gravity;

		if (sptr->MaxYvel)
		{
			if (sptr->Yvel < 0 && sptr->Yvel < sptr->MaxYvel << 5 || sptr->Yvel > 0 && sptr->Yvel > sptr->MaxYvel << 5)
				sptr->Yvel = sptr->MaxYvel << 5;
		}

		if (sptr->Friction)
		{
			sptr->Xvel -= sptr->Xvel >> sptr->Friction;
			sptr->Zvel -= sptr->Zvel >> sptr->Friction;
		}

		sptr->x += sptr->Xvel >> 5;
		sptr->y += sptr->Yvel >> 5;
		sptr->z += sptr->Zvel >> 5;
		sptr->Size = sptr->sSize + uchar((fade * (sptr->dSize - sptr->sSize)) >> 16);
	}
}

void ClearFires()
{
	FIRE_LIST* fire;

	for (int i = 0; i < 32; i++)
	{
		fire = &fires[i];
		fire->on = 0;
	}
}

void AddFire(long x, long y, long z, long size, short room_number, short fade)
{
	FIRE_LIST* fire;

	for (int i = 0; i < 32; i++)
	{
		fire = &fires[i];

		if (fire->on)
			continue;

		if (fade)
			fire->on = (char)fade;
		else
			fire->on = 1;

		fire->x = x;
		fire->y = y;
		fire->z = z;
		fire->size = (char)size;
		fire->room_number = room_number;
		break;
	}
}

void S_DrawFires()
{
	FIRE_LIST* fire;
	ROOM_INFO* r;
	short* bounds;
	short size;

	bounds = (short*)&scratchpad[0];

	for (int i = 0; i < 32; i++)
	{
		fire = &fires[i];

		if (!fire->on)
			continue;

		if (fire->size == 2)
			size = 256;
		else
			size = 384;

		bounds[0] = -size;
		bounds[1] = size;
		bounds[2] = -size * 6;
		bounds[3] = size;
		bounds[4] = -size;
		bounds[5] = size;

		r = &room[fire->room_number];
		phd_left = r->left;
		phd_right = r->right;
		phd_top = r->top;
		phd_bottom = r->bottom;

		phd_PushMatrix();
		phd_TranslateAbs(fire->x, fire->y, fire->z);

		if (S_GetObjectBounds(bounds))
		{
			if (fire->on == 1)
				S_DrawFireSparks((uchar)fire->size, 255);
			else
				S_DrawFireSparks((uchar)fire->size, fire->on & 0xFF);
		}

		phd_PopMatrix();
	}

	phd_top = 0;
	phd_left = 0;
	phd_right = phd_winwidth;
	phd_bottom = phd_winheight;
}

long GetFreeSmokeSpark()
{
	SMOKE_SPARKS* sptr;
	long min_life, min_life_num;

	sptr = &smoke_spark[next_smoke_spark];
	min_life = 4095;
	min_life_num = 0;

	for (int free = next_smoke_spark, i = 0; i < 32; i++)
	{
		if (sptr->On)
		{
			if (sptr->Life < min_life)
			{
				min_life_num = free;
				min_life = sptr->Life;
			}

			if (free == 31)
			{
				sptr = &smoke_spark[0];
				free = 0;
			}
			else
			{
				free++;
				sptr++;
			}
		}
		else
		{
			next_smoke_spark = (free + 1) & 0x1F;
			return free;
		}
	}

	next_smoke_spark = (min_life_num + 1) & 0x1F;
	return min_life_num;
}

void UpdateSmokeSparks()
{
	SMOKE_SPARKS* sptr;
	long fade;

	for (int i = 0; i < 32; i++)
	{
		sptr = &smoke_spark[i];

		if (!sptr->On)
			continue;

		sptr->Life -= 2;

		if (sptr->Life <= 0)
		{
			sptr->On = 0;
			continue;
		}

		if (sptr->sLife - sptr->Life < sptr->ColFadeSpeed)
		{
			fade = ((sptr->sLife - sptr->Life) << 16) / sptr->ColFadeSpeed;
			sptr->Shade = sptr->sShade + uchar(((sptr->dShade - sptr->sShade) * fade) >> 16);
		}
		else if (sptr->Life < sptr->FadeToBlack)
		{
			fade = ((sptr->Life - sptr->FadeToBlack) << 16) / sptr->FadeToBlack + 0x10000;
			sptr->Shade = uchar((sptr->dShade * fade) >> 16);

			if (sptr->Shade < 8)
			{
				sptr->On = 0;
				continue;
			}
		}
		else
			sptr->Shade = sptr->dShade;

		if (sptr->Shade < 24)
			sptr->Def = uchar(objects[DEFAULT_SPRITES].mesh_index + 2);
		else if (sptr->Shade < 80)
			sptr->Def = uchar(objects[DEFAULT_SPRITES].mesh_index + 1);
		else
			sptr->Def = (uchar)objects[DEFAULT_SPRITES].mesh_index;

		if (sptr->Flags & 0x10)
			sptr->RotAng = (sptr->RotAng + sptr->RotAdd) & 0xFFF;

		fade = ((sptr->sLife - sptr->Life) << 16) / sptr->sLife;
		sptr->Yvel += sptr->Gravity;

		if (sptr->MaxYvel)
		{
			if (sptr->Yvel < 0 && sptr->Yvel < sptr->MaxYvel << 5 || sptr->Yvel > 0 && sptr->Yvel > sptr->MaxYvel << 5)
				sptr->Yvel = sptr->MaxYvel << 5;
		}

		if (sptr->Friction & 0xF)
		{
			sptr->Xvel -= sptr->Xvel >> (sptr->Friction & 0xF);
			sptr->Zvel -= sptr->Zvel >> (sptr->Friction & 0xF);
		}

		if (sptr->Friction & 0xF0)
			sptr->Yvel -= sptr->Yvel >> (sptr->Friction >> 4);

		sptr->x += sptr->Xvel >> 5;
		sptr->y += sptr->Yvel >> 5;
		sptr->z += sptr->Zvel >> 5;

		if (sptr->Flags & 0x100)
		{
			sptr->x += SmokeWindX >> 1;
			sptr->z += SmokeWindZ >> 1;
		}

		sptr->Size = sptr->sSize + uchar((fade * (sptr->dSize - sptr->sSize)) >> 16);
	}
}

void TriggerShatterSmoke(long x, long y, long z)
{
	SMOKE_SPARKS* sptr;

	sptr = &smoke_spark[GetFreeSmokeSpark()];
	sptr->On = 1;
	sptr->sShade = 0;
	sptr->dShade = (GetRandomControl() & 0x1F) + 64;
	sptr->ColFadeSpeed = 4;
	sptr->FadeToBlack = 24 - (GetRandomControl() & 7);
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 7) + 48;
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0x1F) + x - 16;
	sptr->y = (GetRandomControl() & 0x1F) + y - 16;
	sptr->z = (GetRandomControl() & 0x1F) + z - 16;
	sptr->Xvel = 2 * (GetRandomControl() & 0x1FF) - 512;
	sptr->Yvel = 2 * (GetRandomControl() & 0x1FF) - 512;
	sptr->Zvel = 2 * (GetRandomControl() & 0x1FF) - 512;
	sptr->Friction = 7;
	
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
}

void DrawLensFlares(ITEM_INFO* item)
{
	GAME_VECTOR sun;

	sun.x = item->pos.x_pos;
	sun.y = item->pos.y_pos;
	sun.z = item->pos.z_pos;
	sun.room_number = item->room_number;
	SetUpLensFlare(0, 0, 0, &sun);
}

void DrawWeaponMissile(ITEM_INFO* item)
{
	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	phd_PutPolygons_train(meshes[objects[item->object_number].mesh_index], 0);
	phd_PopMatrix();

	if (gfLevelFlags & GF_MIRROR && item->room_number == gfMirrorRoom)
	{
		phd_PushMatrix();
		phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, 2 * gfMirrorZPlane - item->pos.z_pos);
		phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
		phd_PutPolygons_train(meshes[objects[item->object_number].mesh_index], 0);
		phd_PopMatrix();
	}
}

long GetFreeGunshell()
{
	GUNSHELL_STRUCT* shell;
	long min_life, min_life_num;

	shell = &Gunshells[next_gunshell];
	min_life = 4095;
	min_life_num = 0;

	for (int free = next_gunshell, i = 0; i < 24; i++)
	{
		if (shell->counter)
		{
			if (shell->counter < min_life)
			{
				min_life_num = free;
				min_life = shell->counter;
			}

			if (free == 23)
			{
				shell = &Gunshells[0];
				free = 0;
			}
			else
			{
				free++;
				shell++;
			}
		}
		else
		{
			next_gunshell = free + 1;

			if (next_gunshell >= 24)
				next_gunshell = 0;

			return free;
		}
	}

	next_gunshell = min_life_num + 1;

	if (next_gunshell >= 24)
		next_gunshell = 0;

	return min_life_num;
}

void TriggerGunShell(short leftright, short objnum, long weapon)
{
	GUNSHELL_STRUCT* shell;
	PHD_VECTOR pos;
	long shade;

	if (leftright)
	{
		switch (weapon)
		{
		case WEAPON_PISTOLS:
			pos.x = 8;
			pos.y = 48;
			pos.z = 40;
			break;

		case WEAPON_UZI:
			pos.x = 8;
			pos.y = 35;
			pos.z = 48;
			break;

		case WEAPON_SHOTGUN:
			pos.x = 16;
			pos.y = 114;
			pos.z = 32;
			break;
		}

		GetLaraJointPos(&pos, 11);
	}
	else
	{
		switch (weapon)
		{
		case WEAPON_PISTOLS:
			pos.x = -12;
			pos.y = 48;
			pos.z = 40;
			break;

		case WEAPON_UZI:
			pos.x = -16;
			pos.y = 35;
			pos.z = 48;
			break;
		}

		GetLaraJointPos(&pos, 14);
	}

	if (lara_item->mesh_bits)
	{
		if (weapon == WEAPON_SHOTGUN)
			shade = 24;
		else
			shade = 16;

		TriggerGunSmoke(pos.x, pos.y, pos.z, 0, 0, 0, 0, weapon, shade);
	}

	shell = &Gunshells[GetFreeGunshell()];
	shell->pos.x_pos = pos.x;
	shell->pos.y_pos = pos.y;
	shell->pos.z_pos = pos.z;
	shell->room_number = lara_item->room_number;
	shell->pos.x_rot = 0;
	shell->pos.y_rot = 0;
	shell->pos.z_rot = (short)GetRandomControl();
	shell->speed = (GetRandomControl() & 0x1F) + 16;
	shell->object_number = objnum;
	shell->fallspeed = -48 - (GetRandomControl() & 7);
	shell->counter = (GetRandomControl() & 0x1F) + 60;

	if (leftright)
	{
		if (weapon == WEAPON_SHOTGUN)
		{
			shell->DirXrot = lara.torso_y_rot + lara_item->pos.y_rot - (GetRandomControl() & 0xFFF) + lara.left_arm.y_rot + 0x2800;
			shell->pos.y_rot += lara.left_arm.y_rot + lara.torso_y_rot + lara_item->pos.y_rot;

			if (shell->speed < 24)
				shell->speed += 24;
		}
		else
			shell->DirXrot = lara_item->pos.y_rot - (GetRandomControl() & 0xFFF) + lara.left_arm.y_rot + 0x4800;
	}
	else
		shell->DirXrot = lara_item->pos.y_rot + (GetRandomControl() & 0xFFF) + lara.left_arm.y_rot - 0x4800;
}

void UpdateGunShells()
{
	GUNSHELL_STRUCT* shell;
	FLOOR_INFO* floor;
	long ox, oy, oz, c, h;
	short oroom;

	for (int i = 0; i < 24; i++)
	{
		shell = &Gunshells[i];

		if (!shell->counter)
			continue;

		ox = shell->pos.x_pos;
		oy = shell->pos.y_pos;
		oz = shell->pos.z_pos;
		oroom = shell->room_number;
		shell->counter--;

		if (room[oroom].flags & ROOM_UNDERWATER)
		{
			shell->fallspeed++;

			if (shell->fallspeed > 8)
				shell->fallspeed = 8;
			else if (shell->fallspeed < 0)
				shell->fallspeed >>= 1;

			shell->speed -= shell->speed >> 1;
		}
		else
			shell->fallspeed += 6;

		shell->pos.x_rot += 182 * ((shell->speed >> 1) + 7);
		shell->pos.y_rot += 182 * shell->speed;
		shell->pos.z_rot += 4186;
		shell->pos.x_pos += shell->speed * phd_sin(shell->DirXrot) >> (W2V_SHIFT + 1);
		shell->pos.y_pos += shell->fallspeed;
		shell->pos.z_pos += shell->speed * phd_cos(shell->DirXrot) >> (W2V_SHIFT + 1);
		floor = GetFloor(shell->pos.x_pos, shell->pos.y_pos, shell->pos.z_pos, &shell->room_number);

		if (room[shell->room_number].flags & ROOM_UNDERWATER && !(room[oroom].flags & ROOM_UNDERWATER))
		{
			TriggerSmallSplash(shell->pos.x_pos, room[shell->room_number].maxceiling, shell->pos.z_pos, 8);
		//	SetupRipple(shell->pos.x_pos, room[shell->room_number].maxceiling, shell->pos.z_pos, (GetRandomControl() & 3) + 8, 2);
			shell->fallspeed >>= 5;
			continue;
		}

		c = GetCeiling(floor, shell->pos.x_pos, shell->pos.y_pos, shell->pos.z_pos);

		if (shell->pos.y_pos < c)
		{
			SoundEffect(SFX_LARA_SHOTGUN_SHELL, &shell->pos, SFX_DEFAULT);
			shell->speed -= 4;

			if (shell->speed < 8)
			{
				shell->counter = 0;
				continue;
			}

			shell->pos.y_pos = c;
			shell->fallspeed = -shell->fallspeed;
		}

		h = GetHeight(floor, shell->pos.x_pos, shell->pos.y_pos, shell->pos.z_pos);

		if (shell->pos.y_pos >= h)
		{
			SoundEffect(SFX_LARA_SHOTGUN_SHELL, &shell->pos, SFX_DEFAULT);
			shell->speed -= 8;

			if (shell->speed < 8)
			{
				shell->counter = 0;
				continue;
			}

			if (oy <= h)
				shell->fallspeed = -shell->fallspeed >> 1;
			else
			{
				shell->DirXrot += 0x8000;
				shell->pos.x_pos = ox;
				shell->pos.z_pos = oz;
			}

			shell->pos.y_pos = oy;
		}
	}
}

void TriggerSmallSplash(long x, long y, long z, long num)
{
	SPARKS* sptr;
	short ang;

	while (num)
	{
		sptr = &spark[GetFreeSpark()];
		sptr->On = 1;
		sptr->sR = 112;
		sptr->sG = (GetRandomControl() & 0x1F) + 128;
		sptr->sB = (GetRandomControl() & 0x1F) + 128;
		sptr->dR = sptr->sR >> 1;
		sptr->dG = sptr->sG >> 1;
		sptr->dB = sptr->sB >> 1;
		sptr->ColFadeSpeed = 4;
		sptr->FadeToBlack = 8;
		sptr->Life = 24;
		sptr->sLife = 24;
		sptr->TransType = 2;
		ang = GetRandomControl() & 0xFFF;
		sptr->Xvel = -rcossin_tbl[ang << 1] >> 5;
		sptr->Yvel = -640 - (GetRandomControl() & 0xFF);
		sptr->Zvel = rcossin_tbl[(ang << 1) + 1] >> 5;
		sptr->x = x + (sptr->Xvel >> 3);
		sptr->y = y - (sptr->Yvel >> 5);
		sptr->z = z + (sptr->Zvel >> 3);
		sptr->Friction = 5;
		sptr->Flags = 0;
		sptr->MaxYvel = 0;
		sptr->Gravity = (GetRandomControl() & 0xF) + 64;
		num--;
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
	INJECT(0x00437E60, GetFreeFireSpark, replace);
	INJECT(0x00438420, TriggerGlobalStaticFlame, replace);
	INJECT(0x004382C0, TriggerGlobalFireFlame, replace);
	INJECT(0x00437EF0, keep_those_fires_burning, replace);
	INJECT(0x00437F20, UpdateFireSparks, replace);
	INJECT(0x004384F0, ClearFires, replace);
	INJECT(0x00438510, AddFire, replace);
	INJECT(0x00438560, S_DrawFires, replace);
	INJECT(0x00438690, GetFreeSmokeSpark, replace);
	INJECT(0x00438700, UpdateSmokeSparks, replace);
	INJECT(0x00438BA0, TriggerShatterSmoke, replace);
	INJECT(0x0043B5F0, DrawLensFlares, replace);
	INJECT(0x0043B630, DrawWeaponMissile, replace);
	INJECT(0x00439060, GetFreeGunshell, replace);
	INJECT(0x004390F0, TriggerGunShell, replace);
	INJECT(0x00439340, UpdateGunShells, replace);
	INJECT(0x00439660, TriggerSmallSplash, replace);
}
