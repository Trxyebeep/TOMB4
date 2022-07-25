#pragma once
#include "../global/vars.h"

void inject_tomb4fx(bool replace);

LIGHTNING_STRUCT* TriggerLightning(PHD_VECTOR* s, PHD_VECTOR* d, char variation, long rgb, uchar flags, uchar size, uchar segments);
long ExplodingDeath2(short item_number, long mesh_bits, short Flags);
void DrawGunshells();
void TriggerGunSmoke(long x, long y, long z, long xVel, long yVel, long zVel, long notLara, long weaponType, long shade);
void LaraBubbles(ITEM_INFO* item);
void UpdateDrips();
long GetFreeFireSpark();
void TriggerGlobalStaticFlame();
void TriggerGlobalFireFlame();
void keep_those_fires_burning();
void UpdateFireSparks();
void ClearFires();
void AddFire(long x, long y, long z, long size, short room_number, short fade);
void S_DrawFires();
long GetFreeSmokeSpark();
void UpdateSmokeSparks();
void TriggerShatterSmoke(long x, long y, long z);
void DrawLensFlares(ITEM_INFO* item);
void DrawWeaponMissile(ITEM_INFO* item);
long GetFreeGunshell();
void TriggerGunShell(short leftright, short objnum, long weapon);
void UpdateGunShells();
void TriggerSmallSplash(long x, long y, long z, long num);
void TriggerGunflash(SVECTOR* pos);
void SetGunFlash(short weapon);
void DrawGunflashes();
long GetFreeBlood();
void UpdateBlood();
void TriggerBlood(long x, long y, long z, long angle, long num);
long GetFreeBubble();
void CreateBubble(PHD_3DPOS* pos, short room_number, long size, long biggest);

#define UpdateFadeClip	( (void(__cdecl*)()) 0x00439D60 )
#define TriggerLaraDrips	( (void(__cdecl*)()) 0x0043A080 )
#define UpdateBubbles	( (void(__cdecl*)()) 0x00439970 )
#define UpdateShockwaves	( (void(__cdecl*)()) 0x0043AD10 )
#define UpdateLightning	( (void(__cdecl*)()) 0x0043AF80 )
#define TriggerLightningGlow	( (void(__cdecl*)(long, long, long, long)) 0x0043B330 )
#define	SetFadeClip	( (void(__cdecl*)(short, short)) 0x00439D40 )
#define	SetScreenFadeOut	( (void(__cdecl*)(long, long)) 0x00439DB0 )
#define	SetScreenFadeIn	( (void(__cdecl*)(long)) 0x00439E00 )
#define GetFreeDrip	( (long(__cdecl*)()) 0x00439F10 )
#define TriggerShockwave	( (void(__cdecl*)(PHD_VECTOR*, long, long, long, long)) 0x0043AA90 )
#define TriggerFlashSmoke	( (void(__cdecl*)(long, long, long, long)) 0x0043B420 )
#define Fade	( (void(__cdecl*)()) 0x00439E40 )
#define S_DrawSparks	( (void(__cdecl*)()) 0x0043A1B0 )
#define CalcLightningSpline	( (void(__cdecl*)(PHD_VECTOR*, SVECTOR*, LIGHTNING_STRUCT*)) 0x0043B0D0 )
