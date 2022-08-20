#pragma once
#include "../global/types.h"

void CLSetup(char* cmd);
void CLNoFMV(char* cmd);
void InitDSDevice(HWND dlg, HWND hwnd);
void InitTFormats(HWND dlg, HWND hwnd);
void InitResolution(HWND dlg, HWND hwnd, bool resetvms);
void InitD3DDevice(HWND dlg, HWND hwnd);
void InitDDDevice(HWND dlg, HWND hwnd);
char* MapASCIIToANSI(char* s, char* d);
BOOL CALLBACK DXSetupDlgProc(HWND dlg, UINT message, WPARAM wParam, LPARAM lParam);
bool DXSetupDialog();

extern bool start_setup;
extern bool fmvs_disabled;
