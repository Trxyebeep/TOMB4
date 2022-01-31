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

#define sgSaveLevel	( (void(__cdecl*)()) 0x0045BDC0 )
#define sgRestoreGame	( (void(__cdecl*)()) 0x0045B040 )
#define OpenSaveGame	( (long(__cdecl*)(uchar, long)) 0x0045A370 )
#define RestoreLevelData	( (void(__cdecl*)(long)) 0x0045B230 )
