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

#define UpdateSky	( (void(__cdecl*)()) 0x00449390 )
#define GetFloor	( (FLOOR_INFO*(__cdecl*)(long, long, long, short*)) 0x0044A1A0 )
#define GetHeight	( (long(__cdecl*)(FLOOR_INFO*, long, long, long)) 0x0044A530 )
#define GetCeiling	( (long(__cdecl*)(FLOOR_INFO*, long, long, long)) 0x0044B690 )
#define GetWaterHeight	( (long(__cdecl*)(long, long, long, short)) 0x0044A390 )
#define KillMoveItems	( (void(__cdecl*)()) 0x004492D0)
#define KillMoveEffects	( (void(__cdecl*)()) 0x00449330 )
#define TriggerActive	( (long(__cdecl*)(ITEM_INFO*)) 0x0044B620 )
#define AnimateItem	( (void(__cdecl*)(ITEM_INFO*)) 0x00449B90 )
#define ExplodeItemNode	( (long(__cdecl*)(ITEM_INFO*, long, long, long)) 0x0044DE50 )
#define IsRoomOutside	( (long(__cdecl*)(long, long, long)) 0x0044C9C0 )
#define GetTargetOnLOS	( (long(__cdecl*)(GAME_VECTOR*, GAME_VECTOR*, long, long)) 0x0044D890 )
#define AlterFloorHeight	( (void(__cdecl*)(ITEM_INFO*, long)) 0x0044A0D0 )
#define LOS	( (long(__cdecl*)(GAME_VECTOR*, GAME_VECTOR*)) 0x0044BBE0 )
#define RefreshCamera	( (void(__cdecl*)(short, short*)) 0x0044AA20 )
#define TriggerCDTrack	( (void(__cdecl*)(short, short, short)) 0x0044C770 )

#ifdef GENERAL_FIXES
extern char DeathMenuActive;
#endif
