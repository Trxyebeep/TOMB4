#pragma once
#include "../global/types.h"

void SetCutSceneCamera(ITEM_INFO* item);
void ClearCutSceneCamera();
void GetAIEnemy(CREATURE_INFO* info, long tfl);
void DoVonCroyCutscene(ITEM_INFO* item, CREATURE_INFO* info);
void InitialiseVoncroy(short item_number);
void VoncroyRaceControl(short item_number);
void VoncroyControl(short item_number);

extern uchar VonCroyCutFlags[64];
extern char bVoncroyCutScene;
