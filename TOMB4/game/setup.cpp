#include "../tomb4/pch.h"
#include "setup.h"
#include "objects.h"
#include "pickup.h"
#include "collide.h"
#include "init.h"
#include "elements.h"
#include "ahmet.h"
#include "effect2.h"
#include "effects.h"
#include "clockworkbeetle.h"

void ObjectObjects()
{
	OBJECT_INFO* obj;

	obj = &objects[CAMERA_TARGET];
	obj->using_drawanimating_item = 0;
	obj->draw_routine = 0;

	obj = &objects[FLARE_ITEM];
	obj->initialise = 0;
//	obj->control = FlareControl;
	obj->collision = PickUpCollision;
//	obj->draw_routine = DrawFlareInAir;
	obj->using_drawanimating_item = 0;
	obj->pivot_length = 256;
	obj->hit_points = 256;
	obj->save_position = 1;
	obj->save_flags = 1;

	for (int i = SMASH_OBJECT1; i <= SMASH_OBJECT8; i++)
	{
		obj = &objects[i];
	//	obj->initialise = InitialiseSmashObject;
	//	obj->control = SmashObjectControl;
		obj->collision = ObjectCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	obj = &objects[BRIDGE_FLAT];
	obj->floor = BridgeFlatFloor;
	obj->ceiling = BridgeFlatCeiling;

	obj = &objects[BRIDGE_TILT1];
	obj->floor = BridgeTilt1Floor;
	obj->ceiling = BridgeTilt1Ceiling;

	obj = &objects[BRIDGE_TILT2];
	obj->floor = BridgeTilt2Floor;
	obj->ceiling = BridgeTilt2Ceiling;

	for (int i = SWITCH_TYPE1; i <= SWITCH_TYPE6; i++)
	{
		obj = &objects[i];
	//	obj->initialise = InitialiseSwitch;
	//	obj->control = SwitchControl;
	//	obj->collision = SwitchCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	for (int i = SEQUENCE_SWITCH1; i <= SEQUENCE_SWITCH3; i++)
	{
		obj = &objects[i];
	//	obj->control = FullBlockSwitchControl;
	//	obj->collision = FullBlockSwitchCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	obj = &objects[UNDERWATER_SWITCH1];
//	obj->control = SwitchControl;
//	obj->collision = SwitchCollision2;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[UNDERWATER_SWITCH2];
//	obj->control = SwitchControl;
//	obj->collision = UnderwaterSwitchCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[TURN_SWITCH];
//	obj->control = TurnSwitchControl;
//	obj->collision = TurnSwitchCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[COG_SWITCH];
//	obj->control = CogSwitchControl;
//	obj->collision = CogSwitchCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[LEVER_SWITCH];
//	obj->control = SwitchControl;
//	obj->collision = RailSwitchCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[JUMP_SWITCH];
//	obj->control = SwitchControl;
//	obj->collision = JumpSwitchCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[CROWBAR_SWITCH];
//	obj->control = SwitchControl;
//	obj->collision = CrowbarSwitchCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[PULLEY];
//	obj->initialise = InitialisePulley;
//	obj->control = SwitchControl;
//	obj->collision = PulleyCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	for (int i = DOOR_TYPE1; i <= DOOR_TYPE8; i++)	//skips over MIPs
	{
		obj = &objects[i];
	//	obj->initialise = InitialiseDoor;
	//	obj->control = DoorControl;
	//	obj->collision = DoorCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	obj = &objects[UNDERWATER_DOOR];
//	obj->initialise = InitialiseDoor;
//	obj->control = PushPullKickDoorControl;
//	obj->collision = UnderwaterDoorCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[DOUBLE_DOORS];
//	obj->initialise = InitialiseDoor;
//	obj->control = PushPullKickDoorControl;
//	obj->collision = DoubleDoorCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[SEQUENCE_DOOR1];
//	obj->initialise = InitialiseDoor;
//	obj->control = SequenceDoorControl;
//	obj->collision = DoorCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	for (int i = PUSHPULL_DOOR1; i <= KICK_DOOR2; i++)
	{
		obj = &objects[i];
	//	obj->initialise = InitialiseDoor;
	//	obj->control = PushPullKickDoorControl;
	//	obj->collision = PushPullKickDoorCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	for (int i = FLOOR_TRAPDOOR1; i <= FLOOR_TRAPDOOR2; i++)
	{
		obj = &objects[i];
	//	obj->initialise = InitialiseTrapDoor;
	//	obj->control = TrapDoorControl;
	//	obj->collision = FloorTrapDoorCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	for (int i = CEILING_TRAPDOOR1; i <= CEILING_TRAPDOOR2; i++)
	{
		obj = &objects[i];
	//	obj->initialise = InitialiseTrapDoor;
	//	obj->control = TrapDoorControl;
	//	obj->collision = CeilingTrapDoorCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	for (int i = TRAPDOOR1; i <= TRAPDOOR3; i++)
	{
		obj = &objects[i];
	//	obj->initialise = InitialiseTrapDoor;
	//	obj->control = TrapDoorControl;
	//	obj->collision = TrapDoorCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	for (int i = PUZZLE_ITEM1; i <= SECRET_MAP; i++)
	{
		obj = &objects[i];
	//	obj->initialise = InitialisePickup;
	//	obj->control = AnimatingPickUp;
		obj->collision = PickUpCollision;
		obj->save_position = 1;
		obj->save_flags = 1;
	}

	for (int i = PISTOLS_ITEM; i <= BINOCULARS_ITEM; i++)
	{
		obj = &objects[i];
	//	obj->initialise = InitialisePickup;
	//	obj->control = AnimatingPickUp;
		obj->collision = PickUpCollision;
		obj->save_position = 1;
		obj->save_flags = 1;
	}

	obj = &objects[BURNING_TORCH_ITEM];
	obj->initialise = 0;
//	obj->control = FlameTorchControl;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[WATERSKIN1_EMPTY];
//	obj->initialise = InitialisePickup;
//	obj->control = AnimatingPickUp;
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[WATERSKIN2_EMPTY];
//	obj->initialise = InitialisePickup;
//	obj->control = AnimatingPickUp;
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[CROSSBOW_BOLT];
	obj->initialise = 0;
//	obj->control = ControlCrossbow;
	obj->collision = 0;
//	obj->draw_routine = DrawWeaponMissile;
	obj->using_drawanimating_item = 0;

	obj = &objects[GRENADE];
	obj->initialise = 0;
//	obj->control = ControlCrossbow;
	obj->collision = 0;
//	obj->draw_routine = DrawWeaponMissile;
	obj->using_drawanimating_item = 0;

	obj = &objects[FLARE_INV_ITEM];
//	obj->initialise = InitialisePickup;
//	obj->control = AnimatingPickUp;
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[COMPASS_ITEM];
//	obj->initialise = InitialisePickup;
//	obj->control = AnimatingPickUp;
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	for (int i = KEY_HOLE1; i <= KEY_HOLE12; i++)
	{
		obj = &objects[i];
	//	obj->collision = KeyHoleCollision;
		obj->save_flags = 1;
	}

	for (int i = PUZZLE_HOLE1; i <= PUZZLE_HOLE12; i++)
	{
		obj = &objects[i];
	//	obj->control = ControlAnimatingSlots;
	//	obj->collision = PuzzleHoleCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	obj = &objects[SARCOPHAGUS];
//	obj->control = ControlAnimatingSlots;
	obj->collision = SarcophagusCollision;


	for (int i = ANIMATING1; i <= ANIMATING12; i++)
	{
		obj = &objects[i];
	//	obj->control = ControlAnimatingSlots;
		obj->collision = ObjectCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	for (int i = ANIMATING13; i <= ANIMATING16_MIP; i++)
	{
		obj = &objects[i];
	//	obj->control = ControlAnimatingSlots;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->HitEffect = 0;
	}

	obj = &objects[FIREROPE];
//	obj->control = ControlBurningRope;
//	obj->collision = BurningRopeCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[EXPANDING_PLATFORM];
//	obj->initialise = InitialiseRaisingBlock;
//	obj->control = ControlRaisingBlock;
//	obj->draw_routine = DrawScaledSpike;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[SQUISHY_BLOCK1];
//	obj->control = ControlLRSquishyBlock;
	obj->collision = ObjectCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[SQUISHY_BLOCK2];
//	obj->control = ControlFallingSquishyBlock;
//	obj->collision = FallingSquishyBlockCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[MAPPER];
	obj->initialise = InitialiseMapper;
	obj->control = ControlMapper;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[OBELISK];
	obj->initialise = InitialiseObelisk;
//	obj->control = ControlObelisk;
	obj->collision = ObjectCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[ELEMENT_PUZZLE];
	obj->initialise = InitialiseElementPuzzle;
	obj->control = ControlElementPuzzle;
	obj->collision = ElementPuzzleCollision;
	obj->save_flags = 1;
	obj->save_mesh = 1;

	obj = &objects[STATUE_PLINTH];
	obj->initialise = InitialiseStatuePlinth;
//	obj->collision = StatuePlinthCollision;
	obj->save_flags = 1;
	obj->save_mesh = 1;

	for (int i = SWITCH_TYPE7; i <= SWITCH_TYPE8; i++)
	{
		obj = &objects[i];
//		obj->control = ControlAnimatingSlots;
		obj->collision = 0;	//AIPickupCollision
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	obj = &objects[SWITCH_TYPE7];
	obj->save_mesh = 1;

	obj = &objects[SCALES];
	obj->control = ScalesControl;
	obj->collision = ScalesCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;
	obj->HitEffect = 0;

	obj = &objects[TEETH_SPIKES];
	obj->initialise = InitialiseScaledSpike;
//	obj->control = ControlScaledSpike;
//	obj->draw_routine = DrawScaledSpike;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[JOBY_SPIKES];
	obj->initialise = InitialiseJobySpike;
//	obj->control = ControlJobySpike;
//	obj->draw_routine = DrawScaledSpike;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[SLICER_DICER];
	obj->initialise = InitialiseSlicerDicer;
//	obj->control = ControlSlicerDicer;
//	obj->collision = GenericDeadlyBoundingBoxCollision;
	obj->save_flags = 1;

	obj = &objects[SARCOPHAGUS_CUT];
	obj->save_mesh = 1;

	obj = &objects[HORUS_STATUE];
	obj->collision = ObjectCollision;
	obj->save_mesh = 1;

	for (int i = RAISING_BLOCK1; i <= RAISING_BLOCK2; i++)
	{
		obj = &objects[i];
		obj->initialise = InitialiseRaisingBlock;
	//	obj->control = ControlRaisingBlock;
		obj->collision = 0;
	//	obj->draw_routine = DrawScaledSpike;
		obj->using_drawanimating_item = 0;
		obj->save_flags = 1;
	}

	for (int i = SMOKE_EMITTER_WHITE; i <= STEAM_EMITTER; i++)
	{
		obj = &objects[i];
		obj->initialise = InitialiseSmokeEmitter;
		obj->control = ControlSmokeEmitter;
		obj->draw_routine = 0;
		obj->using_drawanimating_item = 0;
		obj->save_flags = 1;
	}

	for (int i = RED_LIGHT; i <= BLUE_LIGHT; i++)
	{
		obj = &objects[i];
	//	obj->control = ControlColouredLights;
		obj->draw_routine = 0;
		obj->using_drawanimating_item = 0;
		obj->save_flags = 1;
	}

	obj = &objects[LIGHTNING_CONDUCTOR];
	obj->initialise = InitialiseLightningConductor;
	obj->control = ControlLightningConductor;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[BUBBLES];
//	obj->control = ControlEnemyMissile;
	obj->draw_routine = (void(*)(ITEM_INFO*))1;
	obj->nmeshes = 0;
	obj->loaded = 1;

	obj = &objects[WATERFALLMIST];
	obj->control = WaterFall;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;

	obj = &objects[AMBER_LIGHT];
//	obj->control = ControlPulseLight;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[WHITE_LIGHT];
//	obj->control = ControlElectricalLight;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[BLINKING_LIGHT];
//	obj->control = ControlBlinker;
	obj->save_flags = 1;

	obj = &objects[LENS_FLARE];
//	obj->draw_routine = DrawLensFlares;
	obj->using_drawanimating_item = 0;

	for (int i = WATERFALL1; i <= WATERFALL3; i++)
	{
	//	obj->control = ControlWaterfall;
		obj->save_flags = 1;
	}

	obj = &objects[CLOCKWORK_BEETLE];
	obj->initialise = 0;
	obj->control = ControlClockworkBeetle;

	obj = &objects[GOD_HEAD];
//	obj->control = ControlGodHead;
//	obj->draw_routine = DrawGodHead;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[EARTHQUAKE];
//	obj->control = EarthQuake;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[BODY_PART];
//	obj->control = ControlBodyPart;
	obj->draw_routine = (void(*)(ITEM_INFO*))1;
	obj->nmeshes = 0;
	obj->loaded = 1;
}

void inject_setup(bool replace)
{
	INJECT(0x0045E1F0, ObjectObjects, 0);
}