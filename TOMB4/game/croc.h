#pragma once
#include "../global/vars.h"

void inject_croc(bool replace);

void InitialiseCroc(short item_number);
void CrocControl(short item_number);
long GetFreeLocust();
void TriggerLocust(ITEM_INFO* item);
void InitialiseLocustEmitter(short item_number);
void ControlLocustEmitter(short item_number);
void DrawLocusts();
void UpdateLocusts();
