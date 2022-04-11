#pragma once
#include "../global/vars.h"

void inject_dxshell(bool replace);

void DXBitMask2ShiftCnt(ulong mask, uchar* shift, uchar* count);
void DXReadKeyboard(char* KeyMap);

#define DXAttempt   ( (void(__cdecl*)(HRESULT)) 0x00491C30 )
#define DXSaveScreen	( (void(__cdecl*)(LPDIRECTDRAWSURFACE4, const char*)) 0x00494080 )
#define DXShowFrame	( (HRESULT(__cdecl*)()) 0x00493F60 )
#define DXChangeVideoMode	( (long(__cdecl*)()) 0x004939E0 )
