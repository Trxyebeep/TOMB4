#include "../tomb4/pch.h"
#include "savegame.h"
#include "objects.h"
#include "traps.h"
#include "items.h"
#include "laramisc.h"
#include "control.h"
#include "bike.h"
#include "jeep.h"
#include "lot.h"
#include "pickup.h"
#include "clockworkbeetle.h"
#include "spotcam.h"
#include "../specific/function_stubs.h"
#include "camera.h"
#include "voncroy.h"
#include "lara.h"
#include "newinv.h"
#include "scarab.h"
#include "senet.h"
#include "switch.h"
#include "rope.h"
#include "gameflow.h"
#include "../specific/file.h"

SAVEGAME_INFO savegame;

static char* SGpoint = 0;
static long SGcount = 0;

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

void CreateCheckSum()
{
	char* ptr;
	char checksum;

	savegame.Checksum = 0;
	ptr = (char*)&savegame;
	checksum = 0;

	for (int i = 0; i < sizeof(SAVEGAME_INFO); i++)
		checksum += *ptr++;

	savegame.Checksum = -checksum;
}

void sgSaveLevel()
{
	long level_index;

	level_index = OpenSaveGame(gfCurrentLevel, 1);
	SaveLevelData(0);
	SaveLaraData();
	SaveHubData(level_index);
}

void sgSaveGame()
{
	long level_index;

	level_index = OpenSaveGame(gfCurrentLevel, 1);
	savegame.Game.Timer = GameTimer;
	savegame.CurrentLevel ^= (gfCurrentLevel ^ savegame.CurrentLevel) & 0x7F;
	SaveLevelData(1);
	SaveLaraData();
	SaveHubData(level_index);
	CreateCheckSum();
}

void sgRestoreGame()
{
	OpenSaveGame(savegame.CurrentLevel & 0x7F, 0);
	GameTimer = savegame.Game.Timer;
	gfCurrentLevel = savegame.CurrentLevel & 0x7F;
	RestoreLevelData(1);
	RestoreLaraData(1);
}

long OpenSaveGame(uchar current_level, long saving)
{
	ushort* curOffset;
	ushort* nexOffset;
	long index, i, j;

	index = 0;

	while (index < 10 && savegame.HubLevels[index] != current_level)
		index++;

	if (saving == 1)
	{
		j = index + 1;
		i = index;

		if (index < 10)
		{
			while (j < 10)
			{
				curOffset = &savegame.HubOffsets[i];
				nexOffset = &savegame.HubOffsets[j];

				if (!savegame.HubLevels[j])
					break;

				memcpy(&savegame.buffer[curOffset[0]], &savegame.buffer[nexOffset[0]], nexOffset[10]);
				curOffset[10] = nexOffset[10];
				nexOffset[0] = curOffset[0] + curOffset[10];
				savegame.HubLevels[i] = savegame.HubLevels[j];
				i++;
				j++;
			}

			savegame.HubLevels[i] = 0;
		}

		for (index = 0; index < 10; index++)
		{
			if (!savegame.HubLevels[index])
				break;
		}

		savegame.HubLevels[index] = current_level;
		SGcount = savegame.HubOffsets[index];
		SGpoint = &savegame.buffer[SGcount];
		return index;
	}

	if (index < 10)
	{
		SGcount = savegame.HubOffsets[index];
		SGpoint = &savegame.buffer[SGcount];
		return index;
	}

	return -1;
}

void SaveLevelData(long FullSave)
{
	ITEM_INFO* item;
	ROOM_INFO* r;
	OBJECT_INFO* obj;
	MESH_INFO* mesh;
	CREATURE_INFO* creature;
	ulong flags;
	long k, flare_age;
	ushort packed;
	short pos, word;
	uchar byte;
	char lflags;

	WriteSG(&FmvSceneTriggered, sizeof(long));
	WriteSG(&GLOBAL_lastinvitem, sizeof(long));
	word = 0;

	for (int i = 0; i < 10; i++)
	{
		if (flip_stats[i])
			word |= (1 << i);
	}

	WriteSG(&word, sizeof(short));

	for (int i = 0; i < 10; i++)
	{
		word = short(flipmap[i] >> 8);
		WriteSG(&word, sizeof(short));
	}

	WriteSG(&flipeffect, sizeof(long));
	WriteSG(&fliptimer, sizeof(long));
	WriteSG(&flip_status, sizeof(long));
	WriteSG(cd_flags, 128);
	WriteSG(&CurrentAtmosphere, sizeof(uchar));
	word = 0;
	k = 0;

	for (int i = 0; i < number_rooms; i++)
	{
		r = &room[i];

		for (int j = 0; j < r->num_meshes; j++)
		{
			mesh = &r->mesh[j];

			if (mesh->static_number >= SHATTER0)
			{
				word |= ((mesh->Flags & 1) << k);
				k++;

				if (k == 16)
				{
					WriteSG(&word, sizeof(short));
					k = 0;
					word = 0;
				}
			}
		}
	}

	if (k)
		WriteSG(&word, sizeof(short));

	byte = 0;

	for (int i = 0; i < 8; i++)
		byte |= LibraryTab[i] << i;

	WriteSG(&byte, sizeof(uchar));
	WriteSG(&CurrentSequence, sizeof(uchar));
	byte = 0;

	for (int i = 0; i < 6; i++)
		byte |= SequenceUsed[i] << i;

	WriteSG(&byte, sizeof(uchar));
	WriteSG(Sequences, 3);

	for (int i = 0; i < number_cameras; i++)
		WriteSG(&camera.fixed[i].flags, sizeof(short));

	for (int i = 0; i < number_spotcams; i++)
		WriteSG(&SpotCam[i].flags, sizeof(short));

	for (int i = 0; i < level_items; i++)
	{
		item = &items[i];
		obj = &objects[item->object_number];
		packed = 0;

		if (item->flags & IFL_CLEARBODY || (item->after_death && (item->object_number < GAME_PIECE1 || item->object_number > ENEMY_PIECE)))
		{
			packed = 0x2000;
			WriteSG(&packed, sizeof(ushort));
		}
		else
		{
			if (item->flags & (IFL_CODEBITS | IFL_INVISIBLE | IFL_TRIGGERED) || item->object_number == LARA && FullSave)
			{
				packed = 0x8000;

				if (item->pos.x_rot)
					packed |= 1;

				if (item->pos.z_rot)
					packed |= 2;

				if (item->pos.x_pos & 1)
					packed |= 4;

				if (item->pos.y_pos & 1)
					packed |= 8;

				if (item->pos.z_pos & 1)
					packed |= 0x10;

				if (item->speed)
					packed |= 0x20;

				if (item->fallspeed)
					packed |= 0x40;

				if (item->item_flags[0])
					packed |= 0x80;

				if (item->item_flags[1])
					packed |= 0x100;

				if (item->item_flags[2])
					packed |= 0x200;

				if (item->item_flags[3])
					packed |= 0x400;

				if (item->timer)
					packed |= 0x800;

				if (item->trigger_flags)
					packed |= 0x1000;

				if (obj->save_hitpoints && item->hit_points != obj->hit_points)
					packed |= 0x4000;

				WriteSG(&packed, sizeof(ushort));

				if (obj->save_position)
				{
					pos = short(item->pos.x_pos >> 1);
					WriteSG(&pos, sizeof(short));

					pos = short(item->pos.y_pos >> 1);
					WriteSG(&pos, sizeof(short));

					pos = short(item->pos.z_pos >> 1);
					WriteSG(&pos, sizeof(short));

					byte = (uchar)item->room_number;
					WriteSG(&byte, sizeof(uchar));

					WriteSG(&item->pos.y_rot, sizeof(short));

					if (packed & 1)
						WriteSG(&item->pos.x_rot, sizeof(short));

					if (packed & 2)
						WriteSG(&item->pos.z_rot, sizeof(short));

					if (packed & 0x20)
						WriteSG(&item->speed, sizeof(short));

					if (packed & 0x40)
						WriteSG(&item->fallspeed, sizeof(short));
				}

				if (obj->save_anim)
				{
					byte = (uchar)item->current_anim_state;
					WriteSG(&byte, sizeof(uchar));

					byte = (uchar)item->goal_anim_state;
					WriteSG(&byte, sizeof(uchar));

					byte = (uchar)item->required_anim_state;
					WriteSG(&byte, sizeof(uchar));

					if (item->object_number != LARA)
					{
						byte = item->anim_number - obj->anim_index;
						WriteSG(&byte, sizeof(uchar));
					}
					else
						WriteSG(&item->anim_number, sizeof(short));

					WriteSG(&item->frame_number, sizeof(short));
				}

				if (packed & 0x4000)
					WriteSG(&item->hit_points, sizeof(short));

				if (obj->save_flags)
				{
					flags = item->flags;
					flags |= item->active << 16;
					flags |= item->status << 17;
					flags |= item->gravity_status << 19;
					flags |= item->hit_status << 20;
					flags |= item->collidable << 21;
					flags |= item->looked_at << 22;
					flags |= item->dynamic_light << 23;
					flags |= item->poisoned << 24;
					flags |= item->ai_bits << 25;
					flags |= item->really_active << 30;

					if (obj->intelligent && item->data)
						flags |= 0x80000000;

					WriteSG(&flags, sizeof(ulong));

					if (packed & 0x80)
						WriteSG(&item->item_flags[0], sizeof(short));

					if (packed & 0x100)
						WriteSG(&item->item_flags[1], sizeof(short));

					if (packed & 0x200)
						WriteSG(&item->item_flags[2], sizeof(short));

					if (packed & 0x400)
						WriteSG(&item->item_flags[3], sizeof(short));

					if (packed & 0x800)
						WriteSG(&item->timer, sizeof(short));

					if (packed & 0x1000)
						WriteSG(&item->trigger_flags, sizeof(short));

					if (obj->intelligent)
						WriteSG(&item->carried_item, sizeof(short));

					if (flags & 0x80000000)
					{
						creature = (CREATURE_INFO*)item->data;

						creature->enemy = (ITEM_INFO*)((long)creature->enemy - (long)malloc_buffer);
						WriteSG(item->data, 22);
						creature->enemy = (ITEM_INFO*)((long)creature->enemy + (long)malloc_buffer);

						WriteSG(&creature->ai_target.object_number, sizeof(short));
						WriteSG(&creature->ai_target.room_number, sizeof(short));
						WriteSG(&creature->ai_target.box_number, sizeof(ushort));
						WriteSG(&creature->ai_target.flags, sizeof(short));
						WriteSG(&creature->ai_target.trigger_flags, sizeof(short));
						WriteSG(&creature->ai_target.pos, sizeof(PHD_3DPOS));

						lflags = creature->LOT.can_jump;
						lflags |= creature->LOT.can_monkey << 1;
						lflags |= creature->LOT.is_amphibious << 2;
						lflags |= creature->LOT.is_jumping << 3;
						lflags |= creature->LOT.is_monkeying << 4;
						WriteSG(&lflags, sizeof(char));
					}
				}

				if (obj->save_mesh)
				{
					WriteSG(&item->mesh_bits, sizeof(ulong));
					WriteSG(&item->meshswap_meshbits, sizeof(ulong));
				}

				if (item->object_number == MOTORBIKE)
					WriteSG(item->data, sizeof(BIKEINFO));

				if (item->object_number == JEEP)
					WriteSG(item->data, sizeof(JEEPINFO));
			}
			else
				WriteSG(&packed, sizeof(ushort));
		}
	}

	if (objects[WHEEL_OF_FORTUNE].loaded)
	{
		WriteSG(senet_item, sizeof(short) * 6);
		WriteSG(senet_piece, sizeof(char) * 6);
		WriteSG(senet_board, sizeof(char) * 17);
		WriteSG(&last_throw, sizeof(char));
		WriteSG(&SenetTargetX, sizeof(long));
		WriteSG(&SenetTargetZ, sizeof(long));
		WriteSG(&piece_moving, sizeof(char));
	}

	if (FullSave)
	{
		byte = 0;
		item = &items[level_items];

		for (int i = level_items; i < 256; i++)
		{
			if (item->active && (item->object_number == FLARE_ITEM || item->object_number == BURNING_TORCH_ITEM))
				byte++;

			item++;
		}

		WriteSG(&byte, sizeof(uchar));
		item = &items[level_items];

		for (int i = level_items; i < 256; i++)
		{
			if (item->active && (item->object_number == FLARE_ITEM || item->object_number == BURNING_TORCH_ITEM))
			{
				if (item->object_number == FLARE_ITEM)
					byte = 0;
				else if (item->object_number == BURNING_TORCH_ITEM)
					byte = 1;

				WriteSG(&byte, sizeof(char));
				WriteSG(&item->pos, sizeof(PHD_3DPOS));
				WriteSG(&item->room_number, sizeof(short));
				WriteSG(&item->speed, sizeof(short));
				WriteSG(&item->fallspeed, sizeof(short));

				if (item->object_number == FLARE_ITEM)
				{
					flare_age = (long)item->data;
					WriteSG(&flare_age, sizeof(long));
				}
				else
					WriteSG(&item->item_flags[3], sizeof(short));
			}

			item++;
		}

		if (objects[LITTLE_BEETLE].loaded)
		{
			byte = 0;

			for (int j = 0; j < 128; j++)
			{
				if (Scarabs[j].On)
					byte++;
			}

			WriteSG(&byte, sizeof(uchar));

			for (int j = 0; j < 128; j++)
			{
				if (Scarabs[j].On)
				{
					word = Scarabs[j].room_number << 8;

					if (Scarabs[j].pos.x_pos & 1)
						word |= 1;

					if (Scarabs[j].pos.y_pos & 1)
						word |= 2;

					if (Scarabs[j].pos.z_pos & 1)
						word |= 4;

					if (Scarabs[j].pos.x_rot)
						word |= 8;

					WriteSG(&word, sizeof(short));

					pos = short(Scarabs[j].pos.x_pos >> 1);
					WriteSG(&pos, sizeof(short));

					pos = short(Scarabs[j].pos.y_pos >> 1);
					WriteSG(&pos, sizeof(short));

					pos = short(Scarabs[j].pos.z_pos >> 1);
					WriteSG(&pos, sizeof(short));

					WriteSG(&Scarabs[j].pos.y_rot, sizeof(short));

					if (word & 8)
						WriteSG(&Scarabs[j].pos.x_rot, sizeof(short));
				}
			}
		}

		byte = 0;
		item = &items[level_items];

		for (int i = level_items; i < 256; i++)
		{
			if (item->active && item->object_number == CLOCKWORK_BEETLE)
			{
				byte = 1;
				break;
			}

			item++;
		}

		WriteSG(&byte, sizeof(uchar));

		if (byte)
		{
			WriteSG(&item->pos, sizeof(PHD_3DPOS));
			WriteSG(item->item_flags, sizeof(short) * 4);
		}

		if (gfCurrentLevel == 1)
		{
			flags = 0;

			for (int i = 0; i < 64; i++)
			{
				if (VonCroyCutFlags[i])
					flags |= 1 << (i & 0xF);

				if ((i & 0xF) == 0xF)
				{
					WriteSG(&flags, 2);
					flags = 0;
				}
			}
		}

		if (lara.RopePtr != -1)
		{
			WriteSG(&RopeList[lara.RopePtr], sizeof(ROPE_STRUCT));
			CurrentPendulum.Rope = (ROPE_STRUCT*)((char*)CurrentPendulum.Rope - (char*)RopeList);

			WriteSG(&CurrentPendulum, sizeof(PENDULUM));
			CurrentPendulum.Rope = (ROPE_STRUCT*)((char*)CurrentPendulum.Rope + (long)RopeList);
		}
	}
}

void RestoreLevelData(long FullSave)
{
	ROOM_INFO* r;
	ITEM_INFO* item;
	CREATURE_INFO* creature;
	FLOOR_INFO* floor;
	OBJECT_INFO* obj;
	MESH_INFO* mesh;
	ulong flags;
	long k, flare_age;
	ushort word, packed, uroom_number, uword;
	short sword, item_number, room_number, req, goal, current;
	uchar numberof;
	char byte, anim, lflags;

	ReadSG(&FmvSceneTriggered, sizeof(long));
	ReadSG(&GLOBAL_lastinvitem, sizeof(long));
	ReadSG(&sword, sizeof(short));

	for (int i = 0; i < 10; i++)
	{
		if (sword & (1 << i))
			FlipMap(i);

		ReadSG(&uword, sizeof(ushort));
		flipmap[i] = uword << 8;
	}

	ReadSG(&flipeffect, sizeof(long));
	ReadSG(&fliptimer, sizeof(long));
	ReadSG(&flip_status, sizeof(long));
	ReadSG(cd_flags, 128);
	ReadSG(&CurrentAtmosphere, sizeof(uchar));
	k = 16;

	for (int i = 0; i < number_rooms; i++)
	{
		r = &room[i];

		for (int j = 0; j < r->num_meshes; j++)
		{
			mesh = &r->mesh[j];

			if (mesh->static_number >= SHATTER0)
			{
				if (k == 16)
				{
					ReadSG(&uword, sizeof(ushort));
					k = 0;
				}

				mesh->Flags ^= (uword ^ mesh->Flags) & 1;

				if (!mesh->Flags)
				{
					room_number = i;
					floor = GetFloor(mesh->x, mesh->y, mesh->z, &room_number);
					GetHeight(floor, mesh->x, mesh->y, mesh->z);
					TestTriggers(trigger_index, 1, 0);
					floor->stopper = 0;
				}

				uword >>= 1;
				k++;
			}
		}
	}

	ReadSG(&byte, sizeof(char));

	for (int i = 0; i < 8; i++)
	{
		LibraryTab[i] = byte & 1;
		byte >>= 1;
	}

	ReadSG(&CurrentSequence, sizeof(uchar));
	ReadSG(&byte, sizeof(char));

	for (int i = 0; i < 6; i++)
	{
		SequenceUsed[i] = byte & 1;
		byte >>= 1;
	}

	ReadSG(Sequences, 3);

	for (int i = 0; i < number_cameras; i++)
		ReadSG(&camera.fixed[i].flags, sizeof(short));

	for (int i = 0; i < number_spotcams; i++)
		ReadSG(&SpotCam[i].flags, sizeof(short));

	for (int i = 0; i < level_items; i++)
	{
		item = &items[i];
		obj = &objects[item->object_number];
		ReadSG(&packed, sizeof(ushort));

		if (packed & 0x2000)
		{
			KillItem(i);
			item->status = ITEM_DEACTIVATED;
			item->flags |= IFL_INVISIBLE;
		}
		else if (packed & 0x8000)
		{
			if (obj->save_position)
			{
				uroom_number = 0;

				ReadSG(&word, sizeof(ushort));
				item->pos.x_pos = (word << 1) | (packed >> 2) & 1;

				ReadSG(&sword, sizeof(short));
				item->pos.y_pos = (sword << 1) | (packed >> 3) & 1;

				ReadSG(&word, sizeof(ushort));
				item->pos.z_pos = (word << 1) | (packed >> 4) & 1;

				ReadSG(&uroom_number, sizeof(uchar));
				ReadSG(&item->pos.y_rot, sizeof(short));

				if (packed & 1)
					ReadSG(&item->pos.x_rot, sizeof(short));

				if (packed & 2)
					ReadSG(&item->pos.z_rot, sizeof(short));

				if (packed & 0x20)
					ReadSG(&item->speed, sizeof(short));

				if (packed & 0x40)
					ReadSG(&item->fallspeed, sizeof(short));

				if (item->room_number != uroom_number)
					ItemNewRoom(i, uroom_number);

				if (obj->shadow_size)
				{
					floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, (short*)&uroom_number);
					item->floor = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
				}
			}

			if (obj->save_anim)
			{
				current = 0;
				goal = 0;
				req = 0;
				ReadSG(&current, sizeof(char));
				ReadSG(&goal, sizeof(char));
				ReadSG(&req, sizeof(char));
				item->current_anim_state = current;
				item->goal_anim_state = goal;
				item->required_anim_state = req;

				if (item->object_number != LARA)
				{
					ReadSG(&anim, sizeof(char));
					item->anim_number = obj->anim_index + anim;
				}
				else
					ReadSG(&item->anim_number, sizeof(short));

				ReadSG(&item->frame_number, sizeof(short));
			}

			if (packed & 0x4000)
				ReadSG(&item->hit_points, sizeof(short));

			if (obj->save_flags)
			{
				ReadSG(&flags, sizeof(ulong));
				item->flags = (short)flags;

				if (packed & 0x80)
					ReadSG(&item->item_flags[0], sizeof(short));

				if (packed & 0x100)
					ReadSG(&item->item_flags[1], sizeof(short));

				if (packed & 0x200)
					ReadSG(&item->item_flags[2], sizeof(short));

				if (packed & 0x400)
					ReadSG(&item->item_flags[3], sizeof(short));

				if (packed & 0x800)
					ReadSG(&item->timer, sizeof(short));

				if (packed & 0x1000)
					ReadSG(&item->trigger_flags, sizeof(short));

				if (obj->intelligent)
					ReadSG(&item->carried_item, sizeof(short));

				if (flags & 0x10000 && !item->active)
					AddActiveItem(i);

				item->active = (flags >> 16) & 1;
				item->status = (flags >> 17) & 3;
				item->gravity_status = (flags >> 19) & 1;
				item->hit_status = (flags >> 20) & 1;
				item->collidable = (flags >> 21) & 1;
				item->looked_at = (flags >> 22) & 1;
				item->dynamic_light = (flags >> 23) & 1;
				item->poisoned = (flags >> 24) & 1;
				item->ai_bits = (flags >> 25) & 31;
				item->really_active = (flags >> 30) & 1;

				if (flags & 0x80000000)
				{
					EnableBaddieAI(i, 1);
					creature = (CREATURE_INFO*)item->data;

					if (creature)
					{
						ReadSG(creature, 22);
						creature->enemy = (ITEM_INFO*)((long)creature->enemy + (long)malloc_buffer);

						if (creature->enemy < 0)
							creature->enemy = 0;

						ReadSG(&creature->ai_target.object_number, sizeof(short));
						ReadSG(&creature->ai_target.room_number, sizeof(short));
						ReadSG(&creature->ai_target.box_number, sizeof(ushort));
						ReadSG(&creature->ai_target.flags, sizeof(short));
						ReadSG(&creature->ai_target.trigger_flags, sizeof(short));
						ReadSG(&creature->ai_target.pos, sizeof(PHD_3DPOS));
						ReadSG(&lflags, sizeof(char));
						creature->LOT.can_jump = (lflags & 1) == 1;
						creature->LOT.can_monkey = (lflags & 2) == 2;
						creature->LOT.is_amphibious = (lflags & 4) == 4;
						creature->LOT.is_jumping = (lflags & 8) == 8;
						creature->LOT.is_monkeying = (lflags & 16) == 16;
					}
					else
						SGpoint += 51;
				}
			}

			if (obj->save_mesh)
			{
				ReadSG(&item->mesh_bits, sizeof(ulong));
				ReadSG(&item->meshswap_meshbits, sizeof(ulong));
			}

			if (item->object_number == MOTORBIKE)
				ReadSG(item->data, sizeof(BIKEINFO));

			if (item->object_number == JEEP)
				ReadSG(item->data, sizeof(JEEPINFO));

			if (obj->collision == PuzzleHoleCollision)
			{
				if (item->status == ITEM_DEACTIVATED || item->status == ITEM_ACTIVE)
				{
					item->object_number += 12;
					item->anim_number = objects[item->object_number].anim_index + anim;
				}
			}

			if (item->object_number >= SMASH_OBJECT1 && item->object_number <= SMASH_OBJECT8 && item->flags & IFL_INVISIBLE)
				item->mesh_bits = 0x100;

			if (item->object_number == RAISING_BLOCK1 && item->item_flags[1] ||
				item->object_number == EXPANDING_PLATFORM && item->item_flags[2])
				AlterFloorHeight(item, -1024);

			if (item->object_number == RAISING_BLOCK2 && item->item_flags[1])
				AlterFloorHeight(item, -2048);
		}
	}

	if (objects[WHEEL_OF_FORTUNE].loaded)
	{
		ReadSG(senet_item, sizeof(short) * 6);
		ReadSG(senet_piece, sizeof(char) * 6);
		ReadSG(senet_board, sizeof(char) * 17);
		ReadSG(&last_throw, sizeof(char));
		ReadSG(&SenetTargetX, sizeof(long));
		ReadSG(&SenetTargetZ, sizeof(long));
		ReadSG(&piece_moving, sizeof(char));
	}

	if (FullSave)
	{
		ReadSG(&numberof, sizeof(uchar));

		for (int i = 0; i < numberof; i++)
		{
			item_number = CreateItem();
			item = &items[item_number];
			ReadSG(&byte, sizeof(char));

			if (!byte)
				item->object_number = FLARE_ITEM;
			else
				item->object_number = BURNING_TORCH_ITEM;

			ReadSG(&item->pos, sizeof(PHD_3DPOS));
			ReadSG(&item->room_number, sizeof(short));
			ReadSG(&item->speed, sizeof(short));
			ReadSG(&item->fallspeed, sizeof(short));
			InitialiseItem(item_number);
			AddActiveItem(item_number);

			switch (item->object_number)
			{
			case BURNING_TORCH_ITEM:
				ReadSG(&item->item_flags[3], 2);
				break;

			case FLARE_ITEM:
				ReadSG(&flare_age, sizeof(long));
				item->data = (void*)flare_age;
				break;
			}
		}

		if (objects[LITTLE_BEETLE].loaded)
		{
			ReadSG(&byte, sizeof(char));
			
			for (int i = 0; i < byte; i++)
			{
				ReadSG(&sword, sizeof(short));

				ReadSG(&uword, sizeof(ushort));
				Scarabs[i].pos.x_pos = uword << 1;
				Scarabs[i].pos.x_pos |= sword & 1;

				ReadSG(&req, sizeof(short));
				Scarabs[i].pos.y_pos = req << 1;
				Scarabs[i].pos.y_pos |= (sword >> 1) & 1;

				ReadSG(&uword, sizeof(ushort));
				Scarabs[i].pos.z_pos = uword << 1;
				Scarabs[i].pos.z_pos |= (sword >> 2) & 1;

				ReadSG(&Scarabs[i].pos.y_rot, sizeof(short));

				if (sword & 8)
					ReadSG(&Scarabs[i].pos.x_rot, sizeof(short));

				Scarabs[i].On = 1;
				Scarabs[i].room_number = (sword >> 8) & 0xFF;
			}
		}

		ReadSG(&byte, sizeof(char));

		if (byte)
		{
			item = TriggerClockworkBeetle(1);
			ReadSG(&item->pos, sizeof(PHD_3DPOS));
			ReadSG(item->item_flags, sizeof(short) * 4);
		}

		if (gfCurrentLevel == 1)
		{
			for (int i = 0; i < 64; i++)
			{
				if (!(i & 0xF))
					ReadSG(&uword, sizeof(ushort));

				if (uword & 1 << (i & 0xF))
					VonCroyCutFlags[i] = 1;
			}
		}

		if (lara.RopePtr != -1)
		{
			ReadSG(&RopeList[lara.RopePtr], sizeof(ROPE_STRUCT));
			ReadSG(&CurrentPendulum, sizeof(PENDULUM));
			CurrentPendulum.Rope = (ROPE_STRUCT*)((char*)CurrentPendulum.Rope + (long)RopeList);
		}
	}
}
