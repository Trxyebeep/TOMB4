#include "../tomb4/pch.h"
#include "3dmath.h"

void phd_PushMatrix()
{
	phd_mxptr[indices_count + M00] = phd_mxptr[M00];
	phd_mxptr[indices_count + M01] = phd_mxptr[M01];
	phd_mxptr[indices_count + M02] = phd_mxptr[M02];
	phd_mxptr[indices_count + M03] = phd_mxptr[M03];
	phd_mxptr[indices_count + M10] = phd_mxptr[M10];
	phd_mxptr[indices_count + M11] = phd_mxptr[M11];
	phd_mxptr[indices_count + M12] = phd_mxptr[M12];
	phd_mxptr[indices_count + M13] = phd_mxptr[M13];
	phd_mxptr[indices_count + M20] = phd_mxptr[M20];
	phd_mxptr[indices_count + M21] = phd_mxptr[M21];
	phd_mxptr[indices_count + M22] = phd_mxptr[M22];
	phd_mxptr[indices_count + M23] = phd_mxptr[M23];
	phd_mxptr += 12;
}

void phd_PushUnitMatrix()
{
	phd_mxptr += 12;
	phd_mxptr[M00] = 16384;
	phd_mxptr[M01] = 0;
	phd_mxptr[M02] = 0;
	phd_mxptr[M03] = 0;
	phd_mxptr[M10] = 0;
	phd_mxptr[M11] = 16384;
	phd_mxptr[M12] = 0;
	phd_mxptr[M13] = 0;
	phd_mxptr[M20] = 0;
	phd_mxptr[M21] = 0;
	phd_mxptr[M22] = 16384;
	phd_mxptr[M23] = 0;
}

long phd_TranslateRel(long x, long y, long z)
{
	phd_mxptr[M03] += x * phd_mxptr[M00] + y * phd_mxptr[M01] + z * phd_mxptr[M02];
	phd_mxptr[M13] += x * phd_mxptr[M10] + y * phd_mxptr[M11] + z * phd_mxptr[M12];
	phd_mxptr[M23] += x * phd_mxptr[M20] + y * phd_mxptr[M21] + z * phd_mxptr[M22];
	return 1;
}

void phd_RotX(short angle)
{
	long sin, cos, mx1, mx2;

	if (angle)
	{
		sin = phd_sin(angle);
		cos = phd_cos(angle);

		mx1 = cos * phd_mxptr[M01] + sin * phd_mxptr[M02];
		mx2 = cos * phd_mxptr[M02] - sin * phd_mxptr[M01];
		phd_mxptr[M01] = mx1 >> 14;
		phd_mxptr[M02] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M11] + sin * phd_mxptr[M12];
		mx2 = cos * phd_mxptr[M12] - sin * phd_mxptr[M11];
		phd_mxptr[M11] = mx1 >> 14;
		phd_mxptr[M12] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M21] + sin * phd_mxptr[M22];
		mx2 = cos * phd_mxptr[M22] - sin * phd_mxptr[M21];
		phd_mxptr[M21] = mx1 >> 14;
		phd_mxptr[M22] = mx2 >> 14;
	}
}

void phd_RotY(short angle)
{
	long sin, cos, mx1, mx2;

	if (angle)
	{
		sin = phd_sin(angle);
		cos = phd_cos(angle);

		mx1 = cos * phd_mxptr[M00] - sin * phd_mxptr[M02];
		mx2 = cos * phd_mxptr[M02] + sin * phd_mxptr[M00];
		phd_mxptr[M00] = mx1 >> 14;
		phd_mxptr[M02] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M10] - sin * phd_mxptr[M12];
		mx2 = cos * phd_mxptr[M12] + sin * phd_mxptr[M10];
		phd_mxptr[M10] = mx1 >> 14;
		phd_mxptr[M12] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M20] - sin * phd_mxptr[M22];
		mx2 = cos * phd_mxptr[M22] + sin * phd_mxptr[M20];
		phd_mxptr[M20] = mx1 >> 14;
		phd_mxptr[M22] = mx2 >> 14;
	}
}

void phd_RotZ(short angle)
{
	long sin, cos, mx1, mx2;

	if (angle)
	{
		sin = phd_sin(angle);
		cos = phd_cos(angle);

		mx1 = cos * phd_mxptr[M00] + sin * phd_mxptr[M01];
		mx2 = cos * phd_mxptr[M01] - sin * phd_mxptr[M00];
		phd_mxptr[M00] = mx1 >> 14;
		phd_mxptr[M01] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M10] + sin * phd_mxptr[M11];
		mx2 = cos * phd_mxptr[M11] - sin * phd_mxptr[M10];
		phd_mxptr[M10] = mx1 >> 14;
		phd_mxptr[M11] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M20] + sin * phd_mxptr[M21];
		mx2 = cos * phd_mxptr[M21] - sin * phd_mxptr[M20];
		phd_mxptr[M20] = mx1 >> 14;
		phd_mxptr[M21] = mx2 >> 14;
	}
}

void phd_RotYXZpack(long angles)
{
	long sin, cos, mx1, mx2;
	short angle;

	angle = (angles >> 10) & 0x3FF;
	angle <<= 6;

	if (angle)
	{
		sin = phd_sin(angle);
		cos = phd_cos(angle);

		mx1 = cos * phd_mxptr[M00] - sin * phd_mxptr[M02];
		mx2 = cos * phd_mxptr[M02] + sin * phd_mxptr[M00];
		phd_mxptr[M00] = mx1 >> 14;
		phd_mxptr[M02] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M10] - sin * phd_mxptr[M12];
		mx2 = cos * phd_mxptr[M12] + sin * phd_mxptr[M10];
		phd_mxptr[M10] = mx1 >> 14;
		phd_mxptr[M12] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M20] - sin * phd_mxptr[M22];
		mx2 = cos * phd_mxptr[M22] + sin * phd_mxptr[M20];
		phd_mxptr[M20] = mx1 >> 14;
		phd_mxptr[M22] = mx2 >> 14;
	}

	angle = (angles >> 20) & 0x3FF;
	angle <<= 6;

	if (angle)
	{
		sin = phd_sin(angle);
		cos = phd_cos(angle);

		mx1 = cos * phd_mxptr[M01] + sin * phd_mxptr[M02];
		mx2 = cos * phd_mxptr[M02] - sin * phd_mxptr[M01];
		phd_mxptr[M01] = mx1 >> 14;
		phd_mxptr[M02] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M11] + sin * phd_mxptr[M12];
		mx2 = cos * phd_mxptr[M12] - sin * phd_mxptr[M11];
		phd_mxptr[M11] = mx1 >> 14;
		phd_mxptr[M12] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M21] + sin * phd_mxptr[M22];
		mx2 = cos * phd_mxptr[M22] - sin * phd_mxptr[M21];
		phd_mxptr[M21] = mx1 >> 14;
		phd_mxptr[M22] = mx2 >> 14;
	}

	angle = angles & 0x3FF;
	angle <<= 6;

	if (angle)
	{
		sin = phd_sin(angle);
		cos = phd_cos(angle);

		mx1 = cos * phd_mxptr[M00] + sin * phd_mxptr[M01];
		mx2 = cos * phd_mxptr[M01] - sin * phd_mxptr[M00];
		phd_mxptr[M00] = mx1 >> 14;
		phd_mxptr[M01] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M10] + sin * phd_mxptr[M11];
		mx2 = cos * phd_mxptr[M11] - sin * phd_mxptr[M10];
		phd_mxptr[M10] = mx1 >> 14;
		phd_mxptr[M11] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M20] + sin * phd_mxptr[M21];
		mx2 = cos * phd_mxptr[M21] - sin * phd_mxptr[M20];
		phd_mxptr[M20] = mx1 >> 14;
		phd_mxptr[M21] = mx2 >> 14;
	}
}

void phd_RotYXZ(short y, short x, short z)
{
	long sin, cos, mx1, mx2;

	if (y)
	{
		sin = phd_sin(y);
		cos = phd_cos(y);

		mx1 = cos * phd_mxptr[M00] - sin * phd_mxptr[M02];
		mx2 = cos * phd_mxptr[M02] + sin * phd_mxptr[M00];
		phd_mxptr[M00] = mx1 >> 14;
		phd_mxptr[M02] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M10] - sin * phd_mxptr[M12];
		mx2 = cos * phd_mxptr[M12] + sin * phd_mxptr[M10];
		phd_mxptr[M10] = mx1 >> 14;
		phd_mxptr[M12] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M20] - sin * phd_mxptr[M22];
		mx2 = cos * phd_mxptr[M22] + sin * phd_mxptr[M20];
		phd_mxptr[M20] = mx1 >> 14;
		phd_mxptr[M22] = mx2 >> 14;
	}

	if (x)
	{
		sin = phd_sin(x);
		cos = phd_cos(x);

		mx1 = cos * phd_mxptr[M01] + sin * phd_mxptr[M02];
		mx2 = cos * phd_mxptr[M02] - sin * phd_mxptr[M01];
		phd_mxptr[M01] = mx1 >> 14;
		phd_mxptr[M02] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M11] + sin * phd_mxptr[M12];
		mx2 = cos * phd_mxptr[M12] - sin * phd_mxptr[M11];
		phd_mxptr[M11] = mx1 >> 14;
		phd_mxptr[M12] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M21] + sin * phd_mxptr[M22];
		mx2 = cos * phd_mxptr[M22] - sin * phd_mxptr[M21];
		phd_mxptr[M21] = mx1 >> 14;
		phd_mxptr[M22] = mx2 >> 14;
	}

	if (z)
	{
		sin = phd_sin(z);
		cos = phd_cos(z);

		mx1 = cos * phd_mxptr[M00] + sin * phd_mxptr[M01];
		mx2 = cos * phd_mxptr[M01] - sin * phd_mxptr[M00];
		phd_mxptr[M00] = mx1 >> 14;
		phd_mxptr[M01] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M10] + sin * phd_mxptr[M11];
		mx2 = cos * phd_mxptr[M11] - sin * phd_mxptr[M10];
		phd_mxptr[M10] = mx1 >> 14;
		phd_mxptr[M11] = mx2 >> 14;

		mx1 = cos * phd_mxptr[M20] + sin * phd_mxptr[M21];
		mx2 = cos * phd_mxptr[M21] - sin * phd_mxptr[M20];
		phd_mxptr[M20] = mx1 >> 14;
		phd_mxptr[M21] = mx2 >> 14;
	}
}

void phd_TranslateAbs(long x, long y, long z)
{
	long fx, fy, fz;

	fx = x - w2v_matrix[M03];
	fy = y - w2v_matrix[M13];
	fz = z - w2v_matrix[M23];
	phd_mxptr[M03] = fx * phd_mxptr[M00] + fy * phd_mxptr[M01] + fz * phd_mxptr[M02];
	phd_mxptr[M13] = fx * phd_mxptr[M10] + fy * phd_mxptr[M11] + fz * phd_mxptr[M12];
	phd_mxptr[M23] = fx * phd_mxptr[M20] + fy * phd_mxptr[M21] + fz * phd_mxptr[M22];
}

void phd_GetVectorAngles(long x, long y, long z, short* angles)
{
	short atan;

	angles[0] = (short)phd_atan(z, x);

	while ((short)x != x || (short)y != y || (short)z != z)
	{
		x >>= 2;
		y >>= 2;
		z >>= 2;
	}

	atan = (short)phd_atan(phd_sqrt(SQUARE(z) + SQUARE(x)), y);

	if ((y > 0 && atan > 0) || (y < 0 && atan < 0))
		atan = -atan;

	angles[1] = atan;
}

ulong mGetAngle(long x, long z, long x1, long z1)
{
	long dx, dz, octant, swap, angle;

	dx = x1 - x;
	dz = z1 - z;

	if (!dx && !dz)
		return 0;

	octant = 0;

	if (dx < 0)
	{
		octant = 4;
		dx = -dx;
	}

	if (dz < 0)
	{
		octant += 2;
		dz = -dz;
	}

	if (dz > dx)
	{
		octant++;
		swap = dx;
		dx = dz;
		dz = swap;
	}

	while (short(dz) != dz)
	{
		dx >>= 1;
		dz >>= 1;
	}

	angle = atanOctantTab[octant] + atanTab[(dz << 11) / dx];

	if (angle < 0)
		angle = -angle;

	return -angle & 0xFFFF;
}

void inject_3dmath(bool replace)
{
	INJECT(0x004902B0, phd_PushMatrix, replace);
	INJECT(0x00490310, phd_PushUnitMatrix, replace);
	INJECT(0x00490350, phd_TranslateRel, replace);
	INJECT(0x00490400, phd_RotX, replace);
	INJECT(0x004904B0, phd_RotY, replace);
	INJECT(0x00490560, phd_RotZ, replace);
	INJECT(0x00490610, phd_RotYXZpack, replace);
	INJECT(0x00490820, phd_RotYXZ, replace);
	INJECT(0x00490A10, phd_TranslateAbs, replace);
	INJECT(0x00490A90, phd_GetVectorAngles, replace);
	INJECT(0x0048FD40, mGetAngle, replace);
}
