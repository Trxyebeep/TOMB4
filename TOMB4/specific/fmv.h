#pragma once
#include "../global/vars.h"

void inject_fmv(bool replace);

void ShowBinkFrame();

#define PlayFmvNow	( (long(__cdecl*)(long)) 0x0049C3E0 )

#define BinkCopyToBuffer	( *(void(__stdcall**)(BINK_STRUCT*, LPVOID, LONG, long, long, long, long)) 0x004A9268 )