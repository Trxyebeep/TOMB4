#pragma once
#include "../global/vars.h"

void inject_fmv(bool replace);

void ShowBinkFrame();
long PlayFmvNow(long num);

#define BinkCopyToBuffer	( *(void(__stdcall**)(BINK_STRUCT*, LPVOID, LONG, long, long, long, long)) 0x004A9268 )
#define BinkOpenDirectSound	( *(void(__stdcall**)(ulong)) 0x004A9274 )
#define BinkSetSoundSystem	( *(void(__stdcall**)(void*, LPDIRECTSOUND)) 0x004A9270 )
#define BinkOpen	( *(void*(__stdcall**)(char*, ulong)) 0x004A926C )
#define BinkDDSurfaceType	( *(long(__stdcall**)(LPDIRECTDRAWSURFACEX)) 0x004A9254 )
#define BinkDoFrame	( *(void(__stdcall**)(BINK_STRUCT*)) 0x004A9264 )
#define BinkNextFrame	( *(void(__stdcall**)(BINK_STRUCT*)) 0x004A925C )
#define BinkWait	( *(long(__stdcall**)(BINK_STRUCT*)) 0x004A9260 )
#define BinkClose	( *(void(__stdcall**)(BINK_STRUCT*)) 0x004A9258 )
