#include "../tomb4/pch.h"
#include "jeep.h"
#include "../specific/function_stubs.h"
#include "objects.h"
#include "lara_states.h"
#include "collide.h"
#include "control.h"
#include "../specific/3dmath.h"
#include "newinv.h"
#include "../specific/specificfx.h"
#include "effect2.h"
#include "lara1gun.h"
#include "tomb4fx.h"
#include "items.h"
#include "sound.h"

void InitialiseJeep(short item_number)
{
	ITEM_INFO* item;
	JEEPINFO* jeep;

	item = &items[item_number];
	jeep = (JEEPINFO*)game_malloc(sizeof(JEEPINFO));
	item->data = jeep;
	jeep->velocity = 0;
	jeep->unused1 = 0;
	jeep->turn_rate = 0;
	jeep->camera_angle = 0;
	jeep->extra_rotation = 0;
	jeep->move_angle = item->pos.y_rot;
	jeep->pitch2 = 0;
	jeep->flags = 0;
	jeep->gear = 0;
	jeep->right_front_wheelrot = 0;
	jeep->left_front_wheelrot = 0;
	jeep->left_back_wheelrot = 0;
	jeep->right_back_wheelrot = 0;
	item->mesh_bits = 0x1BFFF;
}

static long GetOnJeep(short item_number, COLL_INFO* coll)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	long h;
	short room_number, ang;

	item = &items[item_number];

	if (input & IN_ACTION || GLOBAL_inventoryitemchosen == PUZZLE_ITEM1)
	{
		if (!(item->flags & IFL_INVISIBLE) && lara.gun_status == LG_NO_ARMS && lara_item->current_anim_state == AS_STOP &&
			lara_item->anim_number == ANIM_BREATH && !lara_item->gravity_status)
		{
			if (ABS(item->pos.y_pos - lara_item->pos.y_pos) < 256 && TestBoundsCollide(item, lara_item, 100))
			{
				room_number = item->room_number;
				floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
				h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
				
				if (h >= NO_HEIGHT + 512)	//mmmm
				{
					ang = short(phd_atan(item->pos.z_pos - lara_item->pos.z_pos, item->pos.x_pos - lara_item->pos.x_pos) - item->pos.y_rot);

					if (ang <= -8190 || ang >= 24570)
					{
						ang = lara_item->pos.y_rot - item->pos.y_rot;

						if (ang > -24586 && ang < -8206)
						{
							if (GLOBAL_inventoryitemchosen == PUZZLE_ITEM1)
							{
								GLOBAL_inventoryitemchosen = NO_ITEM;
								return 1;
							}
							
							if (have_i_got_object(PUZZLE_ITEM1))
								GLOBAL_enterinventory = PUZZLE_ITEM1;
						}
					}
					else
					{
						ang = lara_item->pos.y_rot - item->pos.y_rot;

						if (ang > 8190 && ang < 24570)
						{
							if (GLOBAL_inventoryitemchosen == PUZZLE_ITEM1)
							{
								GLOBAL_inventoryitemchosen = NO_ITEM;
								return 1;
							}

							if (have_i_got_object(PUZZLE_ITEM1))
								GLOBAL_enterinventory = PUZZLE_ITEM1;
						}
					}
				}
			}
		}
	}

	return 0;
}

void DrawJeepExtras(ITEM_INFO* item)
{
	JEEPINFO* jeep;

	if (lara.vehicle == NO_ITEM)
		return;

	jeep = (JEEPINFO*)item->data;
	DrawJeepSpeedo(phd_winwidth - 64, phd_winheight - 16, jeep->velocity, 0x6000, 0x8000, 32, jeep->gear);
}

static void TriggerExhaustSmoke(long x, long y, long z, short angle, long velocity, long thing)
{
	SPARKS* sptr;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->dR = 16;
	sptr->dG = 16;
	sptr->dB = 32;
	sptr->sR = 0;
	sptr->sG = 0;
	sptr->sB = 0;

	if (thing)
	{
		sptr->dR = uchar((16 * velocity) >> 5);
		sptr->dG = uchar((16 * velocity) >> 5);
		sptr->dB = uchar((32 * velocity) >> 5);
	}

	sptr->ColFadeSpeed = 4;
	sptr->FadeToBlack = 4;
	sptr->Life = uchar((GetRandomControl() & 3) - (velocity >> 12) + 20);
	sptr->sLife = sptr->Life;

	if (sptr->Life < 9)
	{
		sptr->Life = 9;
		sptr->sLife = 9;
	}

	sptr->TransType = 2;
	sptr->x = (GetRandomControl() & 0xF) + x - 8;
	sptr->y = (GetRandomControl() & 0xF) + y - 8;
	sptr->z = (GetRandomControl() & 0xF) + z - 8;
	sptr->Xvel = velocity * phd_sin(angle) >> 16;
	sptr->Yvel = -8 - (GetRandomControl() & 7);
	sptr->Zvel = velocity * phd_cos(angle) >> 16;
	sptr->Friction = 4;

	if (GetRandomControl() & 1)
	{
		sptr->Flags = 538;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -24 - (GetRandomControl() & 7);
		else
			sptr->RotAdd = (GetRandomControl() & 7) + 24;
	}
	else
		sptr->Flags = 522;

	sptr->Scalar = 1;
	sptr->Gravity = -4 - (GetRandomControl() & 3);
	sptr->MaxYvel = -8 - (GetRandomControl() & 7);
	sptr->dSize = uchar((GetRandomControl() & 7) + (velocity >> 7) + 32);
	sptr->sSize = sptr->dSize >> 1;
	sptr->Size = sptr->dSize >> 1;
}

void JeepExplode(ITEM_INFO* item)
{
	if (room[item->room_number].flags & ROOM_UNDERWATER)
		TriggerUnderwaterExplosion(item, 1);
	else
	{
		TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 3, -2, 0, item->room_number);

		for (int i = 0; i < 3; i++)
			TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 3, -1, 0, item->room_number);
	}

	ExplodingDeath2(lara.vehicle, -1, 256);
	KillItem(lara.vehicle);
	item->status = ITEM_DEACTIVATED;
	SoundEffect(SFX_EXPLOSION1, 0, SFX_DEFAULT);
	SoundEffect(SFX_EXPLOSION2, 0, SFX_DEFAULT);
	lara.vehicle = NO_ITEM;
}

void inject_jeep(bool replace)
{
	INJECT(0x00466F40, InitialiseJeep, replace);
	INJECT(0x004671B0, GetOnJeep, replace);
	INJECT(0x00467330, DrawJeepExtras, replace);
	INJECT(0x00467920, TriggerExhaustSmoke, replace);
	INJECT(0x00467AC0, JeepExplode, replace);
}
