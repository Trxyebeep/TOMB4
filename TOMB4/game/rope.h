#pragma once
#include "../global/types.h"

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
void SetPendulumPoint(ROPE_STRUCT* Rope, long node);
void SetPendulumVelocity(long x, long y, long z);
void CalculateRope(ROPE_STRUCT* Rope);
long RopeNodeCollision(ROPE_STRUCT* rope, long x, long y, long z, long rad);
void RopeControl(short item_num);
void RopeCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);

extern ROPE_STRUCT RopeList[5];
extern PENDULUM CurrentPendulum;
extern long nRope;
