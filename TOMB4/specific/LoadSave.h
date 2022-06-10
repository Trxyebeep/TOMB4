#pragma once
#include "../global/vars.h"

void inject_loadsave(bool replace);

void S_DrawHealthBar(long pos);
void S_DrawAirBar(long pos);
void S_DrawDashBar(long pos);
void S_InitLoadBar(long maxpos);
void S_LoadBar();
void DoBar(long x, long y, long width, long height, long pos, long clr1, long clr2);
void DoOptions();
void DoStatScreen();
long S_DisplayPauseMenu(long reset);
#ifdef GENERAL_FIXES
long S_LoadSave(long load_or_save, long mono, long inv_active);
#else
long S_LoadSave(long load_or_save, long mono);
#endif
#ifdef IMPROVED_BARS
void S_DrawEnemyBar(long pos);
#endif
void S_DrawTile(long x, long y, long w, long h, IDirect3DTexture2* t, long tU, long tV, long tW, long tH, long c0, long c1, long c2, long c3);
void S_DisplayMonoScreen();
void CreateMonoScreen();
void FreeMonoScreen();
void RGBM_Mono(uchar* r, uchar* g, uchar* b);
void MemBltSurf(void* dest, long x, long y, long w, long h, long dadd, void* source, long x2, long y2, DDSURFACEDESC2 surface, float xsize, float ysize);
void ConvertSurfaceToTextures(LPDIRECTDRAWSURFACE4 surface);
void DoSlider(long x, long y, long width, long height, long pos, long clr1, long clr2, long clr3);
void CheckKeyConflicts();

#define S_PauseMenu	( (long(__cdecl*)()) 0x0047CC60 )
#define GetSaveLoadFiles	( (void(__cdecl*)()) 0x0047A6F0 )
#define DoLoadSave	( (long(__cdecl*)(long)) 0x0047A880 )
