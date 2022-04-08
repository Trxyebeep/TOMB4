#pragma once
#include "../global/vars.h"

#define DSIsChannelPlaying	( (bool(__cdecl*)(long)) 0x00473900 )
#define DSChangeVolume	( (void(__cdecl*)(long, long)) 0x00473390 )
#define S_SoundStopAllSamples	( (void(__cdecl*)()) 0x00473AB0 )
#define DXChangeOutputFormat	( (bool(__cdecl*)(long, bool)) 0x004732E0 )
#define DXFreeSounds	( (void(__cdecl*)()) 0x00473B40 )
