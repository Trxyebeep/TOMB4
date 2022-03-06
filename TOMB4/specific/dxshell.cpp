#include "../tomb4/pch.h"
#include "dxshell.h"

void DXBitMask2ShiftCnt(ulong mask, uchar* shift, uchar* count)
{
	uchar i;

	for (i = 0; !(mask & 1); i++)
		mask >>= 1;

	*shift = i;

	for (i = 0; mask & 1; i++)
		mask >>= 1;

	*count = i;
}

void inject_dxshell(bool replace)
{
	INJECT(0x00492240, DXBitMask2ShiftCnt, replace);
}
