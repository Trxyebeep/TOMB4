#pragma once
#include "../global/vars.h"

#define DisableBaddieAI	( (void(__cdecl*)(short)) 0x00455270 )
#define InitialiseLOTarray	( (void(__cdecl*)(long)) 0x00455200 )
#define EnableBaddieAI	( (long(__cdecl*)(short, long)) 0x004552B0 )
