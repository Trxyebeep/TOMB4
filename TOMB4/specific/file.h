#pragma once
#include "../global/vars.h"

void inject_file(bool replace);

long S_LoadLevelFile(long num);
void FreeLevel();
bool FindCDDrive();
FILE* FileOpen(const char* name);
void FileClose(FILE* file);
long FileSize(FILE* file);
