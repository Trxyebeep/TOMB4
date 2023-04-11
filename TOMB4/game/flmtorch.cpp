#include "../tomb4/pch.h"
#include "flmtorch.h"
#include "effect2.h"
#include "../specific/function_stubs.h"
#include "sound.h"
#include "lara_states.h"
#include "objects.h"
#include "collide.h"
#include "switch.h"
#include "items.h"
#include "laraflar.h"
#include "delstuff.h"
#include "larafire.h"
#include "../specific/3dmath.h"
#include "draw.h"
#include "../specific/input.h"
#include "laramisc.h"
#include "lara.h"
#include "control.h"
#include "tomb4fx.h"

static short FireBounds[12] = { 0, 0, 0, 0, 0, 0, -1820, 1820, -5460, 5460, -1820, 1820 };
static ITEM_INFO* TorchItem = 0;

void TriggerTorchFlame(short item_number, long node)
{
	SPARKS* sptr;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 255;
	sptr->sG = (GetRandomControl() & 0x1F) + 48;
	sptr->sB = 48;
	sptr->dR = (GetRandomControl() & 0x3F) + 192;
	sptr->dG = (GetRandomControl() & 0x3F) + 128;
	sptr->dB = 32;
	sptr->FadeToBlack = 8;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 12;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 7) + 24;
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0xF) - 8;
	sptr->y = 0;
	sptr->z = (GetRandomControl() & 0xF) - 8;
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Yvel = -16 - (GetRandomControl() & 0xF);
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Friction = 5;
	sptr->Flags = 4762;
	sptr->RotAng = GetRandomControl() & 0xFFF;

	if (GetRandomControl() & 1)
		sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
	else
		sptr->RotAdd = (GetRandomControl() & 0xF) + 16;

	sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
	sptr->NodeNumber = (uchar)node;
	sptr->MaxYvel = -16 - (GetRandomControl() & 7);
	sptr->FxObj = (uchar)item_number;
	sptr->Scalar = 1;
	sptr->Size = (GetRandomControl() & 0x1F) + 80;
	sptr->sSize = sptr->Size;
	sptr->dSize = sptr->Size >> 3;
	SoundEffect(SFX_LOOP_FOR_SMALL_FIRES, &items[item_number].pos, SFX_DEFAULT);
}

void FireCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	short rot;

	item = &items[item_number];

	if (lara.gun_type == WEAPON_TORCH && lara.gun_status == LG_READY && !lara.left_arm.lock && (item->status & 1) != lara.LitTorch &&
		item->timer != -1 && input & IN_ACTION && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH && !l->gravity_status)
	{
		rot = item->pos.y_rot;

		if (item->object_number == FLAME_EMITTER)
		{
			FireBounds[0] = -256;
			FireBounds[1] = 256;
			FireBounds[2] = 0;
			FireBounds[3] = 1024;
			FireBounds[4] = -800;
			FireBounds[5] = 800;
		}
		else if (item->object_number == FLAME_EMITTER2)
		{
			FireBounds[0] = -256;
			FireBounds[1] = 256;
			FireBounds[2] = 0;
			FireBounds[3] = 1024;
			FireBounds[4] = -600;
			FireBounds[5] = 600;
		}
		else if (item->object_number == SPRINKLER)
		{
			FireBounds[0] = -256;
			FireBounds[1] = 256;
			FireBounds[2] = 0;
			FireBounds[3] = 1024;
			FireBounds[4] = -384;
			FireBounds[5] = 0;
		}

		item->pos.y_rot = l->pos.y_rot;

		if (TestLaraPosition(FireBounds, item, l))
		{
			if (item->object_number == SPRINKLER)
				l->anim_number = ANIM_LIGHT_TORCH4;
			else
			{
				l->item_flags[3] = 1;
				l->anim_number = short((abs(l->pos.y_pos - item->pos.y_pos) >> 8) + ANIM_LIGHT_TORCH1);
			}

			l->current_anim_state = AS_CONTROLLED;
			l->frame_number = anims[l->anim_number].frame_base;
			lara.flare_control_left = 0;
			lara.left_arm.lock = 3;
			lara.GeneralPtr = (void*)item_number;
		}

		item->pos.y_rot = rot;
	}

	if (lara.GeneralPtr == (void*)item_number && item->status != ITEM_ACTIVE && l->current_anim_state == AS_CONTROLLED &&
		l->anim_number >= ANIM_LIGHT_TORCH1 && l->anim_number <= ANIM_LIGHT_TORCH5 && l->frame_number - anims[l->anim_number].frame_base == 40)
	{
		if (item->object_number == SPRINKLER)
		{
			l->item_flags[3] = 0;
			lara.LitTorch = 0;
		}

		TestTriggersAtXYZ(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 1, item->flags & IFL_CODEBITS);
		item->flags |= IFL_CODEBITS;
		item->item_flags[3] = 0;
		item->status = ITEM_ACTIVE;
		AddActiveItem(item_number);
	}
}

void DoFlameTorch()
{
	PHD_VECTOR pos;

	switch (lara.left_arm.lock)
	{
	case 0:	//holding it

		if (lara.request_gun_type != lara.gun_type)
		{
			lara.left_arm.lock = 2;	//drop it
			lara.left_arm.frame_number = 31;
			lara.left_arm.anim_number = objects[TORCH_ANIM].anim_index + 2;
		}
		else if (input & IN_DRAW && !lara_item->gravity_status && !lara_item->fallspeed && lara_item->current_anim_state != AS_COMPRESS &&
			lara_item->current_anim_state != AS_UPJUMP && lara_item->current_anim_state != AS_FORWARDJUMP &&
			lara_item->current_anim_state != AS_BACKJUMP && lara_item->current_anim_state != AS_RIGHTJUMP &&
			lara_item->current_anim_state != AS_LEFTJUMP || lara.water_status == LW_UNDERWATER)
		{
			lara.left_arm.lock = 1;	//throw it
			lara.left_arm.frame_number = 1;
			lara.left_arm.anim_number = objects[TORCH_ANIM].anim_index + 1;

			if (lara.water_status == LW_UNDERWATER)
				lara.LitTorch = 0;
		}

		break;

	case 1:	//throwing it

		if (lara.left_arm.frame_number < 12 && lara_item->gravity_status)
		{
			lara.left_arm.lock = 0;	//keep holding it
			lara.left_arm.frame_number = 0;
			lara.left_arm.anim_number = objects[TORCH_ANIM].anim_index;
		}
		else
		{
			lara.left_arm.frame_number++;

			if (lara.left_arm.frame_number == 27)
			{
				lara.flare_control_left = 0;
				lara.LitTorch = 0;
				lara.left_arm.lock = 0;
				lara.gun_type = lara.last_gun_type;
				lara.request_gun_type = WEAPON_NONE;
				lara.gun_status = LG_NO_ARMS;
			}
			else if (lara.left_arm.frame_number == 12)
			{
				lara.mesh_ptrs[LM_LHAND] = meshes[objects[LARA].mesh_index + LM_LHAND * 2];
				CreateFlare(BURNING_TORCH_ITEM, 1);
			}
		}

		break;

	case 2:	//dropping it (when pulling out a weapon)

		lara.left_arm.frame_number++;

		if (lara.left_arm.frame_number == 41)
		{
			lara.flare_control_left = 0;
			lara.LitTorch = 0;
			lara.left_arm.lock = 0;
			lara.last_gun_type = WEAPON_NONE;
			lara.gun_type = WEAPON_NONE;
			lara.gun_status = LG_NO_ARMS;
		}
		else if (lara.left_arm.frame_number == 36)
		{
			lara.mesh_ptrs[LM_LHAND] = meshes[objects[LARA].mesh_index + LM_LHAND * 2];
			CreateFlare(BURNING_TORCH_ITEM, 0);
		}

		break;

	case 3:	//lighting it

		if (lara_item->current_anim_state != AS_CONTROLLED)
		{
			lara.LitTorch = lara_item->item_flags[3];
			lara.flare_control_left = 1;
			lara.left_arm.lock = 0;
			lara.left_arm.frame_number = 0;
			lara.left_arm.anim_number = objects[TORCH_ANIM].anim_index;
		}

		break;
	}

	if (lara.flare_control_left)
		lara.gun_status = LG_READY;

	lara.left_arm.frame_base = anims[lara.left_arm.anim_number].frame_ptr;

	if (lara.LitTorch)
	{
		pos.x = -32;
		pos.y = 64;
		pos.z = 256;
		GetLaraJointPos(&pos, 14);
		TriggerDynamic(pos.x, pos.y, pos.z, 12 - (GetRandomControl() & 1), (GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 96, 0);

		if (!(wibble & 7))
			TriggerTorchFlame(lara_item - items, 0);

		TorchItem = lara_item;
	}
}

void GetFlameTorch()
{
	if (lara.gun_type == WEAPON_FLARE)
		CreateFlare(FLARE_ITEM, 0);

	lara.request_gun_type = WEAPON_TORCH;
	lara.gun_type = WEAPON_TORCH;
	lara.flare_control_left = 1;
	lara.left_arm.anim_number = objects[TORCH_ANIM].anim_index;
	lara.gun_status = LG_READY;
	lara.left_arm.lock = 0;
	lara.left_arm.frame_number = 0;
	lara.left_arm.frame_base = anims[objects[TORCH_ANIM].anim_index].frame_ptr;
	lara.mesh_ptrs[LM_LHAND] = meshes[objects[TORCH_ANIM].mesh_index + LM_LHAND * 2];
}

void FlameTorchControl(short item_number)
{
	ITEM_INFO** itemlist;
	MESH_INFO** meshlist;
	ITEM_INFO* item;
	STATIC_INFO* sinfo;
	PHD_3DPOS pos;
	long x, y, z, xv, yv, zv;

	item = &items[item_number];

	if (item->fallspeed)
		item->pos.z_rot += 910;
	else if (!item->speed)
	{
		item->pos.x_rot = 0;
		item->pos.z_rot = 0;
	}

	x = item->pos.x_pos;
	y = item->pos.y_pos;
	z = item->pos.z_pos;
	xv = item->speed * phd_sin(item->pos.y_rot) >> 14;
	zv = item->speed * phd_cos(item->pos.y_rot) >> 14;
	item->pos.x_pos += xv;
	item->pos.z_pos += zv;

	if (room[item->room_number].flags & ROOM_UNDERWATER)
	{
		item->fallspeed += (5 - item->fallspeed) >> 1;
		item->speed += (5 - item->speed) >> 1;

		if (item->item_flags[3])
			item->item_flags[3] = 0;
	}
	else
		item->fallspeed += 6;

	yv = item->fallspeed;
	item->pos.y_pos += yv;
	DoProperDetection(item_number, x, y, z, xv, yv, zv);
	itemlist = (ITEM_INFO**)&tsv_buffer[0];
	meshlist = (MESH_INFO**)&tsv_buffer[1024];

	if (GetCollidedObjects(item, 0, 1, itemlist, meshlist, 0))
	{
		mycoll.enable_baddie_push = 1;

		if (itemlist[0])
		{
			if (!objects[itemlist[0]->object_number].intelligent)
				ObjectCollision(itemlist[0] - items, item, &mycoll);
		}
		else
		{
			sinfo = &static_objects[meshlist[0]->static_number];
			pos.x_pos = meshlist[0]->x;
			pos.y_pos = meshlist[0]->y;
			pos.z_pos = meshlist[0]->z;
			pos.y_rot = meshlist[0]->y_rot;
			ItemPushLaraStatic(item, (short*)&sinfo->x_minc, &pos, &mycoll);
		}

		item->speed >>= 1;
	}

	if (item->item_flags[3])
	{
		TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 12 - (GetRandomControl() & 1),
			(GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 96, 0);

		if (!(wibble & 7))
			TriggerTorchFlame(item_number, 1);

		TorchItem = item;
	}
}
