#pragma once
#include "../global/vars.h"

#define CalcColorSplit	( (void(__cdecl*)(D3DCOLOR, D3DCOLOR*)) 0x00484A20 )
#define InitialiseSortList	( (void(__cdecl*)()) 0x00481860 )
#define SortPolyList	( (void(__cdecl*)(long, SORTLIST**)) 0x00481810 )
#define DrawSortList	( (void(__cdecl*)()) 0x00480A50 )
