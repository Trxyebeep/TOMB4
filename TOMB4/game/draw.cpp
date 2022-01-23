#include "../tomb4/pch.h"
#include "draw.h"
#include "../specific/3dmath.h"
#include "../specific/output.h"

void InitInterpolate(long frac, long rate)
{
	IM_rate = rate;
	IM_frac = frac;
	IMptr = IMstack;
	memcpy(IMstack, phd_mxptr, 48);
}

void phd_PopMatrix_I()
{
	phd_PopMatrix();
	IMptr -= 12;
}

void phd_PushMatrix_I()
{
	phd_PushMatrix();
	memcpy(IMptr + 12, IMptr, 48);
	IMptr += 12;
}

void phd_RotY_I(short ang)
{
	long* ptr;

	phd_RotY(ang);
	ptr = phd_mxptr;
	phd_mxptr = IMptr;
	phd_RotY(ang);
	phd_mxptr = ptr;
}

void phd_RotX_I(short ang)
{
	long* ptr;

	phd_RotX(ang);
	ptr = phd_mxptr;
	phd_mxptr = IMptr;
	phd_RotX(ang);
	phd_mxptr = ptr;
}

void phd_RotZ_I(short ang)
{
	long* ptr;

	phd_RotZ(ang);
	ptr = phd_mxptr;
	phd_mxptr = IMptr;
	phd_RotZ(ang);
	phd_mxptr = ptr;
}

void phd_TranslateRel_I(long x, long y, long z)
{
	long* ptr;

	phd_TranslateRel(x, y, z);
	ptr = phd_mxptr;
	phd_mxptr = IMptr;
	phd_TranslateRel(x, y, z);
	phd_mxptr = ptr;
}

void phd_TranslateRel_ID(long x, long y, long z, long x2, long y2, long z2)
{
	long* ptr;

	phd_TranslateRel(x, y, z);
	ptr = phd_mxptr;
	phd_mxptr = IMptr;
	phd_TranslateRel(x2, y2, z2);
	phd_mxptr = ptr;
}

void phd_RotYXZ_I(short y, short x, short z)
{
	long* ptr;

	phd_RotYXZ(y, x, z);
	ptr = phd_mxptr;
	phd_mxptr = IMptr;
	phd_RotYXZ(y, x, z);
	phd_mxptr = ptr;
}

void gar_RotYXZsuperpack_I(short** pprot1, short** pprot2, long skip)
{
	long* ptr;

	gar_RotYXZsuperpack(pprot1, skip);
	ptr = phd_mxptr;
	phd_mxptr = IMptr;
	gar_RotYXZsuperpack(pprot2, skip);
	phd_mxptr = ptr;
}

void gar_RotYXZsuperpack(short** pprot, long skip)
{
	ushort* prot;

	while (skip)
	{
		prot = (ushort*)*pprot;

		if (*prot & 0xC000)
			*pprot += 1;
		else
			*pprot += 2;

		skip--;
	}

	prot = (ushort*)*pprot;

	if (*prot >> 14)
	{
		if ((*prot >> 14) == 1)
			phd_RotX((short)((*prot & 4095) << 4));
		else if ((*prot >> 14) == 2)
			phd_RotY((short)((*prot & 4095) << 4));
		else
			phd_RotZ((short)((*prot & 4095) << 4));
	}
	else
	{
		phd_RotYXZpack(((ushort) * *pprot << 16) + (ushort)(*pprot)[1]);
		++*pprot;
	}

	++*pprot;
}

void phd_PutPolygons_I(short* ptr, long clip)
{
	phd_PushMatrix();
	InterpolateMatrix();
	phd_PutPolygons(ptr, clip);
	phd_PopMatrix();
}

void InterpolateMatrix()
{
	if (IM_rate == 2 || (IM_frac == 2 && IM_rate == 4))
	{
		phd_mxptr[M00] = (phd_mxptr[M00] + IMptr[M00]) >> 1;
		phd_mxptr[M01] = (phd_mxptr[M01] + IMptr[M01]) >> 1;
		phd_mxptr[M02] = (phd_mxptr[M02] + IMptr[M02]) >> 1;
		phd_mxptr[M03] = (phd_mxptr[M03] + IMptr[M03]) >> 1;
		phd_mxptr[M10] = (phd_mxptr[M10] + IMptr[M10]) >> 1;
		phd_mxptr[M11] = (phd_mxptr[M11] + IMptr[M11]) >> 1;
		phd_mxptr[M12] = (phd_mxptr[M12] + IMptr[M12]) >> 1;
		phd_mxptr[M13] = (phd_mxptr[M13] + IMptr[M13]) >> 1;
		phd_mxptr[M20] = (phd_mxptr[M20] + IMptr[M20]) >> 1;
		phd_mxptr[M21] = (phd_mxptr[M21] + IMptr[M21]) >> 1;
		phd_mxptr[M22] = (phd_mxptr[M22] + IMptr[M22]) >> 1;
		phd_mxptr[M23] = (phd_mxptr[M23] + IMptr[M23]) >> 1;
	}
	else if (IM_frac == 1)
	{
		phd_mxptr[M00] += (IMptr[M00] - phd_mxptr[M00]) >> 2;
		phd_mxptr[M01] += (IMptr[M01] - phd_mxptr[M01]) >> 2;
		phd_mxptr[M02] += (IMptr[M02] - phd_mxptr[M02]) >> 2;
		phd_mxptr[M03] += (IMptr[M03] - phd_mxptr[M03]) >> 2;
		phd_mxptr[M10] += (IMptr[M10] - phd_mxptr[M10]) >> 2;
		phd_mxptr[M11] += (IMptr[M11] - phd_mxptr[M11]) >> 2;
		phd_mxptr[M12] += (IMptr[M12] - phd_mxptr[M12]) >> 2;
		phd_mxptr[M13] += (IMptr[M13] - phd_mxptr[M13]) >> 2;
		phd_mxptr[M20] += (IMptr[M20] - phd_mxptr[M20]) >> 2;
		phd_mxptr[M21] += (IMptr[M21] - phd_mxptr[M21]) >> 2;
		phd_mxptr[M22] += (IMptr[M22] - phd_mxptr[M22]) >> 2;
		phd_mxptr[M23] += (IMptr[M23] - phd_mxptr[M23]) >> 2;
	}
	else
	{
		phd_mxptr[M00] = IMptr[M00] - ((IMptr[M00] - phd_mxptr[M00]) >> 2);
		phd_mxptr[M01] = IMptr[M01] - ((IMptr[M01] - phd_mxptr[M01]) >> 2);
		phd_mxptr[M02] = IMptr[M02] - ((IMptr[M02] - phd_mxptr[M02]) >> 2);
		phd_mxptr[M03] = IMptr[M03] - ((IMptr[M03] - phd_mxptr[M03]) >> 2);
		phd_mxptr[M10] = IMptr[M10] - ((IMptr[M10] - phd_mxptr[M10]) >> 2);
		phd_mxptr[M11] = IMptr[M11] - ((IMptr[M11] - phd_mxptr[M11]) >> 2);
		phd_mxptr[M12] = IMptr[M12] - ((IMptr[M12] - phd_mxptr[M12]) >> 2);
		phd_mxptr[M13] = IMptr[M13] - ((IMptr[M13] - phd_mxptr[M13]) >> 2);
		phd_mxptr[M20] = IMptr[M20] - ((IMptr[M20] - phd_mxptr[M20]) >> 2);
		phd_mxptr[M21] = IMptr[M21] - ((IMptr[M21] - phd_mxptr[M21]) >> 2);
		phd_mxptr[M22] = IMptr[M22] - ((IMptr[M22] - phd_mxptr[M22]) >> 2);
		phd_mxptr[M23] = IMptr[M23] - ((IMptr[M23] - phd_mxptr[M23]) >> 2);
	}
}

void InterpolateArmMatrix(long* mx)
{
	phd_mxptr[M00] = mx[M00];
	phd_mxptr[M01] = mx[M01];
	phd_mxptr[M02] = mx[M02];
	phd_mxptr[M10] = mx[M10];
	phd_mxptr[M11] = mx[M11];
	phd_mxptr[M12] = mx[M12];
	phd_mxptr[M20] = mx[M20];
	phd_mxptr[M21] = mx[M21];
	phd_mxptr[M22] = mx[M22];

	if (IM_rate == 2 || (IM_frac == 2 && IM_rate == 4))
	{
		phd_mxptr[M03] = (phd_mxptr[M03] + IMptr[M03]) >> 1;
		phd_mxptr[M13] = (phd_mxptr[M13] + IMptr[M13]) >> 1;
		phd_mxptr[M23] = (phd_mxptr[M23] + IMptr[M23]) >> 1;
	}
	else if (IM_frac == 1)
	{
		phd_mxptr[M03] += (IMptr[M03] - phd_mxptr[M03]) >> 2;
		phd_mxptr[M13] += (IMptr[M13] - phd_mxptr[M13]) >> 2;
		phd_mxptr[M23] += (IMptr[M23] - phd_mxptr[M23]) >> 2;
	}
	else
	{
		phd_mxptr[M03] = IMptr[M03] - ((IMptr[M03] - phd_mxptr[M03]) >> 2);
		phd_mxptr[M13] = IMptr[M13] - ((IMptr[M13] - phd_mxptr[M13]) >> 2);
		phd_mxptr[M23] = IMptr[M23] - ((IMptr[M23] - phd_mxptr[M23]) >> 2);
	}
}

void inject_draw(bool replace)
{
	INJECT(0x00450520, InitInterpolate, replace);
	INJECT(0x00450560, phd_PopMatrix_I, replace);
	INJECT(0x00450590, phd_PushMatrix_I, replace);
	INJECT(0x004505C0, phd_RotY_I, replace);
	INJECT(0x004505F0, phd_RotX_I, replace);
	INJECT(0x00450620, phd_RotZ_I, replace);
	INJECT(0x00450650, phd_TranslateRel_I, replace);
	INJECT(0x00450690, phd_TranslateRel_ID, replace);
	INJECT(0x004506E0, phd_RotYXZ_I, replace);
	INJECT(0x00450720, gar_RotYXZsuperpack_I, replace);
	INJECT(0x00450760, gar_RotYXZsuperpack, replace);
	INJECT(0x00450810, phd_PutPolygons_I, replace);
	INJECT(0x00450840, InterpolateMatrix, replace);
	INJECT(0x00450AB0, InterpolateArmMatrix, replace);
}
