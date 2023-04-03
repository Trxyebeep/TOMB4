#include "../tomb4/pch.h"
#include "debris.h"
#include "control.h"
#include "../specific/function_stubs.h"
#include "tomb4fx.h"
#include "../specific/3dmath.h"
#include "draw.h"

DEBRIS_STRUCT debris[256];
long next_debris;
short DebrisFlags;

static MESH_DATA* DebrisMesh;
static long DebrisMeshC1;
static long DebrisMeshC2;
static long DebrisMeshC3;
static long DebrisMeshAmbient;
static long DebrisMeshFlags;

void UpdateDebris()
{
	DEBRIS_STRUCT* dptr;
	FLOOR_INFO* floor;
	long height, ceiling;

	for (int i = 0; i < 256; i++)
	{
		dptr = &debris[i];

		if (dptr->On)
		{
			dptr->Yvel += dptr->Gravity;

			if (dptr->Yvel > 4096)
				dptr->Yvel = 4096;

			dptr->Speed -= dptr->Speed >> 4;
			dptr->x += dptr->Speed * phd_sin(dptr->Dir) >> W2V_SHIFT;
			dptr->y += dptr->Yvel >> 4;
			dptr->z += dptr->Speed * phd_cos(dptr->Dir) >> W2V_SHIFT;
			floor = GetFloor(dptr->x, dptr->y, dptr->z, &dptr->RoomNumber);
			height = GetHeight(floor, dptr->x, dptr->y, dptr->z);
			ceiling = GetCeiling(floor, dptr->x, dptr->y, dptr->z);

			if (dptr->y >= height || dptr->y < ceiling)
				dptr->On = 0;
			else
			{
				dptr->XRot += dptr->Yvel >> 6;

				if (dptr->Yvel)
					dptr->YRot += dptr->Speed >> 5;

				if (room[dptr->RoomNumber].flags & ROOM_UNDERWATER)
					dptr->Yvel -= dptr->Yvel >> 2;
			}
		}
	}
}

void TriggerDebris(GAME_VECTOR* pos, void* TextInfo, short* Offsets, long* Vels, short rgb)
{
	DEBRIS_STRUCT* dptr;

	if (GetRandomControl() & 3)
	{
		if (rgb < 0)
			rgb = -rgb;
	}
	else if (rgb >= 0)
		TriggerShatterSmoke(pos->x, pos->y, pos->z);

	dptr = &debris[GetFreeDebris()];
	dptr->On = 1;
	dptr->x = pos->x;
	dptr->y = pos->y;
	dptr->z = pos->z;

	if (DebrisFlags & 1)
	{
		dptr->Dir = short(GetRandomControl() << 1);
		dptr->Speed = (GetRandomControl() & 0xF) + 16;
	}
	else
	{
		dptr->Dir = (short)phd_atan(Vels[2], Vels[0]);

		if (Vels[0] < 0)
			Vels[0] = -Vels[0];

		if (Vels[2] < 0)
			Vels[2] = -Vels[2];

		dptr->Speed = short((Vels[0] + Vels[2]) >> 2);
	}

	if (Vels[1])
	{
		dptr->Yvel = -512 - (GetRandomControl() & 0x1FF);
		dptr->Gravity = (GetRandomControl() & 0x3F) + 64;

		if (Vels[1] == -1)
			dptr->Yvel <<= 1;
		else if (Vels[1] == -2)
			dptr->Yvel >>= 1;
	}
	else
	{
		dptr->Yvel = 0;
		dptr->Gravity = (GetRandomControl() & 0x1F) + 32;
	}

	dptr->RoomNumber = pos->room_number;
	dptr->XYZOffsets1[0] = Offsets[0];
	dptr->XYZOffsets1[1] = Offsets[1];
	dptr->XYZOffsets1[2] = Offsets[2];
	dptr->XYZOffsets2[0] = Offsets[3];
	dptr->XYZOffsets2[1] = Offsets[4];
	dptr->XYZOffsets2[2] = Offsets[5];
	dptr->XYZOffsets3[0] = Offsets[6];
	dptr->XYZOffsets3[1] = Offsets[7];
	dptr->XYZOffsets3[2] = Offsets[8];

	if (DebrisFlags & 1)
	{
		dptr->YRot = uchar(GetRandomControl() << 1);
		dptr->XRot = dptr->YRot;
	}
	else
	{
		dptr->YRot = 0;
		dptr->XRot = 0;
	}

	dptr->TextInfo = TextInfo;
	dptr->r = rgb << 3;
	dptr->g = (rgb >> 5) << 3;
	dptr->b = (rgb >> 10) << 3;

	if (DebrisMesh->prelight)
	{
		dptr->color1 = DebrisMesh->prelight[DebrisMeshC1];
		dptr->color2 = DebrisMesh->prelight[DebrisMeshC2];
		dptr->color3 = DebrisMesh->prelight[DebrisMeshC3];
		dptr->ambient = 0;
	}
	else
	{
		dptr->color1 = 0;
		dptr->color2 = 0;
		dptr->color3 = 0;
		dptr->ambient = DebrisMeshAmbient;
	}

	dptr->flags = DebrisMeshFlags;
}

long GetFreeDebris()
{
	DEBRIS_STRUCT* dptr;
	long eldestage, eldestfree, free;

	free = next_debris;
	eldestfree = 0;
	eldestage = -0x4000;
	dptr = &debris[next_debris];

	for (int i = 0; i < 256; i++)
	{
		if (!dptr->On)
		{
			next_debris = (free + 1) & 0xFF;
			return free;
		}

		if (dptr->Yvel > eldestage)
		{
			eldestfree = free;
			eldestage = dptr->Yvel;
		}

		if (free == 255)
		{
			dptr = debris;
			free = 0;
		}
		else
		{
			free++;
			dptr++;
		}
	}

	next_debris = (eldestfree + 1) & 0xFF;
	return eldestfree;
}

void ShatterObject(SHATTER_ITEM* shatter_item, MESH_INFO* StaticMesh, short Num, short RoomNumber, long NoXZVel)
{
	MESH_DATA* mesh;
	TEXTURESTRUCT* tex;
	PHD_VECTOR TPos;
	PHD_VECTOR VPos;
	PHD_VECTOR pos;
	GAME_VECTOR vec;
	float* vtx;
	long* Vels;
	ushort* face_data;
	short* meshp;
	short* offsets;
	short* RotVerts;
	long lp, nVtx, nTris, nQuads, x, y, z;
	ushort v1, v2, v3, c;
	short rnd, RotY, rgb;

	rnd = 0;
	pos.x = 0;
	pos.y = 0;
	pos.z = 0;

	if (Num < 0)
	{
		Num = -Num;
		rnd = 1;
	}

	if (shatter_item)
	{
		meshp = shatter_item->meshp;
		TPos.x = shatter_item->Sphere.x;
		TPos.y = shatter_item->Sphere.y;
		TPos.z = shatter_item->Sphere.z;
		RotY = shatter_item->YRot;
		rgb = 0;
	}
	else
	{
		meshp = meshes[static_objects[StaticMesh->static_number].mesh_number];
		TPos.x = StaticMesh->x;
		TPos.y = StaticMesh->y;
		TPos.z = StaticMesh->z;
		RotY = StaticMesh->y_rot;
		rgb = StaticMesh->shade;
	}

	mesh = (MESH_DATA*)meshp;
	DebrisMesh = mesh;
	mesh->SourceVB->Lock(DDLOCK_READONLY, (LPVOID*)&vtx, 0);
	nVtx = mesh->nVerts;
	nTris = mesh->ngt3;
	nQuads = mesh->ngt4;

	if (nVtx > 256)
		nVtx = 256;

	phd_PushUnitMatrix();
	phd_RotY(RotY);

	offsets = (short*)&tsv_buffer[0];

	for (lp = 0; lp < nVtx; lp++)
	{
		x = (long)*vtx++;
		y = (long)*vtx++;
		z = (long)*vtx++;
		vtx += 5;

		offsets[0] = short(mMXPtr[M00] * x + mMXPtr[M01] * y + mMXPtr[M02] * z + mMXPtr[M03]);
		offsets[1] = short(mMXPtr[M10] * x + mMXPtr[M11] * y + mMXPtr[M12] * z + mMXPtr[M13]);
		offsets[2] = short(mMXPtr[M20] * x + mMXPtr[M21] * y + mMXPtr[M22] * z + mMXPtr[M23]);

		pos.x += offsets[0];
		pos.y += offsets[1];
		pos.z += offsets[2];

		offsets += 3;
	}

	mesh->SourceVB->Unlock();
	VPos.x = pos.x / lp;
	VPos.y = pos.y / lp;
	VPos.z = pos.z / lp;
	phd_PopMatrix();

	RotVerts = (short*)&tsv_buffer[0];
	Vels = (long*)&tsv_buffer[1536];
	offsets = (short*)&tsv_buffer[1548];
	vec.room_number = RoomNumber;
	DebrisMeshAmbient = room[RoomNumber].ambient;

	face_data = (ushort*)mesh->gt3;

	while (nTris && Num)
	{
		v1 = *face_data++;
		v2 = *face_data++;
		v3 = *face_data++;
		DebrisMeshC1 = v1;
		DebrisMeshC2 = v2;
		DebrisMeshC3 = v3;
		v1 *= 3;
		v2 *= 3;
		v3 *= 3;
		tex = (TEXTURESTRUCT*)*face_data++;
		DebrisMeshFlags = *face_data++;

		if (v1 < 0x300 && v2 < 0x300 && v3 < 0x300 && (!rnd || rnd == 1 && GetRandomControl() & 1))
		{
			offsets[0] = RotVerts[v1];
			offsets[1] = RotVerts[v1 + 1];
			offsets[2] = RotVerts[v1 + 2];

			offsets[3] = RotVerts[v2];
			offsets[4] = RotVerts[v2 + 1];
			offsets[5] = RotVerts[v2 + 2];

			offsets[6] = RotVerts[v3];
			offsets[7] = RotVerts[v3 + 1];
			offsets[8] = RotVerts[v3 + 2];

			vec.x = (offsets[0] + offsets[3] + offsets[6]) / 3;
			vec.y = (offsets[1] + offsets[4] + offsets[7]) / 3;
			vec.z = (offsets[2] + offsets[5] + offsets[8]) / 3;

			offsets[0] -= (short)vec.x;
			offsets[1] -= (short)vec.y;
			offsets[2] -= (short)vec.z;

			offsets[3] -= (short)vec.x;
			offsets[4] -= (short)vec.y;
			offsets[5] -= (short)vec.z;

			offsets[6] -= (short)vec.x;
			offsets[7] -= (short)vec.y;
			offsets[8] -= (short)vec.z;

			if (NoXZVel > 0)
			{
				Vels[0] = 0;
				Vels[1] = 0;
				Vels[2] = 0;
			}
			else
			{
				Vels[0] = vec.x - VPos.x;
				Vels[1] = vec.y - VPos.y;
				Vels[2] = vec.z - VPos.z;
			}

			if (NoXZVel < 0)
				Vels[1] = NoXZVel;

			vec.x += TPos.x;
			vec.y += TPos.y;
			vec.z += TPos.z;
			c = rgb;

			if (shatter_item && shatter_item->Flags & 0x400)
				c = -rgb;

			TriggerDebris(&vec, tex, offsets, Vels, c);
			Num--;
		}

		nTris--;
	}

	face_data = (ushort*)mesh->gt4;

	while (nQuads && Num)
	{
		v1 = *face_data++;
		v2 = *face_data++;
		face_data++;
		v3 = *face_data++;
		DebrisMeshC1 = v1;
		DebrisMeshC2 = v2;
		DebrisMeshC3 = v3;
		v1 *= 3;
		v2 *= 3;
		v3 *= 3;
		tex = (TEXTURESTRUCT*)*face_data++;
		DebrisMeshFlags = *face_data++;

		if (v1 < 0x300 && v2 < 0x300 && v3 < 0x300 && (!rnd || rnd == 1 && GetRandomControl() & 1))
		{
			offsets[0] = RotVerts[v1];
			offsets[1] = RotVerts[v1 + 1];
			offsets[2] = RotVerts[v1 + 2];

			offsets[3] = RotVerts[v2];
			offsets[4] = RotVerts[v2 + 1];
			offsets[5] = RotVerts[v2 + 2];

			offsets[6] = RotVerts[v3];
			offsets[7] = RotVerts[v3 + 1];
			offsets[8] = RotVerts[v3 + 2];

			vec.x = (offsets[0] + offsets[3] + offsets[6]) / 3;
			vec.y = (offsets[1] + offsets[4] + offsets[7]) / 3;
			vec.z = (offsets[2] + offsets[5] + offsets[8]) / 3;

			offsets[0] -= (short)vec.x;
			offsets[1] -= (short)vec.y;
			offsets[2] -= (short)vec.z;

			offsets[3] -= (short)vec.x;
			offsets[4] -= (short)vec.y;
			offsets[5] -= (short)vec.z;

			offsets[6] -= (short)vec.x;
			offsets[7] -= (short)vec.y;
			offsets[8] -= (short)vec.z;

			if (NoXZVel > 0)
			{
				Vels[0] = 0;
				Vels[1] = 0;
				Vels[2] = 0;
			}
			else
			{
				Vels[0] = vec.x - VPos.x;
				Vels[1] = vec.y - VPos.y;
				Vels[2] = vec.z - VPos.z;
			}

			if (NoXZVel < 0)
				Vels[1] = NoXZVel;

			vec.x += TPos.x;
			vec.y += TPos.y;
			vec.z += TPos.z;
			c = rgb;

			if (shatter_item && shatter_item->Flags & 0x400)
				c = -rgb;

			TriggerDebris(&vec, tex, offsets, Vels, c);
			Num--;
		}

		nQuads--;
	}
}
