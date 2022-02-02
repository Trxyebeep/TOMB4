#pragma once
#include "../global/vars.h"

void inject_savegame(bool replace);

long CheckSumValid(char* buffer);
void sgInitialiseHub(long dont_save_lara);
void SaveLaraData();
void WriteSG(void* pointer, long size);
void ReadSG(void* pointer, long size);
void SaveHubData(long index);
void RestoreLaraData(long FullSave);
void sgRestoreLevel();
void CreateCheckSum();
void sgSaveLevel();
void sgSaveGame();
void sgRestoreGame();

#define OpenSaveGame	( (long(__cdecl*)(uchar, long)) 0x0045A370 )
#define RestoreLevelData	( (void(__cdecl*)(long)) 0x0045B230 )
#define SaveLevelData	( (void(__cdecl*)(long)) 0x0045A4B0 )
