#include "../tomb4/pch.h"
#include "registry.h"
#include "LoadSave.h"

#ifdef LEVEL_EDITOR
#define REGISTRY_PATH "Software\\Core Design\\Tomb Raider Level Editor"
#else
#define REGISTRY_PATH "Software\\Core Design\\Tomb Raider IV"
#endif

bool REG_OpenKey(LPCSTR lpSubKey)
{
	return RegCreateKeyEx(HKEY_CURRENT_USER, lpSubKey, 0, lpClass, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &phkResult, &dwDisposition) == ERROR_SUCCESS;
}

bool OpenRegistry(LPCSTR SubKeyName)
{
	char buffer[256];

	if (!SubKeyName)
		return REG_OpenKey(REGISTRY_PATH);

	sprintf(buffer, "%s\\%s", REGISTRY_PATH, SubKeyName);
	return REG_OpenKey(buffer);
}

void REG_CloseKey()
{
	RegCloseKey(phkResult);
}

void CloseRegistry()
{
	REG_CloseKey();
}

void REG_WriteLong(char* SubKeyName, ulong value)
{
	RegSetValueEx(phkResult, SubKeyName, 0, REG_DWORD, (CONST BYTE*) & value, sizeof(ulong));
}

void REG_WriteBool(char* SubKeyName, bool value)
{
	ulong Lvalue;

	Lvalue = (ulong)value;
	RegSetValueEx(phkResult, SubKeyName, 0, REG_DWORD, (CONST BYTE*) & Lvalue, sizeof(ulong));
}

bool REG_ReadLong(char* SubKeyName, ulong& value, ulong defaultValue)
{
	ulong type;
	ulong cbData;

	cbData = 4;

	if (RegQueryValueEx(phkResult, SubKeyName, 0, &type, (LPBYTE)&value, &cbData) == ERROR_SUCCESS && type == REG_DWORD && cbData == 4)
		return 1;

	REG_WriteLong(SubKeyName, defaultValue);
	value = defaultValue;
	return 0;
}

bool REG_ReadBool(char* SubKeyName, bool& value, bool defaultValue)
{
	ulong type;
	ulong cbData;
	ulong data;

	cbData = 4;

	if (RegQueryValueEx(phkResult, SubKeyName, 0, &type, (LPBYTE)&data, &cbData) == ERROR_SUCCESS && type == REG_DWORD && cbData == 4)
	{
		value = (bool)data;
		return 1;
	}

	REG_WriteBool(SubKeyName, defaultValue);
	value = defaultValue;
	return 0;
}

bool LoadSettings()
{
	ulong key;
	bool val;

	if (!OpenRegistry("System"))
		return 0;

	REG_ReadBool((char*)"Setup", REG_Setup, 0);

	if (REG_Setup)
	{
		App.TextureSize = 256;
		App.BumpMapSize = 256;
		App.StartFlags = 32;

		REG_ReadLong((char*)"DD", (ulong&)App.DXInfo.nDD, 0);
		REG_ReadLong((char*)"D3D", (ulong&)App.DXInfo.nD3D, 0);
		REG_ReadLong((char*)"VMode", (ulong&)App.DXInfo.nDisplayMode, 0);
		REG_ReadLong((char*)"TFormat", (ulong&)App.DXInfo.nTexture, 0);
		REG_ReadLong((char*)"DS", (ulong&)App.DXInfo.nDS, 0);
		REG_ReadBool((char*)"BumpMap", App.BumpMapping, 1);
		REG_ReadBool((char*)"Filter", App.Filtering, 1);
		REG_ReadBool((char*)"DisableSound", App.SoundDisabled, 0);
		REG_ReadBool((char*)"Volumetric", App.Volumetric, 1);
		REG_ReadBool((char*)"NoFMV", fmvs_disabled, 0);

		REG_ReadBool((char*)"TextLow", val, 0);

		if (val)
			App.TextureSize = 128;

		REG_ReadBool((char*)"BumpLow", val, 0);

		if (val)
			App.BumpMapSize = 128;

		REG_ReadBool((char*)"HardWare", val, 0);

		if (val)
			App.StartFlags |= 0x90;

		REG_ReadBool((char*)"Window", val, 0);

		if (val)
			App.StartFlags |= 2;
		else
			App.StartFlags |= 1;
	}

	CloseRegistry();

	OpenRegistry("Game");

	REG_ReadLong((char*)"Key0", key, layout[0][0]);
	layout[1][0] = (short)key;

	REG_ReadLong((char*)"Key1", key, layout[0][1]);
	layout[1][1] = (short)key;

	REG_ReadLong((char*)"Key2", key, layout[0][2]);
	layout[1][2] = (short)key;

	REG_ReadLong((char*)"Key3", key, layout[0][3]);
	layout[1][3] = (short)key;

	REG_ReadLong((char*)"Key4", key, layout[0][4]);
	layout[1][4] = (short)key;

	REG_ReadLong((char*)"Key5", key, layout[0][5]);
	layout[1][5] = (short)key;

	REG_ReadLong((char*)"Key6", key, layout[0][6]);
	layout[1][6] = (short)key;

	REG_ReadLong((char*)"Key7", key, layout[0][7]);
	layout[1][7] = (short)key;

	REG_ReadLong((char*)"Key8", key, layout[0][8]);
	layout[1][8] = (short)key;

	REG_ReadLong((char*)"Key9", key, layout[0][9]);
	layout[1][9] = (short)key;

	REG_ReadLong((char*)"Key10", key, layout[0][10]);
	layout[1][10] = (short)key;

	REG_ReadLong((char*)"Key11", key, layout[0][11]);
	layout[1][11] = (short)key;

	REG_ReadLong((char*)"Key12", key, layout[0][12]);
	layout[1][12] = (short)key;

	REG_ReadLong((char*)"Key13", key, layout[0][13]);
	layout[1][13] = (short)key;

	REG_ReadLong((char*)"Key14", key, layout[0][14]);
	layout[1][14] = (short)key;

	REG_ReadLong((char*)"Key15", key, layout[0][15]);
	layout[1][15] = (short)key;

	REG_ReadLong((char*)"Key16", key, layout[0][16]);
	layout[1][16] = (short)key;

	REG_ReadLong((char*)"Key17", key, layout[0][17]);
	layout[1][17] = (short)key;

	REG_ReadLong((char*)"MusicVolume", (ulong&)MusicVolume, 80);
	REG_ReadLong((char*)"SFXVolume", (ulong&)SFXVolume, 90);
	REG_ReadLong((char*)"ControlMethod", (ulong&)ControlMethod, 0);
	REG_ReadLong((char*)"SoundQuality", (ulong&)SoundQuality, 1);
	REG_ReadLong((char*)"AutoTarget", (ulong&)App.AutoTarget, 1);
	REG_ReadLong((char*)"WindowX", (ulong&)App.dx.rScreen, 0);
	REG_ReadLong((char*)"WindowY", (ulong&)App.dx.rScreen.top, 0);

	CloseRegistry();
	CheckKeyConflicts();
	return REG_Setup;
}

void SaveSettings()
{
	OpenRegistry("Game");
	REG_WriteLong((char*)"Key0", layout[1][0]);
	REG_WriteLong((char*)"Key1", layout[1][1]);
	REG_WriteLong((char*)"Key2", layout[1][2]);
	REG_WriteLong((char*)"Key3", layout[1][3]);
	REG_WriteLong((char*)"Key4", layout[1][4]);
	REG_WriteLong((char*)"Key5", layout[1][5]);
	REG_WriteLong((char*)"Key6", layout[1][6]);
	REG_WriteLong((char*)"Key7", layout[1][7]);
	REG_WriteLong((char*)"Key8", layout[1][8]);
	REG_WriteLong((char*)"Key9", layout[1][9]);
	REG_WriteLong((char*)"Key10", layout[1][10]);
	REG_WriteLong((char*)"Key11", layout[1][11]);
	REG_WriteLong((char*)"Key12", layout[1][12]);
	REG_WriteLong((char*)"Key13", layout[1][13]);
	REG_WriteLong((char*)"Key14", layout[1][14]);
	REG_WriteLong((char*)"Key15", layout[1][15]);
	REG_WriteLong((char*)"Key16", layout[1][16]);
	REG_WriteLong((char*)"Key17", layout[1][17]);
	REG_WriteLong((char*)"ControlMethod", ControlMethod);
	REG_WriteLong((char*)"MusicVolume", MusicVolume);
	REG_WriteLong((char*)"SFXVolume", SFXVolume);
	REG_WriteLong((char*)"SoundQuality", SoundQuality);
	REG_WriteLong((char*)"AutoTarget", App.AutoTarget);
	REG_WriteLong((char*)"WindowX", App.dx.rScreen.left);
	REG_WriteLong((char*)"WindowY", App.dx.rScreen.top);
	CloseRegistry();

	OpenRegistry("System");
	REG_WriteLong((char*)"VMode", App.DXInfo.nDisplayMode);
	REG_WriteBool((char*)"Window", (App.dx.Flags & 2) != 0);
	CloseRegistry();
}

bool SaveSetup(HWND hDlg)
{
	OpenRegistry("System");

	REG_WriteLong((char*)"DD", SendMessage(GetDlgItem(hDlg, 1000), CB_GETCURSEL, 0, 0));
	REG_WriteLong((char*)"D3D", SendMessage(GetDlgItem(hDlg, 1003), CB_GETCURSEL, 0, 0));
	REG_WriteLong((char*)"VMode", SendMessage(GetDlgItem(hDlg, 1004), CB_GETITEMDATA, SendMessage(GetDlgItem(hDlg, 1004), CB_GETCURSEL, 0, 0), 0));
	REG_WriteLong((char*)"DS", SendMessage(GetDlgItem(hDlg, 1005), CB_GETCURSEL, 0, 0));
	REG_WriteLong((char*)"TFormat", SendMessage(GetDlgItem(hDlg, 1006), CB_GETCURSEL, 0, 0));

	REG_WriteBool((char*)"Filter", SendMessage(GetDlgItem(hDlg, 1012), BM_GETCHECK, 0, 0));
	REG_WriteBool((char*)"BumpMap", SendMessage(GetDlgItem(hDlg, 1016), BM_GETCHECK, 0, 0));
	REG_WriteBool((char*)"HardWare", SendMessage(GetDlgItem(hDlg, 1010), BM_GETCHECK, 0, 0));
	REG_WriteBool((char*)"DisableSound", SendMessage(GetDlgItem(hDlg, 1018), BM_GETCHECK, 0, 0));
	REG_WriteBool((char*)"TextLow", SendMessage(GetDlgItem(hDlg, 1014), BM_GETCHECK, 0, 0));
	REG_WriteBool((char*)"BumpLow", SendMessage(GetDlgItem(hDlg, 1015), BM_GETCHECK, 0, 0));
	REG_WriteBool((char*)"Window", SendMessage(GetDlgItem(hDlg, 1025), BM_GETCHECK, 0, 0));
	REG_WriteBool((char*)"Volumetric", SendMessage(GetDlgItem(hDlg, 1029), BM_GETCHECK, 0, 0));
	REG_WriteBool((char*)"NoFMV", SendMessage(GetDlgItem(hDlg, 1030), BM_GETCHECK, 0, 0));
	REG_WriteBool((char*)"Setup", 1);

	CloseRegistry();
	CloseRegistry();
	return 1;
}

#ifdef GENERAL_FIXES	//I need this one
bool REG_KeyWasCreated()
{
	return dwDisposition == REG_CREATED_NEW_KEY;
}
#endif

void inject_registry(bool replace)
{
	INJECT(0x00484B00, REG_OpenKey, replace);
	INJECT(0x00484C70, OpenRegistry, replace);
	INJECT(0x00484B30, REG_CloseKey, replace);
	INJECT(0x00484CC0, CloseRegistry, replace);
	INJECT(0x00484B40, REG_WriteLong, replace);
	INJECT(0x00484B60, REG_WriteBool, replace);
	INJECT(0x00484B90, REG_ReadLong, replace);
	INJECT(0x00484BF0, REG_ReadBool, replace);
	INJECT(0x00484CD0, LoadSettings, replace);
	INJECT(0x004853F0, SaveSettings, replace);
	INJECT(0x00485190, SaveSetup, replace);
}
