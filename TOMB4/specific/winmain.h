#pragma once
#include "../global/vars.h"

#define RestoreFPCW	( (void(__cdecl*)(short)) 0x0048F960 )
#define MungeFPCW	( (long(__cdecl*)(short*)) 0x0048F910 )
