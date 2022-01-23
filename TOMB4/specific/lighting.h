#pragma once
#include "../global/vars.h"

void inject_lighting(bool replace);

void S_CalculateStaticMeshLight(int x, int y, int z, int shade, ROOM_INFO* r);
void InitItemDynamicLighting(ITEM_INFO* item);
void InitDynamicLighting();
void SetupLight(D3DLIGHT_STRUCT* d3dlight, PCLIGHT* light, long* ambient);
void ClearDynamicLighting();
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
