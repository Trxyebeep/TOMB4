#pragma once
#include "../global/vars.h"

#define ACMSetVolume	( (void(__cdecl*)()) 0x0046D7B0 )
#define	S_CDStop	( (void(__cdecl*)()) 0x0046D6B0 )
#define S_StartSyncedAudio	( (void(__cdecl*)(int)) 0x0046D790 )
#define S_CDPlay	( (void(__cdecl*)(short, long)) 0x0046D610 )
#define ACMClose	( (void(__cdecl*)()) 0x0046DD00 )
