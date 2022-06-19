#pragma once
#include "../global/vars.h"

void inject_dxsound(bool replace);

bool DXChangeOutputFormat(long nSamplesPerSec, bool force);
void DSChangeVolume(long num, long volume);
void DSAdjustPitch(long num, long pitch);
void DSAdjustPan(long num, long pan);
bool DXSetOutputFormat();
bool DXDSCreate();
bool InitSampleDecompress();
bool FreeSampleDecompress();

#define DSIsChannelPlaying	( (bool(__cdecl*)(long)) 0x00473900 )
#define S_SoundStopAllSamples	( (void(__cdecl*)()) 0x00473AB0 )
#define DXFreeSounds	( (void(__cdecl*)()) 0x00473B40 )
#define	DXCreateSampleADPCM	( (bool(__cdecl*)(char*, long, long, long)) 0x00473710 )
