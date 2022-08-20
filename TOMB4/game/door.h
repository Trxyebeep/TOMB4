#pragma once
#include "../global/types.h"

void ShutThatDoor(DOORPOS_DATA* d);
void OpenThatDoor(DOORPOS_DATA* d);
void DoorControl(short item_number);
void DoorCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll);
void PushPullKickDoorControl(short item_number);
void PushPullKickDoorCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll);
void DoubleDoorCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll);
void UnderwaterDoorCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll);
void SequenceDoorControl(short item_number);
