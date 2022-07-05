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

void inject_rope(bool replace)
{
	INJECT(0x00459410, DrawRopeList, replace);
	INJECT(0x00459F90, ProjectRopePoints, replace);
	INJECT(0x00459450, Normalise, replace);
	INJECT(0x00458A00, vMul, replace);
}
