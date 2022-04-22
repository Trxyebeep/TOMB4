#pragma once
#include "../global/vars.h"

void inject_door(bool replace);

void ShutThatDoor(DOORPOS_DATA* d);
void OpenThatDoor(DOORPOS_DATA* d);
void DoorControl(short item_number);
void DoorCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll);
