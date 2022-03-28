#pragma once
#include "../global/vars.h"

void inject_registry(bool replace);

bool REG_OpenKey(LPCSTR lpSubKey);
bool OpenRegistry(LPCSTR SubKeyName);
void REG_CloseKey();
void CloseRegistry();
void REG_WriteLong(char* SubKeyName, ulong value);
void REG_WriteBool(char* SubKeyName, bool value);
bool REG_ReadLong(char* SubKeyName, ulong& value, ulong defaultValue);
bool REG_ReadBool(char* SubKeyName, bool& value, bool defaultValue);
bool LoadSettings();
void SaveSettings();
#ifdef GENERAL_FIXES
bool REG_KeyWasCreated();
#endif
