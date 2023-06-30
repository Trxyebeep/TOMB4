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
#include "lara_states.h"
#include "collide.h"
#include "draw.h"
#include "newinv.h"
#include "switch.h"
#include "delstuff.h"
#include "effects.h"
#include "../specific/3dmath.h"
#include "debris.h"
#include "camera.h"
#include "box.h"
#include "../specific/input.h"
#include "lara.h"
#include "../specific/file.h"
#include "gameflow.h"

OBJECT_INFO objects[NUMBER_OBJECTS];

static short StatuePlinthBounds[12] = { 0, 0, -64, 0, 0, 0, -1820, 1820, -5460, 5460, -1820, 1820 };
static short PoleBounds[12] = { -256, 256, 0, 0, -512, 512, -1820, 1820, -5460, 5460, -1820, 1820 };
static PHD_VECTOR PolePos = { 0, 0, -208 };
static PHD_VECTOR PolePosR = { 0, 0, 0 };

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

void BridgeFlatFloor(ITEM_INFO* item, long x, long y, long z, long* height)
{
	if (item->pos.y_pos >= y)
	{
		*height = item->pos.y_pos;
		height_type = WALL;
		OnObject = 1;
	}
}

void BridgeFlatCeiling(ITEM_INFO* item, long x, long y, long z, long* height)
{
	if (item->pos.y_pos < y)
		*height = item->pos.y_pos + 256;
}

long GetOffset(ITEM_INFO* item, long x, long z)
{
	if (!item->pos.y_rot)
		return ~x & 0x3FF;
	else if (item->pos.y_rot == -0x8000)
		return x & 0x3FF;
	else if (item->pos.y_rot == 0x4000)
		return z & 0x3FF;
	else
		return ~z & 0x3FF;
}

void BridgeTilt1Floor(ITEM_INFO* item, long x, long y, long z, long* height)
{
	long level;

	level = item->pos.y_pos + (GetOffset(item, x, z) >> 2);

	if (level >= y)
	{
		*height = level;
		height_type = WALL;
		OnObject = 1;
	}
}

void BridgeTilt1Ceiling(ITEM_INFO* item, long x, long y, long z, long* height)
{
	long level;

	level = item->pos.y_pos + (GetOffset(item, x, z) >> 2);

	if (level < y)
		*height = level + 256;
}

void BridgeTilt2Floor(ITEM_INFO* item, long x, long y, long z, long* height)
{
	long level;

	level = item->pos.y_pos + (GetOffset(item, x, z) >> 1);

	if (level >= y)
	{
		*height = level;
		height_type = WALL;
		OnObject = 1;
	}
}

void BridgeTilt2Ceiling(ITEM_INFO* item, long x, long y, long z, long* height)
{
	long level;

	level = item->pos.y_pos + (GetOffset(item, x, z) >> 1);

	if (level < y)
		*height = level + 256;
}

void StatuePlinthCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	short* bounds;
	short room_number, y_rot;

	item = &items[item_number];

	if (input & IN_ACTION && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH && !l->gravity_status &&
		lara.gun_status == LG_NO_ARMS && !item->trigger_flags && !item->item_flags[0])
	{
		if (!item->item_flags[1])
		{
			bounds = GetBoundsAccurate(item);
			StatuePlinthBounds[0] = bounds[0];
			StatuePlinthBounds[1] = bounds[1];
			StatuePlinthBounds[4] = bounds[4] - 200;
			StatuePlinthBounds[5] = bounds[4] + 200;
			y_rot = item->pos.y_rot;
			item->pos.y_rot = l->pos.y_rot;

			if (TestLaraPosition(StatuePlinthBounds, item, l))
			{
				if (have_i_got_object(PUZZLE_ITEM5))
				{
					GLOBAL_enterinventory = PUZZLE_ITEM5;
					item->item_flags[1] = 1;
				}
			}

			item->pos.y_rot = y_rot;
			return;
		}
		
		if (GLOBAL_inventoryitemchosen == PUZZLE_ITEM5)
		{
			l->anim_number = ANIM_PLINTHHI;
			l->frame_number = anims[ANIM_PLINTHHI].frame_base;
			l->current_anim_state = AS_CONTROLLED;
			lara.gun_status = LG_HANDS_BUSY;
			GLOBAL_inventoryitemchosen = NO_ITEM;
			return;
		}
	}
	else if (item->item_flags[1])
	{
		if (GLOBAL_inventoryitemchosen == PUZZLE_ITEM5)
		{
			l->anim_number = ANIM_PLINTHHI;
			l->frame_number = anims[ANIM_PLINTHHI].frame_base;
			l->current_anim_state = AS_CONTROLLED;
			lara.gun_status = LG_HANDS_BUSY;
			GLOBAL_inventoryitemchosen = NO_ITEM;
			return;
		}
	}

	if (l->anim_number == ANIM_PLINTHHI && l->frame_number == anims[ANIM_PLINTHHI].frame_base + 45)
	{
		room_number = item->room_number;
		floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
		GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
		TestTriggers(trigger_index, 1, item->flags & 0x3E00);
		item->mesh_bits = 255;
		item->item_flags[0] = 1;
		lara.puzzleitems[4]--;
	}
	else
	{
		item->item_flags[1] = 0;
		ObjectCollision(item_number, l, coll);
	}
}

void TriggerRopeFlame(PHD_VECTOR* pos)
{
	SPARKS* sptr;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 255;
	sptr->sG = (GetRandomControl() & 0x1F) + 48;
	sptr->sB = 48;
	sptr->dR = (GetRandomControl() & 0x3F) - 64;
	sptr->dG = (GetRandomControl() & 0x3F) + 0x80;
	sptr->dB = 32;
	sptr->FadeToBlack = 4;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 4;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 3) + 24;
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0xF) + pos->x - 8;
	sptr->y = pos->y;
	sptr->z = (GetRandomControl() & 0xF) + pos->z - 8;
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Friction = 5;
	sptr->Flags = 538;

	if (!(GetRandomControl() & 3))
		sptr->Flags |= 0x20;

	sptr->RotAng = GetRandomControl() & 0xFFF;

	if (GetRandomControl() & 1)
		sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
	else
		sptr->RotAdd = (GetRandomControl() & 0xF) + 16;

	if (GetRandomControl() & 0xF)
	{
		sptr->Yvel = -24 - (GetRandomControl() & 0xF);
		sptr->Gravity = -24 - (GetRandomControl() & 0x1F);
		sptr->MaxYvel = -16 - (GetRandomControl() & 7);
	}
	else
	{
		sptr->Yvel = (GetRandomControl() & 0xF) + 24;
		sptr->MaxYvel = 0;
		sptr->Gravity = (GetRandomControl() & 0x1F) + 24;
	}

	sptr->Scalar = 2;
	sptr->Size = (GetRandomControl() & 0xF) + 96;
	sptr->sSize = sptr->Size;
	sptr->dSize = sptr->Size >> 3;
}

void ControlBurningRope(short item_number)
{
	ITEM_INFO* item;
	SPHERE* sphere;
	PHD_VECTOR pos;
	long passes;
	short nmeshes;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	nmeshes = objects[item->object_number].nmeshes - 1;

	if (!(GlobalCounter & 3))
	{
		GetSpheres(item, Slist, 1);
		sphere = &Slist[item->item_flags[0] >> 1];

		for (int i = item->item_flags[0]; i < item->item_flags[1]; i++)
		{
			if (item->mesh_bits & (1 << (i >> 1)))
			{
				if (i & 1 && GlobalCounter & 4)
				{
					pos.x = (sphere->x + sphere[1].x) >> 1;
					pos.y = (sphere->y + sphere[1].y) >> 1;
					pos.z = (sphere->z + sphere[1].z) >> 1;
					TriggerRopeFlame(&pos);
				}
				else if (!(GlobalCounter & 4))
				{
					pos.x = sphere->x;
					pos.y = sphere->y;
					pos.z = sphere->z;
					TriggerRopeFlame(&pos);
				}
			}

			if (i & 1)
				sphere++;
		}

		if (item->item_flags[3])
		{
			if (item->item_flags[0] > 1)
				item->item_flags[0]--;

			if (item->item_flags[1] < nmeshes << 1)
				item->item_flags[1]++;
		}
	}

	DebrisFlags = 1;

	if (item->item_flags[3])
	{
		item->item_flags[3]--;

		if (!item->item_flags[3])
		{
			item->item_flags[1] = item->item_flags[2];
			item->item_flags[0] = item->item_flags[2];
			ExplodeItemNode(item, item->item_flags[2], 0, 256);
		}
	}
	else if (GlobalCounter & 1)
	{
		passes = 0;

		if (item->item_flags[0] > 0)
		{
			item->item_flags[0]--;
			ExplodeItemNode(item, item->item_flags[0], 0, 256);
		}
		else
			passes++;

		if (item->item_flags[1] < nmeshes)
		{
			item->item_flags[1]++;
			ExplodeItemNode(item, item->item_flags[1], 0, 256);
		}
		else
			passes++;

		if (passes == 2)
		{
			if (gfCurrentLevel != 27)
				TestTriggersAtXYZ(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 1, 0);

			KillItem(item_number);
		}
	}

	DebrisFlags = 0;
}

void BurningRopeCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	SPHERE* sphere;
	PHD_VECTOR pos;
	long nSpheres, dx, dy, dz;

	item = &items[item_number];

	if (item->trigger_flags || !lara.LitTorch)
		return;

	nSpheres = GetSpheres(item, Slist, 1);
	dx = item->pos.x_pos - lara_item->pos.x_pos;
	dy = item->pos.y_pos - lara_item->pos.y_pos;
	dz = item->pos.z_pos - lara_item->pos.z_pos;

	if (dx > 0x1000 || dx < -0x1000 || dy > 0x800 || dy < -0x800 || dz > 0x1000 || dz < -0x1000)
		return;

	pos.x = -32;
	pos.y = 64;
	pos.z = 256;
	GetLaraJointPos(&pos, 14);

	for (int i = 0; i < nSpheres; i++)
	{
		sphere = &Slist[i];
		dx = abs(sphere->x - pos.x);
		dy = abs(sphere->y - pos.y);
		dz = abs(sphere->z - pos.z);

		if (dx < sphere->r && dy < sphere->r && dz < sphere->r)
		{
			if (gfCurrentLevel == 27)
			{
				SoundEffect(SFX_BOULDER_FALL, 0, SFX_DEFAULT);
				TestTriggersAtXYZ(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 1, 0);
			}

			item->trigger_flags = 1;
			item->item_flags[0] = i << 1;
			item->item_flags[1] = i << 1;
			item->item_flags[2] = i;
			item->item_flags[3] = 150;
			AddActiveItem(item_number);
			item->status = ITEM_ACTIVE;
			item->flags |= IFL_CODEBITS;
		}
	}
}

void ControlWaterfall(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (TriggerActive(item))
	{
		item->status = ITEM_ACTIVE;

		if (item->trigger_flags == 668)
			SoundEffect(SFX_SAND_LOOP, &item->pos, SFX_DEFAULT);
		else if (item->trigger_flags == 777)
			SoundEffect(SFX_WATERFALL_LOOP, &item->pos, SFX_DEFAULT);
	}
	else
	{
		if (item->trigger_flags == 2 || item->trigger_flags == 668)
			item->status = ITEM_INVISIBLE;
	}
}

void AnimateWaterfalls()
{
	TEXTURESTRUCT* tex;
	OBJECT_INFO* obj;
	float off;
	static long vOff = 0;

	vOff -= 7;
	vOff &= 0x3F;
	off = (float)vOff * float(1.0F / 256.0F);

	for (int i = 0; i < 3; i++)
	{
		obj = &objects[WATERFALL1 + i];

		if (obj->loaded)
		{
			tex = AnimatingWaterfalls[i];
			tex->v1 = AnimatingWaterfallsV[i] + off;
			tex->v2 = AnimatingWaterfallsV[i] + off;
			tex->v3 = AnimatingWaterfallsV[i] + off + 0.25F;
			tex->v4 = AnimatingWaterfallsV[i] + off + 0.25F;

			tex++;
			tex->v1 = AnimatingWaterfallsV[i] + off;
			tex->v2 = AnimatingWaterfallsV[i] + off;
			tex->v3 = AnimatingWaterfallsV[i] + off + 0.25F;
			tex->v4 = AnimatingWaterfallsV[i] + off + 0.25F;
		}
	}
}

void ControlTriggerTriggerer(short item_number)
{
	ITEM_INFO* item;
	short* data;

	item = &items[item_number];
	GetHeight(GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &item->room_number), item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	data = trigger_index;

	if (data)
	{
		if ((*data & 0x1F) == LAVA_TYPE)
		{
			if (*data & 0x8000)
				return;

			data++;
		}

		if ((*data & 0x1F) == CLIMB_TYPE)
		{
			if (*data & 0x8000)
				return;

			data++;
		}

		if ((*data & 0x1F) == MONKEY_TYPE)
		{
			if (*data & 0x8000)
				return;

			data++;
		}

		if ((*data & 0x1F) == TRIGTRIGGER_TYPE)
		{
			if (TriggerActive(item))
				*data |= IFL_TRIGGERED;
			else
				*data &= ~IFL_TRIGGERED;
		}
	}
}

void PoleCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	short roty;

	item = &items[item_num];

	if (input & IN_ACTION && lara.gun_status == LG_NO_ARMS && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH ||
		lara.IsMoving && lara.GeneralPtr == (void*)item_num)
	{
		roty = item->pos.y_rot;
		item->pos.y_rot = l->pos.y_rot;

		if (TestLaraPosition(PoleBounds, item, l))
		{
			if (MoveLaraPosition(&PolePos, item, l))
			{
				l->anim_number = ANIM_STAT2POLE;
				l->frame_number = anims[ANIM_STAT2POLE].frame_base;
				l->current_anim_state = AS_POLESTAT;
				lara.IsMoving = 0;
				lara.gun_status = LG_HANDS_BUSY;
			}
			else
				lara.GeneralPtr = (void*)item_num;
		}
		else
		{
			if (lara.IsMoving && lara.GeneralPtr == (void*)item_num)
			{
				lara.IsMoving = 0;
				lara.gun_status = LG_NO_ARMS;
			}
		}

		item->pos.y_rot = roty;
	}
	else if (input & IN_ACTION && lara.gun_status == LG_NO_ARMS && l->gravity_status && l->fallspeed > 0 &&
		(l->current_anim_state == AS_REACH || l->current_anim_state == AS_UPJUMP))
	{
		if (TestBoundsCollide(item, l, 100) && TestCollision(item, l))
		{
			roty = item->pos.y_rot;
			item->pos.y_rot = l->pos.y_rot;

			if (l->current_anim_state == AS_REACH)
			{
				PolePosR.y = l->pos.y_pos - item->pos.y_pos + 10;
				l->anim_number = ANIM_REACH2POLE;
				l->frame_number = anims[ANIM_REACH2POLE].frame_base;
			}
			else
			{
				PolePosR.y = l->pos.y_pos - item->pos.y_pos + 66;
				l->anim_number = ANIM_JUMP2POLE;
				l->frame_number = anims[ANIM_JUMP2POLE].frame_base;
			}

			AlignLaraPosition(&PolePosR, item, l);
			l->gravity_status = 0;
			l->fallspeed = 0;
			l->current_anim_state = AS_POLESTAT;
			lara.gun_status = LG_HANDS_BUSY;
			item->pos.y_rot = roty;
		}
	}
	else
	{
		if ((l->current_anim_state < AS_POLESTAT || l->current_anim_state > AS_POLERIGHT) && l->current_anim_state != AS_BACKJUMP)
			ObjectCollision(item_num, l, coll);
	}
}

void ControlAnimatingSlots(short item_number)
{
	ITEM_INFO* item;
	PHD_VECTOR pos;

	item = &items[item_number];

	if (TriggerActive(item))
	{
		item->status = ITEM_ACTIVE;
		AnimateItem(item);

		if (item->trigger_flags == 666)
		{
			pos.x = 0;
			pos.y = 0;
			pos.z = 0;
			GetJointAbsPosition(item, &pos, 0);
			SoundEffect(SFX_HELICOPTER_LOOP, (PHD_3DPOS*)&pos, SFX_DEFAULT);

			if (item->frame_number == anims[item->anim_number].frame_end)
				item->flags &= ~IFL_CODEBITS;
		}
	}
	else if (item->trigger_flags == 2)
		item->status = ITEM_INVISIBLE;
}

void SmashObjectControl(short item_number)
{
	ITEM_INFO* item;
	long speed;

	item = &items[item_number];

	if (item->flags & IFL_INVISIBLE)
		return;

	if (lara.vehicle != NO_ITEM)
	{
		if (ItemNearLara(&item->pos, 512))
			SmashObject(item_number);
	}
	else if (item->touch_bits)
	{
		item->touch_bits = 0;
		speed = (lara_item->speed * phd_cos(lara_item->pos.y_rot - item->pos.y_rot)) >> W2V_SHIFT;

		if (abs(speed) >= 50)
			SmashObject(item_number);
	}
}

void SmashObject(short item_number)
{
	ITEM_INFO* item;
	ROOM_INFO* r;
	BOX_INFO* box;
	long sector;

	item = &items[item_number];
	r = &room[item->room_number];
	sector = ((item->pos.z_pos - r->z) >> 10) + r->x_size * ((item->pos.x_pos - r->x) >> 10);
	box = &boxes[r->floor[sector].box];

	if (box->overlap_index & 0x8000)
		box->overlap_index &= ~0x4000;

	SoundEffect(SFX_EXPLOSION1, &item->pos, SFX_DEFAULT);
	SoundEffect(SFX_EXPLOSION2, &item->pos, SFX_DEFAULT);
	item->collidable = 0;
	item->mesh_bits = 0xFFFE;
	ExplodingDeath2(item_number, -1, 256);
	item->flags |= IFL_INVISIBLE;

	if (item->status == ITEM_ACTIVE)
		RemoveActiveItem(item_number);

	item->status = ITEM_DEACTIVATED;
}

void EarthQuake(short item_number)
{
	ITEM_INFO* item;
	long pitch;
	short earth_item;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	if (item->trigger_flags == 888)
	{
		camera.bounce = -64 - (GetRandomControl() & 0x1F);
		SoundEffect(SFX_EARTHQUAKE_LOOP, 0, SFX_DEFAULT);
		item->item_flags[3]++;

		if (item->item_flags[3] > 150)
		{
			SoundEffect(SFX_DOOR_GEN_THUD, 0, SFX_DEFAULT);
			KillItem(item_number);
		}
	}
	else if (item->trigger_flags == 333)
	{
		if (item->item_flags[0] >= 495)
			KillItem(item_number);
		else
		{
			item->item_flags[0]++;
			SoundEffect(SFX_EARTHQUAKE_LOOP, 0, SFX_DEFAULT);
		}
	}
	else
	{
		if (!item->item_flags[1])
			item->item_flags[1] = 100;

		if (!item->item_flags[2])
		{
			if (abs(item->item_flags[0] - item->item_flags[1]) < 16)
			{
				if (item->item_flags[1] == 20)
				{
					item->item_flags[1] = 100;
					item->item_flags[2] = (GetRandomControl() & 0x7F) + 90;
				}
				else
				{
					item->item_flags[1] = 20;
					item->item_flags[2] = (GetRandomControl() & 0x7F) + 30;
				}
			}
		}

		if (item->item_flags[2])
			item->item_flags[2]--;

		if (item->item_flags[0] <= item->item_flags[1])
			item->item_flags[0] += (GetRandomControl() & 7) + 2;
		else
			item->item_flags[0] -= (GetRandomControl() & 7) + 2;

		pitch = (item->item_flags[0] << 16) + 0x1000000;
		SoundEffect(SFX_EARTHQUAKE_LOOP, 0, pitch | SFX_SETPITCH);
		camera.bounce = -item->item_flags[0];

		if (GetRandomControl() < 1024)
		{
			for (earth_item = room[item->room_number].item_number; earth_item != NO_ITEM; earth_item = item->next_item)
			{
				item = &items[earth_item];

				if (item->object_number == FLAME_EMITTER && item->status != ITEM_ACTIVE && item->status != ITEM_DEACTIVATED)
				{
					AddActiveItem(earth_item);
					item->status = ITEM_ACTIVE;
					item->timer = 0;
					item->flags |= IFL_CODEBITS;
					break;
				}
			}
		}
	}
}
