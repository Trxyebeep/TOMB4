#pragma once
#include "../global/vars.h"

void inject_dxshell(bool replace);

void DXBitMask2ShiftCnt(ulong mask, uchar* shift, uchar* count);

#define DXAttempt   ( (void(__cdecl*)(HRESULT)) 0x00491C30 )
