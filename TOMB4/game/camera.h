#pragma once
#include "../global/vars.h"

void inject_camera(bool replace);

void InitialiseCamera();

#define CalculateCamera	( (void(__cdecl*)()) 0x00444890 )
#define LaraTorch	( (void(__cdecl*)(PHD_VECTOR*, PHD_VECTOR*, short, long)) 0x00445890 )
