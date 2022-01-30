#include "../tomb4/pch.h"
#include "lara1gun.h"
#include "traps.h"
#include "objects.h"
#include "larafire.h"
#include "box.h"

void DoGrenadeDamageOnBaddie(ITEM_INFO* baddie, ITEM_INFO* item)
{
	if (baddie->flags & 0x8000)
		return;

    if (baddie == lara_item && lara_item->hit_points > 0)
    {
        lara_item->hit_points -= 50;

        if (!(room[item->room_number].flags & ROOM_UNDERWATER) && lara_item->hit_points <= 50)
            LaraBurn();
    }
    else if (!item->item_flags[2])
    {
        baddie->hit_status = 1;

        if ((!objects[baddie->object_number].undead || baddie->object_number == SKELETON || baddie->object_number == MUMMY) &&
            baddie->object_number != AHMET)
        {
            HitTarget(baddie, 0, 30, 1);

            if (baddie != lara_item)
            {
                savegame.Game.AmmoHits++;

                if (baddie->hit_points <= 0)
                {
                    savegame.Level.Kills++;

                    if (baddie->object_number != BABOON_NORMAL && baddie->object_number != BABOON_INV && baddie->object_number != BABOON_SILENT)
                        CreatureDie(baddie - items, 1);
                }
            }
        }
    }
}

void inject_lara1gun(bool replace)
{
    INJECT(0x0042B600, DoGrenadeDamageOnBaddie, replace);
}
