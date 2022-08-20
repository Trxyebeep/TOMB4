#pragma once
#include "../global/types.h"

void InitialiseCroc(short item_number);
void CrocControl(short item_number);
long GetFreeLocust();
void TriggerLocust(ITEM_INFO* item);
void InitialiseLocustEmitter(short item_number);
void ControlLocustEmitter(short item_number);
void DrawLocusts();
void UpdateLocusts();
void TriggerCrocgodMissile(PHD_3DPOS* pos, short room_number, short num);
void TriggerCrocgodMissileFlame(short fx_number, long xv, long yv, long zv);
void InitialiseCrocgod(short item_number);
void CrocgodControl(short item_number);

extern LOCUST_STRUCT Locusts[64];
