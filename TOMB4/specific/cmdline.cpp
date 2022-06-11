#include "../tomb4/pch.h"
#include "cmdline.h"
#include "function_stubs.h"
#include "../game/gameflow.h"

void CLSetup(char* cmd)
{
	Log(2, "CLSetup");

	if (cmd)
		start_setup = 0;
	else
		start_setup = 1;
}

void CLNoFMV(char* cmd)
{
	Log(2, "CLNoFMV");

	if (cmd)
		fmvs_disabled = 0;
	else
		fmvs_disabled = 1;
}

void InitDSDevice(HWND dlg, HWND hwnd)
{
	SendMessage(hwnd, CB_RESETCONTENT, 0, 0);

	for (int i = 0; i < App.DXInfo.nDSInfo; i++)
		SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)App.DXInfo.DSInfo[i].About);

	if (!App.DXInfo.nDSInfo)
	{
		SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)SCRIPT_TEXT(TXT_No_Sound_Card_Installed));
		EnableWindow(GetDlgItem(dlg, 1018), 0);
		SendMessage(GetDlgItem(dlg, 1018), BM_SETCHECK, 1, 0);
		EnableWindow(hwnd, 0);
	}

	SendMessage(hwnd, CB_SETCURSEL, 0, 0);
}

void InitTFormats(HWND dlg, HWND hwnd)
{
	DXD3DDEVICE* device;
	DXTEXTUREINFO* tex;
	long bpp, r, g, b, a;
	char buffer[40];
	bool software;

	SendMessage(hwnd, CB_RESETCONTENT, 0, 0);
	EnableWindow(GetDlgItem(dlg, 1006), 1);
	software = SendMessage(GetDlgItem(dlg, 1011), BM_GETCHECK, 0, 0);
	device = &App.DXInfo.DDInfo[nDDDevice].D3DDevices[nD3DDevice];

	for (int i = 0; i < device->nTextureInfos; i++)
	{
		tex = &device->TextureInfos[i];
		bpp = tex->bpp;
		r = tex->rbpp;
		g = tex->gbpp;
		b = tex->bbpp;
		a = tex->abpp;

		wsprintf(buffer, "%d %s RGBA %d%d%d%d", bpp, SCRIPT_TEXT(TXT_Bit), r, g, b, a);
		SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)buffer);

		if (software)
		{
			if (bpp == 32 && r == 8 && b == 8 && g == 8 && a == 8)
			{
				SendMessage(hwnd, CB_SETCURSEL, i, 0);
				EnableWindow(GetDlgItem(dlg, 1006), 0);
			}
		}
		else if (bpp == 16 && r == 5 && b == 5 && g == 5 && a == 1)
			SendMessage(hwnd, CB_SETCURSEL, i, 0);
		else
			SendMessage(hwnd, CB_SETCURSEL, 0, 0);
	}
}

void InitResolution(HWND dlg, HWND hwnd, bool resetvms)
{
	DXD3DDEVICE* device;
	DXDISPLAYMODE* dm;
	long bpp, w, h, n;
	char buffer[40];
	bool software;

	n = 0;
	if (nD3DDevice)
	{
		SendMessage(GetDlgItem(dlg, 1010), BM_SETCHECK, 1, 0);
		SendMessage(GetDlgItem(dlg, 1011), BM_SETCHECK, 0, 0);
	}
	else
	{
		SendMessage(GetDlgItem(dlg, 1010), BM_SETCHECK, 0, 0);
		SendMessage(GetDlgItem(dlg, 1011), BM_SETCHECK, 1, 0);
	}

	software = SendMessage(GetDlgItem(dlg, 1011), BM_GETCHECK, 0, 0);

	if (resetvms)
	{
		SendMessage(hwnd, CB_RESETCONTENT, 0, 0);
		device = &App.DXInfo.DDInfo[nDDDevice].D3DDevices[nD3DDevice];

		for (int i = 0; i < device->nDisplayModes; i++)
		{
			dm = &device->DisplayModes[i];
			w = dm->w;
			h = dm->h;
			bpp = dm->bpp;

			if (bpp > 8)
			{
				wsprintf(buffer, "%dx%d %d %s", w, h, bpp, SCRIPT_TEXT(TXT_Bit));
				SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)buffer);
				SendMessage(hwnd, CB_SETITEMDATA, n, i);

				if (software)
				{
					if (w == 320 && h == 240 && bpp == 16)
						SendMessage(hwnd, CB_SETCURSEL, n, 0);
				}
				else if (w == 640 && h == 480 && bpp == 16)
					SendMessage(hwnd, CB_SETCURSEL, n, 0);

				n++;
			}
		}
	}

	if (App.DXInfo.DDInfo[nDDDevice].DDCaps.dwCaps2 & DDCAPS2_CANRENDERWINDOWED)
		EnableWindow(GetDlgItem(dlg, 1025), 1);
	else
	{
		EnableWindow(GetDlgItem(dlg, 1025), 0);
		SendMessage(GetDlgItem(dlg, 1025), BM_SETCHECK, 0, 0);
	}

	SendMessage(GetDlgItem(dlg, 1012), BM_SETCHECK, Filter, 0);

	if (software)
	{
		EnableWindow(GetDlgItem(dlg, 1029), 0);
		volumetric_fx = 0;
	}
	else
		EnableWindow(GetDlgItem(dlg, 1029), 1);

	SendMessage(GetDlgItem(dlg, 1029), BM_SETCHECK, volumetric_fx, 0);

	if (software)
	{
		EnableWindow(GetDlgItem(dlg, 1016), 0);
		BumpMap = 0;
	}
	else
		EnableWindow(GetDlgItem(dlg, 1016), 1);

	SendMessage(GetDlgItem(dlg, 1016), BM_SETCHECK, BumpMap, 0);

	if (software)
	{
		EnableWindow(GetDlgItem(dlg, 1014), 0);
		TextLow = 0;
	}
	else
		EnableWindow(GetDlgItem(dlg, 1014), 1);

	SendMessage(GetDlgItem(dlg, 1014), BM_SETCHECK, TextLow, 0);

	if (TextLow)
	{
		SendMessage(GetDlgItem(dlg, 1015), BM_SETCHECK, 1, 0);
		EnableWindow(GetDlgItem(dlg, 1015), 0);
	}
	else
	{
		EnableWindow(GetDlgItem(dlg, 1015), 1);
		SendMessage(GetDlgItem(dlg, 1015), BM_SETCHECK, 0, 0);
	}

	if (!BumpMap)
	{
		SendMessage(GetDlgItem(dlg, 1015), BM_SETCHECK, 0, 0);
		EnableWindow(GetDlgItem(dlg, 1015), 0);
	}

	if (resetvms)
		InitTFormats(dlg, GetDlgItem(dlg, 1006));
}

void InitD3DDevice(HWND dlg, HWND hwnd)
{
	DXDIRECTDRAWINFO* ddraw;

	SendMessage(hwnd, CB_RESETCONTENT, 0, 0);
	ddraw = &App.DXInfo.DDInfo[nDDDevice];

	for (int i = 0; i < ddraw->nD3DDevices; i++)
		SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)ddraw->D3DDevices[i].About);

	SendMessage(hwnd, CB_SETCURSEL, 1, 0);
	nD3DDevice = 1;
	InitResolution(dlg, GetDlgItem(dlg, 1004), 1);
}

void InitDDDevice(HWND dlg, HWND hwnd)
{
	DDDEVICEIDENTIFIER* id;
	char buffer[256];

	SendMessage(hwnd, CB_RESETCONTENT, 0, 0);

	for (int i = 0; i < App.DXInfo.nDDInfo; i++)
	{
		id = &App.DXInfo.DDInfo[i].DDIdentifier;
		wsprintf(buffer, "%s - %s (%d.%d.%02d.%04d)", id->szDescription, id->szDriver,
			(id->liDriverVersion.HighPart >> 16) & 0xFFFF, id->liDriverVersion.HighPart & 0xFFFF,
			(id->liDriverVersion.LowPart >> 16) & 0xFFFF, id->liDriverVersion.LowPart & 0xFFFF);
		SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)buffer);
	}

	nDDDevice = App.DXInfo.nDDInfo - 1;
	SendMessage(hwnd, 0x14E, nDDDevice, 0);
	InitD3DDevice(dlg, GetDlgItem(dlg, 1003));
}

void inject_cmdline(bool replace)
{
	INJECT(0x0046FE40, CLSetup, replace);
	INJECT(0x0046FE60, CLNoFMV, replace);
	INJECT(0x0046FED0, InitDSDevice, replace);
	INJECT(0x0046FFA0, InitTFormats, replace);
	INJECT(0x004701C0, InitResolution, replace);
	INJECT(0x004705F0, InitD3DDevice, replace);
	INJECT(0x004706B0, InitDDDevice, replace);
}
