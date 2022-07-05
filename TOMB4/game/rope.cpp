#include "../tomb4/pch.h"
#include "rope.h"
#include "../specific/specificfx.h"
#include "../specific/3dmath.h"
#include "draw.h"

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

void AlignLaraToRope(ITEM_INFO* l)
{
	ROPE_STRUCT* rope;
	PHD_VECTOR n, v, u, n2, up;
	PHD_VECTOR v1, v2;
	long* mptr;
	short* frame;
	long temp[indices_count];
	static long ropematrix[indices_count];
	long i, x, y, z, x1, y1, z1;
	short xyz[3];
	static short ropeangle;

	up.x = 4096;
	up.y = 0;
	up.z = 0;
	frame = GetBestFrame(l);
	ropeangle = lara.RopeY - 16380;
	rope = &RopeList[lara.RopePtr];
	i = lara.RopeSegment;
	GetRopePos(rope, (i - 1) * 128 + frame[7], &x, &y, &z);
	GetRopePos(rope, (i - 1) * 128 + frame[7] - 192, &x1, &y1, &z1);
	u.x = (x - x1) << (W2V_SHIFT + 2);
	u.y = (y - y1) << (W2V_SHIFT + 2);
	u.z = (z - z1) << (W2V_SHIFT + 2);
	Normalise(&u);
	u.x >>= 2;
	u.y >>= 2;
	u.z >>= 2;
	vMul(&u, mDotProduct(&up, &u), &v);
	v.x = up.x - v.x;
	v.y = up.y - v.y;
	v.z = up.z - v.z;
	v1 = v;
	v2 = v;
	n2 = u;
	vMul(&v1, phd_cos(ropeangle), &v1);
	vMul(&n2, mDotProduct(&n2, &v), &n2);
	vMul(&n2, 4096 - phd_cos(ropeangle), &n2);
	mCrossProduct(&u, &v, &v2);
	vMul(&v2, phd_sin(ropeangle), &v2);
	n2.x += v1.x;
	n2.y += v1.y;
	n2.z += v1.z;
	v.x = (n2.x + v2.x) << (W2V_SHIFT + 2);
	v.y = (n2.y + v2.y) << (W2V_SHIFT + 2);
	v.z = (n2.z + v2.z) << (W2V_SHIFT + 2);
	Normalise(&v);
	v.x >>= 2;
	v.y >>= 2;
	v.z >>= 2;
	mCrossProduct(&u, &v, &n);
	n.x <<= (W2V_SHIFT + 2);
	n.y <<= (W2V_SHIFT + 2);
	n.z <<= (W2V_SHIFT + 2);
	Normalise(&n);
	n.x >>= 2;
	n.y >>= 2;
	n.z >>= 2;
	temp[M00] = n.x;
	temp[M01] = u.x;
	temp[M02] = v.x;
	temp[M10] = n.y;
	temp[M11] = u.y;
	temp[M12] = v.y;
	temp[M20] = n.z;
	temp[M21] = u.z;
	temp[M22] = v.z;
	phd_GetMatrixAngles(temp, xyz);
	memcpy(ropematrix, temp, sizeof(ropematrix));
	l->pos.x_pos = rope->Position.x + (rope->MeshSegment[i].x >> (W2V_SHIFT + 2));
	l->pos.y_pos = rope->Position.y + (rope->MeshSegment[i].y >> (W2V_SHIFT + 2)) + lara.RopeOffset;
	l->pos.z_pos = rope->Position.z + (rope->MeshSegment[i].z >> (W2V_SHIFT + 2));
	phd_PushUnitMatrix();
	phd_RotYXZ(xyz[1], xyz[0], xyz[2]);
	mptr = phd_mxptr;
	l->pos.x_pos += -112 * mptr[2] >> W2V_SHIFT;
	l->pos.y_pos += -112 * mptr[6] >> W2V_SHIFT;
	l->pos.z_pos += -112 * mptr[10] >> W2V_SHIFT;
	phd_PopMatrix();
	l->pos.x_rot = xyz[0];
	l->pos.y_rot = xyz[1];
	l->pos.z_rot = xyz[2];
}

void ModelRigid(PHD_VECTOR* pa, PHD_VECTOR* pb, PHD_VECTOR* va, PHD_VECTOR* vb, long rlength)
{
	PHD_VECTOR delta, d, a, b;
	long length, scale;

	a.x = pb->x - pa->x;
	a.y = pb->y - pa->y;
	a.z = pb->z - pa->z;
	b.x = vb->x - va->x;
	b.y = vb->y - va->y;
	b.z = vb->z - va->z;
	d.x = a.x + b.x;
	d.y = a.y + b.y;
	d.z = a.z + b.z;
	length = phd_sqrt(ABS(SQUARE(d.x >> (W2V_SHIFT + 2)) + SQUARE(d.y >> 16) + SQUARE(d.z >> 16)));
	scale = ((length << (W2V_SHIFT + 2)) - rlength) >> 1;
	Normalise(&d);
	delta.x = (__int64)scale * d.x >> (W2V_SHIFT + 2);
	delta.y = (__int64)scale * d.y >> (W2V_SHIFT + 2);
	delta.z = (__int64)scale * d.z >> (W2V_SHIFT + 2);
	va->x += delta.x;
	va->y += delta.y;
	va->z += delta.z;
	vb->x -= delta.x;
	vb->y -= delta.y;
	vb->z -= delta.z;
}

void ModelRigidRope(PHD_VECTOR* pa, PHD_VECTOR* pb, PHD_VECTOR* va, PHD_VECTOR* vb, long rlength)
{
	PHD_VECTOR delta, d, a, b;
	long length, scale;

	a.x = pb->x - pa->x;
	a.y = pb->y - pa->y;
	a.z = pb->z - pa->z;
	b.x = vb->x;
	b.y = vb->y;
	b.z = vb->z;
	d.x = a.x + b.x;
	d.y = a.y + b.y;
	d.z = a.z + b.z;
	length = phd_sqrt(ABS(SQUARE(d.x >> (W2V_SHIFT + 2)) + SQUARE(d.y >> (W2V_SHIFT + 2)) + SQUARE(d.z >> (W2V_SHIFT + 2))));
	scale = (length << (W2V_SHIFT + 2)) - rlength;
	Normalise(&d);
	delta.x = (__int64)scale * d.x >> (W2V_SHIFT + 2);
	delta.y = (__int64)scale * d.y >> (W2V_SHIFT + 2);
	delta.z = (__int64)scale * d.z >> (W2V_SHIFT + 2);
	vb->x -= delta.x;
	vb->y -= delta.y;
	vb->z -= delta.z;
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
	INJECT(0x00458B90, AlignLaraToRope, replace);
	INJECT(0x00459510, ModelRigid, replace);
	INJECT(0x00459640, ModelRigidRope, replace);
}
