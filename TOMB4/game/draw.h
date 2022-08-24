#pragma once
#include "../global/types.h"

void InitInterpolate(long frac, long rate);
void phd_PopMatrix_I();
void phd_PushMatrix_I();
void phd_RotY_I(short ang);
void phd_RotX_I(short ang);
void phd_RotZ_I(short ang);
void phd_TranslateRel_I(long x, long y, long z);
void phd_TranslateRel_ID(long x, long y, long z, long x2, long y2, long z2);
void phd_RotYXZ_I(short y, short x, short z);
void gar_RotYXZsuperpack_I(short** pprot1, short** pprot2, long skip);
void gar_RotYXZsuperpack(short** pprot, long skip);
void phd_PutPolygons_I(short* ptr, long clip);
void mInterpolateMatrix();
void mInterpolateArmMatrix(float* mx);
void S_InsertRoom(short room_number);
void CalculateObjectLighting(ITEM_INFO* item, short* frame);
void CalculateObjectLightingLara();
void DrawAnimatingItem(ITEM_INFO* item);
void DrawRooms(short CurrentRoom);
void RenderIt(short CurrentRoom);
long DrawPhaseGame();
void GetRoomBounds();
void SetRoomBounds(short* door, long rn, ROOM_INFO* actualRoom);
void DrawEffect(short fx_num);
void PrintObjects(short room_number);
long GetFrames(ITEM_INFO* item, short* frm[], long* rate);
short* GetBoundsAccurate(ITEM_INFO* item);
short* GetBestFrame(ITEM_INFO* item);
void UpdateSkyLightning();
void mRotBoundingBoxNoPersp(short* bounds, short* rotatedBounds);
void calc_animating_item_clip_window(ITEM_INFO* item, short* bounds);

extern STATIC_INFO static_objects[];

extern long IM_rate;
extern long IM_frac;

extern float* mIMptr;
extern float mIMstack[indices_count * 64];

extern long current_room;
extern short no_rotation[12];

extern long outside;

extern short SkyPos;
extern short SkyPos2;

extern ushort LightningRGB[3];
extern ushort LightningRGBs[3];
extern short LightningCount;
extern short dLightningRand;
