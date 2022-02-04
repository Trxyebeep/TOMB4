#pragma once
#include "../global/vars.h"

void inject_camera(bool replace);

void InitialiseCamera();
void MoveCamera(GAME_VECTOR* ideal, long speed);
long mgLOS(GAME_VECTOR* start, GAME_VECTOR* target, long push);

#define CalculateCamera	( (void(__cdecl*)()) 0x00444890 )
#define LaraTorch	( (void(__cdecl*)(PHD_VECTOR*, PHD_VECTOR*, short, long)) 0x00445890 )
