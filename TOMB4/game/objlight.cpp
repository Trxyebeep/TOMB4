#include "../tomb4/pch.h"
#include "objlight.h"
#include "control.h"
#include "effect2.h"
#include "sound.h"

void ControlPulseLight(short item_number)
{
	ITEM_INFO* item;
	long sin, r, g, b;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	if (!flip_stats[4] && gfLevelFlags & GF_PULSE)
		return;

	if (item->trigger_flags == 1)
	{
		item->trigger_flags = 0;
		FlashFadeR = 128;
		FlashFadeG = 255;
		FlashFadeB = 255;
		FlashFader = 32;
		camera.bounce = -128;
		SoundEffect(SFX_BOULDER_FALL, 0, 0);
		SoundEffect(SFX_EXPLOSION2, 0, 0);
	}
	else if (item->trigger_flags == 2)
	{
		SoundEffect(SFX_MAPPER_PYRAMID_OPEN, &item->pos, 0);

		if (room[camera.pos.room_number].flags & ROOM_NOT_INSIDE)
		{
			FlashFadeR = 64;
			FlashFadeG = 128;
			FlashFadeB = 128;
			FlashFader = 32;
		}
	}

	item->item_flags[0] -= 2048;
	sin = ABS(phd_sin(item->item_flags[0] + ((item->pos.y_pos & 0x3FFF) << 2)) >> 6);

	if (sin > 255)
		sin = 255;

	r = (sin * 64) >> 9;
	g = (sin * 255) >> 9;
	b = (sin * 255) >> 9;
	TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 18, r, g, b);
}

void inject_objlight(bool replace)
{
	INJECT(0x004572D0, ControlPulseLight, replace);
}
