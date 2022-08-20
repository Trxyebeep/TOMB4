#pragma once
#include "../global/types.h"

void ProjectVerts(long nVerts, D3DTLVERTEX* v, short* clip);
void ProjectWaterVerts(long nVerts, D3DTLVERTEX* v, short* clip);
void ProjectShoreVerts(long nVerts, D3DTLVERTEX* v, short* clip);
void ProcessRoomData(ROOM_INFO* r);
void PrelightVertsNonMMX(long nVerts, D3DTLVERTEX* v, ROOM_INFO* r);
void PrelightVertsMMX(long nVerts, D3DTLVERTEX* v, ROOM_INFO* r);
void InsertRoom(ROOM_INFO* r);
void CalcTriFaceNormal(D3DVECTOR* p1, D3DVECTOR* p2, D3DVECTOR* p3, D3DVECTOR* N);
void ProcessMeshData(long num_meshes);
void InitBuckets();
void DrawBucket(TEXTUREBUCKET* bucket);
void FindBucket(long tpage, D3DTLBUMPVERTEX** Vpp, long** nVtxpp);
void DrawBuckets();
void CreateVertexNormals(ROOM_INFO* r);

extern MESH_DATA** mesh_vtxbuf;
extern TEXTUREBUCKET Bucket[20];
extern float clip_left;
extern float clip_top;
extern float clip_right;
extern float clip_bottom;
extern long bWaterEffect;
extern long num_level_meshes;

extern long water_color_R;
extern long water_color_G;
extern long water_color_B;
