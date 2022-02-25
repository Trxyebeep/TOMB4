#pragma once
#include "../global/vars.h"

void inject_functbl(bool replace);

void SetFogColor(long r, long g, long b);

#define AddQuadSorted	( *(void(__cdecl**)(D3DTLVERTEX*, short, short, short, short, TEXTURESTRUCT*, long)) 0x00533AD4 )
#define AddQuadZBuffer	( *(void(__cdecl**)(D3DTLVERTEX*, short, short, short, short, TEXTURESTRUCT*, long)) 0x00533AD8 )
#define AddTriZBuffer	( *(void(__cdecl**)(D3DTLVERTEX*, short, short, short, TEXTURESTRUCT*, long)) 0x00533AE0 )
#define AddTriSorted	( *(void(__cdecl**)(D3DTLVERTEX*, short, short, short, TEXTURESTRUCT*, long)) 0x00533ACC )
#define AddLineSorted	( *(void(__cdecl**)(D3DTLVERTEX*, D3DTLVERTEX*, long)) 0x00533AD0 )
#define IsVisible	( *(bool(__cdecl**)(D3DTLVERTEX, D3DTLVERTEX, D3DTLVERTEX)) 0x00533AE4 )
#define _BeginScene	( *(HRESULT(__cdecl**)()) 0x00533AE8 )
#define _EndScene	( *(HRESULT(__cdecl**)()) 0x00533ADC )
#define _NVisible	( (bool(__cdecl*)(D3DTLVERTEX, D3DTLVERTEX, D3DTLVERTEX)) 0x00476990 )

#define InitialiseFunctionTable	( (void(__cdecl*)()) 0x00476A30 )
#define HWInitialise	( (void(__cdecl*)()) 0x00476B30 )
