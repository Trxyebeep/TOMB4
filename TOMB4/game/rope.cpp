#include "../tomb4/pch.h"
#include "rope.h"
#include "../specific/specificfx.h"
#include "../specific/3dmath.h"
#include "draw.h"
#include "control.h"
#include "lara_states.h"
#include "lara.h"
#include "../specific/input.h"

static PENDULUM NullPendulum = { {0, 0, 0}, {0, 0, 0}, 0, 0 };

ROPE_STRUCT RopeList[5];
PENDULUM CurrentPendulum;
long nRope = 0;

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
		Rope->Coords[i][0] = long(Output.x * zv + f_centerx);
		Rope->Coords[i][1] = long(Output.y * zv + f_centery);
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

	d = abs(SQUARE(a) + SQUARE(b) + SQUARE(c));
	e = phd_sqrt(d);
	mod = 65536 / e;
	v->x = (__int64)mod * v->x >> 16;
	v->y = (__int64)mod * v->y >> 16;
	v->z = (__int64)mod * v->z >> 16;
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
	l->pos.x_pos += long(-112 * mMXPtr[M02]);
	l->pos.y_pos += long(-112 * mMXPtr[M12]);
	l->pos.z_pos += long(-112 * mMXPtr[M22]);
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
	length = phd_sqrt(abs(SQUARE(d.x >> (W2V_SHIFT + 2)) + SQUARE(d.y >> 16) + SQUARE(d.z >> 16)));
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
	length = phd_sqrt(abs(SQUARE(d.x >> (W2V_SHIFT + 2)) + SQUARE(d.y >> (W2V_SHIFT + 2)) + SQUARE(d.z >> (W2V_SHIFT + 2))));
	scale = (length << (W2V_SHIFT + 2)) - rlength;
	Normalise(&d);
	delta.x = (__int64)scale * d.x >> (W2V_SHIFT + 2);
	delta.y = (__int64)scale * d.y >> (W2V_SHIFT + 2);
	delta.z = (__int64)scale * d.z >> (W2V_SHIFT + 2);
	vb->x -= delta.x;
	vb->y -= delta.y;
	vb->z -= delta.z;
}

void SetPendulumPoint(ROPE_STRUCT* Rope, long node)
{
	CurrentPendulum.Position.x = Rope->Segment[node].x;
	CurrentPendulum.Position.y = Rope->Segment[node].y;
	CurrentPendulum.Position.z = Rope->Segment[node].z;

	if (CurrentPendulum.node == -1)
	{
		CurrentPendulum.Velocity.x += Rope->Velocity[node].x;
		CurrentPendulum.Velocity.y += Rope->Velocity[node].y;
		CurrentPendulum.Velocity.z += Rope->Velocity[node].z;
	}

	CurrentPendulum.Rope = Rope;
	CurrentPendulum.node = node;
}

void SetPendulumVelocity(long x, long y, long z)
{
	long scale;

	if (2 * (CurrentPendulum.node >> 1) < 24)
	{
		scale = 4096 / (24 - 2 * (CurrentPendulum.node >> 1)) * 256;
		x = (__int64)scale * x >> (W2V_SHIFT + 2);
		y = (__int64)scale * y >> (W2V_SHIFT + 2);
		z = (__int64)scale * z >> (W2V_SHIFT + 2);
	}

	CurrentPendulum.Velocity.x += x;
	CurrentPendulum.Velocity.y += y;
	CurrentPendulum.Velocity.z += z;
}

void CalculateRope(ROPE_STRUCT* Rope)
{
	PENDULUM* Pendulum;
	PHD_VECTOR dir;
	long n, bSetFlag;

	bSetFlag = 0;

	if (Rope == &RopeList[lara.RopePtr])
	{
		Pendulum = &CurrentPendulum;

		if (CurrentPendulum.node != lara.RopeSegment + 1)
		{
			SetPendulumPoint(Rope, lara.RopeSegment + 1);
			bSetFlag = 1;
		}
	}
	else
	{
		Pendulum = &NullPendulum;

		if (lara.RopePtr == -1 && CurrentPendulum.Rope)
		{
			for (n = 0; n < CurrentPendulum.node; n++)
			{
				CurrentPendulum.Rope->Velocity[n].x = CurrentPendulum.Rope->Velocity[CurrentPendulum.node].x;
				CurrentPendulum.Rope->Velocity[n].y = CurrentPendulum.Rope->Velocity[CurrentPendulum.node].y;
				CurrentPendulum.Rope->Velocity[n].z = CurrentPendulum.Rope->Velocity[CurrentPendulum.node].z;
			}

			CurrentPendulum.Rope = 0;
			CurrentPendulum.node = -1;
			CurrentPendulum.Position.x = 0;
			CurrentPendulum.Position.y = 0;
			CurrentPendulum.Position.z = 0;
			CurrentPendulum.Velocity.x = 0;
			CurrentPendulum.Velocity.y = 0;
			CurrentPendulum.Velocity.z = 0;
		}
	}

	if (lara.RopePtr != -1)
	{
		dir.x = Pendulum->Position.x - Rope->Segment[0].x;
		dir.y = Pendulum->Position.y - Rope->Segment[0].y;
		dir.z = Pendulum->Position.z - Rope->Segment[0].z;
		Normalise(&dir);

		for (n = Pendulum->node; n >= 0; n--)
		{
			Rope->Segment[n].x = Rope->MeshSegment[n - 1].x + ((__int64)Rope->SegmentLength * dir.x >> (W2V_SHIFT + 2));
			Rope->Segment[n].y = Rope->MeshSegment[n - 1].y + ((__int64)Rope->SegmentLength * dir.y >> (W2V_SHIFT + 2));
			Rope->Segment[n].z = Rope->MeshSegment[n - 1].z + ((__int64)Rope->SegmentLength * dir.z >> (W2V_SHIFT + 2));
			Rope->Velocity[n].x = 0;
			Rope->Velocity[n].y = 0;
			Rope->Velocity[n].z = 0;
		}

		if (bSetFlag)
		{
			dir.x = Pendulum->Position.x - Rope->Segment[Pendulum->node].x;
			dir.y = Pendulum->Position.y - Rope->Segment[Pendulum->node].y;
			dir.z = Pendulum->Position.z - Rope->Segment[Pendulum->node].z;
			Rope->Segment[Pendulum->node].x = Pendulum->Position.x;
			Rope->Segment[Pendulum->node].y = Pendulum->Position.y;
			Rope->Segment[Pendulum->node].z = Pendulum->Position.z;

			for (n = Pendulum->node; n < 24; n++)
			{
				Rope->Segment[n].x -= dir.x;
				Rope->Segment[n].y -= dir.y;
				Rope->Segment[n].z -= dir.z;
				Rope->Velocity[n].x = 0;
				Rope->Velocity[n].y = 0;
				Rope->Velocity[n].z = 0;
			}
		}

		ModelRigidRope(&Rope->Segment[0], &Pendulum->Position, &Rope->Velocity[0], &Pendulum->Velocity, Rope->SegmentLength * Pendulum->node);
		Pendulum->Velocity.y += 0x60000;
		Pendulum->Position.x += Pendulum->Velocity.x;
		Pendulum->Position.y += Pendulum->Velocity.y;
		Pendulum->Position.z += Pendulum->Velocity.z;
		Pendulum->Velocity.x -= Pendulum->Velocity.x >> 8;
		Pendulum->Velocity.z -= Pendulum->Velocity.z >> 8;
	}

	for (n = Pendulum->node; n < 23; n++)
		ModelRigid(&Rope->Segment[n], &Rope->Segment[n + 1], &Rope->Velocity[n], &Rope->Velocity[n + 1], Rope->SegmentLength);

	for (n = 0; n < 24; n++)
	{
		Rope->Segment[n].x += Rope->Velocity[n].x;
		Rope->Segment[n].y += Rope->Velocity[n].y;
		Rope->Segment[n].z += Rope->Velocity[n].z;
	}

	for (n = Pendulum->node; n < 24; n++)
	{
		Rope->Velocity[n].y += 0x30000;

		if (Pendulum->Rope)
		{
			Rope->Velocity[n].x -= Rope->Velocity[n].x >> 4;
			Rope->Velocity[n].z -= Rope->Velocity[n].z >> 4;
		}
		else
		{
			Rope->Velocity[n].x -= Rope->Velocity[n].x >> 7;
			Rope->Velocity[n].z -= Rope->Velocity[n].z >> 7;
		}
	}

	Rope->Segment[0].x = 0;
	Rope->Segment[0].y = 0;
	Rope->Segment[0].z = 0;
	Rope->Velocity[0].x = 0;
	Rope->Velocity[0].y = 0;
	Rope->Velocity[0].z = 0;

	for (n = 0; n < 23; n++)
	{
		Rope->NormalisedSegment[n].x = Rope->Segment[n + 1].x - Rope->Segment[n].x;
		Rope->NormalisedSegment[n].y = Rope->Segment[n + 1].y - Rope->Segment[n].y;
		Rope->NormalisedSegment[n].z = Rope->Segment[n + 1].z - Rope->Segment[n].z;
		Normalise(&Rope->NormalisedSegment[n]);
	}

	if (Rope != &RopeList[lara.RopePtr])
	{
		Rope->MeshSegment[0].x = Rope->Segment[0].x;
		Rope->MeshSegment[0].y = Rope->Segment[0].y;
		Rope->MeshSegment[0].z = Rope->Segment[0].z;
		Rope->MeshSegment[1].x = Rope->Segment[0].x + ((__int64)Rope->SegmentLength * Rope->NormalisedSegment[0].x >> 16);
		Rope->MeshSegment[1].y = Rope->Segment[0].y + ((__int64)Rope->SegmentLength * Rope->NormalisedSegment[0].y >> 16);
		Rope->MeshSegment[1].z = Rope->Segment[0].z + ((__int64)Rope->SegmentLength * Rope->NormalisedSegment[0].z >> 16);

		for (n = 2; n < 24; n++)
		{
			Rope->MeshSegment[n].x = Rope->MeshSegment[n - 1].x + ((__int64)Rope->SegmentLength * Rope->NormalisedSegment[n - 1].x >> (W2V_SHIFT + 2));
			Rope->MeshSegment[n].y = Rope->MeshSegment[n - 1].y + ((__int64)Rope->SegmentLength * Rope->NormalisedSegment[n - 1].y >> (W2V_SHIFT + 2));
			Rope->MeshSegment[n].z = Rope->MeshSegment[n - 1].z + ((__int64)Rope->SegmentLength * Rope->NormalisedSegment[n - 1].z >> (W2V_SHIFT + 2));
		}
	}
	else
	{
		Rope->MeshSegment[Pendulum->node].x = Rope->Segment[Pendulum->node].x;
		Rope->MeshSegment[Pendulum->node].y = Rope->Segment[Pendulum->node].y;
		Rope->MeshSegment[Pendulum->node].z = Rope->Segment[Pendulum->node].z;
		Rope->MeshSegment[Pendulum->node + 1].x = Rope->Segment[Pendulum->node].x + ((__int64)Rope->SegmentLength * Rope->NormalisedSegment[Pendulum->node].x >> (W2V_SHIFT + 2));
		Rope->MeshSegment[Pendulum->node + 1].y = Rope->Segment[Pendulum->node].y + ((__int64)Rope->SegmentLength * Rope->NormalisedSegment[Pendulum->node].y >> (W2V_SHIFT + 2));
		Rope->MeshSegment[Pendulum->node + 1].z = Rope->Segment[Pendulum->node].z + ((__int64)Rope->SegmentLength * Rope->NormalisedSegment[Pendulum->node].z >> (W2V_SHIFT + 2));

		for (n = Pendulum->node + 1; n < 23; n++)
		{
			Rope->MeshSegment[n + 1].x = Rope->MeshSegment[n].x + ((__int64)Rope->SegmentLength * Rope->NormalisedSegment[n].x >> (W2V_SHIFT + 2));
			Rope->MeshSegment[n + 1].y = Rope->MeshSegment[n].y + ((__int64)Rope->SegmentLength * Rope->NormalisedSegment[n].y >> (W2V_SHIFT + 2));
			Rope->MeshSegment[n + 1].z = Rope->MeshSegment[n].z + ((__int64)Rope->SegmentLength * Rope->NormalisedSegment[n].z >> (W2V_SHIFT + 2));
		}

		for (n = 0; n < Pendulum->node; n++)
		{
			Rope->MeshSegment[n].x = Rope->Segment[n].x;
			Rope->MeshSegment[n].y = Rope->Segment[n].y;
			Rope->MeshSegment[n].z = Rope->Segment[n].z;
		}
	}
}

long RopeNodeCollision(ROPE_STRUCT* rope, long x, long y, long z, long rad)
{
	long rx, ry, rz;

	for (int i = 0; i < 22; ++i)
	{
		if (y > rope->Position.y + (rope->MeshSegment[i].y >> (W2V_SHIFT + 2)) && y < rope->Position.y + (rope->MeshSegment[i + 1].y >> (W2V_SHIFT + 2)))
		{
			rx = x - ((rope->MeshSegment[i + 1].x + rope->MeshSegment[i].x) >> 17) - rope->Position.x;
			ry = y - ((rope->MeshSegment[i + 1].y + rope->MeshSegment[i].y) >> 17) - rope->Position.y;
			rz = z - ((rope->MeshSegment[i + 1].z + rope->MeshSegment[i].z) >> 17) - rope->Position.z;

			if (SQUARE(rx) + SQUARE(ry) + SQUARE(rz) < SQUARE(rad + 64))
				return i;
		}
	}

	return -1;
}

void RopeControl(short item_num)
{
	ROPE_STRUCT* currope;

	currope = &RopeList[items[item_num].trigger_flags];

	if (TriggerActive(&items[item_num]))
	{
		currope->Active = 1;
		CalculateRope(currope);
	}
	else
		currope->Active = 0;
}

void RopeCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ROPE_STRUCT* rope;
	int i;
	short* bounds;
	long x, y, z, rad;

	rope = &RopeList[items[item_number].trigger_flags];

	if (input & IN_ACTION && lara.gun_status == LG_NO_ARMS && (l->current_anim_state == AS_REACH || l->current_anim_state == AS_UPJUMP) && l->gravity_status && l->fallspeed > 0 && rope->Active)
	{
		bounds = GetBoundsAccurate(l);
		x = l->pos.x_pos;
		y = l->pos.y_pos + bounds[2] + 512;
		z = l->pos.z_pos + (bounds[5] * phd_cos(l->pos.y_rot) >> 14);
		rad = l->current_anim_state == AS_REACH ? 128 : 320;
		i = RopeNodeCollision(rope, x, y, z, rad);

		if (i >= 0)
		{
			if (l->current_anim_state == AS_REACH)
			{
				l->anim_number = 379;
				l->current_anim_state = AS_ROPEFWD;
				lara.RopeFrame = (anims[ANIM_SWINGFWD].frame_base + 32) << 8;
				lara.RopeDFrame = (anims[ANIM_SWINGFWD].frame_base + 60) << 8;
			}
			else
			{
				l->anim_number = ANIM_UPJUMP2ROPE;
				l->current_anim_state = AS_ROPE;
			}

			l->frame_number = anims[l->anim_number].frame_base;
			l->gravity_status = 0;
			l->fallspeed = 0;
			lara.gun_status = LG_HANDS_BUSY;
			lara.RopePtr = items[item_number].trigger_flags;
			lara.RopeSegment = i;
			lara.RopeY = l->pos.y_rot;
			AlignLaraToRope(l);
			CurrentPendulum.Velocity.x = 0;
			CurrentPendulum.Velocity.y = 0;
			CurrentPendulum.Velocity.z = 0;
			ApplyVelocityToRope(i, l->pos.y_rot, lara_item->speed << 4);
		}
	}
}
