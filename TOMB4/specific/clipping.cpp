#include "../tomb4/pch.h"
#include "clipping.h"
#include "3dmath.h"

#define VertClip(result, v1, v2)\
{\
	result.rhw = clipper * (v2->rhw - v1->rhw) + v1->rhw;\
	result.tu = clipper * (v2->tu - v1->tu) + v1->tu;\
	result.tv = clipper * (v2->tv - v1->tv) + v1->tv;\
}

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

long visible_zclip(D3DTLVERTEX* v0, D3DTLVERTEX* v1, D3DTLVERTEX* v2)
{
	return (v2->tu * v0->sz - v2->sz * v0->tu) * v1->tv
		+ (v2->sz * v0->tv - v2->tv * v0->sz) * v1->tu
		+ (v2->tv * v0->tu - v2->tu * v0->tv) * v1->sz < 0;
}

long XYUVGClipper(long n, D3DTLBUMPVERTEX* in)
{
	D3DTLBUMPVERTEX* v1;
	D3DTLBUMPVERTEX* v2;
	D3DTLBUMPVERTEX output[8];
	float cr1, cg1, cb1, ca1;
	float sr1, sg1, sb1, sa1;
	float cr2, cg2, cb2, ca2;
	float sr2, sg2, sb2, sa2;
	float clipper, fR, fG, fB, fA;
	long nPoints, r, g, b, a;

	v2 = &in[n - 1];
	cr2 = float(CLRR(v2->color));
	cg2 = float(CLRG(v2->color));
	cb2 = float(CLRB(v2->color));
	ca2 = float(CLRA(v2->color));
	sr2 = float(CLRR(v2->specular));
	sg2 = float(CLRG(v2->specular));
	sb2 = float(CLRB(v2->specular));
	sa2 = float(CLRA(v2->specular));

	nPoints = 0;
	
	for (int i = 0; i < n; i++)
	{
		v1 = v2;
		cr1 = cr2;
		cg1 = cg2;
		cb1 = cb2;
		ca1 = ca2;
		sr1 = sr2;
		sg1 = sg2;
		sb1 = sb2;
		sa1 = sa2;

		v2 = &in[i];
		cr2 = float(CLRR(v2->color));
		cg2 = float(CLRG(v2->color));
		cb2 = float(CLRB(v2->color));
		ca2 = float(CLRA(v2->color));
		sr2 = float(CLRR(v2->specular));
		sg2 = float(CLRG(v2->specular));
		sb2 = float(CLRB(v2->specular));
		sa2 = float(CLRA(v2->specular));

		if (v1->sx < f_left)
		{
			if (v2->sx < f_left)
				continue;

			clipper = (f_left - v2->sx) / (v1->sx - v2->sx);
			VertClip(output[nPoints], v2, v1);

			fA = ca2 + (ca1 - ca2) * clipper;
			fR = cr2 + (cr1 - cr2) * clipper;
			fG = cg2 + (cg1 - cg2) * clipper;
			fB = cb2 + (cb1 - cb2) * clipper;
			a = (long)fA;
			r = (long)fR;
			g = (long)fG;
			b = (long)fB;
			output[nPoints].color = RGBA(r, g, b, a);

			fA = sa2 + (sa1 - sa2) * clipper;
			fR = sr2 + (sr1 - sr2) * clipper;
			fG = sg2 + (sg1 - sg2) * clipper;
			fB = sb2 + (sb1 - sb2) * clipper;
			a = (long)fA;
			r = (long)fR;
			g = (long)fG;
			b = (long)fB;
			output[nPoints].specular = RGBA(r, g, b, a);

			output[nPoints].sx = f_left;
			output[nPoints].sy = clipper * (v1->sy - v2->sy) + v2->sy;
			nPoints++;
		}
		else if (v1->sx > f_right)
		{
			if (v2->sx > f_right)
				continue;

			clipper = (f_right - v2->sx) / (v1->sx - v2->sx);
			VertClip(output[nPoints], v2, v1);

			fA = ca2 + (ca1 - ca2) * clipper;
			fR = cr2 + (cr1 - cr2) * clipper;
			fG = cg2 + (cg1 - cg2) * clipper;
			fB = cb2 + (cb1 - cb2) * clipper;
			a = (long)fA;
			r = (long)fR;
			g = (long)fG;
			b = (long)fB;
			output[nPoints].color = RGBA(r, g, b, a);

			fA = sa2 + (sa1 - sa2) * clipper;
			fR = sr2 + (sr1 - sr2) * clipper;
			fG = sg2 + (sg1 - sg2) * clipper;
			fB = sb2 + (sb1 - sb2) * clipper;
			a = (long)fA;
			r = (long)fR;
			g = (long)fG;
			b = (long)fB;
			output[nPoints].specular = RGBA(r, g, b, a);

			output[nPoints].sx = f_right;
			output[nPoints].sy = clipper * (v1->sy - v2->sy) + v2->sy;
			nPoints++;
		}


		if (v2->sx < f_left)
		{
			clipper = (f_left - v2->sx) / (v1->sx - v2->sx);
			VertClip(output[nPoints], v2, v1);

			fA = ca2 + (ca1 - ca2) * clipper;
			fR = cr2 + (cr1 - cr2) * clipper;
			fG = cg2 + (cg1 - cg2) * clipper;
			fB = cb2 + (cb1 - cb2) * clipper;
			a = (long)fA;
			r = (long)fR;
			g = (long)fG;
			b = (long)fB;
			output[nPoints].color = RGBA(r, g, b, a);

			fA = sa2 + (sa1 - sa2) * clipper;
			fR = sr2 + (sr1 - sr2) * clipper;
			fG = sg2 + (sg1 - sg2) * clipper;
			fB = sb2 + (sb1 - sb2) * clipper;
			a = (long)fA;
			r = (long)fR;
			g = (long)fG;
			b = (long)fB;
			output[nPoints].specular = RGBA(r, g, b, a);

			output[nPoints].sx = f_left;
			output[nPoints].sy = clipper * (v1->sy - v2->sy) + v2->sy;
			nPoints++;
		}
		else if (v2->sx > f_right)
		{
			clipper = (f_right - v2->sx) / (v1->sx - v2->sx);
			VertClip(output[nPoints], v2, v1);

			fA = ca2 + (ca1 - ca2) * clipper;
			fR = cr2 + (cr1 - cr2) * clipper;
			fG = cg2 + (cg1 - cg2) * clipper;
			fB = cb2 + (cb1 - cb2) * clipper;
			a = (long)fA;
			r = (long)fR;
			g = (long)fG;
			b = (long)fB;
			output[nPoints].color = RGBA(r, g, b, a);

			fA = sa2 + (sa1 - sa2) * clipper;
			fR = sr2 + (sr1 - sr2) * clipper;
			fG = sg2 + (sg1 - sg2) * clipper;
			fB = sb2 + (sb1 - sb2) * clipper;
			a = (long)fA;
			r = (long)fR;
			g = (long)fG;
			b = (long)fB;
			output[nPoints].specular = RGBA(r, g, b, a);

			output[nPoints].sx = f_right;
			output[nPoints].sy = clipper * (v1->sy - v2->sy) + v2->sy;
			nPoints++;
		}
		else
		{
			output[nPoints].sx = v2->sx;
			output[nPoints].sy = v2->sy;
			output[nPoints].sz = v2->sz;
			output[nPoints].rhw = v2->rhw;
			output[nPoints].tu = v2->tu;
			output[nPoints].tv = v2->tv;
			output[nPoints].color = v2->color;
			output[nPoints].specular = v2->specular;
			nPoints++;
		}
	}

	if (nPoints < 3)
		return 0;

	n = nPoints;

	v2 = &output[n - 1];
	cr2 = float(CLRR(v2->color));
	cg2 = float(CLRG(v2->color));
	cb2 = float(CLRB(v2->color));
	ca2 = float(CLRA(v2->color));
	sr2 = float(CLRR(v2->specular));
	sg2 = float(CLRG(v2->specular));
	sb2 = float(CLRB(v2->specular));
	sa2 = float(CLRA(v2->specular));

	nPoints = 0;

	for (int i = 0; i < n; i++)
	{
		v1 = v2;
		cr1 = cr2;
		cg1 = cg2;
		cb1 = cb2;
		ca1 = ca2;
		sr1 = sr2;
		sg1 = sg2;
		sb1 = sb2;
		sa1 = sa2;

		v2 = &output[i];
		cr2 = float(CLRR(v2->color));
		cg2 = float(CLRG(v2->color));
		cb2 = float(CLRB(v2->color));
		ca2 = float(CLRA(v2->color));
		sr2 = float(CLRR(v2->specular));
		sg2 = float(CLRG(v2->specular));
		sb2 = float(CLRB(v2->specular));
		sa2 = float(CLRA(v2->specular));

		if (v1->sy < f_top)
		{
			if (v2->sy < f_top)
				continue;

			clipper = (f_top - v2->sy) / (v1->sy - v2->sy);
			VertClip(in[nPoints], v2, v1);

			fA = ca2 + (ca1 - ca2) * clipper;
			fR = cr2 + (cr1 - cr2) * clipper;
			fG = cg2 + (cg1 - cg2) * clipper;
			fB = cb2 + (cb1 - cb2) * clipper;
			a = (long)fA;
			r = (long)fR;
			g = (long)fG;
			b = (long)fB;
			in[nPoints].color = RGBA(r, g, b, a);

			fA = sa2 + (sa1 - sa2) * clipper;
			fR = sr2 + (sr1 - sr2) * clipper;
			fG = sg2 + (sg1 - sg2) * clipper;
			fB = sb2 + (sb1 - sb2) * clipper;
			a = (long)fA;
			r = (long)fR;
			g = (long)fG;
			b = (long)fB;
			in[nPoints].specular = RGBA(r, g, b, a);

			in[nPoints].sx = clipper * (v1->sx - v2->sx) + v2->sx;
			in[nPoints].sy = f_top;
			nPoints++;
		}
		else if (v1->sy > f_bottom)
		{
			if (v2->sy > f_bottom)
				continue;

			clipper = (f_bottom - v2->sy) / (v1->sy - v2->sy);
			VertClip(in[nPoints], v2, v1);

			fA = ca2 + (ca1 - ca2) * clipper;
			fR = cr2 + (cr1 - cr2) * clipper;
			fG = cg2 + (cg1 - cg2) * clipper;
			fB = cb2 + (cb1 - cb2) * clipper;
			a = (long)fA;
			r = (long)fR;
			g = (long)fG;
			b = (long)fB;
			in[nPoints].color = RGBA(r, g, b, a);

			fA = sa2 + (sa1 - sa2) * clipper;
			fR = sr2 + (sr1 - sr2) * clipper;
			fG = sg2 + (sg1 - sg2) * clipper;
			fB = sb2 + (sb1 - sb2) * clipper;
			a = (long)fA;
			r = (long)fR;
			g = (long)fG;
			b = (long)fB;
			in[nPoints].specular = RGBA(r, g, b, a);

			in[nPoints].sx = clipper * (v1->sx - v2->sx) + v2->sx;
			in[nPoints].sy = f_bottom;
			nPoints++;
		}

		if (v2->sy < f_top)
		{
			clipper = (f_top - v2->sy) / (v1->sy - v2->sy);
			VertClip(in[nPoints], v2, v1);

			fA = ca2 + (ca1 - ca2) * clipper;
			fR = cr2 + (cr1 - cr2) * clipper;
			fG = cg2 + (cg1 - cg2) * clipper;
			fB = cb2 + (cb1 - cb2) * clipper;
			a = (long)fA;
			r = (long)fR;
			g = (long)fG;
			b = (long)fB;
			in[nPoints].color = RGBA(r, g, b, a);

			fA = sa2 + (sa1 - sa2) * clipper;
			fR = sr2 + (sr1 - sr2) * clipper;
			fG = sg2 + (sg1 - sg2) * clipper;
			fB = sb2 + (sb1 - sb2) * clipper;
			a = (long)fA;
			r = (long)fR;
			g = (long)fG;
			b = (long)fB;
			in[nPoints].specular = RGBA(r, g, b, a);

			in[nPoints].sx = clipper * (v1->sx - v2->sx) + v2->sx;
			in[nPoints].sy = f_top;
			nPoints++;
		}
		else if (v2->sy > f_bottom)
		{
			clipper = (f_bottom - v2->sy) / (v1->sy - v2->sy);
			VertClip(in[nPoints], v2, v1);

			fA = ca2 + (ca1 - ca2) * clipper;
			fR = cr2 + (cr1 - cr2) * clipper;
			fG = cg2 + (cg1 - cg2) * clipper;
			fB = cb2 + (cb1 - cb2) * clipper;
			a = (long)fA;
			r = (long)fR;
			g = (long)fG;
			b = (long)fB;
			in[nPoints].color = RGBA(r, g, b, a);

			fA = sa2 + (sa1 - sa2) * clipper;
			fR = sr2 + (sr1 - sr2) * clipper;
			fG = sg2 + (sg1 - sg2) * clipper;
			fB = sb2 + (sb1 - sb2) * clipper;
			a = (long)fA;
			r = (long)fR;
			g = (long)fG;
			b = (long)fB;
			in[nPoints].specular = RGBA(r, g, b, a);

			in[nPoints].sx = clipper * (v1->sx - v2->sx) + v2->sx;
			in[nPoints].sy = f_bottom;
			nPoints++;
		}
		else
		{
			in[nPoints].sx = v2->sx;
			in[nPoints].sy = v2->sy;
			in[nPoints].sz = v2->sz;
			in[nPoints].rhw = v2->rhw;
			in[nPoints].tu = v2->tu;
			in[nPoints].tv = v2->tv;
			in[nPoints].color = v2->color;
			in[nPoints].specular = v2->specular;
			nPoints++;
		}
	}

	if (nPoints < 3)
		nPoints = 0;

	return nPoints;
}
