#pragma once
#include "../global/vars.h"

void inject_output(bool replace);

void phd_PutPolygons(short* objptr, long clip);
void phd_PutPolygons_train(short* objptr, long x);
void ProjectTrainVerts(short nVerts, D3DTLVERTEX* v, short* clip, long x);
void PrelightVerts(long nVerts, D3DTLVERTEX* v, MESH_DATA* mesh);
void _InsertRoom(ROOM_INFO* r);
void RenderLoadPic(long unused);
void S_InitialisePolyList();
void phd_PutPolygonsPickup(short* objptr, float x, float y, long color);
void phd_PutPolygonSkyMesh(short* objptr, long clipstatus);
void S_DrawPickup(short object_number);
long S_GetObjectBounds(short* bounds);
HRESULT DDCopyBitmap(LPDIRECTDRAWSURFACE4 surf, HBITMAP hbm, long x, long y, long dx, long dy);
HRESULT _LoadBitmap(LPDIRECTDRAWSURFACE4 surf, LPCSTR name);
void do_boot_screen(long language);

#define CalcVertsColorSplitMMX ( (void(__cdecl*)(long, D3DTLVERTEX*)) 0x0047D810 )
#define StashSkinVertices	( (void(__cdecl*)(long)) 0x0047D6B0 )
#define SkinVerticesToScratch	( (void(__cdecl*)(long)) 0x0047D760 )
#define S_DumpScreen	( (long(__cdecl*)()) 0x0047FCA0 )
#define S_OutputPolyList	( (void(__cdecl*)()) 0x0047FA10 )
#define S_AnimateTextures	( (void(__cdecl*)(long)) 0x00480070 )
