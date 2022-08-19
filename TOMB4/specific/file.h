#pragma once
#include "../global/types.h"

unsigned int __stdcall LoadLevel(void* name);
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
void AdjustUV(long num);
bool Decompress(char* pDest, char* pCompressed, long compressedSize, long size);

extern TEXTURESTRUCT* textinfo;
extern SPRITESTRUCT* spriteinfo;
extern THREAD LevelLoadingThread;

extern TEXTURESTRUCT* AnimatingWaterfalls[3];
extern long AnimatingWaterfallsV[3];

extern CHANGE_STRUCT* changes;
extern RANGE_STRUCT* ranges;
extern AIOBJECT* AIObjects;
extern short* aranges;
extern short* frames;
extern short* commands;
extern short* floor_data;
extern short* mesh_base;
extern long nAnimUVRanges;
extern long number_cameras;
extern short nAIObjects;
