#pragma once
#include "../global/types.h"

void S_CalculateStaticMeshLight(long x, long y, long z, long shade, ROOM_INFO* r);
void InitItemDynamicLighting(ITEM_INFO* item);
void SetupDynamicLight(DYNAMIC* light, ITEM_INFO* item);
void SetupLight(PCLIGHT* light, ITEM_INFO* item, long* ambient);
void mApplyMatrix(float* matrix, FVECTOR* start, FVECTOR* dest);
void mApplyTransposeMatrix(float* matrix, FVECTOR* start, FVECTOR* dest);
void CreateLightList(ITEM_INFO* item);
void FadeLightList(PCLIGHT* lights, long nLights);
void InitObjectLighting(ITEM_INFO* item);
void CalcAmbientLight(ITEM_INFO* item);
void ResetLighting();

extern ITEM_INFO* current_item;
extern long StaticMeshShade;
extern long ambientR, ambientG, ambientB;

extern FVECTOR lGlobalMeshPos;
extern SUNLIGHT_STRUCT SunLights[64];
extern POINTLIGHT_STRUCT PointLights[64];
extern POINTLIGHT_STRUCT SpotLights[64];
extern long nSunLights, nPointLights, nSpotLights, nShadowLights, nTotalLights;
