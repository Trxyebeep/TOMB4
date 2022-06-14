#pragma once
#include "../global/vars.h"

void inject_audio(bool replace);

void OpenStreamFile(char* name);
void GetADPCMData();
void ACMSetVolume();
void ACMEmulateCDPlay(long track, long mode);
BOOL __stdcall ACMEnumCallBack(HACMDRIVERID hadid, DWORD_PTR dwInstance, DWORD fdwSupport);
long ACMSetupNotifications();
void FillADPCMBuffer(char* p, long track);
long ACMHandleNotifications();
bool ACMInit();

#define	S_CDStop	( (void(__cdecl*)()) 0x0046D6B0 )
#define S_StartSyncedAudio	( (void(__cdecl*)(int)) 0x0046D790 )
#define S_CDPlay	( (void(__cdecl*)(short, long)) 0x0046D610 )
#define ACMClose	( (void(__cdecl*)()) 0x0046DD00 )
