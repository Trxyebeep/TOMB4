#include "../tomb4/pch.h"
#include "clipping.h"

long ZClipper(long n, D3DTLBUMPVERTEX* in, D3DTLBUMPVERTEX* out)
{
	D3DTLBUMPVERTEX* pIn;
	D3DTLBUMPVERTEX* pOut;
	D3DTLBUMPVERTEX* last;
	float lastZ, inZ, dz, iR, iG, iB, iA, lR, lG, lB, lA, fR, fG, fB, fA;
	long nPoints, r, g, b, a;

	pIn = in;
	last = &in[n - 1];
	pOut = out;

	for (nPoints = 0; n--; last = pIn++)
	{
		inZ = f_mznear - pIn->sz;
		lastZ = f_mznear - last->sz;

		if (((*(long*)&lastZ) | (*(long*)&inZ)) >= 0)
			continue;

		if (((*(long*)&lastZ) ^ (*(long*)&inZ)) < 0)
		{
			dz = inZ / (last->sz - pIn->sz);
			pOut->sx = ((last->tx - pIn->tx) * dz + pIn->tx) * f_mperspoznear + f_centerx;
			pOut->sy = ((last->ty - pIn->ty) * dz + pIn->ty) * f_mperspoznear + f_centery;
			pOut->rhw = f_moneoznear;
			pOut->tu = ((last->tu - pIn->tu) * dz + pIn->tu) * pOut->rhw;
			pOut->tv = ((last->tv - pIn->tv) * dz + pIn->tv) * pOut->rhw;

			iR = (float)CLRR(pIn->color);
			iG = (float)CLRG(pIn->color);
			iB = (float)CLRB(pIn->color);
			iA = (float)CLRA(pIn->color);

			lR = (float)CLRR(last->color);
			lG = (float)CLRG(last->color);
			lB = (float)CLRB(last->color);
			lA = (float)CLRA(last->color);

			fA = iA + (lA - iA) * dz;
			fR = iR + (lR - iR) * dz;
			fG = iG + (lG - iG) * dz;
			fB = iB + (lB - iB) * dz;
			a = (long)fA;
			r = (long)fR;
			g = (long)fG;
			b = (long)fB;
			pOut->color = RGBA(r, g, b, a);

			iR = (float)CLRR(pIn->specular);
			iG = (float)CLRG(pIn->specular);
			iB = (float)CLRB(pIn->specular);
			iA = (float)CLRA(pIn->specular);

			lR = (float)CLRR(last->specular);
			lG = (float)CLRG(last->specular);
			lB = (float)CLRB(last->specular);
			lA = (float)CLRA(last->specular);

			fA = iA + (lA - iA) * dz;
			fR = iR + (lR - iR) * dz;
			fG = iG + (lG - iG) * dz;
			fB = iB + (lB - iB) * dz;
			a = (long)fA;
			r = (long)fR;
			g = (long)fG;
			b = (long)fB;
			pOut->specular = RGBA(r, g, b, a);

			pOut++;
			nPoints++;
		}

		if ((*(long*)&inZ) < 0)
		{
			pOut->sx = pIn->sx;
			pOut->sy = pIn->sy;
			pOut->rhw = pIn->rhw;
			pOut->tu = pIn->tu * pIn->rhw;
			pOut->tv = pIn->tv * pIn->rhw;
			pOut->color = pIn->color;
			pOut->specular = pIn->specular;
			pOut++;
			nPoints++;
		}
	}

	if (nPoints < 3)
		nPoints = 0;

	return nPoints;
}

void inject_clipping(bool replace)
{
	INJECT(0x0046E4D0, ZClipper, replace);
}
