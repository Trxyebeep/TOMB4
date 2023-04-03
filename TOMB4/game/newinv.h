#pragma once
#include "../global/types.h"

long LoadGame();
long SaveGame();
void init_new_inventry();
void do_debounced_joystick_poo();
void DrawInventoryItemMe(INVDRAWITEM* item, long shade, long overlay, long shagflag);
void DrawThreeDeeObject2D(long x, long y, long num, long shade, long xrot, long yrot, long zrot, long bright, long overlay);
long go_and_load_game();
long go_and_save_game();
void insert_object_into_list_v2(short num);
void construct_combine_object_list();
void insert_object_into_list(short num);
void construct_object_list();
void handle_object_changeover(long ringnum);
void fade_ammo_selector();
void spinback(ushort* cock);
void update_laras_weapons_status();
long have_i_got_item(short obj);
void combine_revolver_lasersight(long flag);
void combine_crossbow_lasersight(long flag);
void combine_PuzzleItem1(long flag);
void combine_PuzzleItem2(long flag);
void combine_PuzzleItem3(long flag);
void combine_PuzzleItem4(long flag);
void combine_PuzzleItem5(long flag);
void combine_PuzzleItem6(long flag);
void combine_PuzzleItem7(long flag);
void combine_PuzzleItem8(long flag);
void combine_KeyItem1(long flag);
void combine_KeyItem2(long flag);
void combine_KeyItem3(long flag);
void combine_KeyItem4(long flag);
void combine_KeyItem5(long flag);
void combine_KeyItem6(long flag);
void combine_KeyItem7(long flag);
void combine_KeyItem8(long flag);
void combine_PickupItem1(long flag);
void combine_PickupItem2(long flag);
void combine_PickupItem3(long flag);
void combine_PickupItem4(long flag);
void combine_ClockWorkBeetle(long flag);
long do_special_waterskin_combine_bullshit(long flag);
void setup_objectlist_startposition(short newobj);
void setup_objectlist_startposition2(short newobj);
long have_i_got_object(short object_number);
void remove_inventory_item(short object_number);
long convert_obj_to_invobj(short obj);
void draw_compass();
void do_examine_mode();
void dels_give_lara_items_cheat();
void use_current_item();
void DEL_picked_up_object(short objnum);
long is_item_currently_combinable(short obj);
long do_these_objects_combine(long obj1, long obj2);
void combine_these_two_objects(short obj1, short obj2);
void seperate_object(short obj);
void draw_ammo_selector();
void handle_inventry_menu();
void draw_current_object_list(long ringnum);
long S_CallInventory2();

extern INVOBJ inventry_objects_list[];
extern long GLOBAL_enterinventory;
extern long GLOBAL_inventoryitemchosen;
extern long GLOBAL_lastinvitem;
extern long InventoryActive;

enum ring_types
{
	RING_INVENTORY,
	RING_AMMO
};

enum option_types
{
	OPT_UNUSED =		0x1,
	OPT_EQUIP =			0x2,
	OPT_USE =			0x4,
	OPT_COMBINE =		0x8,
	OPT_SEPARATE =		0x10,
	OPT_EXAMINE =		0x20,
	OPT_SHOTGUN =		0x40,
	OPT_CROSSBOW =		0x80,
	OPT_GRENADE =		0x100,
	OPT_UZI =			0x200,
	OPT_PISTOLS =		0x400,
	OPT_REVOLVER =		0x800,
	OPT_LOAD =			0x1000,
	OPT_SAVE =			0x2000
};

enum invobj_types
{
	INV_UZI_ITEM = 0,
	INV_PISTOLS_ITEM,
	INV_SHOTGUN_ITEM,
	INV_REVOLVER_ITEM,
	INV_REVOLVER_LASER_ITEM,
	INV_CROSSBOW_ITEM,
	INV_CROSSBOW_LASER_ITEM,
	INV_GRENADEGUN_ITEM,
	INV_SHOTGUN_AMMO1_ITEM,
	INV_SHOTGUN_AMMO2_ITEM,
	INV_GRENADEGUN_AMMO1_ITEM,
	INV_GRENADEGUN_AMMO2_ITEM,
	INV_GRENADEGUN_AMMO3_ITEM,
	INV_CROSSBOW_AMMO1_ITEM,
	INV_CROSSBOW_AMMO2_ITEM,
	INV_CROSSBOW_AMMO3_ITEM,
	INV_REVOLVER_AMMO_ITEM,
	INV_UZI_AMMO_ITEM,
	INV_PISTOLS_AMMO_ITEM,
	INV_LASERSIGHT_ITEM,
	INV_BIGMEDI_ITEM,
	INV_SMALLMEDI_ITEM,
	INV_BINOCULARS_ITEM,
	INV_FLARE_INV_ITEM,
	INV_COMPASS_ITEM,
	INV_MEMCARD_LOAD_ITEM,
	INV_MEMCARD_SAVE_ITEM,
	INV_WATERSKIN1_EMPTY_ITEM,
	INV_WATERSKIN1_1_ITEM,
	INV_WATERSKIN1_2_ITEM,
	INV_WATERSKIN1_3_ITEM,
	INV_WATERSKIN2_EMPTY_ITEM,
	INV_WATERSKIN2_1_ITEM,
	INV_WATERSKIN2_2_ITEM,
	INV_WATERSKIN2_3_ITEM,
	INV_WATERSKIN2_4_ITEM,
	INV_WATERSKIN2_5_ITEM,
	INV_PUZZLE1_ITEM,
	INV_PUZZLE2_ITEM,
	INV_PUZZLE3_ITEM,
	INV_PUZZLE4_ITEM,
	INV_PUZZLE5_ITEM,
	INV_PUZZLE6_ITEM,
	INV_PUZZLE7_ITEM,
	INV_PUZZLE8_ITEM,
	INV_PUZZLE9_ITEM,
	INV_PUZZLE10_ITEM,
	INV_PUZZLE11_ITEM,
	INV_PUZZLE12_ITEM,
	INV_PUZZLE1_COMBO1_ITEM,
	INV_PUZZLE1_COMBO2_ITEM,
	INV_PUZZLE2_COMBO1_ITEM,
	INV_PUZZLE2_COMBO2_ITEM,
	INV_PUZZLE3_COMBO1_ITEM,
	INV_PUZZLE3_COMBO2_ITEM,
	INV_PUZZLE4_COMBO1_ITEM,
	INV_PUZZLE4_COMBO2_ITEM,
	INV_PUZZLE5_COMBO1_ITEM,
	INV_PUZZLE5_COMBO2_ITEM,
	INV_PUZZLE6_COMBO1_ITEM,
	INV_PUZZLE6_COMBO2_ITEM,
	INV_PUZZLE7_COMBO1_ITEM,
	INV_PUZZLE7_COMBO2_ITEM,
	INV_PUZZLE8_COMBO1_ITEM,
	INV_PUZZLE8_COMBO2_ITEM,
	INV_KEY1_ITEM,
	INV_KEY2_ITEM,
	INV_KEY3_ITEM,
	INV_KEY4_ITEM,
	INV_KEY5_ITEM,
	INV_KEY6_ITEM,
	INV_KEY7_ITEM,
	INV_KEY8_ITEM,
	INV_KEY9_ITEM,
	INV_KEY10_ITEM,
	INV_KEY11_ITEM,
	INV_KEY12_ITEM,
	INV_KEY1_COMBO1_ITEM,
	INV_KEY1_COMBO2_ITEM,
	INV_KEY2_COMBO1_ITEM,
	INV_KEY2_COMBO2_ITEM,
	INV_KEY3_COMBO1_ITEM,
	INV_KEY3_COMBO2_ITEM,
	INV_KEY4_COMBO1_ITEM,
	INV_KEY4_COMBO2_ITEM,
	INV_KEY5_COMBO1_ITEM,
	INV_KEY5_COMBO2_ITEM,
	INV_KEY6_COMBO1_ITEM,
	INV_KEY6_COMBO2_ITEM,
	INV_KEY7_COMBO1_ITEM,
	INV_KEY7_COMBO2_ITEM,
	INV_KEY8_COMBO1_ITEM,
	INV_KEY8_COMBO2_ITEM,
	INV_PICKUP1_ITEM,
	INV_PICKUP2_ITEM,
	INV_PICKUP3_ITEM,
	INV_PICKUP4_ITEM,
	INV_PICKUP1_COMBO1_ITEM,
	INV_PICKUP1_COMBO2_ITEM,
	INV_PICKUP2_COMBO1_ITEM,
	INV_PICKUP2_COMBO2_ITEM,
	INV_PICKUP3_COMBO1_ITEM,
	INV_PICKUP3_COMBO2_ITEM,
	INV_PICKUP4_COMBO1_ITEM,
	INV_PICKUP4_COMBO2_ITEM,
	INV_QUEST1_ITEM,
	INV_QUEST2_ITEM,
	INV_QUEST3_ITEM,
	INV_QUEST4_ITEM,
	INV_QUEST5_ITEM,
	INV_QUEST6_ITEM,
	INV_BURNING_TORCH_ITEM,
	INV_CROWBAR_ITEM,
	INV_CLOCKWORK_BEETLE_ITEM,
	INV_MECHANICAL_SCARAB_ITEM,
	INV_WINDING_KEY_ITEM,
	INV_EXAMINE1_ITEM,
	INV_EXAMINE2_ITEM,
	INV_EXAMINE3_ITEM,

	NUM_INVOBJ
};
