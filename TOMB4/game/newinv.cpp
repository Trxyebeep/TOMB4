#include "../tomb4/pch.h"
#include "newinv.h"
#include "../specific/LoadSave.h"
#include "../specific/3dmath.h"
#include "objects.h"
#include "draw.h"
#include "../specific/output.h"
#include "../specific/d3dmatrix.h"
#include "lara2gun.h"
#include "lara1gun.h"
#include "text.h"
#include "sound.h"
#include "lara_states.h"
#include "larafire.h"
#include "../specific/input.h"
#include "health.h"
#include "gameflow.h"
#include "control.h"
#include "camera.h"
#include "../specific/dxshell.h"
#include "../specific/gamemain.h"
#include "lara.h"
#include "savegame.h"
#include "../tomb4/tomb4.h"
#include "../specific/dxsound.h"
#include "../specific/drawbars.h"

#pragma warning(push)
#pragma warning(disable : 4838)
#pragma warning(disable : 4309)
INVOBJ inventry_objects_list[NUM_INVOBJ] =
{
	//main items
	{UZI_ITEM, -4, 0x3E8, 0x4000, 0x6000, 0x4000, 2, TXT_Uzi, -1},
	{PISTOLS_ITEM, 6, 0x3E8, 0x4000, 0xAD4C, 0xBD40, 2, TXT_Pistols, -1},
	{SHOTGUN_ITEM, -6, 0x280, 0x8000, 0xC000, 0x2000, 2, TXT_Shotgun, 1},
	{SIXSHOOTER_ITEM, 0, 0x320, 0x4000, 0x2AAA, 0x3BC2, 2, TXT_Revolver, 1},
	{SIXSHOOTER_ITEM, 0, 0x320, 0x4000, 0x2AAA, 0x3BC2, 2, TXT_Revolver_LaserSight, 11},
	{CROSSBOW_ITEM, 0, 0x300, 0x2000, 0x1800, 0, 2, TXT_Crossbow, 1},
	{CROSSBOW_ITEM, 0, 0x300, 0x2000, 0x1800, 0, 2, TXT_Crossbow_LaserSight, 11},
	{GRENADE_GUN_ITEM, 0, 0x320, 0x4000, 0, 0x2ECC, 2, TXT_Grenade_Gun, -1},
	{SHOTGUN_AMMO1_ITEM, 0, 0x1F4, 0x4000, 0, 0, 2, TXT_Shotgun_Normal_Ammo, -1},
	{SHOTGUN_AMMO2_ITEM, 0, 0x1F4, 0x4000, 0, 0, 2, TXT_Shotgun_Wideshot_Ammo, -1},
	{GRENADE_GUN_AMMO1_ITEM, 3, 0x320, 0x4000, 0, 0, 2, TXT_Grenadegun_Normal_Ammo, -1},
	{GRENADE_GUN_AMMO2_ITEM, 3, 0x320, 0x4000, 0, 0, 2, TXT_Grenadegun_Super_Ammo, -1},
	{GRENADE_GUN_AMMO3_ITEM, 3, 0x320, 0x4000, 0, 0, 2, TXT_Grenadegun_Flash_Ammo, -1},
	{CROSSBOW_AMMO1_ITEM, 0, 0x44C, 0x4000, 0, 0, 2, TXT_Crossbow_Normal_Ammo, -1},
	{CROSSBOW_AMMO2_ITEM, 0, 0x44C, 0x4000, 0, 0, 2, TXT_Crossbow_Poison_Ammo, -1},
	{CROSSBOW_AMMO3_ITEM, 0, 0x44C, 0x4000, 0, 0, 2, TXT_Crossbow_Explosive_Ammo, -1},
	{SIXSHOOTER_AMMO_ITEM, 0, 0x2BC, 0x4000, 0xF448, 0, 2, TXT_Revolver_Ammo, -1},
	{UZI_AMMO_ITEM, 5, 0x2BC, 0, 0x1508, 0, 2, TXT_Uzi_Ammo, -1},
	{PISTOLS_AMMO_ITEM, 4, 0x2BC, 0, 0x4000, 0, 2, TXT_Pistol_Ammo, -1},
	{LASERSIGHT_ITEM, 2, 0x2BC, 0x4000, 0x7D0, 0, 2, TXT_LaserSight, -1},
	{BIGMEDI_ITEM, 2, 0x320, 0, 0, 0, 2, TXT_Large_Medipack, -1},
	{SMALLMEDI_ITEM, 0, 0x200, 0, 0x5000, 0, 2, TXT_Small_Medipack, -1},
	{BINOCULARS_ITEM, -1, 0x2BC, 0x1000, 0x7D0, 0, 2, TXT_Binoculars, -1},
	{FLARE_INV_ITEM, 2, 0x44C, 0x4000, 0, 0, 2, TXT_Flares, -1},
	{COMPASS_ITEM, -14, 0x258, 0, 0x36B0, 0, 0, TXT_Compass, -1},
	{MEMCARD_LOAD_INV_ITEM, 2, 0xFA0, 0, 0, 0, 2, TXT_Load, -1},
	{MEMCARD_SAVE_INV_ITEM, 2, 0xFA0, 0, 0, 0, 2, TXT_Save, -1},

	//waterskins
	{WATERSKIN1_EMPTY, 2, 0x640, 0, 0xC950, 0, 2, TXT_Small_Waterskin_Empty, -1},
	{WATERSKIN1_1, 2, 0x640, 0, 0xC950, 0, 2, TXT_Small_Waterskin, -1},
	{WATERSKIN1_2, 2, 0x640, 0, 0xC950, 0, 2, TXT_Small_Waterskin2, -1},
	{WATERSKIN1_3, 2, 0x640, 0, 0xC950, 0, 2, TXT_Small_Waterskin3, -1},
	{WATERSKIN2_EMPTY, 2, 0x640, 0, 0xC950, 0, 2, TXT_Large_WaterSkin_Empty, -1},
	{WATERSKIN2_1, 2, 0x640, 0, 0xC950, 0, 2, TXT_Large_WaterSkin, -1},
	{WATERSKIN2_2, 2, 0x640, 0, 0xC950, 0, 2, TXT_Large_WaterSkin2, -1},
	{WATERSKIN2_3, 2, 0x640, 0, 0xC950, 0, 2, TXT_Large_WaterSkin3, -1},
	{WATERSKIN2_4, 2, 0x640, 0, 0xC950, 0, 2, TXT_Large_WaterSkin4, -1},
	{WATERSKIN2_5, 2, 0x640, 0, 0xC950, 0, 2, TXT_Large_WaterSkin5, -1},

	//puzzles and their combines
	{PUZZLE_ITEM1, 14, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{PUZZLE_ITEM2, 14, 0x258, 0, 0xC000, 0, 2, TXT_Load, -1},
	{PUZZLE_ITEM3, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{PUZZLE_ITEM4, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{PUZZLE_ITEM5, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{PUZZLE_ITEM6, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{PUZZLE_ITEM7, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{PUZZLE_ITEM8, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{PUZZLE_ITEM9, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{PUZZLE_ITEM10, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{PUZZLE_ITEM11, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{PUZZLE_ITEM12, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{PUZZLE_ITEM1_COMBO1, 18, 1200, 0, 0, 0, 2, TXT_Load, -1},
	{PUZZLE_ITEM1_COMBO2, 18, 1200, 0, 0, 0, 2, TXT_Load, -1},
	{PUZZLE_ITEM2_COMBO1, 8, 1200, 0, 0, 0, 2, TXT_Load, -1},
	{PUZZLE_ITEM2_COMBO2, 8, 1200, 0, 0, 0, 2, TXT_Load, -1},
	{PUZZLE_ITEM3_COMBO1, 8, 1200, 0, 0, 0, 2, TXT_Load, -1},
	{PUZZLE_ITEM3_COMBO2, 8, 1200, 0, 0, 0, 2, TXT_Load, -1},
	{PUZZLE_ITEM4_COMBO1, 8, 1200, 0, 0, 0, 2, TXT_Load, -1},
	{PUZZLE_ITEM4_COMBO2, 8, 1200, 0, 0, 0, 2, TXT_Load, -1},
	{PUZZLE_ITEM5_COMBO1, 8, 1200, 0, 0, 0, 2, TXT_Load, -1},
	{PUZZLE_ITEM5_COMBO2, 8, 1200, 0, 0, 0, 2, TXT_Load, -1},
	{PUZZLE_ITEM6_COMBO1, 8, 1200, 0, 0, 0, 2, TXT_Load, -1},
	{PUZZLE_ITEM6_COMBO2, 8, 1200, 0, 0, 0, 2, TXT_Load, -1},
	{PUZZLE_ITEM7_COMBO1, 8, 1200, 0, 0, 0, 2, TXT_Load, -1},
	{PUZZLE_ITEM7_COMBO2, 8, 1200, 0, 0, 0, 2, TXT_Load, -1},
	{PUZZLE_ITEM8_COMBO1, 8, 1200, 0, 0, 0, 2, TXT_Load, -1},
	{PUZZLE_ITEM8_COMBO2, 8, 1200, 0, 0, 0, 2, TXT_Load, -1},

	//keys and their combines
	{KEY_ITEM1, 14, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{KEY_ITEM2, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{KEY_ITEM3, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{KEY_ITEM4, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{KEY_ITEM5, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{KEY_ITEM6, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{KEY_ITEM7, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{KEY_ITEM8, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{KEY_ITEM9, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{KEY_ITEM10, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{KEY_ITEM11, 4, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{KEY_ITEM12, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{KEY_ITEM1_COMBO1, 18, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{KEY_ITEM1_COMBO2, 18, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{KEY_ITEM2_COMBO1, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{KEY_ITEM2_COMBO2, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{KEY_ITEM3_COMBO1, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{KEY_ITEM3_COMBO2, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{KEY_ITEM4_COMBO1, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{KEY_ITEM4_COMBO2, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{KEY_ITEM5_COMBO1, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{KEY_ITEM5_COMBO2, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{KEY_ITEM6_COMBO1, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{KEY_ITEM6_COMBO2, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{KEY_ITEM7_COMBO1, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{KEY_ITEM7_COMBO2, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{KEY_ITEM8_COMBO1, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{KEY_ITEM8_COMBO2, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},

	//pickup items and their combines
	{PICKUP_ITEM1, 14, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{PICKUP_ITEM2, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{PICKUP_ITEM3, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{PICKUP_ITEM4, 8, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{PICKUP_ITEM1_COMBO1, 14, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{PICKUP_ITEM1_COMBO2, 14, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{PICKUP_ITEM2_COMBO1, 14, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{PICKUP_ITEM2_COMBO2, 14, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{PICKUP_ITEM3_COMBO1, 14, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{PICKUP_ITEM3_COMBO2, 14, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{PICKUP_ITEM4_COMBO1, 14, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},
	{PICKUP_ITEM4_COMBO2, 14, 0x4B0, 0, 0, 0, 2, TXT_Load, -1},

	//quest items
	{QUEST_ITEM1, 0, 0x4B0, 0x8000, 0xC000, 0, 2, TXT_QuestItem1, -1},
	{QUEST_ITEM2, 0, 0x400, 0x8000, 0, 0, 2, TXT_QuestItem2, -1},
	{QUEST_ITEM3, 0, 0x500, 0xC000, 0, 0xC000, 2, TXT_QuestItem3, -1},
	{QUEST_ITEM4, 0, 0x500, 0x4000, 0, 0xC000, 2, TXT_QuestItem4, -1},
	{QUEST_ITEM5, 0, 0x300, 0, 0, 0x2000, 2, TXT_QuestItem5, -1},
	{QUEST_ITEM6, 0, 0x300, 0x8000, 0, 0x2000, 2, TXT_QuestItem6, -1},
	
	//the rest
	{BURNING_TORCH_ITEM, 14, 0x4B0, 0, 0x4000, 0, 2, TXT_Load, -1},
	{CROWBAR_ITEM, 4, 0x76C, 0, 0x4000, 0, 2, TXT_Crowbar, -1},
	{CLOCKWORK_BEETLE, 14, 0x2BC, 0x4000, 0, 0, 2, TXT_Mechanical_Scarab_With, -1},
	{CLOCKWORK_BEETLE_COMBO2, 14, 0x2BC, 0x4000, 0, 0, 2, TXT_Mechanical_Scarab, -1},
	{CLOCKWORK_BEETLE_COMBO1, 18, 0x2BC, 0x4000, 0, 0, 2, TXT_Winding_Key, -1},
	{EXAMINE1, 4, 0x514, 0, 0x4000, 0, 2, TXT_Load, -1},
	{EXAMINE2, 14, 0x4B0, 0, 0x4000, 0, 2, TXT_Load, -1},
	{EXAMINE3, 14, 0x4B0, 0, 0x4000, 0, 2, TXT_Load, -1}
};
#pragma warning(pop)

COMBINELIST dels_handy_combine_table[23] =
{
	{combine_revolver_lasersight, INV_REVOLVER_ITEM, INV_LASERSIGHT_ITEM, INV_REVOLVER_LASER_ITEM},
	{combine_crossbow_lasersight, INV_CROSSBOW_ITEM, INV_LASERSIGHT_ITEM, INV_CROSSBOW_LASER_ITEM},
	{combine_PuzzleItem1, INV_PUZZLE1_COMBO1_ITEM, INV_PUZZLE1_COMBO2_ITEM, INV_PUZZLE1_ITEM},
	{combine_PuzzleItem2, INV_PUZZLE2_COMBO1_ITEM, INV_PUZZLE2_COMBO2_ITEM, INV_PUZZLE2_ITEM},
	{combine_PuzzleItem3, INV_PUZZLE3_COMBO1_ITEM, INV_PUZZLE3_COMBO2_ITEM, INV_PUZZLE3_ITEM},
	{combine_PuzzleItem4, INV_PUZZLE4_COMBO1_ITEM, INV_PUZZLE4_COMBO2_ITEM, INV_PUZZLE4_ITEM},
	{combine_PuzzleItem5, INV_PUZZLE5_COMBO1_ITEM, INV_PUZZLE5_COMBO2_ITEM, INV_PUZZLE5_ITEM},
	{combine_PuzzleItem6, INV_PUZZLE6_COMBO1_ITEM, INV_PUZZLE6_COMBO2_ITEM, INV_PUZZLE6_ITEM},
	{combine_PuzzleItem7, INV_PUZZLE7_COMBO1_ITEM, INV_PUZZLE7_COMBO2_ITEM, INV_PUZZLE7_ITEM},
	{combine_PuzzleItem8, INV_PUZZLE8_COMBO1_ITEM, INV_PUZZLE8_COMBO2_ITEM, INV_PUZZLE8_ITEM},
	{combine_KeyItem1, INV_KEY1_COMBO1_ITEM, INV_KEY1_COMBO2_ITEM, INV_KEY1_ITEM},
	{combine_KeyItem2, INV_KEY2_COMBO1_ITEM, INV_KEY1_COMBO2_ITEM, INV_KEY2_ITEM},
	{combine_KeyItem3, INV_KEY3_COMBO1_ITEM, INV_KEY1_COMBO2_ITEM, INV_KEY3_ITEM},
	{combine_KeyItem4, INV_KEY4_COMBO1_ITEM, INV_KEY1_COMBO2_ITEM, INV_KEY4_ITEM},
	{combine_KeyItem5, INV_KEY5_COMBO1_ITEM, INV_KEY1_COMBO2_ITEM, INV_KEY5_ITEM},
	{combine_KeyItem6, INV_KEY6_COMBO1_ITEM, INV_KEY1_COMBO2_ITEM, INV_KEY6_ITEM},
	{combine_KeyItem7, INV_KEY7_COMBO1_ITEM, INV_KEY1_COMBO2_ITEM, INV_KEY7_ITEM},
	{combine_KeyItem8, INV_KEY8_COMBO1_ITEM, INV_KEY1_COMBO2_ITEM, INV_KEY8_ITEM},
	{combine_PickupItem1, INV_PICKUP1_COMBO1_ITEM, INV_PICKUP1_COMBO2_ITEM, INV_PICKUP1_ITEM},
	{combine_PickupItem2, INV_PICKUP2_COMBO1_ITEM, INV_PICKUP2_COMBO2_ITEM, INV_PICKUP2_ITEM},
	{combine_PickupItem3, INV_PICKUP3_COMBO1_ITEM, INV_PICKUP3_COMBO2_ITEM, INV_PICKUP3_ITEM},
	{combine_PickupItem4, INV_PICKUP4_COMBO1_ITEM, INV_PICKUP4_COMBO2_ITEM, INV_PICKUP4_ITEM},
	{combine_ClockWorkBeetle, INV_MECHANICAL_SCARAB_ITEM, INV_WINDING_KEY_ITEM, INV_CLOCKWORK_BEETLE_ITEM}
};

static short options_table[NUM_INVOBJ] =
{
	OPT_EQUIP | OPT_COMBINE | OPT_UZI,			//INV_UZI_ITEM
	OPT_EQUIP | OPT_COMBINE | OPT_PISTOLS,		//INV_PISTOLS_ITEM
	OPT_EQUIP | OPT_COMBINE | OPT_SHOTGUN,		//INV_SHOTGUN_ITEM
	OPT_EQUIP | OPT_COMBINE | OPT_REVOLVER,		//INV_REVOLVER_ITEM
	OPT_EQUIP | OPT_SEPARATE | OPT_REVOLVER,	//INV_REVOLVER_LASER_ITEM
	OPT_EQUIP | OPT_COMBINE | OPT_CROSSBOW,		//INV_CROSSBOW_ITEM
	OPT_EQUIP | OPT_SEPARATE | OPT_CROSSBOW,	//INV_CROSSBOW_LASER_ITEM
	OPT_EQUIP | OPT_COMBINE | OPT_GRENADE,		//INV_GRENADEGUN_ITEM
	OPT_USE,									//INV_SHOTGUN_AMMO1_ITEM
	OPT_USE,									//INV_SHOTGUN_AMMO2_ITEM
	OPT_USE,									//INV_GRENADEGUN_AMMO1_ITEM
	OPT_USE,									//INV_GRENADEGUN_AMMO2_ITEM
	OPT_USE,									//INV_GRENADEGUN_AMMO3_ITEM
	OPT_USE,									//INV_CROSSBOW_AMMO1_ITEM
	OPT_USE,									//INV_CROSSBOW_AMMO2_ITEM
	OPT_USE,									//INV_CROSSBOW_AMMO3_ITEM
	OPT_USE,									//INV_REVOLVER_AMMO_ITEM
	OPT_USE,									//INV_UZI_AMMO_ITEM
	OPT_USE,									//INV_PISTOLS_AMMO_ITEM
	OPT_USE | OPT_COMBINE,						//INV_LASERSIGHT_ITEM
	OPT_USE,									//INV_BIGMEDI_ITEM
	OPT_USE,									//INV_SMALLMEDI_ITEM
	OPT_USE,									//INV_BINOCULARS_ITEM
	OPT_USE,									//INV_FLARE_INV_ITEM
	OPT_USE,									//INV_COMPASS_ITEM
	OPT_LOAD,									//INV_MEMCARD_LOAD_ITEM
	OPT_SAVE,									//INV_MEMCARD_SAVE_ITEM
	OPT_USE | OPT_COMBINE,						//INV_WATERSKIN1_EMPTY_ITEM
	OPT_USE | OPT_COMBINE,						//INV_WATERSKIN1_1_ITEM
	OPT_USE | OPT_COMBINE,						//INV_WATERSKIN1_2_ITEM
	OPT_USE | OPT_COMBINE,						//INV_WATERSKIN1_3_ITEM
	OPT_USE | OPT_COMBINE,						//INV_WATERSKIN2_EMPTY_ITEM
	OPT_USE | OPT_COMBINE,						//INV_WATERSKIN2_1_ITEM
	OPT_USE | OPT_COMBINE,						//INV_WATERSKIN2_2_ITEM
	OPT_USE | OPT_COMBINE,						//INV_WATERSKIN2_3_ITEM
	OPT_USE | OPT_COMBINE,						//INV_WATERSKIN2_4_ITEM
	OPT_USE | OPT_COMBINE,						//INV_WATERSKIN2_5_ITEM
	OPT_USE,									//INV_PUZZLE1_ITEM
	OPT_USE,									//INV_PUZZLE2_ITEM
	OPT_USE,									//INV_PUZZLE3_ITEM
	OPT_USE,									//INV_PUZZLE4_ITEM
	OPT_USE,									//INV_PUZZLE5_ITEM
	OPT_USE,									//INV_PUZZLE6_ITEM
	OPT_USE,									//INV_PUZZLE7_ITEM
	OPT_USE,									//INV_PUZZLE8_ITEM
	OPT_USE,									//INV_PUZZLE9_ITEM
	OPT_USE,									//INV_PUZZLE10_ITEM
	OPT_USE,									//INV_PUZZLE11_ITEM
	OPT_USE,									//INV_PUZZLE12_ITEM
	OPT_USE | OPT_COMBINE,						//INV_PUZZLE1_COMBO1_ITEM
	OPT_USE | OPT_COMBINE,						//INV_PUZZLE1_COMBO2_ITEM
	OPT_USE | OPT_COMBINE,						//INV_PUZZLE2_COMBO1_ITEM
	OPT_USE | OPT_COMBINE,						//INV_PUZZLE2_COMBO2_ITEM
	OPT_USE | OPT_COMBINE,						//INV_PUZZLE3_COMBO1_ITEM
	OPT_USE | OPT_COMBINE,						//INV_PUZZLE3_COMBO2_ITEM
	OPT_USE | OPT_COMBINE,						//INV_PUZZLE4_COMBO1_ITEM
	OPT_USE | OPT_COMBINE,						//INV_PUZZLE4_COMBO2_ITEM
	OPT_USE | OPT_COMBINE,						//INV_PUZZLE5_COMBO1_ITEM
	OPT_USE | OPT_COMBINE,						//INV_PUZZLE5_COMBO2_ITEM
	OPT_USE | OPT_COMBINE,						//INV_PUZZLE6_COMBO1_ITEM
	OPT_USE | OPT_COMBINE,						//INV_PUZZLE6_COMBO2_ITEM
	OPT_USE | OPT_COMBINE,						//INV_PUZZLE7_COMBO1_ITEM
	OPT_USE | OPT_COMBINE,						//INV_PUZZLE7_COMBO2_ITEM
	OPT_USE | OPT_COMBINE,						//INV_PUZZLE8_COMBO1_ITEM
	OPT_USE | OPT_COMBINE,						//INV_PUZZLE8_COMBO2_ITEM
	OPT_USE,									//INV_KEY1_ITEM
	OPT_USE,									//INV_KEY2_ITEM
	OPT_USE,									//INV_KEY3_ITEM
	OPT_USE,									//INV_KEY4_ITEM
	OPT_USE,									//INV_KEY5_ITEM
	OPT_USE,									//INV_KEY6_ITEM
	OPT_USE,									//INV_KEY7_ITEM
	OPT_USE,									//INV_KEY8_ITEM
	OPT_USE,									//INV_KEY9_ITEM
	OPT_USE,									//INV_KEY10_ITEM
	OPT_USE,									//INV_KEY11_ITEM
	OPT_USE,									//INV_KEY12_ITEM
	OPT_USE | OPT_COMBINE,						//INV_KEY1_COMBO1_ITEM
	OPT_USE | OPT_COMBINE,						//INV_KEY1_COMBO2_ITEM
	OPT_USE | OPT_COMBINE,						//INV_KEY2_COMBO1_ITEM
	OPT_USE | OPT_COMBINE,						//INV_KEY2_COMBO2_ITEM
	OPT_USE | OPT_COMBINE,						//INV_KEY3_COMBO1_ITEM
	OPT_USE | OPT_COMBINE,						//INV_KEY3_COMBO2_ITEM
	OPT_USE | OPT_COMBINE,						//INV_KEY4_COMBO1_ITEM
	OPT_USE | OPT_COMBINE,						//INV_KEY4_COMBO2_ITEM
	OPT_USE | OPT_COMBINE,						//INV_KEY5_COMBO1_ITEM
	OPT_USE | OPT_COMBINE,						//INV_KEY5_COMBO2_ITEM
	OPT_USE | OPT_COMBINE,						//INV_KEY6_COMBO1_ITEM
	OPT_USE | OPT_COMBINE,						//INV_KEY6_COMBO2_ITEM
	OPT_USE | OPT_COMBINE,						//INV_KEY7_COMBO1_ITEM
	OPT_USE | OPT_COMBINE,						//INV_KEY7_COMBO2_ITEM
	OPT_USE | OPT_COMBINE,						//INV_KEY8_COMBO1_ITEM
	OPT_USE | OPT_COMBINE,						//INV_KEY8_COMBO2_ITEM
	OPT_USE,									//INV_PICKUP1_ITEM
	OPT_USE,									//INV_PICKUP2_ITEM
	OPT_USE,									//INV_PICKUP3_ITEM
	OPT_USE,									//INV_PICKUP4_ITEM
	OPT_USE | OPT_COMBINE,						//INV_PICKUP1_COMBO1_ITEM
	OPT_USE | OPT_COMBINE,						//INV_PICKUP1_COMBO2_ITEM
	OPT_USE | OPT_COMBINE,						//INV_PICKUP2_COMBO1_ITEM
	OPT_USE | OPT_COMBINE,						//INV_PICKUP2_COMBO2_ITEM
	OPT_USE | OPT_COMBINE,						//INV_PICKUP3_COMBO1_ITEM
	OPT_USE | OPT_COMBINE,						//INV_PICKUP3_COMBO2_ITEM
	OPT_USE | OPT_COMBINE,						//INV_PICKUP4_COMBO1_ITEM
	OPT_USE | OPT_COMBINE,						//INV_PICKUP4_COMBO2_ITEM
	OPT_USE,									//INV_QUEST1_ITEM
	OPT_USE,									//INV_QUEST2_ITEM
	OPT_USE,									//INV_QUEST3_ITEM
	OPT_USE,									//INV_QUEST4_ITEM
	OPT_USE,									//INV_QUEST5_ITEM
	OPT_USE,									//INV_QUEST6_ITEM
	OPT_USE,									//INV_BURNING_TORCH_ITEM
	OPT_USE,									//INV_CROWBAR_ITEM
	OPT_USE,									//INV_CLOCKWORK_BEETLE_ITEM
	OPT_USE | OPT_COMBINE,						//INV_MECHANICAL_SCARAB_ITEM
	OPT_USE | OPT_COMBINE,						//INV_WINDING_KEY_ITEM
	OPT_EXAMINE,								//INV_EXAMINE1_ITEM
	OPT_EXAMINE,								//INV_EXAMINE2_ITEM
	OPT_EXAMINE									//INV_EXAMINE3_ITEM
};

static short optmessages[9] = { TXT_USE, TXT_CHOOSE_AMMO, TXT_COMBINE, TXT_SEPERATE, TXT_EQUIP, TXT_COMBINE_WITH, TXT_LOAD_GAME, TXT_SAVE_GAME, TXT_EXAMINE };

long GLOBAL_enterinventory = NO_ITEM;
long GLOBAL_inventoryitemchosen = NO_ITEM;
long GLOBAL_lastinvitem = NO_ITEM;
long InventoryActive = 0;

static RINGME* rings[2];
static RINGME pcring1;
static RINGME pcring2;

static AMMOLIST ammo_object_list[3];
static MENUTHANG current_options[3];

static long compass_settle_thang;
static short examine_mode = 0;
static short stats_mode;
static uchar current_selected_option;
static char menu_active;
static char ammo_active;
static char oldLaraBusy;

static char* current_ammo_type;
static short AmountShotGunAmmo1 = 0;
static short AmountShotGunAmmo2 = 0;
static short AmountGrenadeAmmo1 = 0;
static short AmountGrenadeAmmo2 = 0;
static short AmountGrenadeAmmo3 = 0;
static short AmountCrossBowAmmo1 = 0;
static short AmountCrossBowAmmo2 = 0;
static short AmountCrossBowAmmo3 = 0;
static short AmountUziAmmo = 0;
static short AmountRevolverAmmo = 0;
static short AmountPistolsAmmo = 0;
static char CurrentPistolsAmmoType = 0;
static char CurrentUziAmmoType = 0;
static char CurrentRevolverAmmoType = 0;
static char CurrentShotGunAmmoType = 0;
static char CurrentGrenadeGunAmmoType = 0;
static char CurrentCrossBowAmmoType = 0;
static char StashedCurrentPistolsAmmoType = 0;
static char StashedCurrentUziAmmoType = 0;
static char StashedCurrentRevolverAmmoType = 0;
static char StashedCurrentShotGunAmmoType = 0;
static char StashedCurrentGrenadeGunAmmoType = 0;
static char StashedCurrentCrossBowAmmoType = 0;
static char Stashedcurrent_selected_option = 0;
static char num_ammo_slots;

static long combine_obj1;
static long combine_obj2;
static short ammo_selector_fade_val;
static short ammo_selector_fade_dir;
static short combine_ring_fade_val;
static short combine_ring_fade_dir;
static short normal_ring_fade_val;
static short normal_ring_fade_dir;
static char ammo_selector_flag;
static char combine_type_flag;
static char seperate_type_flag;

static long xoffset;
static long yoffset;
static long OBJLIST_SPACING;
static long pcbright = 0x7F7F7F;
static short inventry_xpos = 0;
static short inventry_ypos = 0;

static uchar go_left;
static uchar go_right;
static uchar go_up;
static uchar go_down;
static uchar go_select;
static uchar go_deselect;
static uchar left_repeat;
static uchar left_debounce;
static uchar right_repeat;
static uchar right_debounce;
static uchar up_debounce;
static uchar down_debounce;
static uchar select_debounce;
static uchar deselect_debounce;
static uchar friggrimmer;
static uchar friggrimmer2;
static char loading_or_saving;
static char use_the_bitch;

long LoadGame()
{
	if (S_LoadSave(IN_LOAD, 1, 1) < 0)
		return -1;
	else
		return 1;
}

long SaveGame()
{
	long Save;

	Save = S_LoadSave(IN_SAVE, 1, 1);
	input = 0;
	dbinput = 0;

	if (Save < 0)
		return -1;
	else
		return 1;
}

void init_new_inventry()
{
	compass_settle_thang = 4096;
	examine_mode = 0;
	stats_mode = 0;
	AlterFOV(14560);
	lara.Busy = 0;
	GLOBAL_inventoryitemchosen = -1;
	left_debounce = 0;
	right_debounce = 0;
	up_debounce = 0;
	down_debounce = 0;
	go_left = 0;
	go_right = 0;
	go_up = 0;
	go_down = 0;
	select_debounce = 0;
	deselect_debounce = 0;
	go_select = 0;
	go_deselect = 0;
	left_repeat = 0;
	right_repeat = 0;
	loading_or_saving = 0;
	use_the_bitch = 0;

	if (lara.num_shotgun_ammo1 == -1)
		AmountShotGunAmmo1 = -1;
	else
		AmountShotGunAmmo1 = lara.num_shotgun_ammo1 / 6;

	if (lara.num_shotgun_ammo2 == -1)
		AmountShotGunAmmo2 = -1;
	else
		AmountShotGunAmmo2 = lara.num_shotgun_ammo2 / 6;

	AmountGrenadeAmmo1 = lara.num_grenade_ammo1;
	AmountGrenadeAmmo2 = lara.num_grenade_ammo2;
	AmountGrenadeAmmo3 = lara.num_grenade_ammo3;
	AmountCrossBowAmmo1 = lara.num_crossbow_ammo1;
	AmountCrossBowAmmo2 = lara.num_crossbow_ammo2;
	AmountCrossBowAmmo3 = lara.num_crossbow_ammo3;
	AmountUziAmmo = lara.num_uzi_ammo;
	AmountRevolverAmmo = lara.num_revolver_ammo;
	AmountPistolsAmmo = lara.num_pistols_ammo;
	construct_object_list();

	if (GLOBAL_enterinventory == NO_ITEM)
	{
		if (GLOBAL_lastinvitem != NO_ITEM)
		{
			if (have_i_got_item((short)GLOBAL_lastinvitem))
				setup_objectlist_startposition((short)GLOBAL_lastinvitem);
			else
			{
				if (GLOBAL_lastinvitem >= INV_WATERSKIN1_EMPTY_ITEM && GLOBAL_lastinvitem <= INV_WATERSKIN1_3_ITEM)
				{
					for (int i = INV_WATERSKIN1_EMPTY_ITEM; i <= INV_WATERSKIN1_3_ITEM; i++)
					{
						if (have_i_got_item(i))
							setup_objectlist_startposition(i);
					}
				}
				else if (GLOBAL_lastinvitem >= INV_WATERSKIN2_EMPTY_ITEM && GLOBAL_lastinvitem <= INV_WATERSKIN2_5_ITEM)
				{
					for (int i = INV_WATERSKIN2_EMPTY_ITEM; i <= INV_WATERSKIN2_5_ITEM; i++)
					{
						if (have_i_got_item(i))
							setup_objectlist_startposition(i);
					}
				}
			}

			GLOBAL_lastinvitem = -1;
		}
	}
	else
	{
		if (have_i_got_object((short)GLOBAL_enterinventory))
			setup_objectlist_startposition2((short)GLOBAL_enterinventory);

		GLOBAL_enterinventory = -1;
	}

	ammo_selector_fade_val = 0;
	ammo_selector_fade_dir = 0;
	combine_ring_fade_val = 0;
	combine_ring_fade_dir = 0;
	combine_type_flag = 0;
	seperate_type_flag = 0;
	combine_obj1 = 0;
	combine_obj2 = 0;
	normal_ring_fade_val = 128;
	normal_ring_fade_dir = 0;
	handle_object_changeover(RING_INVENTORY);
}

void do_debounced_joystick_poo()
{
	go_left = 0;
	go_right = 0;
	go_up = 0;
	go_down = 0;
	go_select = 0;
	go_deselect = 0;

	if (input & IN_LEFT)
	{
		if (left_repeat >= 8)
			go_left = 1;
		else
			left_repeat++;

		if (!left_debounce)
			go_left = 1;

		left_debounce = 1;
	}
	else
	{
		left_debounce = 0;
		left_repeat = 0;
	}

	if (input & IN_RIGHT)
	{
		if (right_repeat >= 8)
			go_right = 1;
		else
			right_repeat++;

		if (!right_debounce)
			go_right = 1;

		right_debounce = 1;
	}
	else
	{
		right_debounce = 0;
		right_repeat = 0;
	}

	if (input & IN_FORWARD)
	{
		if (!up_debounce)
			go_up = 1;

		up_debounce = 1;
	}
	else
		up_debounce = 0;

	if (input & IN_BACK)
	{
		if (!down_debounce)
			go_down = 1;

		down_debounce = 1;
	}
	else
		down_debounce = 0;

	if (input & IN_ACTION || input & IN_SELECT)
		select_debounce = 1;
	else
	{
		if (select_debounce == 1 && !friggrimmer)
			go_select = 1;

		select_debounce = 0;
		friggrimmer = 0;
	}

	if (input & IN_DESELECT)
		deselect_debounce = 1;
	else
	{
		if (deselect_debounce == 1 && !friggrimmer2)
			go_deselect = 1;

		deselect_debounce = 0;
		friggrimmer2 = 0;
	}
}

void DrawInventoryItemMe(INVDRAWITEM* item, long shade, long overlay, long shagflag)
{
	ANIM_STRUCT* anim;
	OBJECT_INFO* object;
	PHD_VECTOR vec;
	short** meshpp;
	long* bone;
	short* rotation1;
	short* frmptr;
	ulong bit;
	long poppush, alpha, compass;

	anim = &anims[objects[item->object_number].anim_index];
	frmptr = anim->frame_ptr;
	object = &objects[item->object_number];
	phd_PushMatrix();
	phd_RotYXZ(item->yrot, item->xrot, item->zrot);

	if (item->object_number >= EXAMINE1 && item->object_number <= EXAMINE3 && examine_mode)
	{
		if (item->object_number == EXAMINE1)
		{
			vec.x = 7168;
			vec.y = 4608;
			vec.z = 4096;
			ScaleCurrentMatrix(&vec);
		}
		else if (item->object_number == EXAMINE2)
		{
			vec.x = 5760;
			vec.y = 5120;
			vec.z = 4096;
			ScaleCurrentMatrix(&vec);
		}
	}

	bit = 1;
	meshpp = &meshes[object->mesh_index];
	bone = &bones[object->bone_index];

	if (!shagflag)
		phd_TranslateRel(frmptr[6], frmptr[7], frmptr[8]);

	rotation1 = frmptr + 9;
	gar_RotYXZsuperpack(&rotation1, 0);

	if (item->mesh_bits & 1)
	{
		if (overlay)
			phd_PutPolygonsPickup(*meshpp, (float)xoffset, (float)yoffset, pcbright);
		else
		{
			alpha = GlobalAlpha;

			if (shade > 127)
				shade = 255;
			else
				shade <<= 1;

			GlobalAlpha = shade << 24;
			phd_PutPolygonsPickup(*meshpp, (float)xoffset, (float)yoffset, pcbright);
			GlobalAlpha = alpha;
		}
	}

	meshpp += 2;

	for (int i = 0; i < object->nmeshes - 1; i++, meshpp += 2, bone += 4)
	{
		poppush = *bone;

		if (poppush & 1)
			phd_PopMatrix();

		if (poppush & 2)
			phd_PushMatrix();

		phd_TranslateRel(bone[1], bone[2], bone[3]);
		gar_RotYXZsuperpack(&rotation1, 0);

		if (item->object_number == COMPASS_ITEM)
		{
			compass = (compass_settle_thang * phd_sin(1024 * (GnFrameCounter & 0x3F))) >> W2V_SHIFT;
			compass += lara_item->pos.y_rot;
			phd_RotY(short(compass - 0x8000));

			if (lara_item->pos.y_rot > -48 && lara_item->pos.y_rot <= 48 && tomb4.cheats)
			{
				shade = 96;

				if (rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem == INV_SMALLMEDI_ITEM)
				{
					if (keymap[DIK_G] && keymap[DIK_U] && keymap[DIK_N] && keymap[DIK_S])
					{
						lara.num_large_medipack = -1;
						lara.num_small_medipack = -1;

						if (!(gfLevelFlags & GF_YOUNGLARA))
						{
							lara.num_revolver_ammo = -1;
							lara.num_uzi_ammo = -1;
							lara.num_crossbow_ammo1 = -1;
							lara.num_crossbow_ammo2 = -1;
							lara.num_crossbow_ammo3 = -1;
							lara.num_grenade_ammo1 = -1;
							lara.num_grenade_ammo2 = -1;
							lara.num_grenade_ammo3 = -1;
							lara.num_flares = -1;
							lara.num_shotgun_ammo1 = -1;
							lara.num_shotgun_ammo2 = -1;

							lara.lasersight = 1;
							lara.pistols_type_carried |= W_PRESENT;
							lara.uzis_type_carried |= W_PRESENT;
							lara.shotgun_type_carried |= W_PRESENT;
							lara.crossbow_type_carried |= W_PRESENT;
							lara.grenade_type_carried |= W_PRESENT;
							lara.sixshooter_type_carried |= W_PRESENT;
						}
					}
				}

				if (rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem == INV_BIGMEDI_ITEM)
				{
					if (keymap[DIK_B] && keymap[DIK_I] && keymap[DIK_T] && keymap[DIK_S])
						dels_give_lara_items_cheat();
				}

				if (rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem == INV_MEMCARD_LOAD_ITEM)
				{
					if (keymap[DIK_H] && keymap[DIK_E] && keymap[DIK_L] && keymap[DIK_P])
					{
						if (gfCurrentLevel == 2 || gfCurrentLevel == 6 || gfCurrentLevel == 13 || gfCurrentLevel == 21 || gfCurrentLevel == 27)
							skipped_level = 1;

						gfRequiredStartPos = 0;
						gfLevelComplete = gfCurrentLevel + 1;
					}
				}

				if (rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem == INV_MEMCARD_SAVE_ITEM)
				{
					if (keymap[DIK_H] && keymap[DIK_E] && keymap[DIK_A] && keymap[DIK_L])
						lara_item->hit_points = 1000;
				}
			}
		}

		bit <<= 1;

		if (bit & item->mesh_bits)
		{
			if (overlay)
				phd_PutPolygonsPickup(*meshpp, (float)xoffset, (float)yoffset, pcbright);
			else
			{
				alpha = GlobalAlpha;

				if (shade > 127)
					shade = 255;
				else
					shade <<= 1;

				GlobalAlpha = shade << 24;
				phd_PutPolygonsPickup(*meshpp, (float)xoffset, (float)yoffset, pcbright);
				GlobalAlpha = alpha;
			}
		}
	}

	phd_PopMatrix();
}

void DrawThreeDeeObject2D(long x, long y, long num, long shade, long xrot, long yrot, long zrot, long bright, long overlay)
{
	INVOBJ* objme;
	INVDRAWITEM item;

	objme = &inventry_objects_list[num];
	item.xrot = (short)xrot + objme->xrot;
	item.yrot = (short)yrot + objme->yrot;
	item.zrot = (short)zrot + objme->zrot;
	item.object_number = objme->object_number;
	item.mesh_bits = objme->meshbits;
	phd_LookAt(0, 1024, 0, 0, 0, 0, 0);

	if (!bright)
		pcbright = 0x7F7F7F;
	else if (bright == 1)
		pcbright = 0x2F2F2F;
	else
		pcbright = RGBONLY(bright, bright, bright);

	SetD3DViewMatrix();
	phd_PushUnitMatrix();
	phd_TranslateRel(0, 0, objme->scale1);
	xoffset = x;
	yoffset = objme->yoff + y;
	DrawInventoryItemMe(&item, shade, overlay, objme->flags & 8);
	phd_PopMatrix();
	xoffset = phd_centerx;
	yoffset = phd_centery;
}

long go_and_load_game()
{
	return LoadGame();
}

long go_and_save_game()
{
	return SaveGame();
}

void insert_object_into_list_v2(short num)
{
	if (options_table[num] & 9)
	{
		if (rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem != num)
		{
			rings[RING_AMMO]->current_object_list[rings[RING_AMMO]->numobjectsinlist].invitem = num;
			rings[RING_AMMO]->current_object_list[rings[RING_AMMO]->numobjectsinlist].yrot = 0;
			rings[RING_AMMO]->current_object_list[rings[RING_AMMO]->numobjectsinlist].bright = 32;
			rings[RING_AMMO]->numobjectsinlist++;
		}
	}
}

void construct_combine_object_list()
{
	rings[RING_AMMO]->numobjectsinlist = 0;

	for (int i = 0; i < NUM_INVOBJ; i++)
		rings[RING_AMMO]->current_object_list[i].invitem = NO_ITEM;

	if (!(gfLevelFlags & GF_YOUNGLARA))
	{
		if (lara.sixshooter_type_carried & W_PRESENT)
		{
			if (lara.sixshooter_type_carried & W_LASERSIGHT)
				insert_object_into_list_v2(INV_REVOLVER_LASER_ITEM);
			else
				insert_object_into_list_v2(INV_REVOLVER_ITEM);
		}

		if (lara.crossbow_type_carried & W_PRESENT)
		{
			if (lara.crossbow_type_carried & W_LASERSIGHT)
				insert_object_into_list_v2(INV_CROSSBOW_LASER_ITEM);
			else
				insert_object_into_list_v2(INV_CROSSBOW_ITEM);
		}

		if (lara.lasersight)
			insert_object_into_list_v2(INV_LASERSIGHT_ITEM);
	}

	if (lara.small_water_skin)
		insert_object_into_list_v2(lara.small_water_skin + INV_WATERSKIN1_EMPTY_ITEM - 1);

	if (lara.big_water_skin)
		insert_object_into_list_v2(lara.big_water_skin + INV_WATERSKIN2_EMPTY_ITEM - 1);

	for (int i = 0; i < 16; i++)
	{
		if ((1 << i) & lara.puzzleitemscombo)
			insert_object_into_list_v2(i + INV_PUZZLE1_COMBO1_ITEM);
	}

	for (int i = 0; i < 16; i++)
	{
		if ((1 << i) & lara.keyitemscombo)
			insert_object_into_list_v2(i + INV_KEY1_COMBO1_ITEM);
	}

	for (int i = 0; i < 8; i++)
	{
		if ((1 << i) & lara.pickupitemscombo)
			insert_object_into_list_v2(i + INV_PICKUP1_COMBO1_ITEM);
	}

	if (lara.mechanical_scarab & 2)
		insert_object_into_list_v2(INV_WINDING_KEY_ITEM);

	if (lara.mechanical_scarab & 4)
		insert_object_into_list_v2(INV_MECHANICAL_SCARAB_ITEM);

	rings[RING_AMMO]->objlistmovement = 0;
	rings[RING_AMMO]->curobjinlist = 0;
	rings[RING_AMMO]->ringactive = 0;
}

void insert_object_into_list(short num)
{
	rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->numobjectsinlist].invitem = num;
	rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->numobjectsinlist].yrot = 0;
	rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->numobjectsinlist].bright = 32;
	rings[RING_INVENTORY]->numobjectsinlist++;
}

void construct_object_list()
{
	rings[RING_INVENTORY]->numobjectsinlist = 0;

	for (int i = 0; i < NUM_INVOBJ; i++)
		rings[RING_INVENTORY]->current_object_list[i].invitem = NO_ITEM;

	CurrentPistolsAmmoType = 0;
	CurrentUziAmmoType = 0;
	CurrentRevolverAmmoType = 0;
	CurrentShotGunAmmoType = 0;
	CurrentGrenadeGunAmmoType = 0;
	CurrentCrossBowAmmoType = 0;

	if (!(gfLevelFlags & GF_YOUNGLARA))
	{
		if (lara.pistols_type_carried & W_PRESENT)
			insert_object_into_list(INV_PISTOLS_ITEM);

		if (lara.uzis_type_carried & W_PRESENT)
			insert_object_into_list(INV_UZI_ITEM);
		else if (AmountUziAmmo)
			insert_object_into_list(INV_UZI_AMMO_ITEM);

		if (lara.sixshooter_type_carried & W_PRESENT)
		{
			if (lara.sixshooter_type_carried & W_LASERSIGHT)
				insert_object_into_list(INV_REVOLVER_LASER_ITEM);
			else
				insert_object_into_list(INV_REVOLVER_ITEM);
		}
		else if (AmountRevolverAmmo)
			insert_object_into_list(INV_REVOLVER_AMMO_ITEM);

		if (lara.shotgun_type_carried & W_PRESENT)
		{
			insert_object_into_list(INV_SHOTGUN_ITEM);

			if (lara.shotgun_type_carried & W_AMMO2)
				CurrentShotGunAmmoType = 1;
		}
		else
		{
			if (AmountShotGunAmmo1)
				insert_object_into_list(INV_SHOTGUN_AMMO1_ITEM);

			if (AmountShotGunAmmo2)
				insert_object_into_list(INV_SHOTGUN_AMMO2_ITEM);
		}

		if (lara.grenade_type_carried & W_PRESENT)
		{
			insert_object_into_list(INV_GRENADEGUN_ITEM);

			if (lara.grenade_type_carried & W_AMMO2)
				CurrentGrenadeGunAmmoType = 1;
			else if (lara.grenade_type_carried & W_AMMO3)
				CurrentGrenadeGunAmmoType = 2;
		}
		else
		{
			if (AmountGrenadeAmmo1)
				insert_object_into_list(INV_GRENADEGUN_AMMO1_ITEM);

			if (AmountGrenadeAmmo2)
				insert_object_into_list(INV_GRENADEGUN_AMMO2_ITEM);

			if (AmountGrenadeAmmo3)
				insert_object_into_list(INV_GRENADEGUN_AMMO3_ITEM);
		}

		if (lara.crossbow_type_carried & W_PRESENT)
		{
			if (lara.crossbow_type_carried & W_LASERSIGHT)
				insert_object_into_list(INV_CROSSBOW_LASER_ITEM);
			else
				insert_object_into_list(INV_CROSSBOW_ITEM);

			if (lara.crossbow_type_carried & W_AMMO2)
				CurrentCrossBowAmmoType = 1;
			else if (lara.crossbow_type_carried & W_AMMO3)
				CurrentCrossBowAmmoType = 2;
		}
		else
		{
			if (AmountCrossBowAmmo1)
				insert_object_into_list(INV_CROSSBOW_AMMO1_ITEM);

			if (AmountCrossBowAmmo2)
				insert_object_into_list(INV_CROSSBOW_AMMO2_ITEM);

			if (AmountCrossBowAmmo3)
				insert_object_into_list(INV_CROSSBOW_AMMO3_ITEM);
		}

		if (lara.lasersight)
			insert_object_into_list(INV_LASERSIGHT_ITEM);

		if (lara.binoculars)
			insert_object_into_list(INV_BINOCULARS_ITEM);

		if (lara.num_flares)
			insert_object_into_list(INV_FLARE_INV_ITEM);
	}

	if (lara.num_small_medipack)
		insert_object_into_list(INV_SMALLMEDI_ITEM);

	if (lara.num_large_medipack)
		insert_object_into_list(INV_BIGMEDI_ITEM);

	if (lara.small_water_skin)
		insert_object_into_list(lara.small_water_skin + INV_WATERSKIN1_EMPTY_ITEM - 1);

	if (lara.big_water_skin)
		insert_object_into_list(lara.big_water_skin + INV_WATERSKIN2_EMPTY_ITEM - 1);

	if (lara.crowbar)
		insert_object_into_list(INV_CROWBAR_ITEM);

	if (lara.mechanical_scarab & 1)
		insert_object_into_list(INV_CLOCKWORK_BEETLE_ITEM);

	if (lara.mechanical_scarab & 2)
		insert_object_into_list(INV_WINDING_KEY_ITEM);

	if (lara.mechanical_scarab & 4)
		insert_object_into_list(INV_MECHANICAL_SCARAB_ITEM);

	for (int i = 0; i < 12; i++)
	{
		if (lara.puzzleitems[i])
			insert_object_into_list(i + INV_PUZZLE1_ITEM);
	}

	for (int i = 0; i < 16; i++)
	{
		if ((1 << i) & lara.puzzleitemscombo)
			insert_object_into_list(i + INV_PUZZLE1_COMBO1_ITEM);
	}

	for (int i = 0; i < 12; i++)
	{
		if ((1 << i) & lara.keyitems)
			insert_object_into_list(i + INV_KEY1_ITEM);
	}

	for (int i = 0; i < 16; i++)
	{
		if ((1 << i) & lara.keyitemscombo)
			insert_object_into_list(i + INV_KEY1_COMBO1_ITEM);
	}

	for (int i = 0; i < 4; i++)
	{
		if ((1 << i) & lara.pickupitems)
			insert_object_into_list(i + INV_PICKUP1_ITEM);
	}

	for (int i = 0; i < 8; i++)
	{
		if ((1 << i) & lara.pickupitemscombo)
			insert_object_into_list(i + INV_PICKUP1_COMBO1_ITEM);
	}

	for (int i = 0; i < 6; i++)
	{
		if ((1 << i) & lara.questitems)
			insert_object_into_list(i + INV_QUEST1_ITEM);
	}

	if (lara.examine1)
		insert_object_into_list(INV_EXAMINE1_ITEM);

	if (lara.examine2)
		insert_object_into_list(INV_EXAMINE2_ITEM);

	if (lara.examine3)
		insert_object_into_list(INV_EXAMINE3_ITEM);

	if (Gameflow->LoadSaveEnabled)
	{
		insert_object_into_list(INV_MEMCARD_LOAD_ITEM);
		insert_object_into_list(INV_MEMCARD_SAVE_ITEM);
	}

	rings[RING_INVENTORY]->objlistmovement = 0;
	rings[RING_INVENTORY]->curobjinlist = 0;
	rings[RING_AMMO]->objlistmovement = 0;
	rings[RING_AMMO]->curobjinlist = 0;
	handle_object_changeover(RING_INVENTORY);
	rings[RING_INVENTORY]->ringactive = 1;
	rings[RING_AMMO]->ringactive = 0;
	ammo_active = 0;
}

void handle_object_changeover(long ringnum)
{
	long opts;
	char num;

	current_selected_option = 0;
	menu_active = 1;

	//inlined setup_ammo_selector

	num = 0;
	opts = options_table[rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem];
	ammo_selector_flag = 0;
	num_ammo_slots = 0;

	if (rings[RING_AMMO]->ringactive)
		return;

	ammo_object_list[0].yrot = 0;
	ammo_object_list[1].yrot = 0;
	ammo_object_list[2].yrot = 0;

	if (opts & 0xFC0)
	{
		ammo_selector_flag = 1;
		ammo_selector_fade_dir = 1;

		if (opts & 0x200)
		{
			ammo_object_list[0].invitem = INV_UZI_AMMO_ITEM;
			ammo_object_list[0].amount = AmountUziAmmo;
			num = 1;
			num_ammo_slots = 1;
			current_ammo_type = &CurrentUziAmmoType;
		}

		if (opts & 0x400)
		{
			ammo_object_list[0].invitem = INV_PISTOLS_AMMO_ITEM;
			ammo_object_list[0].amount = -1;
			num = 1;
			num_ammo_slots = 1;
			current_ammo_type = &CurrentPistolsAmmoType;
		}

		if (opts & 0x800)
		{
			ammo_object_list[0].invitem = INV_REVOLVER_AMMO_ITEM;
			ammo_object_list[0].amount = AmountRevolverAmmo;
			num = 1;
			num_ammo_slots = 1;
			current_ammo_type = &CurrentRevolverAmmoType;
		}

		if (opts & 0x80)
		{
			current_ammo_type = &CurrentCrossBowAmmoType;
			ammo_object_list[num].invitem = INV_CROSSBOW_AMMO1_ITEM;
			ammo_object_list[num].amount = AmountCrossBowAmmo1;
			num++;
			ammo_object_list[num].invitem = INV_CROSSBOW_AMMO2_ITEM;
			ammo_object_list[num].amount = AmountCrossBowAmmo2;
			num++;
			ammo_object_list[num].invitem = INV_CROSSBOW_AMMO3_ITEM;
			ammo_object_list[num].amount = AmountCrossBowAmmo3;
			num++;
			num_ammo_slots = num;
		}

		if (opts & 0x100)
		{
			current_ammo_type = &CurrentGrenadeGunAmmoType;
			ammo_object_list[num].invitem = INV_GRENADEGUN_AMMO1_ITEM;
			ammo_object_list[num].amount = AmountGrenadeAmmo1;
			num++;
			ammo_object_list[num].invitem = INV_GRENADEGUN_AMMO2_ITEM;
			ammo_object_list[num].amount = AmountGrenadeAmmo2;
			num++;
			ammo_object_list[num].invitem = INV_GRENADEGUN_AMMO3_ITEM;
			ammo_object_list[num].amount = AmountGrenadeAmmo3;
			num++;
			num_ammo_slots = num;
		}

		if (opts & 0x40)
		{
			current_ammo_type = &CurrentShotGunAmmoType;
			ammo_object_list[num].invitem = INV_SHOTGUN_AMMO1_ITEM;
			ammo_object_list[num].amount = AmountShotGunAmmo1;
			num++;
			ammo_object_list[num].invitem = INV_SHOTGUN_AMMO2_ITEM;
			ammo_object_list[num].amount = AmountShotGunAmmo2;
			num++;
			num_ammo_slots = num;
		}
	}
}

void fade_ammo_selector()
{
	if (rings[RING_INVENTORY]->ringactive && (right_repeat >= 8 || left_repeat >= 8))
		ammo_selector_fade_val = 0;
	else if (ammo_selector_fade_dir == 1)
	{
		if (ammo_selector_fade_val < 128)
			ammo_selector_fade_val += 32;

		if (ammo_selector_fade_val > 128)
		{
			ammo_selector_fade_val = 128;
			ammo_selector_fade_dir = 0;
		}
	}
	else if (ammo_selector_fade_dir == 2)
	{
		if (ammo_selector_fade_val > 0)
			ammo_selector_fade_val -= 32;

		if (ammo_selector_fade_val < 0)
		{
			ammo_selector_fade_val = 0;
			ammo_selector_fade_dir = 0;
		}
	}
}

void spinback(ushort* cock)
{
	ushort val, val2;

	val = *cock;

	if (val)
	{
		if (val <= 32768)
		{
			val2 = val;

			if (val2 < 1022)
				val = 1022;
			else if (val2 > 16384)
				val2 = 16384;

			val -= (val2 >> 3);

			if (val > 32768)
				val = 0;
		}
		else
		{
			val2 = -val;

			if (val2 < 1022)
				val = 1022;
			else if (val2 > 16384)
				val2 = 16384;

			val += (val2 >> 3);

			if (val < 32768)
				val = 0;
		}

		*cock = val;
	}
}

void update_laras_weapons_status()
{
	if (lara.shotgun_type_carried & W_PRESENT)
	{
		lara.shotgun_type_carried &= ~(W_AMMO1 | W_AMMO2 | W_AMMO3);

		if (CurrentShotGunAmmoType)
			lara.shotgun_type_carried |= W_AMMO2;
		else
			lara.shotgun_type_carried |= W_AMMO1;
	}

	if (lara.grenade_type_carried & W_PRESENT)
	{
		lara.grenade_type_carried &= ~(W_AMMO1 | W_AMMO2 | W_AMMO3);

		if (!CurrentGrenadeGunAmmoType)
			lara.grenade_type_carried |= W_AMMO1;
		else if (CurrentGrenadeGunAmmoType == 1)
			lara.grenade_type_carried |= W_AMMO2;
		else if (CurrentGrenadeGunAmmoType == 2)
			lara.grenade_type_carried |= W_AMMO3;
	}

	if (lara.crossbow_type_carried & W_PRESENT)
	{
		lara.crossbow_type_carried &= ~(W_AMMO1 | W_AMMO2 | W_AMMO3);

		if (!CurrentCrossBowAmmoType)
			lara.crossbow_type_carried |= W_AMMO1;
		else if (CurrentCrossBowAmmoType == 1)
			lara.crossbow_type_carried |= W_AMMO2;
		else if (CurrentCrossBowAmmoType == 2)
			lara.crossbow_type_carried |= W_AMMO3;
	}
}

long have_i_got_item(short obj)
{
	for (int i = 0; i < NUM_INVOBJ; i++)
	{
		if (rings[RING_INVENTORY]->current_object_list[i].invitem == obj)
			return 1;
	}

	return 0;
}

void combine_revolver_lasersight(long flag)
{
	if (flag)
	{
		lara.lasersight = 1;
		lara.sixshooter_type_carried &= ~W_LASERSIGHT;
	}
	else
	{
		lara.lasersight = 0;
		lara.sixshooter_type_carried |= W_LASERSIGHT;
	}

	if (lara.gun_status && lara.gun_type == WEAPON_REVOLVER)
	{
		undraw_pistol_mesh_right(WEAPON_REVOLVER);
		draw_pistol_meshes(WEAPON_REVOLVER);
	}
}

void combine_crossbow_lasersight(long flag)
{
	if (flag)
	{
		lara.lasersight = 1;
		lara.crossbow_type_carried &= ~W_LASERSIGHT;
	}
	else
	{
		lara.lasersight = 0;
		lara.crossbow_type_carried |= W_LASERSIGHT;
	}

	if (lara.gun_status && lara.gun_type == WEAPON_CROSSBOW)
	{
		undraw_shotgun_meshes(WEAPON_CROSSBOW);
		draw_shotgun_meshes(WEAPON_CROSSBOW);
	}
}

void combine_PuzzleItem1(long flag)
{
	lara.puzzleitemscombo &= ~3;
	lara.puzzleitems[0] = 1;
}

void combine_PuzzleItem2(long flag)
{
	lara.puzzleitemscombo &= ~0xC;
	lara.puzzleitems[1] = 1;
}

void combine_PuzzleItem3(long flag)
{
	lara.puzzleitemscombo &= ~0x30;
	lara.puzzleitems[2] = 1;
}

void combine_PuzzleItem4(long flag)
{
	lara.puzzleitemscombo &= ~0xC0;
	lara.puzzleitems[3] = 1;
}

void combine_PuzzleItem5(long flag)
{
	lara.puzzleitemscombo &= ~0x300;
	lara.puzzleitems[4] = 1;
}

void combine_PuzzleItem6(long flag)
{
	lara.puzzleitemscombo &= ~0xC00;
	lara.puzzleitems[5] = 1;
}

void combine_PuzzleItem7(long flag)
{
	lara.puzzleitemscombo &= ~0x3000;
	lara.puzzleitems[6] = 1;
}

void combine_PuzzleItem8(long flag)
{
	lara.puzzleitemscombo &= ~0xC000;
	lara.puzzleitems[7] = 1;
}

void combine_KeyItem1(long flag)
{
	lara.keyitems |= 1;
	lara.keyitemscombo &= ~3;
}

void combine_KeyItem2(long flag)
{
	lara.keyitems |= 2;
	lara.keyitemscombo &= ~0xC;
}

void combine_KeyItem3(long flag)
{
	lara.keyitems |= 4;
	lara.keyitemscombo &= ~0x30;
}

void combine_KeyItem4(long flag)
{
	lara.keyitems |= 8;
	lara.keyitemscombo &= ~0xC0;
}

void combine_KeyItem5(long flag)
{
	lara.keyitems |= 0x10;
	lara.keyitemscombo &= ~0x300;
}

void combine_KeyItem6(long flag)
{
	lara.keyitems |= 0x20;
	lara.keyitemscombo &= ~0xC00;
}

void combine_KeyItem7(long flag)
{
	lara.keyitems |= 0x40;
	lara.keyitemscombo &= ~0x3000;
}

void combine_KeyItem8(long flag)
{
	lara.keyitems |= 0x80;
	lara.keyitemscombo &= ~0xC000;
}

void combine_PickupItem1(long flag)
{
	lara.pickupitems |= 1;
	lara.pickupitemscombo &= ~3;
}

void combine_PickupItem2(long flag)
{
	lara.pickupitems |= 2;
	lara.pickupitemscombo &= ~0xC;
}

void combine_PickupItem3(long flag)
{
	lara.pickupitems |= 4;
	lara.pickupitemscombo &= ~0x30;
}

void combine_PickupItem4(long flag)
{
	lara.pickupitems |= 8;
	lara.pickupitemscombo &= ~0xC0;
}

void combine_ClockWorkBeetle(long flag)
{
	lara.mechanical_scarab = 1;
}

long do_special_waterskin_combine_bullshit(long flag)
{
	long lp;
	short small_liters, big_liters, small_capacity, big_capacity;

	small_liters = lara.small_water_skin - 1;
	big_liters = lara.big_water_skin - 1;
	small_capacity = 3 - small_liters;
	big_capacity = 5 - big_liters;

	if (flag)
	{
		if (lara.big_water_skin != 1 && small_capacity)
		{
			for (lp = big_liters; lp; lp--)
			{
				if (small_capacity)
				{
					big_liters--;
					small_liters++;
					small_capacity--;
				}
			}

			lara.small_water_skin = small_liters + 1;
			lara.big_water_skin = big_liters + 1;
			combine_obj1 = lara.small_water_skin + INV_WATERSKIN1_EMPTY_ITEM - 1;
			return 1;
		}
	}
	else if (lara.small_water_skin != 1 && big_capacity)
	{
		for (lp = small_liters; lp; lp--)
		{
			if (big_capacity)
			{
				small_liters--;
				big_liters++;
				big_capacity--;
			}
		}

		lara.small_water_skin = small_liters + 1;
		lara.big_water_skin = big_liters + 1;
		combine_obj1 = lara.big_water_skin + INV_WATERSKIN2_EMPTY_ITEM - 1;
		return 1;
	}

	return 0;
}

void setup_objectlist_startposition(short newobj)
{
	for (int i = 0; i < NUM_INVOBJ; i++)
	{
		if (rings[RING_INVENTORY]->current_object_list[i].invitem == newobj)
			rings[RING_INVENTORY]->curobjinlist = i;
	}
}

void setup_objectlist_startposition2(short newobj)
{
	for (int i = 0; i < NUM_INVOBJ; i++)
	{
		if (inventry_objects_list[rings[RING_INVENTORY]->current_object_list[i].invitem].object_number == newobj)
			rings[RING_INVENTORY]->curobjinlist = i;
	}
}

long have_i_got_object(short object_number)
{
	if (object_number >= PUZZLE_ITEM1_COMBO1 && object_number <= PUZZLE_ITEM8_COMBO2)
		return (lara.puzzleitemscombo >> (object_number - PUZZLE_ITEM1_COMBO1)) & 1;

	if (object_number >= PUZZLE_ITEM1 && object_number <= PUZZLE_ITEM12)
		return lara.puzzleitems[object_number - PUZZLE_ITEM1];

	if (object_number >= KEY_ITEM1_COMBO1 && object_number <= KEY_ITEM8_COMBO2)
		return (lara.keyitemscombo >> (object_number - KEY_ITEM1_COMBO1)) & 1;

	if (object_number >= KEY_ITEM1 && object_number <= KEY_ITEM12)
		return (lara.keyitems >> (object_number - KEY_ITEM1)) & 1;

	if (object_number >= PICKUP_ITEM1_COMBO1 && object_number <= PICKUP_ITEM4_COMBO2)
		return (lara.pickupitemscombo >> (object_number - PICKUP_ITEM1_COMBO1)) & 1;

	if (object_number >= PICKUP_ITEM1 && object_number <= PICKUP_ITEM4)
		return (lara.pickupitems >> (object_number - PICKUP_ITEM1)) & 1;

	if (object_number >= QUEST_ITEM1 && object_number <= QUEST_ITEM6)
		return (lara.questitems >> (object_number - QUEST_ITEM1)) & 1;

	if (object_number == CROWBAR_ITEM)
		return lara.crowbar;

	return 0;
}

void remove_inventory_item(short object_number)
{
	if (object_number >= PUZZLE_ITEM1_COMBO1 && object_number <= PUZZLE_ITEM8_COMBO2)
		lara.puzzleitemscombo &= ~(1 << (object_number - PUZZLE_ITEM1_COMBO1));
	else if (object_number >= PUZZLE_ITEM1 && object_number <= PUZZLE_ITEM12)
	{
		if (lara.puzzleitems[object_number - PUZZLE_ITEM1])
			lara.puzzleitems[object_number - PUZZLE_ITEM1] -= 1;
	}
	else if (object_number >= KEY_ITEM1_COMBO1 && object_number <= KEY_ITEM8_COMBO2)
		lara.keyitemscombo &= ~(1 << (object_number - KEY_ITEM1_COMBO1));
	else if (object_number >= KEY_ITEM1 && object_number <= KEY_ITEM11)	//why 11
		lara.keyitems &= ~(1 << (object_number - KEY_ITEM1));
	else if (object_number >= PICKUP_ITEM1_COMBO1 && object_number <= PICKUP_ITEM4_COMBO2)
		lara.pickupitemscombo &= ~(1 << (object_number - PICKUP_ITEM1_COMBO1));
	else if (object_number >= PICKUP_ITEM1 && object_number <= PICKUP_ITEM4)
		lara.pickupitems &= ~(1 << (object_number - PICKUP_ITEM1));
	else if (object_number >= QUEST_ITEM1 && object_number <= QUEST_ITEM6)
		lara.questitems &= ~(1 << (object_number - QUEST_ITEM1));
}

long convert_obj_to_invobj(short obj)
{
	for (int i = 0; i < NUM_INVOBJ; i++)
	{
		if (inventry_objects_list[i].object_number == obj)
			return i;
	}
		
	return INV_MEMCARD_LOAD_ITEM;
}

void draw_compass()
{
	DrawThreeDeeObject2D(long((float)phd_centerx / 256.0F * 60.0F + inventry_xpos), phd_centery + inventry_ypos, INV_COMPASS_ITEM, 128, 0, 0, 0, 0, 0);
}

void do_examine_mode()
{
	INVOBJ* objme;
	static long WANK_RULES_YPOS;
	static long WANK_SCROL_YPOS;
	short saved_scale, invitem;

	WANK_RULES_YPOS = font_height;
	WANK_SCROL_YPOS = font_height * 5;

	examine_mode += 8;

	if (examine_mode > 128)
		examine_mode = 128;

	invitem = rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem;
	objme = &inventry_objects_list[invitem];
	saved_scale = objme->scale1;

	switch (invitem)
	{
	case INV_EXAMINE1_ITEM:
		saved_scale = objme->scale1;
		objme->scale1 = 300;
		DrawThreeDeeObject2D(long(((float)phd_centerx / 256) * 256 + inventry_xpos), long(((float)phd_centery / 120 * 256 + inventry_ypos) / 2),
			INV_EXAMINE1_ITEM, examine_mode, 0x8000, 0x4000, 0x4000, 96, 0);
		objme->scale1 = saved_scale;
		break;

	case INV_EXAMINE2_ITEM:
		saved_scale = objme->scale1;
		objme->scale1 = 300;
		DrawThreeDeeObject2D(long(((float)phd_centerx / 256) * 256 + inventry_xpos), long(((float)phd_centery / 120 * 256 + inventry_ypos) / 2),
			INV_EXAMINE2_ITEM, examine_mode, 0, 0, 0, 0, 0);
		objme->scale1 = saved_scale;
		PrintString(phd_centerx, WANK_RULES_YPOS, 5, SCRIPT_TEXT(TXT_RULES1), FF_CENTER);
		PrintString(phd_centerx, WANK_RULES_YPOS + phd_winheight / 2, 5, SCRIPT_TEXT(TXT_RULES2), FF_CENTER);
		break;

	case INV_EXAMINE3_ITEM:
		saved_scale = objme->scale1;
		objme->scale1 = 400;
		DrawThreeDeeObject2D(long(((float)phd_centerx / 256) * 256 + inventry_xpos), long(((float)phd_centery / 120 * 256 + inventry_ypos) / 2 - 8),
			INV_EXAMINE3_ITEM, examine_mode, 0x8000, 0x4000, 0x4000, 96, 0);
		objme->scale1 = saved_scale;
		PrintString(phd_centerx, WANK_SCROL_YPOS, 8, SCRIPT_TEXT(TXT_PETEPOO), FF_CENTER);
		break;
	}

	if (go_deselect)
	{
		SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
		go_deselect = 0;
		examine_mode = 0;
	}
}

void dels_give_lara_items_cheat()
{
	long piss;

	if (objects[CROWBAR_ITEM].loaded)
		lara.crowbar = 1;

	if (objects[CLOCKWORK_BEETLE].loaded)
		lara.mechanical_scarab = 1;

	for (piss = 0; piss < 12; piss++)
	{
		if (objects[PUZZLE_ITEM1 + piss].loaded)
			lara.puzzleitems[piss] = 1;
	}

	for (piss = 0; piss < 12; piss++)
	{
		if (objects[KEY_ITEM1 + piss].loaded)
			lara.keyitems |= 1 << piss;
	}

	for (piss = 0; piss < 4; piss++)
	{
		if (objects[PICKUP_ITEM1 + piss].loaded)
			lara.pickupitems |= 1 << piss;
	}

	for (piss = 0; piss < 6; piss++)
	{
		if (objects[QUEST_ITEM1 + piss].loaded)
			lara.questitems |= 1 << piss;
	}

	if (objects[WATERSKIN1_EMPTY].loaded)
		lara.small_water_skin = 1;

	if (objects[WATERSKIN2_EMPTY].loaded)
		lara.big_water_skin = 1;

	lara.pickupitemscombo = 0;
	lara.puzzleitemscombo = 0;
	lara.keyitemscombo = 0;
}

void use_current_item()
{
	long OldBinocular;
	short invobject, gmeobject;

	OldBinocular = BinocularRange;
	oldLaraBusy = 0;
	BinocularRange = 0;
	lara_item->mesh_bits = -1;
	invobject = rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem;
	gmeobject = inventry_objects_list[invobject].object_number;

	if (lara.water_status == LW_ABOVE_WATER || lara.water_status == LW_WADE)
	{
		if (gmeobject == PISTOLS_ITEM)
		{
			lara.request_gun_type = WEAPON_PISTOLS;

			if (lara.gun_status == LG_NO_ARMS && lara.gun_type == WEAPON_PISTOLS)
				lara.gun_status = LG_DRAW_GUNS;

			return;
		}

		if (gmeobject == SHOTGUN_ITEM)
		{
			lara.request_gun_type = WEAPON_SHOTGUN;

			if (lara.gun_status == LG_NO_ARMS && lara.gun_type == WEAPON_SHOTGUN)
				lara.gun_status = LG_DRAW_GUNS;

			return;
		}

		if (gmeobject == UZI_ITEM)
		{
			lara.request_gun_type = WEAPON_UZI;

			if (lara.gun_status == LG_NO_ARMS && lara.gun_type == WEAPON_UZI)
				lara.gun_status = LG_DRAW_GUNS;

			return;
		}

		if (gmeobject == SIXSHOOTER_ITEM)
		{
			lara.request_gun_type = WEAPON_REVOLVER;

			if (lara.gun_status == LG_NO_ARMS && lara.gun_type == WEAPON_REVOLVER)
				lara.gun_status = LG_DRAW_GUNS;

			return;
		}

		if (gmeobject == GRENADE_GUN_ITEM)
		{
			lara.request_gun_type = WEAPON_GRENADE;

			if (lara.gun_status == LG_NO_ARMS && lara.gun_type == WEAPON_GRENADE)
				lara.gun_status = LG_DRAW_GUNS;

			return;
		}

		if (gmeobject == CROSSBOW_ITEM)
		{
			lara.request_gun_type = WEAPON_CROSSBOW;

			if (lara.gun_status == LG_NO_ARMS && lara.gun_type == WEAPON_CROSSBOW)
				lara.gun_status = LG_DRAW_GUNS;

			return;
		}
	}

	if (gmeobject == FLARE_INV_ITEM)
	{
		if (lara.gun_status == LG_NO_ARMS && lara_item->current_anim_state != AS_ALL4S && lara_item->current_anim_state != AS_CRAWL &&
			lara_item->current_anim_state != AS_ALL4TURNL && lara_item->current_anim_state != AS_ALL4TURNR &&
			lara_item->current_anim_state != AS_CRAWLBACK && lara_item->current_anim_state != AS_CRAWL2HANG)
		{
			if (lara.gun_type != WEAPON_FLARE)
			{
				input = IN_FLARE;
				LaraGun();
				input = 0;
			}
		}
		else
			SayNo();
	}
	else if (invobject == INV_BINOCULARS_ITEM)
	{
		if ((lara_item->current_anim_state == AS_STOP && lara_item->anim_number == ANIM_BREATH || lara.IsDucked && !(input & IN_DUCK)))
		{
			oldLaraBusy = 1;
			BinocularRange = 128;

			if (lara.gun_status != LG_NO_ARMS)
				lara.gun_status = LG_UNDRAW_GUNS;
		}

		if (OldBinocular)
			BinocularRange = OldBinocular;
		else
			BinocularOldCamera = camera.old_type;
	}
	else if (invobject == INV_SMALLMEDI_ITEM)
	{
		if ((lara_item->hit_points <= 0 || lara_item->hit_points >= 1000) && !lara.poisoned)
		{
			SayNo();
			return;
		}

		if (lara.num_small_medipack != -1)
			lara.num_small_medipack--;

		lara.dpoisoned = 0;
		lara_item->hit_points += 500;

		if (lara_item->hit_points > 1000)
			lara_item->hit_points = 1000;

		SoundEffect(SFX_MENU_MEDI, 0, SFX_ALWAYS);
		savegame.Game.HealthUsed++;
	}
	else  if (invobject == INV_BIGMEDI_ITEM)
	{
		if ((lara_item->hit_points <= 0 || lara_item->hit_points >= 1000) && !lara.poisoned)
		{
			SayNo();
			return;
		}

		if (lara.num_large_medipack != -1)
			lara.num_large_medipack--;

		lara.dpoisoned = 0;
		lara_item->hit_points += 1000;

		if (lara_item->hit_points > 1000)
			lara_item->hit_points = 1000;

		SoundEffect(SFX_MENU_MEDI, 0, SFX_ALWAYS);
		savegame.Game.HealthUsed++;
	}
	else
		GLOBAL_inventoryitemchosen = gmeobject;
}

void DEL_picked_up_object(short objnum)	//notice fallthroughs
{
	long lf;

	switch (objnum)
	{
	case WATERSKIN1_EMPTY:
		lara.small_water_skin = 1;
		break;

	case WATERSKIN2_EMPTY:
		lara.big_water_skin = 1;
		break;

	case UZI_ITEM:

		if (!(lara.uzis_type_carried & W_PRESENT))
			lara.uzis_type_carried = W_PRESENT | W_AMMO1;

	case UZI_AMMO_ITEM:

		if (lara.num_uzi_ammo != -1)
			lara.num_uzi_ammo += 30;

		break;

	case PISTOLS_ITEM:

		if (!(lara.pistols_type_carried & W_PRESENT))
			lara.pistols_type_carried = W_PRESENT | W_AMMO1;

		lara.num_pistols_ammo = -1;
		break;

	case SHOTGUN_ITEM:

		if (!(lara.shotgun_type_carried & W_PRESENT))
			lara.shotgun_type_carried = W_PRESENT | W_AMMO1;

	case SHOTGUN_AMMO1_ITEM:

		if (lara.num_shotgun_ammo1 != -1)
			lara.num_shotgun_ammo1 += 36;

		break;

	case SHOTGUN_AMMO2_ITEM:

		if (lara.num_shotgun_ammo2 != -1)
			lara.num_shotgun_ammo2 += 36;

		break;

	case SIXSHOOTER_ITEM:

		if (!(lara.sixshooter_type_carried & W_PRESENT))
			lara.sixshooter_type_carried = W_PRESENT | W_AMMO1;

	case SIXSHOOTER_AMMO_ITEM:

		if (lara.num_revolver_ammo != -1)
			lara.num_revolver_ammo += 6;

		break;

	case CROSSBOW_ITEM:

		if (!(lara.crossbow_type_carried & W_PRESENT))
			lara.crossbow_type_carried = W_PRESENT | W_AMMO1;

	case CROSSBOW_AMMO1_ITEM:

		if (lara.num_crossbow_ammo1 != -1)
			lara.num_crossbow_ammo1 += 10;

		break;

	case CROSSBOW_AMMO2_ITEM:

		if (lara.num_crossbow_ammo2 != -1)
			lara.num_crossbow_ammo2 += 10;

		break;

	case CROSSBOW_AMMO3_ITEM:

		if (lara.num_crossbow_ammo3 != -1)
			lara.num_crossbow_ammo3 += 10;

		break;

	case GRENADE_GUN_ITEM:

		if (!(lara.grenade_type_carried & W_PRESENT))
			lara.grenade_type_carried = W_PRESENT | W_AMMO1;

	case GRENADE_GUN_AMMO1_ITEM:

		if (lara.num_grenade_ammo1 != -1)
			lara.num_grenade_ammo1 += 4;

		break;

	case GRENADE_GUN_AMMO2_ITEM:

		if (lara.num_grenade_ammo2 != -1)
			lara.num_grenade_ammo2 += 4;

		break;

	case GRENADE_GUN_AMMO3_ITEM:

		if (lara.num_grenade_ammo3 != -1)
			lara.num_grenade_ammo3 += 4;

		break;

	case FLARE_INV_ITEM:

		if (lara.num_flares != -1)
			lara.num_flares += 12;

		break;

	case LASERSIGHT_ITEM:
		lf = (lara.pistols_type_carried | lara.uzis_type_carried | lara.shotgun_type_carried | lara.sixshooter_type_carried |
			lara.crossbow_type_carried | lara.grenade_type_carried) & W_LASERSIGHT;

		if (!lf)
			lara.lasersight = 1;

		break;

	case BIGMEDI_ITEM:

		if (lara.num_large_medipack != -1)
			lara.num_large_medipack++;

		break;

	case SMALLMEDI_ITEM:

		if (lara.num_small_medipack != -1)
			lara.num_small_medipack++;

		break;

	case BINOCULARS_ITEM:
		lara.binoculars = 1;
		break;

	case CROWBAR_ITEM:
		lara.crowbar = 1;
		break;

	case CLOCKWORK_BEETLE:
		lara.mechanical_scarab |= 1;
		break;

	case CLOCKWORK_BEETLE_COMBO1:
		lara.mechanical_scarab |= 2;
		break;

	case CLOCKWORK_BEETLE_COMBO2:
		lara.mechanical_scarab |= 4;
		break;

	case EXAMINE1:
		lara.examine1 = 1;
		break;

	case EXAMINE2:
		lara.examine2 = 1;
		break;

	case EXAMINE3:
		lara.examine3 = 1;
		break;

	default:

		if (objnum >= PUZZLE_ITEM1_COMBO1 && objnum <= PUZZLE_ITEM8_COMBO2)
			lara.puzzleitemscombo |= 1 << (objnum - PUZZLE_ITEM1_COMBO1);
		else if (objnum >= PUZZLE_ITEM1 && objnum <= PUZZLE_ITEM12)
			lara.puzzleitems[objnum - PUZZLE_ITEM1]++;
		else if (objnum >= KEY_ITEM1_COMBO1 && objnum <= KEY_ITEM8_COMBO2)
			lara.keyitemscombo |= 1 << (objnum - KEY_ITEM1_COMBO1);
		else if (objnum >= PICKUP_ITEM1_COMBO1 && objnum <= PICKUP_ITEM4_COMBO2)
			lara.pickupitemscombo |= 1 << (objnum - PICKUP_ITEM1_COMBO1);
		else if (objnum >= KEY_ITEM1 && objnum <= KEY_ITEM12)
			lara.keyitems |= 1 << (objnum - KEY_ITEM1);
		else if (objnum >= PICKUP_ITEM1 && objnum <= PICKUP_ITEM4)
			lara.pickupitems |= 1 << (objnum - PICKUP_ITEM1);
		else if (objnum >= QUEST_ITEM1 && objnum <= QUEST_ITEM6)
			lara.questitems |= 1 << (objnum - QUEST_ITEM1);
	}
}

long is_item_currently_combinable(short obj)
{
	if (obj < INV_WATERSKIN1_EMPTY_ITEM || obj > INV_WATERSKIN2_5_ITEM)
	{
		for (int i = 0; i < 23; i++)
		{
			if (dels_handy_combine_table[i].item1 == obj && have_i_got_item(dels_handy_combine_table[i].item2))
				return 1;

			if (dels_handy_combine_table[i].item2 == obj && have_i_got_item(dels_handy_combine_table[i].item1))
				return 1;
		}
	}
	else if (obj > INV_WATERSKIN1_3_ITEM)
	{
		for (int i = 0; i < 4; i++)
		{
			if (have_i_got_item(i + INV_WATERSKIN1_EMPTY_ITEM))
				return 1;
		}
	}
	else
	{
		for (int i = 0; i < 6; i++)
		{
			if (have_i_got_item(i + INV_WATERSKIN2_EMPTY_ITEM))
				return 1;
		}
	}

	return 0;
}

long do_these_objects_combine(long obj1, long obj2)
{
	for (int i = 0; i < 23; i++)
	{
		if (dels_handy_combine_table[i].item1 == obj1 && dels_handy_combine_table[i].item2 == obj2)
			return 1;

		if (dels_handy_combine_table[i].item1 == obj2 && dels_handy_combine_table[i].item2 == obj1)
			return 1;
	}

	return 0;
}

void combine_these_two_objects(short obj1, short obj2)
{
	long i;

	for (i = 0; i < 23; i++)
	{
		if (dels_handy_combine_table[i].item1 == obj1 &&
			dels_handy_combine_table[i].item2 == obj2)
			break;

		if (dels_handy_combine_table[i].item1 == obj2 &&
			dels_handy_combine_table[i].item2 == obj1)
			break;
	}

	dels_handy_combine_table[i].combine_routine(0);
	construct_object_list();
	setup_objectlist_startposition(dels_handy_combine_table[i].combined_item);
	handle_object_changeover(RING_INVENTORY);
}

void seperate_object(short obj)
{
	long i;

	for (i = 0; i < 23; i++)
	{
		if (dels_handy_combine_table[i].combined_item == obj)
			break;
	}

	dels_handy_combine_table[i].combine_routine(1);
	construct_object_list();
	setup_objectlist_startposition(dels_handy_combine_table[i].item1);
}

void draw_ammo_selector()
{
	INVOBJ* objme;
	long xpos;
	short yrot;
	char cunter[256];

	if (!ammo_selector_flag)
		return;

	xpos = (2 * phd_centerx - OBJLIST_SPACING) >> 1;

	if (num_ammo_slots == 2)
		xpos -= OBJLIST_SPACING / 2;
	else if (num_ammo_slots == 3)
		xpos -= OBJLIST_SPACING;

	for (int i = 0; i < num_ammo_slots; i++)
	{
		objme = &inventry_objects_list[ammo_object_list[i].invitem];

		if (i == *current_ammo_type)
		{
			if (objme->flags & 2)
				ammo_object_list[i].yrot += 1022;
		}
		else
			spinback(&ammo_object_list[i].yrot);

		yrot = ammo_object_list[i].yrot;

		if (i == *current_ammo_type)
		{
			if (ammo_object_list[i].amount == -1)
				sprintf(cunter, SCRIPT_TEXT(TXT_Unlimited_s), SCRIPT_TEXT(objme->objname));
			else
				sprintf(cunter, "%d x %s", ammo_object_list[i].amount, SCRIPT_TEXT(objme->objname));

			if (ammo_selector_fade_val)
				PrintString(phd_centerx, font_height + phd_centery + 2 * font_height - 9, 8, cunter, FF_CENTER);

			DrawThreeDeeObject2D(long((float)phd_centerx / 256.0F * 64.0F + inventry_xpos + xpos),
				long((float)phd_centery / 120.0F * 190.0F + inventry_ypos), ammo_object_list[i].invitem, ammo_selector_fade_val, 0, yrot, 0, 0, 0);
		}
		else
			DrawThreeDeeObject2D(long((float)phd_centerx / 256.0F * 64.0F + inventry_xpos + xpos),
				long((float)phd_centery / 120.0F * 190.0F + inventry_ypos), ammo_object_list[i].invitem, ammo_selector_fade_val, 0, yrot, 0, 1, 0);

		xpos += OBJLIST_SPACING;
	}
}

void handle_inventry_menu()
{
	long num, opts, ypos, type;
	short inv_item, ammo_item;

	if (rings[RING_AMMO]->ringactive)
	{
		PrintString(phd_centerx, phd_centery, 1, SCRIPT_TEXT(optmessages[5]), FF_CENTER);

		if (rings[RING_INVENTORY]->objlistmovement || rings[RING_AMMO]->objlistmovement)
			return;

		if (go_select)
		{
			inv_item = rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem;
			ammo_item = rings[RING_AMMO]->current_object_list[rings[RING_AMMO]->curobjinlist].invitem;

			if (inv_item >= INV_WATERSKIN1_EMPTY_ITEM && inv_item <= INV_WATERSKIN1_3_ITEM &&	//small one selected
				ammo_item >= INV_WATERSKIN2_EMPTY_ITEM && ammo_item <= INV_WATERSKIN2_5_ITEM)	//combining with big one
			{
				if (do_special_waterskin_combine_bullshit(0))
				{
					combine_type_flag = 2;
					combine_ring_fade_dir = 2;
					SoundEffect(SFX_MENU_COMBINE, 0, SFX_ALWAYS);
				}
			}
			else if (ammo_item >= INV_WATERSKIN1_EMPTY_ITEM && ammo_item <= INV_WATERSKIN1_3_ITEM &&	//big one selected
				inv_item >= INV_WATERSKIN2_EMPTY_ITEM && inv_item <= INV_WATERSKIN2_5_ITEM)				//combining with small one
			{
				if (do_special_waterskin_combine_bullshit(1))
				{
					combine_type_flag = 2;
					combine_ring_fade_dir = 2;
					SoundEffect(SFX_MENU_COMBINE, 0, SFX_ALWAYS);
				}
			}
			else if (do_these_objects_combine(inv_item, ammo_item))
			{
				combine_ring_fade_dir = 2;
				combine_type_flag = 1;
				combine_obj1 = inv_item;
				combine_obj2 = ammo_item;
				SoundEffect(SFX_MENU_COMBINE, 0, SFX_ALWAYS);
			}
			else
			{
				SayNo();
				combine_ring_fade_dir = 2;
			}
		}

		if (go_deselect)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			combine_ring_fade_dir = 2;
			go_deselect = 0;
		}

		return;
	}

	inv_item = rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem;

	for (int i = 0; i < 3; i++)
	{
		current_options[i].type = 0;
		current_options[i].text = 0;
	}

	num = 0;
	opts = options_table[inv_item];

	if (ammo_active)
	{
		current_options[num].type = 6;
		current_options[num].text = SCRIPT_TEXT(inventry_objects_list[ammo_object_list[num].invitem].objname);
		num++;
		current_options[num].type = 7;
		current_options[num].text = SCRIPT_TEXT(inventry_objects_list[ammo_object_list[num].invitem].objname);
		num++;

		if (opts & 0x80 || opts & 0x100)
		{
			current_options[num].type = 8;
			current_options[num].text = SCRIPT_TEXT(inventry_objects_list[ammo_object_list[num].invitem].objname);
			num++;
		}

		current_selected_option = *current_ammo_type;
	}
	else
	{
		if (opts & 0x1000)
		{
			current_options[num].type = 9;
			current_options[num].text = SCRIPT_TEXT(optmessages[6]);
			num++;
		}

		if (opts & 0x2000)
		{
			current_options[num].type = 10;
			current_options[num].text = SCRIPT_TEXT(optmessages[7]);
			num++;
		}

		if (opts & 0x20)
		{
			current_options[num].type = 11;
			current_options[num].text = SCRIPT_TEXT(optmessages[8]);
			num++;
		}

		if (opts & 4)
		{
			current_options[num].type = 1;
			current_options[num].text = SCRIPT_TEXT(optmessages[0]);
			num++;
		}

		if (opts & 2)
		{
			current_options[num].type = 5;
			current_options[num].text = SCRIPT_TEXT(optmessages[4]);
			num++;
		}

		if (opts & 0xC0 || opts & 0x100)
		{
			current_options[num].type = 2;
			current_options[num].text = SCRIPT_TEXT(optmessages[1]);
			num++;
		}

		if (opts & 8)
		{
			if (is_item_currently_combinable(inv_item))
			{
				current_options[num].type = 3;
				current_options[num].text = SCRIPT_TEXT(optmessages[2]);
				num++;
			}
		}

		if (opts & 1)
		{
			current_options[num].type = 3;
			current_options[num].text = SCRIPT_TEXT(optmessages[2]);
			num++;
		}

		if (opts & 0x10)
		{
			current_options[num].type = 4;
			current_options[num].text = SCRIPT_TEXT(optmessages[3]);
			num++;
		}
	}
	
	ypos = phd_centery - font_height;

	if (num == 1)
		ypos += font_height;
	else if (num == 2)
		ypos += font_height >> 1;

	for (int i = 0; i < num; i++)
	{
		if (i == current_selected_option)
			PrintString(phd_centerx, ypos, 1, current_options[i].text, FF_CENTER);
		else
			PrintString(phd_centerx, ypos, 5, current_options[i].text, FF_CENTER);

		ypos += font_height;
	}

	if (menu_active)
	{
		if (rings[RING_INVENTORY]->objlistmovement || rings[RING_AMMO]->objlistmovement)
			return;

		if (go_up && current_selected_option)
		{
			current_selected_option--;
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
		}

		if (go_down && current_selected_option < num - 1)
		{
			current_selected_option++;
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
		}

		if (ammo_active)
		{
			if (go_left && current_selected_option > 0)
			{
				current_selected_option--;
				SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			}

			if (go_right && current_selected_option < num - 1)
			{
				current_selected_option++;
				SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			}

			*current_ammo_type = current_selected_option;
		}

		if (go_select)
		{
			type = current_options[current_selected_option].type;

			if (type != 5 && type != 1)
				SoundEffect(SFX_MENU_CHOOSE, 0, SFX_ALWAYS);

			switch (type)
			{
			case 2:
				rings[RING_INVENTORY]->ringactive = 0;
				ammo_active = 1;
				Stashedcurrent_selected_option = current_selected_option;
				StashedCurrentPistolsAmmoType = CurrentPistolsAmmoType;
				StashedCurrentUziAmmoType = CurrentUziAmmoType;
				StashedCurrentRevolverAmmoType = CurrentRevolverAmmoType;
				StashedCurrentShotGunAmmoType = CurrentShotGunAmmoType;
				StashedCurrentGrenadeGunAmmoType = CurrentGrenadeGunAmmoType;
				StashedCurrentCrossBowAmmoType = CurrentCrossBowAmmoType;
				break;

			case 9:
				loading_or_saving = 1;
				break;

			case 10:
				loading_or_saving = 2;
				break;

			case 11:
				examine_mode = 1;
				stats_mode = 0;
				break;

			case 6:
			case 7:
			case 8:
				ammo_active = 0;
				rings[RING_INVENTORY]->ringactive = 1;
				current_selected_option = 0;
				break;

			case 3:
				construct_combine_object_list();
				rings[RING_INVENTORY]->ringactive = 0;
				rings[RING_AMMO]->ringactive = 1;
				ammo_selector_flag = 0;
				menu_active = 0;
				combine_ring_fade_dir = 1;
				break;

			case 4:
				seperate_type_flag = 1;
				normal_ring_fade_dir = 2;
				break;

			case 5:
			case 1:
				menu_active = 0;
				use_the_bitch = 1;
				break;
			}
		}

		if (go_deselect)
		{
			if (ammo_active)
			{
				SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
				go_deselect = 0;
				ammo_active = 0;
				rings[RING_INVENTORY]->ringactive = 1;
				CurrentPistolsAmmoType = StashedCurrentPistolsAmmoType;
				CurrentUziAmmoType = StashedCurrentUziAmmoType;
				CurrentRevolverAmmoType = StashedCurrentRevolverAmmoType;
				CurrentShotGunAmmoType = StashedCurrentShotGunAmmoType;
				CurrentGrenadeGunAmmoType = StashedCurrentGrenadeGunAmmoType;
				CurrentCrossBowAmmoType = StashedCurrentCrossBowAmmoType;
				current_selected_option = Stashedcurrent_selected_option;
			}
		}
	}
}

void draw_current_object_list(long ringnum)
{
	INVOBJ* objme;
	long n, maxobj, xoff, shade, minobj, objmeup, nummeup, activenum;
	short ymeup, yrot;
	char textbufme[128];

	if (rings[ringnum]->numobjectsinlist <= 0)
		return;

	if (ringnum == RING_AMMO)
	{
		ammo_selector_fade_val = 0;
		ammo_selector_fade_dir = 0;

		if (combine_ring_fade_dir == 1)
		{
			if (combine_ring_fade_val < 128)
				combine_ring_fade_val += 32;

			if (combine_ring_fade_val > 128)
			{
				combine_ring_fade_val = 128;
				combine_ring_fade_dir = 0;
			}
		}
		else if (combine_ring_fade_dir == 2)
		{
			if (combine_ring_fade_val > 0)
				combine_ring_fade_val -= 32;

			if (combine_ring_fade_val <= 0)
			{
				combine_ring_fade_val = 0;
				combine_ring_fade_dir = 0;

				if (combine_type_flag)
					normal_ring_fade_dir = 2;
				else
				{
					rings[RING_INVENTORY]->ringactive = 1;
					menu_active = 1;
					rings[RING_AMMO]->ringactive = 0;
					handle_object_changeover(0);
				}

				rings[RING_AMMO]->ringactive = 0;
			}
		}
	}
	else 
	{
		if (normal_ring_fade_dir == 1)
		{
			if (normal_ring_fade_val < 128)
				normal_ring_fade_val += 32;

			if (normal_ring_fade_val > 128)
			{
				normal_ring_fade_val = 128;
				normal_ring_fade_dir = 0;
				rings[RING_INVENTORY]->ringactive = 1;
				menu_active = 1;
			}
		}
		else if (normal_ring_fade_dir == 2)
		{
			if (normal_ring_fade_val > 0)
				normal_ring_fade_val -= 32;

			if (normal_ring_fade_val <= 0)
			{
				normal_ring_fade_val = 0;
				normal_ring_fade_dir = 1;

				if (combine_type_flag == 1)
				{
					combine_type_flag = 0;
					combine_these_two_objects((short)combine_obj1, (short)combine_obj2);
				}
				else if (combine_type_flag == 2)
				{
					combine_type_flag = 0;
					construct_object_list();
					setup_objectlist_startposition((short)combine_obj1);
				}
				else if (seperate_type_flag)
					seperate_object(rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem);

				handle_object_changeover(RING_INVENTORY);
			}
		}
	}

	minobj = 0;
	maxobj = 0;
	xoff = 0;
	n = 0;

	if (rings[ringnum]->numobjectsinlist != 1)
		xoff = (OBJLIST_SPACING * rings[ringnum]->objlistmovement) >> 16;

	if (rings[ringnum]->numobjectsinlist == 2)
	{
		minobj = -1;
		maxobj = 0;
		n = rings[ringnum]->curobjinlist - 1;
	}

	if (rings[ringnum]->numobjectsinlist == 3 || rings[ringnum]->numobjectsinlist == 4)
	{
		minobj = -2;
		maxobj = 1;
		n = rings[ringnum]->curobjinlist - 2;
	}

	if (rings[ringnum]->numobjectsinlist >= 5)
	{
		minobj = -3;
		maxobj = 2;
		n = rings[ringnum]->curobjinlist - 3;
	}

	if (n < 0)
		n += rings[ringnum]->numobjectsinlist;

	if (rings[ringnum]->objlistmovement < 0)
		maxobj++;

	for (int i = minobj; i <= maxobj; i++)
	{
		if (minobj == i)
		{
			if (rings[ringnum]->objlistmovement < 0)
				shade = 0;
			else
				shade = rings[ringnum]->objlistmovement >> 9;
		}
		else if (i == minobj + 1 && maxobj != minobj + 1)
		{
			if (rings[ringnum]->objlistmovement < 0)
				shade = 128 - ((-128 * rings[ringnum]->objlistmovement) >> 16);
			else
				shade = 128;
		}
		else if (i == maxobj)
		{
			if (rings[ringnum]->objlistmovement < 0)
				shade = (-128 * rings[ringnum]->objlistmovement) >> 16;
			else
				shade = 128 - (short)(rings[ringnum]->objlistmovement >> 9);
		}
		else
			shade = 128;

		if (!minobj && !maxobj)
			shade = 128;

		if (ringnum == RING_AMMO && combine_ring_fade_val < 128 && shade)
			shade = combine_ring_fade_val;
		else if (ringnum == RING_INVENTORY && normal_ring_fade_val < 128 && shade)
			shade = normal_ring_fade_val;

		objme = &inventry_objects_list[rings[ringnum]->current_object_list[n].invitem];

		if (!i)
		{
			nummeup = 0;

			switch (objme->object_number)
			{
			case BIGMEDI_ITEM:
				nummeup = lara.num_large_medipack;
				break;

			case SMALLMEDI_ITEM:
				nummeup = lara.num_small_medipack;
				break;

			case FLARE_INV_ITEM:
				nummeup = lara.num_flares;
				break;

			case SHOTGUN_AMMO1_ITEM:
				nummeup = lara.num_shotgun_ammo1;

				if (lara.num_shotgun_ammo1 != -1)
					nummeup = lara.num_shotgun_ammo1 / 6;

				break;

			case SHOTGUN_AMMO2_ITEM:
				nummeup = lara.num_shotgun_ammo2;

				if (lara.num_shotgun_ammo2 != -1)
					nummeup = lara.num_shotgun_ammo2 / 6;

				break;

			case GRENADE_GUN_AMMO1_ITEM:
				nummeup = lara.num_grenade_ammo1;
				break;

			case GRENADE_GUN_AMMO2_ITEM:
				nummeup = lara.num_grenade_ammo2;
				break;

			case GRENADE_GUN_AMMO3_ITEM:
				nummeup = lara.num_grenade_ammo3;
				break;

			case CROSSBOW_AMMO1_ITEM:
				nummeup = lara.num_crossbow_ammo1;
				break;

			case CROSSBOW_AMMO2_ITEM:
				nummeup = lara.num_crossbow_ammo2;
				break;

			case CROSSBOW_AMMO3_ITEM:
				nummeup = lara.num_crossbow_ammo3;
				break;

			case SIXSHOOTER_AMMO_ITEM:
				nummeup = lara.num_revolver_ammo;
				break;

			case UZI_AMMO_ITEM:
				nummeup = lara.num_uzi_ammo;
				break;

			default:

				if (objme->object_number >= PUZZLE_ITEM1 && objme->object_number <= PUZZLE_ITEM12)
				{
					nummeup = lara.puzzleitems[objme->object_number - PUZZLE_ITEM1];

					if (nummeup <= 1)	//don't show "1 x puzzle item"
						nummeup = 0;
				}
			}

			if (tomb4.hpbar_inv && (objme->object_number == SMALLMEDI_ITEM || objme->object_number == BIGMEDI_ITEM))
				S_DrawHealthBar(lara_item->hit_points / 10);

			if (nummeup)
			{
				if (nummeup == -1)
					sprintf(textbufme, SCRIPT_TEXT(TXT_Unlimited_s), SCRIPT_TEXT(objme->objname));
				else
					sprintf(textbufme, "%d x %s", nummeup, SCRIPT_TEXT(objme->objname));
			}
			else
				sprintf(textbufme, SCRIPT_TEXT(objme->objname));

			if (ringnum == RING_INVENTORY)
				objmeup = long(phd_centery - float(phd_winymax + 1) / 16.0F * 3.0F);
			else
				objmeup = long(phd_centery + float(phd_winymax + 1) / 16.0F * 3.0F);

			PrintString(phd_centerx, objmeup, 8, textbufme, FF_CENTER);
		}

		if (!i && !rings[ringnum]->objlistmovement)
		{
			if (objme->flags & 2)
				rings[ringnum]->current_object_list[n].yrot += 1022;
		}
		else
			spinback(&rings[ringnum]->current_object_list[n].yrot);

		yrot = rings[ringnum]->current_object_list[n].yrot;
		ymeup = ringnum == RING_INVENTORY ? 42 : 190;

		if (rings[ringnum]->objlistmovement)
			activenum = rings[ringnum]->objlistmovement <= 0 ? 1 : -1;
		else
			activenum = 0;

		if (i == activenum)
		{
			if (rings[ringnum]->current_object_list[n].bright < 160)
				rings[ringnum]->current_object_list[n].bright += 16;

			if (rings[ringnum]->current_object_list[n].bright > 160)
				rings[ringnum]->current_object_list[n].bright = 160;
		}
		else
		{
			if (rings[ringnum]->current_object_list[n].bright > 32)
				rings[ringnum]->current_object_list[n].bright -= 16;

			if (rings[ringnum]->current_object_list[n].bright < 32)
				rings[ringnum]->current_object_list[n].bright = 32;
		}

		DrawThreeDeeObject2D(long((((float)phd_centerx / 256.0F) * 256 + inventry_xpos) + xoff + i * OBJLIST_SPACING),
			long((float)phd_centery / 120.0F * ymeup + inventry_ypos),
			rings[ringnum]->current_object_list[n].invitem,
			shade, 0, yrot, 0, rings[ringnum]->current_object_list[n].bright, 0);

		n++;

		if (n >= rings[ringnum]->numobjectsinlist)
			n = 0;
	}

	if (rings[ringnum]->ringactive)
	{
		if (rings[ringnum]->numobjectsinlist != 1)
		{
			if (ringnum == RING_AMMO)
			{
				if (combine_ring_fade_val != 128)
					return;
			}

			if (rings[ringnum]->objlistmovement > 0)
				rings[ringnum]->objlistmovement += 8192;

			if (rings[ringnum]->objlistmovement < 0)
				rings[ringnum]->objlistmovement -= 8192;

			if (go_left)
			{
				if (!rings[ringnum]->objlistmovement)
				{
					SoundEffect(SFX_MENU_ROTATE, 0, SFX_ALWAYS);
					rings[ringnum]->objlistmovement += 8192;

					if (ammo_selector_flag)
						ammo_selector_fade_dir = 2;
				}
			}

			if (go_right)
			{
				if (!rings[ringnum]->objlistmovement)
				{
					SoundEffect(SFX_MENU_ROTATE, 0, SFX_ALWAYS);
					rings[ringnum]->objlistmovement -= 8192;

					if (ammo_selector_flag)
						ammo_selector_fade_dir = 2;
				}
			}

			if (rings[ringnum]->objlistmovement >= 65536)
			{
				rings[ringnum]->curobjinlist--;

				if (rings[ringnum]->curobjinlist < 0)
					rings[ringnum]->curobjinlist = rings[ringnum]->numobjectsinlist - 1;

				rings[ringnum]->objlistmovement = 0;

				if (ringnum == RING_INVENTORY)
					handle_object_changeover(0);
			}
			else if (rings[ringnum]->objlistmovement < -65535)
			{
				rings[ringnum]->curobjinlist++;

				if (rings[ringnum]->curobjinlist >= rings[ringnum]->numobjectsinlist)
					rings[ringnum]->curobjinlist = 0;

				rings[ringnum]->objlistmovement = 0;

				if (ringnum == RING_INVENTORY)
					handle_object_changeover(0);
			}
		}
	}
}

long S_CallInventory2()
{
	long val, return_value, flag;

	val = 0;
	oldLaraBusy = lara.Busy;
	friggrimmer = (input & IN_SELECT) != 0;
	rings[RING_INVENTORY] = &pcring1;
	rings[RING_AMMO] = &pcring2;
	CreateMonoScreen();
	InventoryActive = 1;
	init_new_inventry();
	camera.number_frames = 2;
	return_value = 0;
	S_SetReverbType(1);

	while (!reset_flag && !val)
	{
		OBJLIST_SPACING = phd_centerx >> 1;

		if (compass_settle_thang != 1024)
			compass_settle_thang -= 32;

		S_InitialisePolyList();
		SetDebounce = 1;
		S_UpdateInput();
		input = inputBusy;
		UpdatePulseColour();
		GameTimer++;

		if (dbinput & IN_OPTION)
		{
			SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
			val = 1;
		}

		if (MainThread.ended)
			return MainThread.ended;

		S_DisplayMonoScreen();
		do_debounced_joystick_poo();

		if (examine_mode)
			do_examine_mode();
		else
		{
			draw_current_object_list(RING_INVENTORY);
			handle_inventry_menu();

			if (rings[RING_AMMO]->ringactive)
				draw_current_object_list(RING_AMMO);

			draw_ammo_selector();
			fade_ammo_selector();
			draw_compass();
		}

		if (use_the_bitch && !input)
			val = 1;

		S_OutputPolyList();
		camera.number_frames = S_DumpScreen();

		if (loading_or_saving)
		{
			while (1)
			{
				flag = 0;
				S_InitialisePolyList();
				SetDebounce = 1;
				S_UpdateInput();
				input = inputBusy;
				UpdatePulseColour();

				if (loading_or_saving == 1)
					flag = go_and_load_game();
				else if (go_and_save_game())
					flag = 1;

				if (flag == 1)
				{
					if (loading_or_saving == flag)
					{
						return_value = 1;
						val = 1;
					}

					break;
				}
				else if (flag)
					break;
			}

			friggrimmer2 = 1;
			friggrimmer = 1;
			deselect_debounce = 0;
			go_deselect = 0;
			loading_or_saving = 0;
		}
	}

	InitialisePickUpDisplay();
	GLOBAL_lastinvitem = rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem;
	update_laras_weapons_status();

	if (use_the_bitch)
		use_current_item();

	FreeMonoScreen();
	InventoryActive = 0;
	lara.Busy = oldLaraBusy;
	return return_value;
}
