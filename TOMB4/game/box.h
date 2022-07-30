#pragma once
#include "../global/vars.h"

void inject_box(bool replace);

void CreatureDie(short item_number, long explode);
void InitialiseCreature(short item_number);
long CreatureActive(short item_number);
void CreatureAIInfo(ITEM_INFO* item, AI_INFO* info);
long SearchLOT(LOT_INFO* LOT, long expansion);
long UpdateLOT(LOT_INFO* LOT, long expansion);
void TargetBox(LOT_INFO* LOT, short box_number);
long EscapeBox(ITEM_INFO* item, ITEM_INFO* enemy, short box_number);
long ValidBox(ITEM_INFO* item, short zone_number, short box_number);
long StalkBox(ITEM_INFO* item, ITEM_INFO* enemy, short box_number);
target_type CalculateTarget(PHD_VECTOR* target, ITEM_INFO* item, LOT_INFO* LOT);
void CreatureMood(ITEM_INFO* item, AI_INFO* info, long violent);
void GetCreatureMood(ITEM_INFO* item, AI_INFO* info, long violent);
long CreatureCreature(short item_number);

#define AlertNearbyGuards	( (void(__cdecl*)(ITEM_INFO*)) 0x004425D0 )
#define CreatureTurn	( (short(__cdecl*)(ITEM_INFO*, short)) 0x00441C60 )
#define CreatureTilt	( (void(__cdecl*)(ITEM_INFO*, short)) 0x00441EE0 )
#define CreatureJoint	( (void(__cdecl*)(ITEM_INFO*, short, short)) 0x00441F20 )
#define CreatureAnimation	( (long(__cdecl*)(short, short, short)) 0x00441230 )
#define GetAITarget	( (void(__cdecl*)(CREATURE_INFO*)) 0x00442720 )
#define AlertAllGuards	( (void(__cdecl*)(short)) 0x00442550 )
#define AIGuard	( (short(__cdecl*)(CREATURE_INFO*)) 0x00442670 )
typedef short(__cdecl* generate)(long, long, long, short, short, short);
#define CreatureEffectT	( (short(__cdecl*)(ITEM_INFO*, BITE_INFO*, short, short, generate)) 0x00442170 )
#define CreatureEffect	( (short(__cdecl*)(ITEM_INFO*, BITE_INFO*, generate)) 0x00442110 )
#define CreatureUnderwater	( (void(__cdecl*)(ITEM_INFO*, long)) 0x00442080 )
#define CreatureFloat	( (void(__cdecl*)(short)) 0x00441F80 )
#define CreatureVault	( (long(__cdecl*)(short, short, long, long)) 0x004421D0 )
#define CreatureYRot	( (void(__cdecl*)(PHD_3DPOS*, short, short)) 0x00442D30 )
#define MoveCreature3DPos	( (long(__cdecl*)(PHD_3DPOS*, PHD_3DPOS*, long, short, short)) 0x00442D70 )
