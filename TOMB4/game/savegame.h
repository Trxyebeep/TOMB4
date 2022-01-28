#pragma once
#include "../global/vars.h"

#define sgInitialiseHub	( (void(__cdecl*)(long)) 0x0045A100 )
#define sgSaveLevel	( (void(__cdecl*)()) 0x0045BDC0 )
#define sgRestoreGame	( (void(__cdecl*)()) 0x0045B040 )
#define sgRestoreLevel	( (void(__cdecl*)()) 0x0045BDF0 )
