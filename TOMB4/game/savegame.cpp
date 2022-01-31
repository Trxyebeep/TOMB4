#include "../tomb4/pch.h"
#include "savegame.h"
#include "objects.h"
#include "traps.h"
#include "items.h"
#include "laramisc.h"
#include "control.h"
#include "bike.h"
#include "jeep.h"

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

	memset(savegame.Lara.puzzleitems, 0, 12);
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

void SaveLaraData()
{
	ITEM_INFO* item;

	for (int i = 0; i < 15; i++)
		lara.mesh_ptrs[i] = (short*)((long)lara.mesh_ptrs[i] - (long)mesh_base);

	lara.left_arm.frame_base = (short*)((long)lara.left_arm.frame_base - (long)objects[PISTOLS_ANIM].frame_base);
	lara.right_arm.frame_base = (short*)((long)lara.right_arm.frame_base - (long)objects[PISTOLS_ANIM].frame_base);
	lara.GeneralPtr = (void*)((long)lara.GeneralPtr - (long)malloc_buffer);
	memcpy(&savegame.Lara, &lara, sizeof(savegame.Lara));

	for (int i = 0; i < 15; i++)
		lara.mesh_ptrs[i] = (short*)((long)lara.mesh_ptrs[i] + (long)mesh_base);

	lara.left_arm.frame_base = (short*)((long)lara.left_arm.frame_base + (long)objects[PISTOLS_ANIM].frame_base);
	lara.right_arm.frame_base = (short*)((long)lara.right_arm.frame_base + (long)objects[PISTOLS_ANIM].frame_base);
	lara.GeneralPtr = (void*)((long)lara.GeneralPtr + (long)malloc_buffer);

	if (lara.weapon_item != NO_ITEM)
	{
		item = &items[lara.weapon_item];
		savegame.WeaponObject = item->object_number;
		savegame.WeaponAnim = item->anim_number;
		savegame.WeaponFrame = item->frame_number;
		savegame.WeaponCurrent = item->current_anim_state;
		savegame.WeaponGoal = item->goal_anim_state;
	}

	savegame.cutscene_triggered = CutSceneTriggered;
}

void WriteSG(void* pointer, long size)
{
	char* data;

	SGcount += size;

	for (data = (char*)pointer; size > 0; size--)
		*SGpoint++ = *data++;
}

void ReadSG(void* pointer, long size)
{
	char* data;

	SGcount += size;

	for (data = (char*)pointer; size > 0; size--)
		*data++ = *SGpoint++;
}

void SaveHubData(long index)
{
	savegame.HubSizes[index] = ushort(SGcount - savegame.HubOffsets[index]);

	if (index < 10)
		savegame.HubSizes[index - 9] = savegame.HubSizes[index] + savegame.HubOffsets[index];
}

void RestoreLaraData(long FullSave)
{
	ITEM_INFO* item;

	if (!FullSave)
		savegame.Lara.item_number = lara.item_number;

	memcpy(&lara, &savegame.Lara, sizeof(lara));
	lara.target = 0;
	lara.spaz_effect = 0;
	lara.left_arm.frame_base = (short*)((long)lara.left_arm.frame_base + (long)objects[PISTOLS_ANIM].frame_base);
	lara.right_arm.frame_base = (short*)((long)lara.right_arm.frame_base + (long)objects[PISTOLS_ANIM].frame_base);
	lara.GeneralPtr = (void*)((long)lara.GeneralPtr + (long)malloc_buffer);

	if (lara.burn)
	{
		lara.burn = 0;
		LaraBurn();
	}

	if (lara.weapon_item != NO_ITEM)
	{
		lara.weapon_item = CreateItem();
		item = &items[lara.weapon_item];
		item->object_number = savegame.WeaponObject;
		item->anim_number = savegame.WeaponAnim;
		item->frame_number = savegame.WeaponFrame;
		item->current_anim_state = savegame.WeaponCurrent;
		item->goal_anim_state = savegame.WeaponGoal;
		item->status = ITEM_ACTIVE;
		item->room_number = 255;
	}

	if (savegame.HubSavedLara)
	{
		LaraInitialiseMeshes();
		savegame.HubSavedLara &= ~1;
		lara.last_gun_type = WEAPON_PISTOLS;
		lara.gun_type = WEAPON_PISTOLS;
		lara.request_gun_type = WEAPON_PISTOLS;

		if (lara.weapon_item != NO_ITEM)
		{
			KillItem(lara.weapon_item);
			lara.weapon_item = NO_ITEM;
		}
	}
	else
	{
		for (int i = 0; i < 15; i++)
			lara.mesh_ptrs[i] = (short*)((long)lara.mesh_ptrs[i] + (long)mesh_base);
	}

	CutSceneTriggered = savegame.cutscene_triggered;
}

void sgRestoreLevel()
{
	AIOBJECT* lsp;
	ITEM_INFO* item;
	FLOOR_INFO* floor;

	if (OpenSaveGame(gfCurrentLevel, 0) >= 0)
		RestoreLevelData(0);

	RestoreLaraData(0);

	if (gfRequiredStartPos)
	{
		lsp = &AIObjects[gfRequiredStartPos - 1];
		lara_item->pos.x_pos = lsp->x;
		lara_item->pos.y_pos = lsp->y;
		lara_item->pos.z_pos = lsp->z;
		lara_item->pos.y_rot = lsp->y_rot;

		if (lara_item->room_number != lsp->room_number)
			ItemNewRoom(lara.item_number, lsp->room_number);
	}

	InitialiseLaraAnims(lara_item);

	if (savegame.Lara.vehicle != NO_ITEM)
	{
		for (int i = 0; i < level_items; i++)
		{
			item = &items[i];

			if (item->object_number == MOTORBIKE || item->object_number == JEEP)
			{
				item->pos.x_pos = lara_item->pos.x_pos;
				item->pos.y_pos = lara_item->pos.y_pos;
				item->pos.z_pos = lara_item->pos.z_pos;
				item->pos.y_rot = lara_item->pos.y_rot;

				if (item->room_number != lara_item->room_number)
					ItemNewRoom(i, lara_item->room_number);

				floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &item->room_number);
				item->floor = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
				lara.vehicle = i;

				if (item->object_number == MOTORBIKE)
					BikeStart(item, lara_item);
				else
					JeepStart(item, lara_item);

				break;
			}
		}
	}
}

void inject_savegame(bool replace)
{
	INJECT(0x0045A0E0, CheckSumValid, replace);
	INJECT(0x0045A100, sgInitialiseHub, replace);
	INJECT(0x0045A1B0, SaveLaraData, replace);
	INJECT(0x0045B000, WriteSG, replace);
	INJECT(0x0045BD80, ReadSG, replace);
	INJECT(0x0045A470, SaveHubData, replace);
	INJECT(0x0045B080, RestoreLaraData, replace);
	INJECT(0x0045BDF0, sgRestoreLevel, replace);
}
