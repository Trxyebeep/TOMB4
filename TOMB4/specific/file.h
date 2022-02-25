#pragma once
#include "../global/vars.h"

void inject_file(bool replace);

long S_LoadLevelFile(long num);

#define FreeLevel	( (void(__cdecl*)()) 0x00476790 )
