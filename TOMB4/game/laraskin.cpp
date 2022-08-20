#include "../tomb4/pch.h"
#include "laraskin.h"
#include "objects.h"
#include "delstuff.h"
#include "gameflow.h"
#include "control.h"

static uchar SkinJoints[14][4] =
{
	{ 0, 1, 1, 3 },
	{ 1, 2, 4, 5 },
	{ 2, 3, 6, 7 },
	{ 0, 4, 2, 8 },
	{ 4, 5, 9, 10 },
	{ 5, 6, 11, 12 },
	{ 7, 0, 13, 0 },
	{ 7, 8, 14, 17 },
	{ 8, 9, 18, 19 },
	{ 9, 10, 20, 21 },
	{ 7, 11, 15, 22 },
	{ 11, 12, 23, 24 },
	{ 12, 13, 25, 26 },
	{ 14, 7, 27, 16 }
};

static char HairSkinVertNums[6][12] =
{
	{ 37, 38, 39, 40, -1, 0, 0, 0, 0, 0, 0, 0 },
	{ 79, 77, 78, 76, -1, 0, 0, 0, 0, 0, 0, 0 },
	{ 68, 71, 69, 70, -1, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 1, 2, 3, -1, 0, 0, 0, 0, 0, 0, 0 },
	{ 4, 5, 6, 7, -1, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 3, 1, 2, -1, 0, 0, 0, 0, 0, 0, 0 }
};

static char HairScratchVertNums[3][12] =
{
	{ 0, 3, 1, 2, -1, 0, 0, 0, 0, 0, 0, 0 },
	{ 4, 5, 6, 7, -1, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 1, 2, 3, -1, 0, 0, 0, 0, 0, 0, 0 }
};

static PHD_VECTOR* SkinXYZPtr;
static PHD_VECTOR BelowMeshXYZ;
static PHD_VECTOR JointMeshXYZ;
static PHD_VECTOR AboveMeshXYZ;
static PHD_VECTOR XYZList[32];
static uchar PointsToCalc[26][12];

void PushXYZ()
{
	PHD_VECTOR* ptr;

	ptr = SkinXYZPtr++;
	SkinXYZPtr->x = ptr->x;
	SkinXYZPtr->y = ptr->y;
	SkinXYZPtr->z = ptr->z;
}

void PopXYZ()
{
	SkinXYZPtr--;
}

void CreateSkinningData()
{
	MESH_DATA* aboveMesh;
	MESH_DATA* belowMesh;
	MESH_DATA* jointMesh;
	MESH_DATA* hairMesh;
	OBJECT_INFO* obj;
	short** meshpp;
	long* bone;
	short* joint;
	short* LaraMesh;
	long vertCount, aboveVerts, belowVerts, jointVerts, laraVerts, laraX, laraY, laraZ, jointX, jointY, jointZ, calcPointsCounter;
	short aboveMeshNum, belowMeshNum, jointMeshNum;
	uchar vertBuf[128];

	for (int i = 0; i < 14; i++)
	{
		SkinXYZPtr = XYZList;
		SkinXYZPtr->x = 0;
		SkinXYZPtr->y = 0;
		SkinXYZPtr->z = 0;
		obj = &objects[LARA_SKIN];
		meshpp = &meshes[obj->mesh_index];
		bone = &bones[obj->bone_index];
		aboveMeshNum = SkinJoints[i][0];
		belowMeshNum = SkinJoints[i][1];
		aboveMesh = (MESH_DATA*)*meshpp;
		belowMesh = (MESH_DATA*)*meshpp;
		jointMesh = (MESH_DATA*)*meshpp;

		if (aboveMeshNum)
		{
			meshpp++;

			for (int j = 1; j < obj->nmeshes; j++, bone += 4, meshpp++)
			{
				if (bone[0] & 1)
					PopXYZ();

				if (bone[0] & 2)
					PushXYZ();

				SkinXYZPtr->x += bone[1];
				SkinXYZPtr->y += bone[2];
				SkinXYZPtr->z += bone[3];

				if (aboveMeshNum == j)
				{
					aboveMesh = (MESH_DATA*)*meshpp;
					AboveMeshXYZ.x = SkinXYZPtr->x;
					AboveMeshXYZ.y = SkinXYZPtr->y;
					AboveMeshXYZ.z = SkinXYZPtr->z;
					break;
				}
			}
		}
		else
		{
			aboveMesh = (MESH_DATA*)*meshpp;
			AboveMeshXYZ.x = 0;
			AboveMeshXYZ.y = 0;
			AboveMeshXYZ.z = 0;
		}

		SkinXYZPtr = XYZList;
		SkinXYZPtr->x = 0;
		SkinXYZPtr->y = 0;
		SkinXYZPtr->z = 0;
		meshpp = &meshes[obj->mesh_index];
		bone = &bones[obj->bone_index];

		if (belowMeshNum)
		{
			meshpp++;

			for (int j = 1; j < obj->nmeshes; j++, bone += 4, meshpp++)
			{
				if (bone[0] & 1)
					PopXYZ();

				if (bone[0] & 2)
					PushXYZ();

				SkinXYZPtr->x += bone[1];
				SkinXYZPtr->y += bone[2];
				SkinXYZPtr->z += bone[3];

				if (belowMeshNum == j)
				{
					belowMesh = (MESH_DATA*)*meshpp;
					BelowMeshXYZ.x = SkinXYZPtr->x;
					BelowMeshXYZ.y = SkinXYZPtr->y;
					BelowMeshXYZ.z = SkinXYZPtr->z;
					break;
				}
			}
		}
		else
		{
			belowMesh = (MESH_DATA*)*meshpp;
			BelowMeshXYZ.x = 0;
			BelowMeshXYZ.y = 0;
			BelowMeshXYZ.z = 0;
		}

		SkinXYZPtr = XYZList;
		SkinXYZPtr->x = 0;
		SkinXYZPtr->y = 0;
		SkinXYZPtr->z = 0;
		obj = &objects[LARA_SKIN_JOINTS];
		meshpp = &meshes[obj->mesh_index + 1];
		bone = &bones[obj->bone_index];
		jointMeshNum = i + 1;

		for (int j = 1; j < obj->nmeshes; j++, bone += 4, meshpp++)
		{
			if (*bone & 1)
				PopXYZ();

			if (*bone & 2)
				PushXYZ();

			SkinXYZPtr->x += bone[1];
			SkinXYZPtr->y += bone[2];
			SkinXYZPtr->z += bone[3];

			if (jointMeshNum == j)
			{
				jointMesh = (MESH_DATA*)*meshpp;
				JointMeshXYZ.x = SkinXYZPtr->x;
				JointMeshXYZ.y = SkinXYZPtr->y;
				JointMeshXYZ.z = SkinXYZPtr->z;
				break;
			}
		}

		vertCount = 0;
		aboveVerts = 0;
		jointVerts = jointMesh->nVerts & 0xFF;
		laraVerts = aboveMesh->nVerts & 0xFF;

		if (jointVerts > 0)
			memset(vertBuf, 0, jointVerts);

		joint = &jointMesh->nNorms;

		for (int j = 0; j < jointVerts; j++)
		{
			jointX = JointMeshXYZ.x + joint[0];
			jointY = JointMeshXYZ.y + joint[1];
			jointZ = JointMeshXYZ.z + joint[2];
			joint += 3;
			LaraMesh = &aboveMesh->nNorms;

			for (int ii = 0; ii < laraVerts; ii++)
			{
				laraX = AboveMeshXYZ.x + LaraMesh[0];
				laraY = AboveMeshXYZ.y + LaraMesh[1];
				laraZ = AboveMeshXYZ.z + LaraMesh[2];
				LaraMesh += 3;

				if (abs(laraX - jointX) <= 1 && abs(laraY - jointY) <= 1 && abs(laraZ - jointZ) <= 1)
				{
					vertCount++;
					vertBuf[j] = 1;
					SkinVertNums[SkinJoints[i][2]][aboveVerts] = ii;
					ScratchVertNums[SkinJoints[i][2]][aboveVerts] = j;
					aboveVerts++;
				}
			}
		}

		SkinVertNums[SkinJoints[i][2]][aboveVerts] = -1;
		ScratchVertNums[SkinJoints[i][2]][aboveVerts] = -1;
		belowVerts = 0;
		laraVerts = belowMesh->nVerts & 0xFF;
		joint = &jointMesh->nNorms;

		for (int j = 0; j < jointVerts; j++)
		{
			jointX = JointMeshXYZ.x + joint[0];
			jointY = JointMeshXYZ.y + joint[1];
			jointZ = JointMeshXYZ.z + joint[2];
			joint += 3;
			LaraMesh = &belowMesh->nNorms;

			for (int ii = 0; ii < laraVerts; ii++)
			{
				laraY = BelowMeshXYZ.y + LaraMesh[1];
				laraX = BelowMeshXYZ.x + LaraMesh[0];
				laraZ = BelowMeshXYZ.z + LaraMesh[2];
				LaraMesh += 3;

				if (abs(laraX - jointX) <= 1 && abs(laraY - jointY) <= 1 && abs(laraZ - jointZ) <= 1)
				{
					vertCount++;
					vertBuf[j] = 1;
					SkinVertNums[SkinJoints[i][3]][belowVerts] = ii;
					ScratchVertNums[SkinJoints[i][3]][belowVerts] = j;
					belowVerts++;
				}
			}
		}

		SkinVertNums[SkinJoints[i][3]][belowVerts] = -1;
		ScratchVertNums[SkinJoints[i][3]][belowVerts] = -1;
		calcPointsCounter = 0;

		if (vertCount == jointVerts)
			jointMesh->nVerts <<= 8;
		else
		{
			for (int j = 0; j < jointVerts; j++)
			{
				if (!vertBuf[j])
				{
					PointsToCalc[jointMeshNum - 1][calcPointsCounter] = j;
					calcPointsCounter++;
				}
			}
		}

		PointsToCalc[jointMeshNum - 1][calcPointsCounter] = 0xFF;
	}

	for (int i = 0; ; i++)
	{
		if (gfLevelFlags & GF_YOUNGLARA)
		{
			SkinVertNums[28][i] = HairSkinVertNums[1][i];
			SkinVertNums[34][i] = HairSkinVertNums[2][i];
		}
		else
		{
			SkinVertNums[28][i] = HairSkinVertNums[0][i];
			SkinVertNums[34][i] = HairSkinVertNums[0][i];
		}

		SkinVertNums[29][i] = HairSkinVertNums[3][i];
		SkinVertNums[35][i] = HairSkinVertNums[3][i];
		ScratchVertNums[28][i] = HairSkinVertNums[5][i];
		ScratchVertNums[34][i] = HairScratchVertNums[0][i];
		ScratchVertNums[29][i] = HairScratchVertNums[1][i];
		ScratchVertNums[35][i] = HairScratchVertNums[1][i];

		if (HairSkinVertNums[0][i] == -1)
			break;
	}

	for (int i = 0; i < 4; i += 2)
	{
		for (int j = 0; ; j++)
		{
			SkinVertNums[i + 30][j] = HairSkinVertNums[4][j];
			SkinVertNums[i + 31][j] = HairSkinVertNums[3][j];
			ScratchVertNums[i + 30][j] = HairScratchVertNums[2][j];
			ScratchVertNums[i + 31][j] = HairScratchVertNums[1][j];
			ScratchVertNums[i + 36][j] = HairScratchVertNums[2][j];
			ScratchVertNums[i + 37][j] = HairScratchVertNums[1][j];
			SkinVertNums[i + 36][j] = HairSkinVertNums[4][j];
			SkinVertNums[i + 37][j] = HairSkinVertNums[3][j];

			if (HairSkinVertNums[2][j] == -1)
				break;
		}
	}

	obj = &objects[HAIR];
	meshpp = &meshes[obj->mesh_index];

	for (int i = 0; i < 3; i++, meshpp += 2)
	{
		hairMesh = (MESH_DATA*)*meshpp;
		hairMesh->nVerts <<= 8;
	}

	OptomiseSkinningData();
}

void OptomiseSkinningData()
{
	OBJECT_INFO* object;
	short** meshpp;
	short* JointMesh, * MeshJ, * MeshNormals, * Src, * Dest;
	short VertTable[128];
	short NormalTable[128];
	long c, lp, lp1, numvertsj, numvertstocalc, padval, numtris, numquads;
	uchar RemapTable[32];


	object = &objects[LARA_SKIN_JOINTS];
	meshpp = &meshes[object->mesh_index + 1];

	for (c = 0; c < 14; ++c)
	{
		JointMesh = *meshpp;
		meshpp++;
		numvertsj = JointMesh[5] & 0xFF;
		numvertstocalc = JointMesh[5] & 0xFF;

		if (!numvertstocalc)
			numvertstocalc = (ushort)JointMesh[5] >> 8;

		lp = 0;

		while (PointsToCalc[c][lp] != 255)
			lp++;

		if (numvertsj)
		{
			for (lp1 = 0; lp1 < lp; lp1++)
				RemapTable[PointsToCalc[c][lp1]] = (uchar)lp1;

			padval = lp;

			for (lp1 = 0; ScratchVertNums[SkinJoints[c][2]][lp1] != -1; lp1++)
			{
				RemapTable[ScratchVertNums[SkinJoints[c][2]][lp1]] = (uchar)padval;
				ScratchVertNums[SkinJoints[c][2]][lp1] = (char)padval;
				padval++;
			}

			for (lp1 = 0; ScratchVertNums[SkinJoints[c][3]][lp1] != -1; lp1++)
			{
				RemapTable[ScratchVertNums[SkinJoints[c][3]][lp1]] = (uchar)padval;
				ScratchVertNums[SkinJoints[c][3]][lp1] = (char)padval;
				padval++;
			}

			MeshJ = &JointMesh[6];
			MeshNormals = &JointMesh[3 * numvertstocalc + 7];

			for (lp1 = 0; lp1 < numvertsj; lp1++)
			{
				VertTable[4 * lp1] = MeshJ[0];
				VertTable[4 * lp1 + 1] = MeshJ[1];
				VertTable[4 * lp1 + 2] = MeshJ[2];
				NormalTable[4 * lp1] = MeshNormals[0];
				NormalTable[4 * lp1 + 1] = MeshNormals[1];
				NormalTable[4 * lp1 + 2] = MeshNormals[2];
				VertTable[4 * lp1 + 3] = 0;
				NormalTable[4 * lp1 + 3] = 0;
				MeshJ += 3;
				MeshNormals += 3;
			}

			for (lp1 = 0; lp1 < lp; lp1++)
			{
				Src = &VertTable[4 * PointsToCalc[c][lp1]];
				Dest = &JointMesh[3 * (RemapTable[PointsToCalc[c][lp1]] + 2)];
				Dest[0] = Src[0];
				Dest[1] = Src[1];
				Dest[2] = Src[2];
				Src = &NormalTable[4 * PointsToCalc[c][lp1]];
				Dest = &JointMesh[3 * (RemapTable[PointsToCalc[c][lp1]] + lp) + 7];
				Dest[0] = Src[0];
				Dest[1] = Src[1];
				Dest[2] = Src[2];
			}

			Dest = &JointMesh[6 * numvertstocalc + 7];
			numquads = *Dest;
			Dest++;

			for (lp1 = 0; lp1 < numquads; lp1++)
			{
				Dest[0] = RemapTable[Dest[0]];
				Dest[1] = RemapTable[Dest[1]];
				Dest[2] = RemapTable[Dest[2]];
				Dest[3] = RemapTable[Dest[3]];
				Dest += 6;
			}

			numtris = *Dest;
			Dest++;

			for (lp1 = 0; lp1 < numtris; lp1++)
			{
				Dest[0] = RemapTable[Dest[0]];
				Dest[1] = RemapTable[Dest[1]];
				Dest[2] = RemapTable[Dest[2]];
				Dest += 5;
			}

			JointMesh[5] = (short)lp;
			JointMesh[3 * (lp + 2)] = 0;
			Src = &JointMesh[6 * numvertstocalc + 7];
			Dest = &JointMesh[6 * lp + 7];
			numquads = 6 * *Src + 1;

			for (lp1 = 0; lp1 < numquads; lp1++)
			{
				*Dest = *Src;
				Src++;
				Dest++;
			}

			Src = &JointMesh[6 * numvertstocalc + numquads + 7];
			Dest = &JointMesh[6 * lp + numquads + 7];
			numtris = 5 * *Src + 1;

			for (lp1 = 0; lp1 < numtris; lp1++)
			{
				*Dest = *Src;
				Src++;
				Dest++;
			}
		}
	}
}
