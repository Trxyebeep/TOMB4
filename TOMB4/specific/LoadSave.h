#pragma once
#include "../global/types.h"

void DoOptions();
void DisplayStatsUCunt();
long S_DisplayPauseMenu(long reset);
long DoLoadSave(long LoadSave);
long S_LoadSave(long load_or_save, long mono, long inv_active);
void S_DisplayMonoScreen();
void CreateMonoScreen();
void FreeMonoScreen();
void RGBM_Mono(uchar* r, uchar* g, uchar* b);
void ConvertSurfaceToTextures(LPDIRECTDRAWSURFACEX surface);
void CheckKeyConflicts();
long S_PauseMenu();
long GetSaveLoadFiles();

extern long sfx_frequencies[3];
extern long SoundQuality;
extern long MusicVolume;
extern long SFXVolume;
extern long ControlMethod;
extern char MonoScreenOn;
