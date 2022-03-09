#pragma once
#include "../global/vars.h"

void inject_voncroy(bool replace);

void SetCutSceneCamera(ITEM_INFO* item);
void ClearCutSceneCamera();
void GetAIEnemy(CREATURE_INFO* info, long tfl);
void DoVonCroyCutscene(ITEM_INFO* item, CREATURE_INFO* info);
void InitialiseVoncroy(short item_number);
