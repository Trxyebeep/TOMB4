#pragma once
#include "../global/types.h"

void FullBlockSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
long SwitchTrigger(short item_number, short timer);
long GetSwitchTrigger(ITEM_INFO* item, short* ItemNos, long AttatchedToSwitch);
void TestTriggersAtXYZ(long x, long y, long z, short room_number, short heavy, short flags);
void SwitchControl(short item_number);
void SwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void SwitchCollision2(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void SwitchType78Collision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void UnderwaterSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void PulleyCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void TurnSwitchControl(short item_number);
void TurnSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void RailSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void JumpSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void CrowbarSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void FullBlockSwitchControl(short item_number);
void CogSwitchControl(short item_number);
void CogSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);

extern PHD_VECTOR OldPickupPos;
extern uchar CurrentSequence;
extern uchar Sequences[3];
extern uchar SequenceUsed[6];
extern uchar SequenceResults[3][3][3];
