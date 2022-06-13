#pragma once
#include "../global/vars.h"

void inject_file(bool replace);

long S_LoadLevelFile(long num);
void FreeLevel();
bool FindCDDrive();
FILE* FileOpen(const char* name);
void FileClose(FILE* file);
long FileSize(FILE* file);
long LoadFile(const char* name, char** dest);
bool LoadTextures(long RTPages, long OTPages, long BTPages);
bool LoadRooms();
bool LoadObjects();
bool LoadSprites();
bool LoadCameras();
bool LoadSoundEffects();
bool LoadBoxes();
bool LoadAnimatedTextures();
bool LoadTextureInfos();
bool LoadItems();
bool LoadCinematic();
bool LoadAIInfo();
bool LoadSamples();
void S_GetUVRotateTextures();

#define Decompress	( (void(__cdecl*)(char*, char*, long, long)) 0x00473E80 )
#define AdjustUV	( (void(__cdecl*)(long)) 0x004752A0 )
