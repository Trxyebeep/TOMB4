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
#include "traps.h"
#include "deathsld.h"
#include "../specific/specificfx.h"
#include "door.h"
#include "bike.h"
#include "jeep.h"
#include "voncroy.h"
#include "sphinx.h"
#include "laradouble.h"
#include "croc.h"
#include "demigod.h"
#include "wildboar.h"
#include "baboon.h"
#include "train.h"
#include "bat.h"
#include "jean.h"
#include "senet.h"
#include "wraith.h"
#include "harpy.h"
#include "rope.h"
#include "moveblok.h"
#include "objlight.h"
#include "laraflar.h"
#include "troops.h"

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
	obj->draw_routine = DrawFlareInAir;
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
		obj->initialise = InitialiseDoor;
		obj->control = DoorControl;
		obj->collision = DoorCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	obj = &objects[UNDERWATER_DOOR];
	obj->initialise = InitialiseDoor;
	obj->control = PushPullKickDoorControl;
	obj->collision = UnderwaterDoorCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[DOUBLE_DOORS];
	obj->initialise = InitialiseDoor;
	obj->control = PushPullKickDoorControl;
	obj->collision = DoubleDoorCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[SEQUENCE_DOOR1];
	obj->initialise = InitialiseDoor;
	obj->control = SequenceDoorControl;
	obj->collision = DoorCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	for (int i = PUSHPULL_DOOR1; i <= KICK_DOOR2; i++)
	{
		obj = &objects[i];
		obj->initialise = InitialiseDoor;
		obj->control = PushPullKickDoorControl;
		obj->collision = PushPullKickDoorCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	for (int i = FLOOR_TRAPDOOR1; i <= FLOOR_TRAPDOOR2; i++)
	{
		obj = &objects[i];
		obj->initialise = InitialiseTrapDoor;
	//	obj->control = TrapDoorControl;
	//	obj->collision = FloorTrapDoorCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	for (int i = CEILING_TRAPDOOR1; i <= CEILING_TRAPDOOR2; i++)
	{
		obj = &objects[i];
		obj->initialise = InitialiseTrapDoor;
	//	obj->control = TrapDoorControl;
	//	obj->collision = CeilingTrapDoorCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	for (int i = TRAPDOOR1; i <= TRAPDOOR3; i++)
	{
		obj = &objects[i];
		obj->initialise = InitialiseTrapDoor;
	//	obj->control = TrapDoorControl;
	//	obj->collision = TrapDoorCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	for (int i = PUZZLE_ITEM1; i <= SECRET_MAP; i++)
	{
		obj = &objects[i];
		obj->initialise = InitialisePickUp;
	//	obj->control = AnimatingPickUp;
		obj->collision = PickUpCollision;
		obj->save_position = 1;
		obj->save_flags = 1;
	}

	for (int i = PISTOLS_ITEM; i <= BINOCULARS_ITEM; i++)
	{
		obj = &objects[i];
		obj->initialise = InitialisePickUp;
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
	obj->initialise = InitialisePickUp;
//	obj->control = AnimatingPickUp;
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[WATERSKIN2_EMPTY];
	obj->initialise = InitialisePickUp;
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
	obj->initialise = InitialisePickUp;
//	obj->control = AnimatingPickUp;
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[COMPASS_ITEM];
	obj->initialise = InitialisePickUp;
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
		obj->control = ControlAnimatingSlots;
	//	obj->collision = PuzzleHoleCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	obj = &objects[SARCOPHAGUS];
	obj->control = ControlAnimatingSlots;
	obj->collision = SarcophagusCollision;


	for (int i = ANIMATING1; i <= ANIMATING12; i++)
	{
		obj = &objects[i];
		obj->control = ControlAnimatingSlots;
		obj->collision = ObjectCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	for (int i = ANIMATING13; i <= ANIMATING16_MIP; i++)
	{
		obj = &objects[i];
		obj->control = ControlAnimatingSlots;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->HitEffect = 0;
	}

	obj = &objects[FIREROPE];
	obj->control = ControlBurningRope;
	obj->collision = BurningRopeCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[EXPANDING_PLATFORM];
//	obj->initialise = InitialiseRaisingBlock;
//	obj->control = ControlRaisingBlock;
	obj->draw_routine = DrawScaledSpike;
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
	obj->collision = StatuePlinthCollision;
	obj->save_flags = 1;
	obj->save_mesh = 1;

	for (int i = SWITCH_TYPE7; i <= SWITCH_TYPE8; i++)
	{
		obj = &objects[i];
		obj->control = ControlAnimatingSlots;
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
	obj->draw_routine = DrawScaledSpike;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[JOBY_SPIKES];
	obj->initialise = InitialiseJobySpike;
	obj->control = ControlJobySpike;
	obj->draw_routine = DrawScaledSpike;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[SLICER_DICER];
	obj->initialise = InitialiseSlicerDicer;
	obj->control = ControlSlicerDicer;
	obj->collision = GenericDeadlyBoundingBoxCollision;
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
		obj->draw_routine = DrawScaledSpike;
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
	obj->control = ControlEnemyMissile;
	obj->draw_routine = (void(*)(ITEM_INFO*))1;
	obj->nmeshes = 0;
	obj->loaded = 1;

	obj = &objects[WATERFALLMIST];
	obj->control = WaterFall;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;

	obj = &objects[AMBER_LIGHT];
	obj->control = ControlPulseLight;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[WHITE_LIGHT];
	obj->control = ControlElectricalLight;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[BLINKING_LIGHT];
	obj->control = ControlBlinker;
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
	obj->control = EarthQuake;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[BODY_PART];
//	obj->control = ControlBodyPart;
	obj->draw_routine = (void(*)(ITEM_INFO*))1;
	obj->nmeshes = 0;
	obj->loaded = 1;
}

void TrapObjects()
{
	OBJECT_INFO* obj;

	obj = &objects[ROLLINGBALL];
//	obj->control = ControlRollingBall;
//	obj->collision = RollingBallCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[CHAIN];
//	obj->control = ControlChain;
	obj->collision = GenericSphereBoxCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[PLOUGH];
//	obj->control = ControlPlough;
	obj->collision = GenericSphereBoxCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;
	
	obj = &objects[STARGATE];
//	obj->control = ControlStargate;
//	obj->collision = StargateCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[HAMMER];
//	obj->control = ControlHammer;
	obj->collision = GenericSphereBoxCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[BURNING_FLOOR];
	obj->initialise = InitialiseBurningFloor;
//	obj->control = ControlBurningFloor;
//	obj->collision = empty func here;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[COG];
	obj->control = ControlAnimatingSlots;
//	obj->collision = CogCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[SPIKEBALL];
//	obj->control = ControlSpikeball;
	obj->collision = GenericSphereBoxCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[TWOBLOCK_PLATFORM];
	obj->initialise = InitialiseTwoBlockPlatform;
	obj->control = ControlTwoBlockPlatform;
	obj->floor = TwoBlockPlatformFloor;
	obj->ceiling = TwoBlockPlatformCeiling;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[FLOOR_4BLADE];
//	obj->control = Control4xFloorRoofBlade;
	obj->collision = GenericSphereBoxCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[ROOF_4BLADE];
//	obj->control = Control4xFloorRoofBlade;
	obj->collision = GenericSphereBoxCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[BIRD_BLADE];
//	obj->control = ControlBirdBlade;
	obj->collision = GenericSphereBoxCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[CATWALK_BLADE];
//	obj->control = ControlCatwalkBlade;
	obj->collision = GenericDeadlyBoundingBoxCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[MOVING_BLADE];
//	obj->control = ControlMovingBlade;
	obj->collision = GenericDeadlyBoundingBoxCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[PLINTH_BLADE];
//	obj->control = ControlPlinthBlade;
	obj->collision = GenericDeadlyBoundingBoxCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[SETH_BLADE];
	obj->initialise = InitialiseSethBlade;
//	obj->control = ControlSethBlade;
	obj->collision = GenericSphereBoxCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[KILL_ALL_TRIGGERS];
//	obj->control = KillAllCurrentItems;
	obj->draw_routine = 0;
	obj->hit_points = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[DEATH_SLIDE];
	obj->initialise = InitialiseDeathSlide;
	obj->control = ControlDeathSlide;
	obj->collision = DeathSlideCollision;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	for (int i = FALLING_BLOCK; i <= FALLING_BLOCK2; i++)
	{
		obj = &objects[i];
		obj->initialise = InitialiseFallingBlock2;
	//	obj->control = FallingBlock;
	//	obj->collision = FallingBlockCollision;
	//	obj->floor = FallingBlockFloor;
	//	obj->ceiling = FallingBlockCeiling;
		obj->save_position = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	obj = &objects[FALLING_CEILING];
//	obj->control = FallingCeiling;
//	obj->collision = TrapCollision;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[SMASHABLE_BIKE_WALL];
	obj->initialise = InitialiseFallingBlock2;
//	obj->control = ControlSmashableBikeWall;
	obj->collision = ObjectCollision;
	obj->save_flags = 1;

	obj = &objects[SMASHABLE_BIKE_FLOOR];
	obj->initialise = InitialiseFallingBlock2;
//	obj->control = ControlFallingBlock2;
	obj->collision = ObjectCollision;
	obj->floor = TwoBlockPlatformFloor;
	obj->ceiling = TwoBlockPlatformCeiling;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	for (int i = PUSHABLE_OBJECT1; i < PUSHABLE_OBJECT5; i++)
	{
		obj = &objects[i];
		obj->initialise = InitialiseMovingBlock;
		obj->control = MovableBlock;
		obj->collision = MovableBlockCollision;
		obj->save_position = 1;
		obj->save_flags = 1;
	}

	obj = &objects[SAS_DRAG_BLOKE];
	obj->control = ControlAnimatingSlots;
//	obj->collision = DragSASCollision;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[DARTS];
//	obj->control = DartsControl;
	obj->collision = ObjectCollision;
	obj->draw_routine = S_DrawDarts;
	obj->using_drawanimating_item = 0;
	obj->shadow_size = 128;

	obj = &objects[DART_EMITTER];
//	obj->control = DartEmitterControl;
	obj->draw_routine = 0;
	obj->save_flags = 1;
	obj->using_drawanimating_item = 0;

	obj = &objects[HOMING_DART_EMITTER];
//	obj->control = DartEmitterControl;
	obj->draw_routine = 0;
	obj->save_flags = 1;
	obj->using_drawanimating_item = 0;

	obj = &objects[FLAME];
//	obj->control = FlameControl;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;

	obj = &objects[FLAME_EMITTER];
	obj->initialise = InitialiseFlameEmitter;
	obj->control = FlameEmitterControl;
//	obj->collision = FireCollision;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[FLAME_EMITTER2];
	obj->initialise = InitialiseFlameEmitter2;
//	obj->control = FlameEmitter2Control;
//	obj->collision = FireCollision;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[FLAME_EMITTER3];
//	obj->control = FlameEmitter3Control;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	init_all_ropes();
	obj = &objects[ROPE];
	obj->initialise = InitialiseRope;
	obj->control = RopeControl;
	obj->collision = RopeCollision;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[POLEROPE];
	obj->collision = PoleCollision;
	obj->save_flags = 1;

	obj = &objects[MINE];
	obj->initialise = InitialiseMineHelicopter;
//	obj->control = ControlMineHelicopter;
//	obj->collision = MineCollision;
	obj->HitEffect = 3;

	obj = &objects[SPRINKLER];
//	obj->control = ControlSprinkler;
//	obj->collision = FireCollision;
	obj->save_flags = 1;

	obj = &objects[TRIGGER_TRIGGERER];
	obj->initialise = ControlTriggerTriggerer;
	obj->control = ControlTriggerTriggerer;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[PLANET_EFFECT];
//	obj->initialise = InitialisePlanetEffect;
//	obj->control = ControlPlanetEffect;
//	obj->draw_routine = DrawPlanetEffect;
	obj->save_flags = 1;
	obj->using_drawanimating_item = 0;
	obj->save_mesh = 1;
}

void BaddyObjects()
{
	OBJECT_INFO* obj;

	obj = &objects[LARA];
//	obj->initialise = InitialiseLaraLoad;
	obj->draw_routine = 0;
	obj->shadow_size = 160;
	obj->hit_points = 1000;
	obj->using_drawanimating_item = 0;
	obj->save_hitpoints = 1;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[MOTORBIKE];
	obj->initialise = InitialiseBike;
	obj->control = BikeControl;
	obj->collision = BikeCollision;
	obj->draw_routine_extra = DrawBikeExtras;
	obj->save_hitpoints = 1;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;
	bones[obj->bone_index + 4] |= 4;
	bones[obj->bone_index + 12] |= 4;
	bones[obj->bone_index + 28] |= 4;

	obj = &objects[JEEP];
	obj->initialise = InitialiseJeep;
	obj->control = JeepControl;
	obj->collision = JeepCollision;
	obj->draw_routine_extra = DrawJeepExtras;
	obj->save_hitpoints = 1;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;
	bones[obj->bone_index + 32] |= 4;
	bones[obj->bone_index + 36] |= 4;
	bones[obj->bone_index + 44] |= 4;
	bones[obj->bone_index + 48] |= 4;

	obj = &objects[SKELETON];

	if (obj->loaded)
	{
//		obj->initialise = InitialiseSkeleton;
//		obj->control = SkeletonControl;
//		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 15;
		obj->pivot_length = 50;
		obj->radius = 128;
		obj->object_mip = 5120;
		obj->explodable_meshbits = 0xA00;
		obj->intelligent = 1;
		obj->save_mesh = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_hitpoints = 1;
		obj->save_position = 1;
		obj->HitEffect = 2;
		obj->undead = 1;
	}

	obj = &objects[VON_CROY];

	if (obj->loaded)
	{
		obj->initialise = InitialiseVoncroy;
		obj->control = VoncroyControl;

		if (gfCurrentLevel != 1)
			obj->control = VoncroyRaceControl;

//		obj->collision = CreatureCollision;
		obj->pivot_length = 0;
		obj->shadow_size = 128;
		obj->hit_points = 15;
		obj->radius = 128;
		obj->object_mip = 5120;
		obj->explodable_meshbits = 0x200000;
		obj->intelligent = 1;
		obj->save_mesh = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_hitpoints = 1;
		obj->save_position = 1;
		obj->HitEffect = 0;
		obj->undead = 1;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 80] |= 4;
		bones[obj->bone_index + 80] |= 8;
		meshes[obj->mesh_index + 15] = meshes[objects[MESHSWAP1].mesh_index + 14];
		meshes[obj->mesh_index + 31] = meshes[objects[MESHSWAP1].mesh_index + 30];
		meshes[obj->mesh_index + 37] = meshes[objects[MESHSWAP1].mesh_index + 36];
	}

	obj = &objects[VON_CROY_MIP];

	if (obj->loaded)
	{
		meshes[obj->mesh_index + 15] = meshes[objects[MESHSWAP1].mesh_index + 14];
		meshes[obj->mesh_index + 31] = meshes[objects[MESHSWAP1].mesh_index + 30];
		meshes[obj->mesh_index + 37] = meshes[objects[MESHSWAP1].mesh_index + 36];
	}

	obj = &objects[GUIDE];

	if (obj->loaded)
	{
//		obj->initialise = InitialiseGuide;
//		obj->control = GuideControl;
//		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = -16384;
		obj->pivot_length = 0;
		obj->radius = 128;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 0;
		obj->save_flags = 1;
		obj->save_mesh = 1;
		obj->save_anim = 1;
		obj->save_hitpoints = 1;
		obj->save_position = 1;
		obj->undead = 1;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 80] |= 4;
		bones[obj->bone_index + 80] |= 8;
		meshes[obj->mesh_index + 31] = meshes[objects[MESHSWAP2].mesh_index + 30];
		meshes[obj->mesh_index + 37] = meshes[objects[MESHSWAP2].mesh_index + 36];
		meshes[obj->mesh_index + 43] = meshes[objects[MESHSWAP2].mesh_index + 42];
	}

	obj = &objects[GUIDE_MIP];

	if (obj->loaded)
	{
		meshes[obj->mesh_index + 31] = meshes[objects[MESHSWAP2].mesh_index + 30];
		meshes[obj->mesh_index + 37] = meshes[objects[MESHSWAP2].mesh_index + 36];
		meshes[obj->mesh_index + 43] = meshes[objects[MESHSWAP2].mesh_index + 42];
	}

	obj = &objects[RAGHEAD];

	if (obj->loaded)
	{
//		obj->initialise = InitialiseRaghead;
//		obj->control = RagheadControl;
//		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 25;
		obj->pivot_length = 50;
		obj->radius = 102;
		obj->bite_offset = 1;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->save_mesh = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_hitpoints = 1;
		obj->save_position = 1;
		obj->HitEffect = 1;
		bones[obj->bone_index + 28] |= 4;
		bones[obj->bone_index + 28] |= 8;
		bones[obj->bone_index + 88] |= 4;
		bones[obj->bone_index + 88] |= 8;
		meshes[obj->mesh_index + 9] = meshes[objects[MESHSWAP3].mesh_index + 8];
		meshes[obj->mesh_index + 15] = meshes[objects[MESHSWAP3].mesh_index + 14];

		for (int i = 0; i < 12; i++)
			meshes[obj->mesh_index + 2 * i + 23] = meshes[objects[MESHSWAP3].mesh_index + 2 * i + 22];

		if (objects[SUPER_RAGHEAD].loaded)
			obj->anim_index = objects[SUPER_RAGHEAD].anim_index;
	}

	obj = &objects[RAGHEAD_MIP];

	if (obj->loaded)
	{
		meshes[obj->mesh_index + 9] = meshes[objects[MESHSWAP3].mesh_index + 8];
		meshes[obj->mesh_index + 15] = meshes[objects[MESHSWAP3].mesh_index + 14];

		for (int i = 0; i < 12; i++)
			meshes[obj->mesh_index + 2 * i + 23] = meshes[objects[MESHSWAP3].mesh_index + 2 * i + 22];
	}

	obj = &objects[SUPER_RAGHEAD];

	if (obj->loaded)
	{
//		obj->initialise = InitialiseRaghead;
//		obj->control = RagheadControl;
//		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 35;
		obj->pivot_length = 50;
		obj->radius = 102;
		obj->bite_offset = 1;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_mesh = 1;
		obj->save_hitpoints = 1;
		obj->save_position = 1;
		bones[obj->bone_index + 28] |= 4;
		bones[obj->bone_index + 28] |= 8;
		bones[obj->bone_index + 88] |= 4;
		bones[obj->bone_index + 88] |= 8;
		meshes[obj->mesh_index + 9] = meshes[objects[MESHSWAP2].mesh_index + 8];
		meshes[obj->mesh_index + 15] = meshes[objects[MESHSWAP2].mesh_index + 14];

		for (int i = 0; i < 12; i++)
			meshes[obj->mesh_index + 2 * i + 23] = meshes[objects[MESHSWAP2].mesh_index + 2 * i + 22];
	}

	obj = &objects[SUPER_RAGHEAD_MIP];

	if (obj->loaded)
	{
		meshes[obj->mesh_index + 9] = meshes[objects[MESHSWAP2].mesh_index + 8];
		meshes[obj->mesh_index + 15] = meshes[objects[MESHSWAP2].mesh_index + 14];

		for (int i = 0; i < 12; i++)
			meshes[obj->mesh_index + 2 * i + 23] = meshes[objects[MESHSWAP3].mesh_index + 2 * i + 22];
	}

	obj = &objects[SCORPION];

	if (obj->loaded)
	{
//		obj->initialise = InitialiseScorpion;
//		obj->control = ScorpionControl;
//		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 80;
		obj->pivot_length = 50;
		obj->radius = 512;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_hitpoints = 1;
		obj->save_position = 1;
	}

	obj = &objects[SMALL_SCORPION];

	if (obj->loaded)
	{
//		obj->initialise = InitialiseSmlscorp;
//		obj->control = SmlscorpControl;
//		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 8;
		obj->pivot_length = 20;
		obj->radius = 128;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_hitpoints = 1;
		obj->save_position = 1;
	}

	obj = &objects[MUMMY];

	if (obj->loaded)
	{
//		obj->initialise = InitialiseMummy;
//		obj->control = MummyControl;
//		obj->collision = CreatureCollision;
		obj->pivot_length = 50;
		obj->shadow_size = 128;
		obj->hit_points = 15;
		obj->radius = 170;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_hitpoints = 1;
		obj->save_position = 1;
		obj->HitEffect = 2;
		obj->undead = 1;
		bones[obj->bone_index + 28] |= 4;
		bones[obj->bone_index + 28] |= 8;
		bones[obj->bone_index + 72] |= 8;
	}

	obj = &objects[KNIGHTS_TEMPLAR];

	if (obj->loaded)
	{
//		obj->initialise = InitialiseTemplar;
//		obj->control = TemplarControl;
//		obj->collision = CreatureCollision;
		obj->pivot_length = 50;
		obj->shadow_size = 128;
		obj->hit_points = 15;
		obj->radius = 128;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_hitpoints = 1;
		obj->save_position = 1;
		obj->HitEffect = 2;
		obj->undead = 1;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 56] |= 8;
	}

	obj = &objects[SPHINX];

	if (obj->loaded)
	{
		obj->initialise = InitialiseSphinx;
		obj->control = SphinxControl;
//		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 1000;
		obj->pivot_length = 500;
		obj->radius = 512;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_hitpoints = 1;
		obj->save_position = 1;
		obj->HitEffect = 3;
		obj->undead = 1;
	}

	obj = &objects[SETHA];

	if (obj->loaded)
	{
//		obj->initialise = InitialiseSeth;
//		obj->control = SethControl;
//		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 500;
		obj->pivot_length = 50;
		obj->radius = 341;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->undead = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_hitpoints = 1;
		obj->save_position = 1;
	}

	obj = &objects[LARA_DOUBLE];

	if (obj->loaded)
	{
		obj->initialise = InitialiseLaraDouble;
		obj->control = LaraDoubleControl;
//		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 1000;
		obj->pivot_length = 50;
		obj->radius = 128;
		obj->intelligent = 1;
		obj->HitEffect = 3;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_hitpoints = 1;
		obj->save_position = 1;
	}

	obj = &objects[HORSEMAN];

	if (obj->loaded)
	{
//		obj->initialise = InitialiseHorseman;
//		obj->control = HorsemanControl;
//		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 25;
		obj->pivot_length = 50;
		obj->radius = 409;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 3;
		obj->undead = 1;
		obj->save_mesh = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_hitpoints = 1;
		obj->save_position = 1;
	}

	obj = &objects[HAMMERHEAD];

	if (obj->loaded)
	{
//		obj->initialise = InitialiseHammerhead;
//		obj->control = HammerheadControl;
//		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 45;
		obj->pivot_length = 300;
		obj->radius = 341;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->water_creature = 1;
		obj->save_hitpoints = 1;
		obj->save_position = 1;
		bones[obj->bone_index] |= 8;
		bones[obj->bone_index + 4] |= 8;
		bones[obj->bone_index + 8] |= 8;
		bones[obj->bone_index + 36] |= 8;
	}

	obj = &objects[CROCODILE];

	if (obj->loaded)
	{
		obj->initialise = InitialiseCroc;
		obj->control = CrocControl;
//		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 36;
		obj->pivot_length = 300;
		obj->radius = 409;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->water_creature = 1;
		obj->save_hitpoints = 1;
		obj->save_position = 1;
		bones[obj->bone_index] |= 8;
		bones[obj->bone_index + 28] |= 8;
		bones[obj->bone_index + 36] |= 8;
		bones[obj->bone_index + 40] |= 8;
	}

	obj = &objects[DEMIGOD1];

	if (obj->loaded)
	{
		obj->initialise = InitialiseDemigod;
		obj->control = DemigodControl;
//		obj->collision = CreatureCollision;
		obj->pivot_length = 50;
		obj->shadow_size = 128;
		obj->hit_points = 200;
		obj->radius = 341;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->save_flags = 1;
		obj->undead = 1;
		obj->save_anim = 1;
		obj->HitEffect = 3;
		obj->save_hitpoints = 1;
		obj->save_position = 1;
		bones[obj->bone_index + 32] |= 4;
		bones[obj->bone_index + 32] |= 8;
		bones[obj->bone_index + 32] |= 16;
		bones[obj->bone_index + 80] |= 8;
	}

	obj = &objects[DEMIGOD2];

	if (obj->loaded)
	{
		obj->initialise = InitialiseDemigod;
		obj->control = DemigodControl;
//		obj->collision = CreatureCollision;
		obj->pivot_length = 50;
		obj->shadow_size = 128;
		obj->hit_points = 200;
		obj->radius = 341;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_hitpoints = 1;
		obj->save_position = 1;
		bones[obj->bone_index + 32] |= 4;
		bones[obj->bone_index + 32] |= 8;
		bones[obj->bone_index + 32] |= 16;
		bones[obj->bone_index + 80] |= 8;
	}

	obj = &objects[DEMIGOD3];

	if (obj->loaded)
	{
		obj->initialise = InitialiseDemigod;
		obj->control = DemigodControl;
//		obj->collision = CreatureCollision;
		obj->pivot_length = 50;
		obj->shadow_size = 128;
		obj->hit_points = 200;
		obj->radius = 341;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_hitpoints = 1;
		obj->save_position = 1;
		bones[obj->bone_index + 32] |= 4;
		bones[obj->bone_index + 32] |= 8;
		bones[obj->bone_index + 32] |= 16;
		bones[obj->bone_index + 80] |= 8;
	}

	obj = &objects[MUTANT];

	if (obj->loaded)
	{
		obj->initialise = InitialiseCrocgod;
		obj->control = CrocgodControl;
//		obj->collision = CreatureCollision;
		obj->pivot_length = 50;
		obj->shadow_size = 128;
		obj->hit_points = 15;
		obj->radius = 128;
		obj->intelligent = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_hitpoints = 1;
		obj->save_position = 1;
		obj->HitEffect = 3;
		obj->undead = 1;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 28] |= 4;
		bones[obj->bone_index + 28] |= 8;
	}

	obj = &objects[TROOPS];

	if (obj->loaded)
	{
		obj->initialise = InitialiseTroop;
		obj->control = TroopControl;
//		obj->collision = CreatureCollision;
		obj->pivot_length = 50;
		obj->shadow_size = 128;
		obj->hit_points = 40;
		obj->radius = 102;
		obj->bite_offset = 0;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_hitpoints = 1;
		obj->save_position = 1;
		bones[obj->bone_index] |= 4;
		bones[obj->bone_index] |= 8;
		bones[obj->bone_index + 28] |= 4;
		bones[obj->bone_index + 28] |= 8;
	}

	obj = &objects[SAS];

	if (obj->loaded)
	{
//		obj->initialise = InitialiseSas;
//		obj->control = SasControl;
//		obj->collision = CreatureCollision;
		obj->pivot_length = 50;
		obj->shadow_size = 128;
		obj->hit_points = 40;
		obj->radius = 102;
		obj->bite_offset = 0;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_hitpoints = 1;
		obj->save_position = 1;
		bones[obj->bone_index] |= 4;
		bones[obj->bone_index] |= 8;
		bones[obj->bone_index + 28] |= 4;
		bones[obj->bone_index + 28] |= 8;
	}

	obj = &objects[HARPY];

	if (obj->loaded)
	{
		obj->initialise = InitialiseHarpy;
		obj->control = HarpyControl;
//		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 60;
		obj->pivot_length = 50;
		obj->radius = 409;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_hitpoints = 1;
		obj->save_position = 1;
	}

	obj = &objects[WILD_BOAR];

	if (obj->loaded)
	{
		obj->initialise = InitialiseWildboar;
		obj->control = WildboarControl;
//		obj->collision = CreatureCollision;
		obj->pivot_length = 50;
		obj->shadow_size = 128;
		obj->hit_points = 40;
		obj->radius = 102;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_hitpoints = 1;
		obj->save_position = 1;
		bones[obj->bone_index + 48] |= 8;
		bones[obj->bone_index + 48] |= 16;
		bones[obj->bone_index + 52] |= 8;
		bones[obj->bone_index + 52] |= 16;
	}

	obj = &objects[FUCKED_UP_DOG];

	if (obj->loaded)
	{
//		obj->initialise = InitialiseDog;
//		obj->control = DogControl;
//		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 16;
		obj->pivot_length = 300;
		obj->radius = 341;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_hitpoints = 1;
		obj->save_position = 1;
		bones[obj->bone_index] |= 8;
		bones[obj->bone_index + 8] |= 4;
		bones[obj->bone_index + 8] |= 8;
	}

	obj = &objects[AHMET];

	if (obj->loaded)
	{
		obj->initialise = InitialiseAhmet;
		obj->control = AhmetControl;
//		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 80;
		obj->pivot_length = 300;
		obj->radius = 341;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_hitpoints = 1;
		obj->save_position = 1;
		bones[obj->bone_index + 36] |= 8;
	}

	for (int i = 0; i < 3; i++)
	{
		obj = &objects[BABOON_NORMAL + 2 * i];

		if (obj->loaded)
		{
			obj->initialise = InitialiseBaboon;
			obj->control = BaboonControl;
//			obj->collision = CreatureCollision;
			obj->shadow_size = 128;
			obj->hit_points = 30;
			obj->pivot_length = 200;
			obj->radius = 256;
			obj->intelligent = 1;
			obj->HitEffect = 1;
			obj->save_flags = 1;
			obj->save_anim = 1;
			obj->save_hitpoints = 1;
			obj->save_position = 1;

			if (i)
				obj->anim_index = objects[BABOON_NORMAL].anim_index;
		}
	}

	obj = &objects[ENEMY_JEEP];

	if (obj->loaded)
	{
		if (gfLevelFlags & GF_TRAIN)
		{
			obj->initialise = InitialiseTrainJeep;
			obj->control = TrainJeepControl;
//			obj->collision = CreatureCollision;
			obj->save_flags = 1;
			obj->save_anim = 1;
			obj->save_hitpoints = 1;
			obj->save_position = 1;
		}
		else
		{
			obj->initialise = InitialiseEnemyJeep;
			obj->control = EnemyJeepControl;
//			obj->collision = CreatureCollision;
			obj->shadow_size = 128;
			obj->hit_points = 40;
			obj->pivot_length = 500;
			obj->radius = 512;
			obj->intelligent = 1;
			obj->save_flags = 1;
			obj->save_anim = 1;
			obj->save_hitpoints = 1;
			obj->save_position = 1;
			obj->HitEffect = 2;
			obj->undead = 1;
			bones[obj->bone_index + 32] |= 4;
			bones[obj->bone_index + 36] |= 4;
			bones[obj->bone_index + 44] |= 4;
			bones[obj->bone_index + 48] |= 4;
		}
	}

	obj = &objects[BAT];

	if (obj->loaded)
	{
		obj->initialise = InitialiseBat;
		obj->control = BatControl;
//		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 5;
		obj->pivot_length = 10;
		obj->radius = 102;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_hitpoints = 1;
		obj->save_position = 1;
	}

	obj = &objects[BIG_BEETLE];

	if (obj->loaded)
	{
//		obj->initialise = InitialiseScarab;
//		obj->control = ScarabControl;
//		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 30;
		obj->pivot_length = 50;
		obj->radius = 204;
		obj->object_mip = 5120;
		obj->intelligent = 1;
		obj->HitEffect = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_hitpoints = 1;
		obj->save_position = 1;
	}

	obj = &objects[SENTRY_GUN];

	if (obj->loaded)
	{
//		obj->initialise = InitialiseAutogun;
//		obj->control = AutogunControl;
//		obj->collision = CreatureCollision;
		obj->radius = 204;
		obj->shadow_size = 128;
		obj->hit_points = 30;
		obj->pivot_length = 50;
		obj->explodable_meshbits = 0x40;
		obj->intelligent = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_hitpoints = 1;
		obj->save_position = 1;
		obj->HitEffect = 3;
		obj->undead = 1;
		bones[obj->bone_index] |= 8;
		bones[obj->bone_index + 4] |= 4;
		bones[obj->bone_index + 8] |= 16;
		bones[obj->bone_index + 12] |= 16;
	}

	obj = &objects[HORSE];

	if (obj->loaded)
	{
//		obj->initialise = InitialiseHorse;
//		obj->collision = ObjectCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	for (int i = 0; i < 2; i++)
	{
		obj = &objects[SAS_DYING + 2 * i];

		if (obj->loaded)
		{
//			obj->initialise = InitialiseInjuredSas;
//			obj->control = InjuredSasControl;
//			obj->collision = ObjectCollision;
			obj->save_flags = 1;
			obj->save_anim = 1;
			obj->save_position = 1;
		}
	}

	obj = &objects[JEAN_YVES];

	if (obj->loaded)
	{
		obj->initialise = InitialiseJeanYves;
		obj->control = JeanYvesControl;
//		obj->collision = ObjectCollision;
		obj->object_mip = 10240;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_position = 1;
	}

	for (int i = 0; i < 3; i++)
	{
		obj = &objects[GAME_PIECE1 + i];

		if (obj->loaded)
		{
			obj->initialise = InitialiseSenet;
			obj->control = SenetControl;
//			obj->collision = ObjectCollision;
			obj->save_flags = 1;
			obj->save_hitpoints = 1;
			obj->save_position = 1;
		}
	}

	obj = &objects[ENEMY_PIECE];

	if (obj->loaded)
	{
//		obj->collision = ObjectCollision;
		obj->save_flags = 1;
		obj->save_position = 1;
	}

	obj = &objects[WHEEL_OF_FORTUNE];

	if (obj->loaded)
	{
		obj->initialise = InitialiseGameStix;
		obj->control = GameStixControl;
		obj->collision = GameStixCollision;
		obj->hit_points = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index] |= 16;
		bones[obj->bone_index + 4] |= 16;
		bones[obj->bone_index + 8] |= 16;
		bones[obj->bone_index + 12] |= 16;
	}

	for (int i = 0; i < 4; i++)
	{
		obj = &objects[WRAITH1 + i];

		if (obj->loaded)
		{
			obj->initialise = InitialiseWraith;
			obj->control = WraithControl;
			obj->draw_routine_extra = DrawWraithTrail;
			obj->save_flags = 1;
			obj->save_anim = 1;
			obj->save_hitpoints = 1;
			obj->save_position = 1;
		}
	}

	obj = &objects[LITTLE_BEETLE];

	if (obj->loaded)
	{
//		obj->initialise = InitialiseScarabGenerator;
//		obj->control = TriggerScarab;
		obj->draw_routine = 0;
		obj->using_drawanimating_item = 0;
	}

	obj = &objects[FISH];

	if (obj->loaded)
	{
		obj->initialise = InitialiseLocustEmitter;
		obj->control = ControlLocustEmitter;
		obj->draw_routine = 0;
		obj->using_drawanimating_item = 0;
		obj->save_flags = 1;
	}
}

void inject_setup(bool replace)
{
	INJECT(0x0045E1F0, ObjectObjects, 0);
	INJECT(0x0045DC10, TrapObjects, 0);
	INJECT(0x0045C1E0, BaddyObjects, 0);
}