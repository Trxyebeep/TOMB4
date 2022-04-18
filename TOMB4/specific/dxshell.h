#pragma once
#include "../global/vars.h"

void inject_dxshell(bool replace);

void DXBitMask2ShiftCnt(ulong mask, uchar* shift, uchar* count);
void DXReadKeyboard(char* KeyMap);
long DXAttempt(HRESULT r);

#define DXGetErrorString	( (const char*(__cdecl*)(HRESULT)) 0x00490FF0 )
#define DXSaveScreen	( (void(__cdecl*)(LPDIRECTDRAWSURFACE4, const char*)) 0x00494080 )
#define DXShowFrame	( (HRESULT(__cdecl*)()) 0x00493F60 )
#define DXChangeVideoMode	( (long(__cdecl*)()) 0x004939E0 )
