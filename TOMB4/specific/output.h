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

#define S_GetObjectBounds	( (long(__cdecl*)(short*)) 0x0047FCF0 )
#define CalcVertsColorSplitMMX ( (void(__cdecl*)(long, D3DTLVERTEX*)) 0x0047D810 )
#define do_boot_screen	( (void(__cdecl*)(long)) 0x004808E0 )
#define StashSkinVertices	( (void(__cdecl*)(long)) 0x0047D6B0 )
#define SkinVerticesToScratch	( (void(__cdecl*)(long)) 0x0047D760 )
#define S_DumpScreen	( (long(__cdecl*)()) 0x0047FCA0 )
#define S_OutputPolyList	( (void(__cdecl*)()) 0x0047FA10 )
#define S_AnimateTextures	( (void(__cdecl*)(long)) 0x00480070 )
