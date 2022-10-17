#pragma once
#include "../global/types.h"

bool DXChangeOutputFormat(long nSamplesPerSec, bool force);
void DSChangeVolume(long num, long volume);
void DSAdjustPitch(long num, long pitch);
void DSAdjustPan(long num, long pan);
bool DXSetOutputFormat();
bool DXDSCreate();
bool InitSampleDecompress();
bool FreeSampleDecompress();
bool DXCreateSampleADPCM(char* data, long comp_size, long uncomp_size, long num);
void DXStopSample(long num);
bool DSIsChannelPlaying(long num);
long DSGetFreeChannel();
long DXStartSample(long num, long volume, long pitch, long pan, ulong flags);
long CalcVolume(long volume);
void S_SoundStopAllSamples();
void S_SoundStopSample(long num);
long S_SoundPlaySample(long num, ushort volume, long pitch, short pan);
long S_SoundPlaySampleLooped(long num, ushort volume, long pitch, short pan);
void DXFreeSounds();
long S_SoundSampleIsPlaying(long num);
void S_SoundSetPanAndVolume(long num, short pan, ushort volume);
void S_SoundSetPitch(long num, long pitch);
void S_SetReverbType(long reverb);
void DXDSClose();

extern char* samples_buffer;
