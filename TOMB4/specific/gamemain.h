#pragma once
#include "../global/vars.h"

void inject_gamemain(bool replace);

void GameClose();
unsigned int __stdcall GameMain(void* ptr);
ushort GetRandom(WATERTAB* wt, long lp);
void init_water_table();
bool GameInitialise();

#define S_LoadGame	( (void(__cdecl*)(long)) 0x00477600 )
#define S_SaveGame	( (void(__cdecl*)(long)) 0x00477430 )
