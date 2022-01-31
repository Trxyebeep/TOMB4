#pragma once
#include "../global/vars.h"

void inject_savegame(bool replace);

long CheckSumValid(char* buffer);
void sgInitialiseHub(long dont_save_lara);
void SaveLaraData();

#define sgSaveLevel	( (void(__cdecl*)()) 0x0045BDC0 )
#define sgRestoreGame	( (void(__cdecl*)()) 0x0045B040 )
#define sgRestoreLevel	( (void(__cdecl*)()) 0x0045BDF0 )
