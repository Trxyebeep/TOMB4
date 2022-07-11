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
HRESULT DDCopyBitmap(LPDIRECTDRAWSURFACEX surf, HBITMAP hbm, long x, long y, long dx, long dy);
HRESULT _LoadBitmap(LPDIRECTDRAWSURFACEX surf, LPCSTR name);
void do_boot_screen(long language);
void S_AnimateTextures(long n);
long S_DumpScreen();
void S_OutputPolyList();
void CalcVertsColorSplitMMX(long nVerts, D3DTLVERTEX* v);
void StashSkinVertices(long node);
void SkinVerticesToScratch(long node);
