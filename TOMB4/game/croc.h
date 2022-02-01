#pragma once
#include "../global/vars.h"

void inject_croc(bool replace);

void InitialiseCroc(short item_number);
void CrocControl(short item_number);

#define UpdateLocusts	( (void(__cdecl*)()) 0x00403870 )
#define DrawLocusts	( (void(__cdecl*)()) 0x00403C10 )
