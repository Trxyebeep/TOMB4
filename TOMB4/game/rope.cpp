#include "../tomb4/pch.h"
#include "rope.h"
#include "../specific/specificfx.h"
#include "../specific/3dmath.h"

void DrawRopeList()
{
	for (int i = 0; i < nRope; i++)
	{
		if (RopeList[i].Active)
			DrawRope(&RopeList[i]);
	}
}

void ProjectRopePoints(ROPE_STRUCT* Rope)
{
	D3DVECTOR Output;
	PHD_VECTOR t;
	float zv;

	phd_PushMatrix();
	phd_TranslateAbs(Rope->Position.x, Rope->Position.y, Rope->Position.z);

	for (int i = 0; i < 24; i++)
	{
		t.x = Rope->MeshSegment[i].x >> (W2V_SHIFT + 2);
		t.y = Rope->MeshSegment[i].y >> (W2V_SHIFT + 2);
		t.z = Rope->MeshSegment[i].z >> (W2V_SHIFT + 2);
		Output.x = D3DVALUE(t.x * phd_mxptr[M00] + t.y * phd_mxptr[M01] + t.z * phd_mxptr[M02] + phd_mxptr[M03]);
		Output.y = D3DVALUE(t.x * phd_mxptr[M10] + t.y * phd_mxptr[M11] + t.z * phd_mxptr[M12] + phd_mxptr[M13]);
		Output.z = D3DVALUE(t.x * phd_mxptr[M20] + t.y * phd_mxptr[M21] + t.z * phd_mxptr[M22] + phd_mxptr[M23]);
		zv = phd_persp / Output.z;
		Rope->Coords[i][0] = long(Output.x * zv + (phd_winwidth >> 1));
		Rope->Coords[i][1] = long(Output.y * zv + (phd_winheight >> 1));
		Rope->Coords[i][2] = (long)Output.z;
	}

	phd_PopMatrix();
}

PHD_VECTOR* Normalise(PHD_VECTOR* v)
{
	long mod, a, b, c, d, e;

	a = v->x >> 16;
	b = v->y >> 16;
	c = v->z >> 16;

	if (!a && !b && !c)
		return v;

	d = ABS(SQUARE(a) + SQUARE(b) + SQUARE(c));
	e = phd_sqrt(d);
	mod = 65536 / e;
	v->x = (long long)mod * v->x >> 16;
	v->y = (long long)mod * v->y >> 16;
	v->z = (long long)mod * v->z >> 16;
	return v;
}

void vMul(PHD_VECTOR* v, long scale, PHD_VECTOR* d)
{
	d->x = scale * v->x >> W2V_SHIFT;
	d->y = scale * v->y >> W2V_SHIFT;
	d->z = scale * v->z >> W2V_SHIFT;
}

long mDotProduct(PHD_VECTOR* a, PHD_VECTOR* b)
{
	return (a->x * b->x + a->y * b->y + a->z * b->z) >> W2V_SHIFT;
}

void mCrossProduct(PHD_VECTOR* a, PHD_VECTOR* b, PHD_VECTOR* n)
{
	PHD_VECTOR t;

	t.x = a->y * b->z - a->z * b->y;
	t.y = a->z * b->x - a->x * b->z;
	t.z = a->x * b->y - a->y * b->x;
	n->x = t.x >> W2V_SHIFT;
	n->y = t.y >> W2V_SHIFT;
	n->z = t.z >> W2V_SHIFT;
}

void phd_GetMatrixAngles(long* m, short* dest)
{
	long sy, cy;
	short roll, pitch, yaw;

	pitch = (short)phd_atan(phd_sqrt(SQUARE(m[M22]) + SQUARE(m[M02])), m[M12]);

	if (m[M12] >= 0 && pitch > 0 || m[M12] < 0 && pitch < 0)
		pitch = -pitch;

	yaw = (short)phd_atan(m[M22], m[M02]);
	sy = phd_sin(yaw);
	cy = phd_cos(yaw);
	roll = (short)phd_atan(m[M00] * cy - m[M20] * sy, m[M21] * sy - m[M01] * cy);
	dest[0] = pitch;
	dest[1] = yaw;
	dest[2] = roll;
}

void GetRopePos(ROPE_STRUCT* rope, long pos, long* x, long* y, long* z)
{
	long segment;

	segment = pos >> 7;
	pos &= 0x7F;
	*x = (rope->NormalisedSegment[segment].x * pos >> (W2V_SHIFT + 2)) + (rope->MeshSegment[segment].x >> (W2V_SHIFT + 2)) + rope->Position.x;
	*y = (rope->NormalisedSegment[segment].y * pos >> (W2V_SHIFT + 2)) + (rope->MeshSegment[segment].y >> (W2V_SHIFT + 2)) + rope->Position.y;
	*z = (rope->NormalisedSegment[segment].z * pos >> (W2V_SHIFT + 2)) + (rope->MeshSegment[segment].z >> (W2V_SHIFT + 2)) + rope->Position.z;
}

void inject_rope(bool replace)
{
	INJECT(0x00459410, DrawRopeList, replace);
	INJECT(0x00459F90, ProjectRopePoints, replace);
	INJECT(0x00459450, Normalise, replace);
	INJECT(0x00458A00, vMul, replace);
	INJECT(0x00458A40, mDotProduct, replace);
	INJECT(0x00458A70, mCrossProduct, replace);
	INJECT(0x00458AD0, phd_GetMatrixAngles, replace);
	INJECT(0x00459060, GetRopePos, replace);
}
