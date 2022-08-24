#pragma once
#include "../global/types.h"

void S_CalculateStaticMeshLight(long x, long y, long z, long shade, ROOM_INFO* r);
void InitItemDynamicLighting(ITEM_INFO* item);
void InitDynamicLighting();
void SetupLight(D3DLIGHT_STRUCT* d3dlight, PCLIGHT* light, long* ambient);
void ClearDynamicLighting();
void mApplyMatrix(float* matrix, FVECTOR* start, FVECTOR* dest);
void mApplyTransposeMatrix(float* matrix, FVECTOR* start, FVECTOR* dest);
void ApplyMatrix(long* matrix, PHD_VECTOR* start, PHD_VECTOR* dest);
void ApplyTransposeMatrix(long* matrix, PHD_VECTOR* start, PHD_VECTOR* dest);
void MallocD3DLights();
void CreateD3DLights();
void FreeD3DLights();
void CreateLightList(ITEM_INFO* item);
void FadeLightList(PCLIGHT* lights, long nLights);
void InitObjectLighting(ITEM_INFO* item);
void ClearObjectLighting();
void CalcAmbientLight(ITEM_INFO* item);

extern ITEM_INFO* current_item;
extern long StaticMeshShade;
extern long MaxRoomLights;
