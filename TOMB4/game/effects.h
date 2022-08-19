#pragma once
#include "../global/types.h"

void SetFog(ITEM_INFO* item);
void finish_level_effect(ITEM_INFO* item);
void turn180_effect(ITEM_INFO* item);
void floor_shake_effect(ITEM_INFO* item);
void SoundFlipEffect(ITEM_INFO* item);
void RubbleFX(ITEM_INFO* item);
void PoseidonSFX(ITEM_INFO* item);
void ActivateCamera(ITEM_INFO* item);
void ActivateKey(ITEM_INFO* item);
void SwapCrowbar(ITEM_INFO* item);
void ExplosionFX(ITEM_INFO* item);
void LaraLocation(ITEM_INFO* item);
void LaraLocationPad(ITEM_INFO* item);
void GhostTrap(ITEM_INFO* item);
void KillActiveBaddies(ITEM_INFO* item);
void lara_hands_free(ITEM_INFO* item);
void draw_right_gun(ITEM_INFO* item);
void draw_left_gun(ITEM_INFO* item);
void shoot_right_gun(ITEM_INFO* item);
void shoot_left_gun(ITEM_INFO* item);
void swap_meshes_with_meshswap1(ITEM_INFO* item);
void swap_meshes_with_meshswap2(ITEM_INFO* item);
void swap_meshes_with_meshswap3(ITEM_INFO* item);
void invisibility_on(ITEM_INFO* item);
void invisibility_off(ITEM_INFO* item);
void reset_hair(ITEM_INFO* item);
void ClearScarabsPatch(ITEM_INFO* item);
void MeshSwapToPour(ITEM_INFO* item);
void MeshSwapFromPour(ITEM_INFO* item);
void void_effect(ITEM_INFO* item);
void WaterFall(short item_number);
void WadeSplash(ITEM_INFO* item, long water, long depth);
void Splash(ITEM_INFO* item);
short DoBloodSplat(long x, long y, long z, short speed, short ang, short room_number);
void DoLotsOfBlood(long x, long y, long z, short speed, short ang, short room_number, long num);
void Richochet(GAME_VECTOR* pos);
void SoundEffects();
long ItemNearLara(PHD_3DPOS* pos, long rad);

extern FX_INFO* effects;
extern OBJECT_VECTOR* sound_effects;
extern long GlobalFogOff;
extern long number_sound_effects;
extern long FogTableColor[28];
extern void(*effect_routines[])(ITEM_INFO* item);
