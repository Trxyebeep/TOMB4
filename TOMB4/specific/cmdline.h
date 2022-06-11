#pragma once
#include "../global/vars.h"

void inject_cmdline(bool replace);

void CLSetup(char* cmd);
void CLNoFMV(char* cmd);
void InitDSDevice(HWND dlg, HWND hwnd);
void InitTFormats(HWND dlg, HWND hwnd);
