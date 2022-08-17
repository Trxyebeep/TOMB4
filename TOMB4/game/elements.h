#pragma once
#include "../global/types.h"

void inject_elements(bool replace);

void ElementPuzzleCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void InitialiseElementPuzzle(short item_number);
void ControlElementPuzzle(short item_number);
