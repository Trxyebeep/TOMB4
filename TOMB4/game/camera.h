#pragma once
#include "../global/vars.h"

void inject_camera(bool replace);

void InitialiseCamera();
void MoveCamera(GAME_VECTOR* ideal, long speed);

#define CalculateCamera	( (void(__cdecl*)()) 0x00444890 )
#define LaraTorch	( (void(__cdecl*)(PHD_VECTOR*, PHD_VECTOR*, short, long)) 0x00445890 )
#define mgLOS	( (long(__cdecl*)(GAME_VECTOR*, GAME_VECTOR*, long)) 0x004451C0 )
