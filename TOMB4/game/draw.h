#pragma once
#include "../global/vars.h"

void inject_draw(bool replace);

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
void InterpolateMatrix();
void InterpolateArmMatrix(long* mx);
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

#define calc_animating_item_clip_window	( (void(__cdecl*)(ITEM_INFO*, short*)) 0x0044FC00 )
