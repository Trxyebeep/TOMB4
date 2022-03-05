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
long S_LoadSave(long load_or_save, long mono);
#ifdef IMPROVED_BARS
void S_DrawEnemyBar(long pos);
#endif
void DoStatScreen();
void S_DrawTile(long x, long y, long w, long h, IDirect3DTexture2* t, long tU, long tV, long tW, long tH, long c0, long c1, long c2, long c3);
void S_DisplayMonoScreen();
void CreateMonoScreen();
void FreeMonoScreen();

#define S_PauseMenu	( (long(__cdecl*)()) 0x0047CC60 )
#define DoSlider	( (void(__cdecl*)(long, long, long, long, long, long, long, long)) 0x0047AB80 )
#define CheckKeyConflicts	( (void(__cdecl*)()) 0x0047B130 )
#define GetSaveLoadFiles	( (void(__cdecl*)()) 0x0047A6F0 )
#define DoLoadSave	( (long(__cdecl*)(long)) 0x0047A880 )
#define ConvertSurfaceToTextures	( (void(__cdecl*)(LPDIRECTDRAWSURFACE4)) 0x00479C10 )
