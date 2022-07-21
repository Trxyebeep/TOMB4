#pragma once
#include "../global/vars.h"

void inject_scarab(bool replace);

void InitialiseScarab(short item_number);

#define UpdateScarabs	( (void(__cdecl*)()) 0x0040E3C0 )
#define DrawScarabs	( (void(__cdecl*)()) 0x0040E630 )
#define ClearScarabs	( (void(__cdecl*)()) 0x0040E2A0 )
