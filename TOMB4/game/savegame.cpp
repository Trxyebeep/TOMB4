#include "../tomb4/pch.h"
#include "savegame.h"

long CheckSumValid(char* buffer)
{
	char checksum;

	checksum = 0;

	for (int i = 0; i < sizeof(SAVEGAME_INFO); i++)
		checksum += *buffer++;

	return !checksum;
}

void sgInitialiseHub(long dont_save_lara)
{
	for (int i = 0; i < 10; i++)
	{
		savegame.HubLevels[i] = 0;
		savegame.HubOffsets[i] = 0;
		savegame.HubSizes[i] = 0;
	}

	if (dont_save_lara)
		savegame.HubSavedLara = 0;
	else
	{
		SaveLaraData();
		savegame.HubSavedLara = 1;
	}

	memset(lara.puzzleitems, 0, 12);
	savegame.Lara.puzzleitemscombo = 0;
	savegame.Lara.keyitems = 0;
	savegame.Lara.keyitemscombo = 0;
	savegame.Lara.pickupitems = 0;
	savegame.Lara.pickupitemscombo = 0;
	savegame.Lara.examine1 = 0;
	savegame.Lara.examine2 = 0;
	savegame.Lara.examine3 = 0;
	savegame.Lara.mechanical_scarab = 0;
	savegame.Lara.location = -1;
	savegame.Lara.highest_location = -1;
	savegame.Lara.vehicle = NO_ITEM;
}

void inject_savegame(bool replace)
{
	INJECT(0x0045A0E0, CheckSumValid, replace);
	INJECT(0x0045A100, sgInitialiseHub, replace);
}
