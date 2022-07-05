#pragma once
#include "../global/vars.h"

void inject_rope(bool replace);

void DrawRopeList();
void ProjectRopePoints(ROPE_STRUCT* Rope);
PHD_VECTOR* Normalise(PHD_VECTOR* v);
void vMul(PHD_VECTOR* v, long scale, PHD_VECTOR* d);
long mDotProduct(PHD_VECTOR* a, PHD_VECTOR* b);
void mCrossProduct(PHD_VECTOR* a, PHD_VECTOR* b, PHD_VECTOR* n);
void phd_GetMatrixAngles(long* m, short* dest);
void GetRopePos(ROPE_STRUCT* rope, long pos, long* x, long* y, long* z);
void AlignLaraToRope(ITEM_INFO* l);
void ModelRigid(PHD_VECTOR* pa, PHD_VECTOR* pb, PHD_VECTOR* va, PHD_VECTOR* vb, long rlength);
void ModelRigidRope(PHD_VECTOR* pa, PHD_VECTOR* pb, PHD_VECTOR* va, PHD_VECTOR* vb, long rlength);
