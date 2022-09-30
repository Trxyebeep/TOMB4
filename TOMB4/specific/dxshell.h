#pragma once
#include "../global/types.h"

void DXBitMask2ShiftCnt(ulong mask, uchar* shift, uchar* count);
void DXReadKeyboard(char* KeyMap);
long DXAttempt(HRESULT r);
void* AddStruct(void* p, long num, long size);
long DXDDCreate(LPGUID pGuid, void** pDD4);
long DXD3DCreate(LPDIRECTDRAWX pDD4, void** pD3D);
long DXSetCooperativeLevel(LPDIRECTDRAWX pDD4, HWND hwnd, long flags);
BOOL __stdcall DXEnumDirectDraw(GUID FAR* lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext);
BOOL __stdcall DXEnumDirectSound(LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext);
long DXGetInfo(DXINFO* dxinfo, HWND hwnd);
void DXFreeInfo(DXINFO* dxinfo);
HRESULT __stdcall DXEnumDisplayModes(LPDDSURFACEDESCX lpDDSurfaceDesc2, LPVOID lpContext);
long BPPToDDBD(long BPP);
HRESULT __stdcall DXEnumTextureFormats(LPDDPIXELFORMAT lpDDPixFmt, LPVOID lpContext);
HRESULT __stdcall DXEnumZBufferFormats(LPDDPIXELFORMAT lpDDPixFmt, LPVOID lpContext);
long DXCreateSurface(LPDIRECTDRAWX dd, LPDDSURFACEDESCX desc, LPDIRECTDRAWSURFACEX* surf);
long DXSetVideoMode(LPDIRECTDRAWX dd, long dwWidth, long dwHeight, long dwBPP);
long DXCreateD3DDevice(LPDIRECT3DX d3d, GUID guid, LPDIRECTDRAWSURFACEX surf, LPDIRECT3DDEVICEX* device);
long DXCreateViewport(LPDIRECT3DX d3d, LPDIRECT3DDEVICEX device, long w, long h, LPDIRECT3DVIEWPORTX* viewport);
HRESULT DXShowFrame();
void DXMove(long x, long y);
void DXSaveScreen(LPDIRECTDRAWSURFACEX surf, const char* name);
void DXClose();
long DXCreate(long w, long h, long bpp, long Flags, DXPTR* dxptr, HWND hWnd, long WindowStyle);
long DXChangeVideoMode();
long DXToggleFullScreen();
HRESULT __stdcall DXEnumDirect3D(LPGUID lpGuid, LPSTR lpDeviceDescription, LPSTR lpDeviceName, LPD3DDEVICEDESC lpHWDesc, LPD3DDEVICEDESC lpHELDesc, LPVOID lpContext);
const char* DXGetErrorString(HRESULT hr);
void DXInitKeyboard(HWND hwnd, HINSTANCE hinstance);

extern DXPTR* G_dxptr;
extern DXINFO* G_dxinfo;
extern LPDIRECTDRAWX G_ddraw;
extern LPDIRECT3DX G_d3d;
extern HWND G_hwnd;
extern char keymap[256];
