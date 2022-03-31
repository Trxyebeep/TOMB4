#pragma once
#include "../global/vars.h"

void inject_drawroom(bool replace);

void ProjectVerts(long nVerts, D3DTLVERTEX* v, short* clip);
void ProjectWaterVerts(long nVerts, D3DTLVERTEX* v, short* clip);
void ProjectShoreVerts(long nVerts, D3DTLVERTEX* v, short* clip);
void ProcessRoomData(ROOM_INFO* r);
void PrelightVertsNonMMX(long nVerts, D3DTLVERTEX* v, ROOM_INFO* r);
void InsertRoom(ROOM_INFO* r);
void CalcTriFaceNormal(D3DVECTOR* p1, D3DVECTOR* p2, D3DVECTOR* p3, D3DVECTOR* N);
void ProcessMeshData(long num_meshes);
void InitBuckets();
void DrawBucket(TEXTUREBUCKET* bucket);

#define CreateVertexNormals	( (void(__cdecl*)(ROOM_INFO*)) 0x00472F50 )
#define PrelightVertsMMX	( (void(__cdecl*)(long, D3DTLVERTEX*, ROOM_INFO*)) 0x00472400 )
#define DrawBuckets	( (void(__cdecl*)()) 0x00472C10 )
