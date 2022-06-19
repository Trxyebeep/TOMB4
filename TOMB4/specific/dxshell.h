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
long DXGetInfo(DXINFO* dxinfo, HWND hwnd);
void DXFreeInfo(DXINFO* dxinfo);
HRESULT __stdcall DXEnumDisplayModes(LPDDSURFACEDESC2 lpDDSurfaceDesc2, LPVOID lpContext);
long BPPToDDBD(long BPP);
HRESULT __stdcall DXEnumTextureFormats(LPDDPIXELFORMAT lpDDPixFmt, LPVOID lpContext);
HRESULT __stdcall DXEnumZBufferFormats(LPDDPIXELFORMAT lpDDPixFmt, LPVOID lpContext);
long DXCreateSurface(LPDIRECTDRAW4 dd, LPDDSURFACEDESC2 desc, LPDIRECTDRAWSURFACE4* surf);
long DXSetVideoMode(LPDIRECTDRAW4 dd, long dwWidth, long dwHeight, long dwBPP);
long DXCreateD3DDevice(LPDIRECT3D3 d3d, GUID guid, LPDIRECTDRAWSURFACE4 surf, LPDIRECT3DDEVICE3* device);
long DXCreateViewport(LPDIRECT3D3 d3d, LPDIRECT3DDEVICE3 device, long w, long h, LPDIRECT3DVIEWPORT3* viewport);
HRESULT DXShowFrame();
void DXMove(long x, long y);
//void DXInitKeyboard(HWND hwnd, HINSTANCE hinstance);
void DXSaveScreen(LPDIRECTDRAWSURFACE4 surf, const char* name);
void DXClose();
long DXCreate(long w, long h, long bpp, long Flags, DXPTR* dxptr, HWND hWnd, long WindowStyle);
long DXChangeVideoMode();
long DXToggleFullScreen();
HRESULT __stdcall DXEnumDirect3D(LPGUID lpGuid, LPSTR lpDeviceDescription, LPSTR lpDeviceName, LPD3DDEVICEDESC lpHWDesc, LPD3DDEVICEDESC lpHELDesc, LPVOID lpContext);

#define DXGetErrorString	( (const char*(__cdecl*)(HRESULT)) 0x00490FF0 )
#define DXInitKeyboard	( (void(__cdecl*)(HWND, HINSTANCE)) 0x00494270 )
