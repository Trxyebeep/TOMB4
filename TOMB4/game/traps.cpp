#include "../tomb4/pch.h"
#include "traps.h"
#include "control.h"
#include "effect2.h"
#include "../specific/function_stubs.h"
#include "sound.h"
#include "tomb4fx.h"
#include "effects.h"
#include "items.h"
#include "draw.h"
#include "objects.h"
#include "../specific/3dmath.h"
#include "../specific/output.h"
#include "sphere.h"
#include "lara_states.h"
#include "collide.h"

short SPxzoffs[8] = { 0, 0, 0x200, 0, 0, 0, -0x200, 0 };
short SPyoffs[8] = { -0x400, 0, -0x200, 0, 0, 0, -0x200, 0 };
short SPDETyoffs[8] = { 0x400, 0x200, 0x200, 0x200, 0, 0x200, 0x200, 0x200 };

void FlameEmitterControl(short item_number)
{
	ITEM_INFO* item;
	ulong distance;
	long x, z;

	item = &items[item_number];

	if (!TriggerActive(item))
	{
		if (item->trigger_flags >= 0)
			LibraryTab[item->trigger_flags] = 0;

		return;
	}

	if (item->trigger_flags < 0)
	{
		if ((-item->trigger_flags & 7) == 2 || (-item->trigger_flags & 7) == 7)
		{
			SoundEffect(SFX_FLAME_EMITTER, &item->pos, 0);
			TriggerSuperJetFlame(item, -256 - (3072 * GlobalCounter & 0x1C00), GlobalCounter & 1);
			TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos,
				(GetRandomControl() & 3) + 20, (GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 96, 0);
		}
		else
		{
			if (item->item_flags[0])
			{
				if (item->item_flags[1])
					item->item_flags[1] -= item->item_flags[1] >> 2;

				if (item->item_flags[2])
					item->item_flags[2] += 8;

				item->item_flags[0]--;

				if (!item->item_flags[0])
					item->item_flags[3] = (GetRandomControl() & 0x3F) + 150;
			}
			else
			{
				item->item_flags[3]--;

				if (!item->item_flags[3])
				{
					if (-item->trigger_flags >> 3)
						item->item_flags[0] = (GetRandomControl() & 0x1F) + 30 * (-item->trigger_flags >> 3);
					else
						item->item_flags[0] = (GetRandomControl() & 0x3F) + 60;
				}

				if (item->item_flags[2])
					item->item_flags[2] -= 8;

				if (item->item_flags[1] > -8192)
					item->item_flags[1] -= 512;
			}

			if (item->item_flags[2])
				AddFire(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 0, item->room_number, item->item_flags[2] & 0xFF);

			if (item->item_flags[1])
			{
				SoundEffect(SFX_FLAME_EMITTER, &item->pos, 0);

				if (item->item_flags[1] > -8192)
					TriggerSuperJetFlame(item, item->item_flags[1], GlobalCounter & 1);
				else
					TriggerSuperJetFlame(item, -256 - (3072 * GlobalCounter & 0x1C00), GlobalCounter & 1);

				TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, (-item->item_flags[1] >> 10) - (GetRandomControl() & 1) + 16, (GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 96, 0);
			}
			else
				TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 10 - (GetRandomControl() & 1), (GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 96, 0);
		}

		SoundEffect(SFX_LOOP_FOR_SMALL_FIRES, &item->pos, 0);
	}
	else
	{
		LibraryTab[item->trigger_flags] = 1;
		AddFire(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 2, item->room_number, 0);
		TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 16 - (GetRandomControl() & 1),
			(GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 96, 0);
		SoundEffect(SFX_LOOP_FOR_SMALL_FIRES, &item->pos, SFX_DEFAULT);

		if (!lara.burn && ItemNearLara(&item->pos, 600))
		{
			x = lara_item->pos.x_pos - item->pos.x_pos;
			z = lara_item->pos.z_pos - item->pos.z_pos;
			distance = SQUARE(x) + SQUARE(z);

			if (distance < 0x40000)
				LaraBurn();
		}
	}
}

static long OnTwoBlockPlatform(ITEM_INFO* item, long x, long z)
{
	long tx, tz;

	if (!item->mesh_bits)
		return 0;

	x >>= 10;
	z >>= 10;
	tx = item->pos.x_pos >> 10;
	tz = item->pos.z_pos >> 10;

	if (!item->pos.y_rot && (x == tx || x == tx - 1) && (z == tz || z == tz + 1))
		return 1;

	if (item->pos.y_rot == 0x8000 && (x == tx || x == tx + 1) && (z == tz || z == tz - 1))
		return 1;

	if (item->pos.y_rot == 0x4000 && (z == tz || z == tz - 1) && (x == tx || x == tx + 1))
		return 1;

	if (item->pos.y_rot == -0x4000 && (z == tz || z == tz - 1) && (x == tx || x == tx - 1))
		return 1;

	return 0;
}

void TwoBlockPlatformFloor(ITEM_INFO* item, long x, long y, long z, long* height)
{
	if (OnTwoBlockPlatform(item, x, z))
	{
		if (y <= item->pos.y_pos + 32 && item->pos.y_pos < *height)
		{
			*height = item->pos.y_pos;
			OnObject = 1;
			height_type = 0;
		}
	}
}

void TwoBlockPlatformCeiling(ITEM_INFO* item, long x, long y, long z, long* height)
{
	if (OnTwoBlockPlatform(item, x, z))
	{
		if (y > item->pos.y_pos + 32 && item->pos.y_pos > *height)
			*height = item->pos.y_pos + 256;
	}
}

void ControlTwoBlockPlatform(short item_number)
{
	ITEM_INFO* item;
	long height;
	short room_number;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	if (item->trigger_flags)
	{
		if (item->pos.y_pos > item->item_flags[0] - (item->trigger_flags << 4))
			item->pos.y_pos -= item->trigger_flags & 0xF;

		room_number = item->room_number;
		item->floor = GetHeight(GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number),
			item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

		if (room_number != item->room_number)
			ItemNewRoom(item_number, room_number);
	}
	else
	{
		OnObject = 0;
		height = lara_item->pos.y_pos + 1;
		TwoBlockPlatformFloor(item, lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, &height);

		if (!OnObject || lara_item->anim_number == 89)
			item->item_flags[1] = -1;
		else
			item->item_flags[1] = 1;

		if (item->item_flags[1] > 0)
		{
			if (item->pos.y_pos >= item->item_flags[0] + 128)
				item->item_flags[1] = -1;
			else
			{
				SoundEffect(SFX_RUMBLE_NEXTDOOR, &item->pos, SFX_DEFAULT);
				item->pos.y_pos += 4;
			}
		}
		else if (item->item_flags[1] < 0)
		{
			if (item->pos.y_pos <= item->item_flags[0])
				item->item_flags[1] = 1;
			else
			{
				SoundEffect(SFX_RUMBLE_NEXTDOOR, &item->pos, SFX_DEFAULT);
				item->pos.y_pos -= 4;
			}
		}
	}
}

void ControlJobySpike(short item_number)
{
	ITEM_INFO* item;
	short* frm[2];
	long rate, y, h;

	item = &items[item_number];

	if (TriggerActive(item))
	{
		SoundEffect(SFX_METAL_SCRAPE_LOOP, &item->pos, SFX_DEFAULT);
		GetFrames(lara_item, frm, &rate);
		y = lara_item->pos.y_pos + frm[0][2];
		h = item->pos.y_pos + (3328 * item->item_flags[1] >> 12);

		if (lara_item->hit_points > 0 && h > y && ABS(item->pos.x_pos - lara_item->pos.x_pos) < 512 && ABS(item->pos.z_pos - lara_item->pos.z_pos) < 512)
		{
			DoBloodSplat(lara_item->pos.x_pos + (GetRandomControl() & 0x7F) - 64, GetRandomControl() % (h - y) + y, lara_item->pos.z_pos + (GetRandomControl() & 0x7F) - 64, (GetRandomControl() & 3) + 2, (short)(2 * GetRandomControl()), item->room_number);
			lara_item->hit_points -= 8;
		}

		if (!item->item_flags[2])
		{
			if (item->item_flags[0] < 4096)
				item->item_flags[0] += (item->item_flags[0] >> 6) + 2;
		}
		else if (item->item_flags[0] > -4096)
			item->item_flags[0] += (item->item_flags[0] >> 6) - 2;

		if (item->item_flags[1] < item->item_flags[3])
			item->item_flags[1] += 3;

		item->pos.y_rot += item->item_flags[0];
	}
}

void DrawScaledSpike(ITEM_INFO* item)
{
	PHD_VECTOR scale;
	ROOM_INFO* r;
	short** meshpp;
	short* frm[2];
	long rate, clip, lp;

	if (item->object_number == TEETH_SPIKES || item->item_flags[1])
	{
		if ((item->object_number == RAISING_BLOCK1 || item->object_number == RAISING_BLOCK2) && item->trigger_flags && !item->item_flags[0])
		{
			for (lp = 1; lp < 8; lp++)
			{
				if (!LibraryTab[lp])
					break;
			}

			if (lp == 8)
			{
				item->item_flags[0] = 1;
				item->touch_bits = 0;
				AddActiveItem(item - items);
				item->flags |= IFL_CODEBITS;
				item->status = ITEM_ACTIVE;
			}
		}

		r = &room[item->room_number];
		phd_left = r->left;
		phd_right = r->right;
		phd_top = r->top;
		phd_bottom = r->bottom;
		GetFrames(item, frm, &rate);
		phd_PushMatrix();
		phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
		phd_RotX(item->pos.x_rot);
		phd_RotZ(item->pos.z_rot);
		phd_RotY(item->pos.y_rot);
		clip = S_GetObjectBounds(frm[0]);

		if (clip)
		{
			meshpp = &meshes[objects[item->object_number].mesh_index];

			if (item->object_number == EXPANDING_PLATFORM)
			{
				scale.x = 16384;
				scale.y = 16384;
				scale.z = 4 * item->item_flags[1];
			}
			else
			{
				scale.y = 4 * item->item_flags[1];

				if (item->object_number != JOBY_SPIKES)
				{
					scale.x = 16384;
					scale.z = 16384;
				}
				else
				{
					scale.x = 12288;
					scale.z = 12288;
				}
			}

			ScaleCurrentMatrix(&scale);
			CalculateObjectLighting(item, frm[0]);
			phd_PutPolygons(*meshpp, clip);
		}

		phd_left = 0;
		phd_right = phd_winwidth;
		phd_top = 0;
		phd_bottom = phd_winheight;
		phd_PopMatrix();
	}
}

void ControlSlicerDicer(short item_number)
{
	ITEM_INFO* item;
	long distance;
	short room_number;

	item = &items[item_number];
	SoundEffect(SFX_METAL_SCRAPE_LOOP, &item->pos, SFX_DEFAULT);
	SoundEffect(SFX_METAL_SCRAPE_LOOP1, &item->pos, SFX_DEFAULT);
	distance = 4608 * phd_cos(item->trigger_flags) >> 14;
	item->pos.x_pos = 256 * item->item_flags[0] + (phd_sin(item->pos.y_rot) * distance >> 14);
	item->pos.y_pos = 256 * item->item_flags[1] - (4608 * phd_sin(item->trigger_flags) >> 14);
	item->pos.z_pos = 256 * item->item_flags[2] + (phd_cos(item->pos.y_rot) * distance >> 14);
	item->trigger_flags += 170;
	room_number = item->room_number;
	GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

	if (item->room_number != room_number)
		ItemNewRoom(item_number, room_number);

	AnimateItem(item);
}

void ControlSprinkler(short item_number)
{
	ITEM_INFO* item;
	DRIP_STRUCT* drip;
	SPARKS* sptr;
	SMOKE_SPARKS* smokeptr;
	long vel, size;

	item = &items[item_number];

	if (item->item_flags[0] < 1200)
	{
		item->item_flags[0]++;

		if (!(wibble & 0xF) && (item->item_flags[0] <= 600 || GetRandomControl() % (item->item_flags[0] - 600) < 100))
		{
			drip = &Drips[GetFreeDrip()];
			drip->x = (GetRandomControl() & 0x1F) + item->pos.x_pos - 16;
			drip->y = (GetRandomControl() & 0x1F) + item->pos.y_pos - 944;
			drip->z = (GetRandomControl() & 0x1F) + item->pos.z_pos - 16;
			drip->On = 1;
			drip->R = 112;
			drip->G = (GetRandomControl() & 0x1F) + 128;
			drip->B = (GetRandomControl() & 0x1F) + 128;
			drip->Yvel = (GetRandomControl() & 0xF) + 16;
			drip->Gravity = (GetRandomControl() & 0x1F) + 32;
			drip->Life = (GetRandomControl() & 0x1F) + 16;
			drip->RoomNumber = item->room_number;
		}
	}

	if (item->item_flags[0] <= 600)
	{
		SoundEffect(SFX_SANDHAM_IN_THE_HOUSE, &item->pos, SFX_DEFAULT);

		for (int i = 0; i < 3; i++)
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
			sptr->Life = 20;
			sptr->sLife = 20;
			sptr->TransType = 2;
			vel = ((GlobalCounter & 3) << 10) + (GetRandomControl() & 0x3FF);
			sptr->Xvel = -rcossin_tbl[vel << 1] >> 2;
			sptr->Xvel = (sptr->Xvel * (GetRandomControl() & 0xFF)) >> 8;
			sptr->Yvel = -32 - (GetRandomControl() & 0x1F);
			sptr->Zvel = rcossin_tbl[(vel << 1) + 1] >> 2;
			sptr->Zvel = (sptr->Zvel * (GetRandomControl() & 0xFF)) >> 8;
			sptr->x = item->pos.x_pos + (sptr->Xvel >> 3);
			sptr->y = (sptr->Yvel >> 5) + item->pos.y_pos - 928;
			sptr->z = item->pos.z_pos + (sptr->Zvel >> 3);
			sptr->Friction = 4;
			sptr->Flags = GetRandomControl() & 0x20;
			sptr->Gravity = (GetRandomControl() & 0x3F) + 64;
			sptr->MaxYvel = 0;
		}

		for (int i = 0; i < 1; i++)
		{
			smokeptr = &smoke_spark[GetFreeSmokeSpark()];
			smokeptr->On = 1;
			smokeptr->sShade = 0;
			smokeptr->dShade = (GetRandomControl() & 0x1F) + 32;
			smokeptr->ColFadeSpeed = 4;
			smokeptr->FadeToBlack = 8 - (GetRandomControl() & 3);
			smokeptr->TransType = 2;
			smokeptr->Life = (GetRandomControl() & 3) + 24;
			smokeptr->sLife = smokeptr->Life;
			smokeptr->x = (GetRandomControl() & 0x1F) + item->pos.x_pos - 16;
			smokeptr->y = (GetRandomControl() & 0x1F) + item->pos.y_pos - 944;
			smokeptr->z = (GetRandomControl() & 0x1F) + item->pos.z_pos - 16;
			smokeptr->Xvel = 2 * (GetRandomControl() & 0x1FF) - 512;

			if (i)
				smokeptr->Yvel = (GetRandomControl() & 0x1F) - 16;
			else
				smokeptr->Yvel = 2 * (GetRandomControl() & 0x1FF) + 512;

			smokeptr->Zvel = 2 * (GetRandomControl() & 0x1FF) - 512;
			smokeptr->Friction = 3;
			smokeptr->Flags = 16;
			smokeptr->RotAng = GetRandomControl() & 0xFFF;

			if (GetRandomControl() & 1)
				smokeptr->RotAdd = -64 - (GetRandomControl() & 0x3F);
			else
				smokeptr->RotAdd = (GetRandomControl() & 0x3F) + 64;

			smokeptr->MaxYvel = 0;
			smokeptr->Gravity = -4 - (GetRandomControl() & 3);
			size = (GetRandomControl() & 0xF) + 16;

			if (!i)
				size <<= 2;

			smokeptr->dSize = (uchar)size;
			smokeptr->sSize = smokeptr->dSize >> 3;
			smokeptr->Size = smokeptr->dSize >> 3;
		}
	}
}

void ControlMineHelicopter(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* sentry;
	SPHERE* sphere;
	long nSpheres;
	short sentries, fade;

	item = &items[item_number];
	nSpheres = GetSpheres(item, Slist, 1);

	if (item->item_flags[0] < 150)
	{
		item->item_flags[0]++;
		fade = item->item_flags[0] * 4;

		if (fade > 255)
			fade = 0;

		for (int i = 0; i < nSpheres; i++)
		{
			sphere = &Slist[i];

			if (!i || i > 5)
				AddFire(sphere->x, sphere->y, sphere->z, 2, item->room_number, fade);
		}

		SoundEffect(SFX_LOOP_FOR_SMALL_FIRES, &item->pos, SFX_DEFAULT);
	}
	else
	{
		SoundEffect(SFX_EXPLOSION1, &item->pos, SFX_DEFAULT);
		SoundEffect(SFX_EXPLOSION2, &item->pos, SFX_DEFAULT);
		SoundEffect(SFX_EXPLOSION1, &item->pos, 0x1800000 | SFX_SETPITCH);

		for (int i = 0; i < nSpheres; i++)
		{
			sphere = &Slist[i];

			if (i >= 7 && i != 9)
			{
				TriggerExplosionSparks(sphere->x, sphere->y, sphere->y, 3, -2, 0, -item->room_number);
				TriggerExplosionSparks(sphere->x, sphere->y, sphere->y, 3, -1, 0, -item->room_number);
				TriggerShockwave((PHD_VECTOR*)&sphere->x, 0x1300030, (GetRandomControl() & 0x1F) + 112, 0x20806000, 0x800);
			}
		}

		for (int i = 0; i < nSpheres; i++)
			ExplodeItemNode(item, i, 0, -128);

		FlashFadeR = 255;
		FlashFadeG = 192;
		FlashFadeB = 64;
		FlashFader = 32;

		for (sentries = room[item->room_number].item_number; sentries != NO_ITEM; sentries = sentry->next_item)
		{
			sentry = &items[sentries];

			if (sentry->object_number == SENTRY_GUN)
				sentry->mesh_bits &= ~0x40;
		}

		KillItem(item_number);
	}
}

void MineCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	ITEM_INFO* mines;

	item = &items[item_number];

	if (item->trigger_flags || item->item_flags[3])
		return;

	if (l->anim_number == ANIM_MINEDETECT && l->frame_number >= anims[ANIM_MINEDETECT].frame_base + 57)
	{
		for (int i = 0; i < level_items; i++)
		{
			mines = &items[i];

			if (mines->object_number != MINE || mines->status == ITEM_INVISIBLE || mines->trigger_flags)
				continue;

			TriggerExplosionSparks(mines->pos.x_pos, mines->pos.y_pos, mines->pos.z_pos, 3, -2, 0, mines->room_number);

			for (int j = 0; j < 2; j++)
				TriggerExplosionSparks(mines->pos.x_pos, mines->pos.y_pos, mines->pos.z_pos, 3, -1, 0, mines->room_number);

			mines->mesh_bits = 1;
			ExplodeItemNode(mines, 0, 0, -32);
			KillItem(i);

			if (!(GetRandomControl() & 3))
				SoundEffect(SFX_MINE_EXP_OVERLAY, &mines->pos, SFX_DEFAULT);

			mines->status = ITEM_INVISIBLE;
		}
	}
	else if (TestBoundsCollide(item, l, 512))
	{
		TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 3, -2, 0, item->room_number);

		for (int i = 0; i < 2; i++)
			TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 3, -1, 0, item->room_number);

		item->mesh_bits = 1;
		ExplodeItemNode(item, 0, 0, 128);
		KillItem(item_number);
		l->anim_number = ANIM_MINEDEATH;
		l->frame_number = anims[ANIM_MINEDEATH].frame_base;
		l->current_anim_state = AS_DEATH;
		l->speed = 0;
		SoundEffect(SFX_MINE_EXP_OVERLAY, &item->pos, SFX_DEFAULT);
	}
}

void FallingSquishyBlockCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO * item;

	item = &items[item_number];

	if (TestBoundsCollide(item, l, coll->radius) && TestCollision(item, l))
	{
		if (item->frame_number - anims[item->anim_number].frame_base <= 8)
		{
			item->frame_number += 2;
			l->hit_points = 0;
			l->current_anim_state = AS_DEATH;
			l->goal_anim_state = AS_DEATH;
			l->anim_number = ANIM_FBLOCK_DEATH;
			l->frame_number = anims[ANIM_FBLOCK_DEATH].frame_base + 50;
			l->fallspeed = 0;
			l->speed = 0;

			for (int i = 0; i < 12; i++)
				TriggerBlood(l->pos.x_pos, l->pos.y_pos - 128, l->pos.z_pos, GetRandomControl() << 1, 3);
		}
		else if (l->hit_points > 0)
			ItemPushLara(item, l, coll, 0, 1);
	}
}

void ControlFallingSquishyBlock(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (TriggerActive(item))
	{
		if (item->item_flags[0] < 60)
		{
			SoundEffect(SFX_EARTHQUAKE_LOOP, &item->pos, SFX_DEFAULT);
			camera.bounce = (item->item_flags[0] - 92) >> 1;
			item->item_flags[0]++;
		}
		else
		{
			if (item->frame_number - anims[item->anim_number].frame_base == 8)
				camera.bounce = -96;

			AnimateItem(item);
		}
	}
}

void ControlLRSquishyBlock(short item_number)
{
	ITEM_INFO* item;
	ushort ang;
	short frame;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	frame = item->frame_number - anims[item->anim_number].frame_base;

	if (item->touch_bits)
	{
		ang = (ushort)phd_atan(item->pos.z_pos - lara_item->pos.z_pos, item->pos.x_pos - lara_item->pos.x_pos) - item->pos.y_rot;

		if (!frame && ang > 0xA000 && ang < 0xE000)
		{
			item->item_flags[0] = 9;
			lara_item->hit_points = 0;
			lara_item->pos.y_rot = item->pos.y_rot - 0x4000;
		}
		else if (frame == 33 && ang > 0x2000 && ang < 0x6000)
		{
			item->item_flags[0] = 42;
			lara_item->hit_points = 0;
			lara_item->pos.y_rot = item->pos.y_rot + 0x4000;
		}
	}

	if (!item->item_flags[0] || frame != item->item_flags[0])
		AnimateItem(item);
}

void ControlSethBlade(short item_number)
{
	ITEM_INFO* item;
	short frame;

	item = &items[item_number];
	*(long*)&item->item_flags[0] = 0;

	if (!TriggerActive(item))
		return;

	if (item->current_anim_state == 2)
	{
		if (item->item_flags[2] > 1)
			item->item_flags[2]--;
		else if (item->item_flags[2] == 1)
		{
			item->goal_anim_state = 1;
			item->item_flags[2] = 0;
		}
		else if (!item->item_flags[2] && item->trigger_flags > 0)
			item->item_flags[2] = item->trigger_flags;
	}
	else
	{
		frame = item->frame_number - anims[item->anim_number].frame_base;

		if (frame && frame <= 6)
			*(long*)&item->item_flags[0] = -1;
		else if (frame >= 7 && frame <= 15)
			*(long*)&item->item_flags[0] = 448;
		else
			*(long*)&item->item_flags[0] = 0;

		item->item_flags[3] = 1000;
	}

	AnimateItem(item);
}

void inject_traps(bool replace)
{
	INJECT(0x004142F0, FlameEmitterControl, replace);
	INJECT(0x00415B60, OnTwoBlockPlatform, replace);
	INJECT(0x00415AD0, TwoBlockPlatformFloor, replace);
	INJECT(0x00415B20, TwoBlockPlatformCeiling, replace);
	INJECT(0x00415C30, ControlTwoBlockPlatform, replace);
	INJECT(0x00415750, ControlJobySpike, replace);
	INJECT(0x004158E0, DrawScaledSpike, replace);
	INJECT(0x00415DB0, ControlSlicerDicer, replace);
	INJECT(0x00417FC0, ControlSprinkler, replace);
	INJECT(0x00417DB0, ControlMineHelicopter, replace);
	INJECT(0x00417BB0, MineCollision, replace);
	INJECT(0x004173A0, FallingSquishyBlockCollision, replace);
	INJECT(0x00417310, ControlFallingSquishyBlock, replace);
	INJECT(0x00417200, ControlLRSquishyBlock, replace);
	INJECT(0x00417100, ControlSethBlade, replace);
}
