#pragma once
#include "../global/vars.h"

void inject_delstuff(bool replace);

void DrawLara(ITEM_INFO* item, long mirror);
void GetLaraJointPos(PHD_VECTOR* pos, long node);
void SetLaraUnderwaterNodes();

#define Rich_CalcLaraMatrices_Normal	( (void(__cdecl*)(short*, long*, long)) 0x0041DAF0 )
