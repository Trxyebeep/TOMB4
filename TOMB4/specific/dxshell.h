#pragma once
#include "../global/vars.h"

void inject_dxshell(bool replace);

void DXBitMask2ShiftCnt(ulong mask, uchar* shift, uchar* count);
void DXReadKeyboard(char* KeyMap);
long DXAttempt(HRESULT r);
void* AddStruct(void* p, long num, long size);
long DXDDCreate(LPGUID pGuid, void** pDD4);
long DXD3DCreate(LPDIRECTDRAW4 pDD4, void** pD3D);
long DXSetCooperativeLevel(LPDIRECTDRAW4 pDD4, HWND hwnd, long flags);
BOOL __stdcall DXEnumDirectDraw(GUID FAR* lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext);
BOOL __stdcall DXEnumDirectSound(LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext);

#define DXGetErrorString	( (const char*(__cdecl*)(HRESULT)) 0x00490FF0 )
#define DXSaveScreen	( (void(__cdecl*)(LPDIRECTDRAWSURFACE4, const char*)) 0x00494080 )
#define DXShowFrame	( (HRESULT(__cdecl*)()) 0x00493F60 )
#define DXChangeVideoMode	( (long(__cdecl*)()) 0x004939E0 )
#define DXEnumDisplayModes	( (HRESULT(__stdcall*)(LPDDSURFACEDESC2, LPVOID)) 0x00492280 )
#define DXEnumDirect3D	( (HRESULT(__stdcall*)(LPGUID, LPSTR, LPSTR, LPD3DDEVICEDESC, LPD3DDEVICEDESC, LPVOID)) 0x00492430 )
