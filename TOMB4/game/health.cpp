#include "../tomb4/pch.h"
#include "health.h"
#include "../specific/drawbars.h"
#include "text.h"
#include "newinv.h"
#include "spotcam.h"
#include "control.h"
#include "../specific/3dmath.h"
#include "objects.h"
#include "larafire.h"
#include "camera.h"
#include "../specific/input.h"
#include "lara.h"
#include "deltapak.h"
#include "savegame.h"
#include "gameflow.h"
#include "../tomb4/tomb4.h"

DISPLAYPU pickups[8];
short PickupX;
short CurrentPickup;

long health_bar_timer = 0;

static short PickupY;
static short PickupVel;

long FlashIt()
{
	static long flash_state = 0;
	static long flash_count = 0;

	if (flash_count)
		flash_count--;
	else
	{
		flash_state ^= 1;
		flash_count = 5;
	}

	return flash_state;
}

void DrawGameInfo(long timed)
{
	long flash_state, seconds, length, btm;
	short ammo;
	char buf[80];

	if (!GLOBAL_playing_cutseq && !bDisableLaraControl && gfGameMode != 1)
	{
		flash_state = FlashIt();
		DrawHealthBar(flash_state);
		DrawAirBar(flash_state);
		DrawPickups();

		if (DashTimer < 120)
			S_DrawDashBar(100 * DashTimer / 120);

		if (lara.target)
		{
			if (tomb4.enemy_bars && lara.target->hit_points > 0)
			{
				if (lara.target->object_number == LARA_DOUBLE)
					S_DrawEnemyBar(lara_item->hit_points / 10);
				else if (lara.target->object_number == SKELETON)
					S_DrawEnemyBar(100);
				else if (lara.target->object_number == HORSEMAN)
				{
					if (lara.target->dynamic_light)
						S_DrawEnemyBar(100 * lara.target->hit_points / 100);
					else
						S_DrawEnemyBar(100 * lara.target->hit_points / objects[lara.target->object_number].hit_points);
				}
				else
					S_DrawEnemyBar(100 * lara.target->hit_points / objects[lara.target->object_number].hit_points);
			}
		}

		if (gfLevelFlags & GF_TIMER && savegame.Level.Timer && savegame.Level.Timer < 108000)
		{
			seconds = savegame.Level.Timer / 30;
			sprintf(buf, "%.2d:%.2d:%.2d", seconds / 60, seconds % 60, (334 * (savegame.Level.Timer % 30)) / 100);
			PrintString(phd_winwidth >> 1, font_height, 0, buf, 0x8000);
		}

		if (tomb4.ammo_counter)
		{
			if (lara.gun_status == LG_READY)
			{
				ammo = *get_current_ammo_pointer(lara.gun_type);

				if (ammo != -1)
				{
					if (lara.gun_type == WEAPON_SHOTGUN)
						ammo /= 6;

					sprintf(buf, "%i", ammo);
					length = GetStringLength(buf, 0, &btm);
					PrintString(LaserSight ? phd_centerx + 30 : (phd_winxmax - length - 80), phd_winymax - btm - 70, 0, buf, 0);
				}
			}
		}

		if (ammo_change_timer)
		{
			ammo_change_timer--;
			PrintString(phd_winwidth >> 1, font_height, 5, ammo_change_buf, 0x8000);

			if (ammo_change_timer <= 0)
				ammo_change_timer = 0;
		}
	}
}

void DrawHealthBar(long flash_state)
{
	static long old_hitpoints;
	long hitpoints;

	hitpoints = lara_item->hit_points;

	if (hitpoints < 0)
		hitpoints = 0;
	else if (hitpoints > 1000)
		hitpoints = 1000;

	if (old_hitpoints != hitpoints)
	{
		old_hitpoints = hitpoints;
		health_bar_timer = 40;
	}

	if (health_bar_timer < 0)
		health_bar_timer = 0;

	if (hitpoints <= 250)
	{
		if (flash_state)
			S_DrawHealthBar(hitpoints / 10);
		else
			S_DrawHealthBar(0);
	}
	else if (health_bar_timer > 0 || lara.gun_status == LG_READY && lara.gun_type != 8 || lara.poisoned >= 256)
		S_DrawHealthBar(hitpoints / 10);

	if (PoisonFlag)
		PoisonFlag--;
}

void DrawAirBar(long flash_state)
{
	long air;

	if (lara.vehicle == NO_ITEM && (lara.water_status == LW_UNDERWATER || lara.water_status == LW_SURFACE))
	{
		air = lara.air;

		if (air < 0)
			air = 0;
		else if (air > 1800)
			air = 1800;

		if (air <= 450)
		{
			if (flash_state)
				S_DrawAirBar(100 * air / 1800);
			else
				S_DrawAirBar(0);
		}
		else
			S_DrawAirBar(100 * air / 1800);
	}
}

void InitialisePickUpDisplay()
{
	for (int i = 0; i < 8; i++)
		pickups[i].life = -1;

	PickupY = 128;
	PickupX = 128;
	PickupVel = 0;
	CurrentPickup = 0;
}

void DrawPickups()
{
	DISPLAYPU* pu;
	long lp;

	pu = &pickups[CurrentPickup];

	if (pu->life > 0)
	{
		if (PickupX > 0)
			PickupX += -PickupX >> 3;
		else
			pu->life--;
	}
	else if (!pu->life)
	{
		if (PickupX < 128)
		{
			if (PickupVel < 16)
			{
				PickupVel++;
				PickupX += PickupVel;
			}
		}
		else
		{
			pu->life = -1;
			PickupVel = 0;
		}
	}
	else
	{
		for (lp = 0; lp < 8; lp++)
		{
			if (pickups[CurrentPickup].life > 0)
				break;

			CurrentPickup++;
			CurrentPickup &= 7;
		}

		if (lp == 8)
			CurrentPickup = 0;
	}
}

void AddDisplayPickup(short object_number)
{
	DISPLAYPU* pu;

	for (int i = 0; i < 8; i++)
	{
		pu = &pickups[i];

		if (pu->life < 0)
		{
			pu->life = 45;
			pu->object_number = object_number;
			break;
		}
	}

	DEL_picked_up_object(object_number);
}
