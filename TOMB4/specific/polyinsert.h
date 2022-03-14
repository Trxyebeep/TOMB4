#pragma once
#include "../global/vars.h"

void inject_polyinsert(bool replace);

void HWR_DrawSortList(D3DTLBUMPVERTEX* info, short num_verts, short texture, short type);
void DrawSortList();
void CreateFogPos(FOGBULB_STRUCT* FogBulb);
void ControlFXBulb(FOGBULB_STRUCT* FogBulb);
void CreateFXBulbs();
void ClearFXFogBulbs();
void TriggerFXFogBulb(long x, long y, long z, long FXRad, long density, long r, long g, long b, long room_number);
long IsVolumetric();
int DistCompare(const void* a, const void* b);
void InitialiseFogBulbs();
void OmniEffect(D3DTLVERTEX* v);
void OmniFog(D3DTLVERTEX* v);
void AddTriClippedSorted(D3DTLVERTEX* v, short v0, short v1, short v2, TEXTURESTRUCT* tex, long double_sided);

#define CalcColorSplit	( (void(__cdecl*)(D3DCOLOR, D3DCOLOR*)) 0x00484A20 )
#define InitialiseSortList	( (void(__cdecl*)()) 0x00481860 )
#define SortPolyList	( (void(__cdecl*)(long, SORTLIST**)) 0x00481810 )
#define AddQuadClippedSorted	( (void(__cdecl*)(D3DTLVERTEX*, short, short, short, short, TEXTURESTRUCT*, long)) 0x004842A0 )
#define mD3DTransform	( (void(__cdecl*)(FVECTOR*, D3DMATRIX*)) 0x00481AE0 )
#define AddClippedPoly	( (void(__cdecl*)(D3DTLBUMPVERTEX*, long, D3DTLBUMPVERTEX*, TEXTURESTRUCT*)) 0x00482E40 )
