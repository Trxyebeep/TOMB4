#pragma once
#include "../global/vars.h"

void inject_moveblok(bool replace);

void InitialiseMovingBlock(short item_number);
void MovableBlock(short item_number);
void MovableBlockCollision(short item_number, ITEM_INFO* laraitem, COLL_INFO* coll);
void InitialisePlanetEffect(short item_number);
void ControlPlanetEffect(short item_number);
