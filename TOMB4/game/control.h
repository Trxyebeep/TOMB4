#pragma once
#include "../global/vars.h"

void inject_control(bool replace);

long ControlPhase(long nframes, long demo_mode);
void FlipMap(long FlipNumber);
void RemoveRoomFlipItems(ROOM_INFO* r);
void AddRoomFlipItems(ROOM_INFO* r);
void TestTriggers(short* data, long heavy, long HeavyFlags);
short GetDoor(FLOOR_INFO* floor);
long CheckNoColFloorTriangle(FLOOR_INFO* floor, long x, long z);
long CheckNoColCeilingTriangle(FLOOR_INFO* floor, long x, long z);
FLOOR_INFO* GetFloor(long x, long y, long z, short* room_number);
long GetWaterHeight(long x, long y, long z, short room_number);
long GetHeight(FLOOR_INFO* floor, long x, long y, long z);
long GetCeiling(FLOOR_INFO* floor, long x, long y, long z);
void AlterFloorHeight(ITEM_INFO* item, long height);
void TranslateItem(ITEM_INFO* item, short x, short y, short z);
long GetChange(ITEM_INFO* item, ANIM_STRUCT* anim);
void UpdateSky();
void KillMoveEffects();
void KillMoveItems();
void RefreshCamera(short type, short* data);
long TriggerActive(ITEM_INFO* item);
void TriggerNormalCDTrack(short value, short flags, short type);
void TriggerCDTrack(short value, short flags, short type);
long ClipTarget(GAME_VECTOR* start, GAME_VECTOR* target);
long xLOS(GAME_VECTOR* start, GAME_VECTOR* target);
long zLOS(GAME_VECTOR* start, GAME_VECTOR* target);
long LOS(GAME_VECTOR* start, GAME_VECTOR* target);
void FireCrossBowFromLaserSight(GAME_VECTOR* start, GAME_VECTOR* target);
long ExplodeItemNode(ITEM_INFO* item, long Node, long NoXZVel, long bits);
long IsRoomOutside(long x, long y, long z);
long ObjectOnLOS2(GAME_VECTOR* start, GAME_VECTOR* target, PHD_VECTOR* Coord, MESH_INFO** StaticMesh);

#define AnimateItem	( (void(__cdecl*)(ITEM_INFO*)) 0x00449B90 )
#define GetTargetOnLOS	( (long(__cdecl*)(GAME_VECTOR*, GAME_VECTOR*, long, long)) 0x0044D890 )
#define DoRayBox	( (long(__cdecl*)(GAME_VECTOR*, GAME_VECTOR*, short*, PHD_3DPOS*, PHD_VECTOR*, short)) 0x0044CDF0 )

#ifdef GENERAL_FIXES
extern char DeathMenuActive;
#endif
