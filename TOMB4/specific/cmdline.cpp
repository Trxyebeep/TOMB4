#include "../tomb4/pch.h"
#include "cmdline.h"
#include "function_stubs.h"
#include "../game/gameflow.h"
#include "registry.h"
#include "winmain.h"

char ASCIIToANSITable[7][2] =
{
	{'‚', 'é'},
	{'Š', 'è'},
	{'ˆ', 'ê'},
	{'”', 'ö'},
	{'…', 'à'},
	{' ', 'á'},
	{'¢', 'ó'}
};

bool start_setup = 0;
bool fmvs_disabled = 0;

static long nDDDevice = 0;
static long nD3DDevice = 0;
static bool Filter = 1;
static bool VolumetricFx = 0;
static bool BumpMap = 0;
static bool TextLow = 0;

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

	SendMessage(GetDlgItem(dlg, 1010), BM_SETCHECK, 1, 0);
	SendMessage(GetDlgItem(dlg, 1011), BM_SETCHECK, 0, 0);
	EnableWindow(GetDlgItem(dlg, 1011), 0);
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

				if (w == 640 && h == 480)
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
		VolumetricFx = 0;
	}
	else
		EnableWindow(GetDlgItem(dlg, 1029), 1);

	SendMessage(GetDlgItem(dlg, 1029), BM_SETCHECK, VolumetricFx, 0);

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
	SendMessage(hwnd, CB_SETCURSEL, nDDDevice, 0);
	InitD3DDevice(dlg, GetDlgItem(dlg, 1003));
}

char* MapASCIIToANSI(char* s, char* d)
{
	char* p;
	long l;
	char c;
	bool found;

	l = strlen(s);
	p = d;

	for (int i = 0; i < l; i++)
	{
		c = *s++;

		if (c >= 0x80)
		{
			found = 0;

			for (int i = 0; i < 7; i++)
			{
				if (c == ASCIIToANSITable[i][0])
				{
					c = ASCIIToANSITable[i][1];
					found = 1;
					break;
				}
			}

			if (!found)
				Log(1, "Reqd : %x", c);
		}

		*d++ = c;
	}

	*d = 0;
	return p;
}

BOOL CALLBACK DXSetupDlgProc(HWND dlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HFONT hfont = 0;
	char d[256];

	if (message == WM_INITDIALOG)
	{
		Log(2, "WM_INITDIALOG");

		if (Gameflow->Language == JAPAN)
		{
			hfont = (HFONT)GetStockObject(SYSTEM_FONT);
			SendMessage(GetDlgItem(dlg, 1000), WM_SETFONT, 0, (LPARAM)hfont);
			SendMessage(GetDlgItem(dlg, 1003), WM_SETFONT, 0, (LPARAM)hfont);
			SendMessage(GetDlgItem(dlg, 1004), WM_SETFONT, 0, (LPARAM)hfont);
			SendMessage(GetDlgItem(dlg, 1006), WM_SETFONT, 0, (LPARAM)hfont);
			SendMessage(GetDlgItem(dlg, 1005), WM_SETFONT, 0, (LPARAM)hfont);
		}

		SendMessage(GetDlgItem(dlg, 1001), WM_SETTEXT, 0, (LPARAM)MapASCIIToANSI(SCRIPT_TEXT(TXT_Graphics_Adapter), d));
		SendMessage(GetDlgItem(dlg, 1002), WM_SETTEXT, 0, (LPARAM)MapASCIIToANSI(SCRIPT_TEXT(TXT_Output_Settings), d));
		SendMessage(GetDlgItem(dlg, 1), WM_SETTEXT, 0, (LPARAM)MapASCIIToANSI(SCRIPT_TEXT(TXT_OK), d));
		SendMessage(GetDlgItem(dlg, 2), WM_SETTEXT, 0, (LPARAM)MapASCIIToANSI(SCRIPT_TEXT(TXT_Cancel), d));
		SendMessage(GetDlgItem(dlg, 1009), WM_SETTEXT, 0, (LPARAM)MapASCIIToANSI(SCRIPT_TEXT(TXT_Output_Resolution), d));
		SendMessage(GetDlgItem(dlg, 1012), WM_SETTEXT, 0, (LPARAM)MapASCIIToANSI(SCRIPT_TEXT(TXT_Bilinear_Filtering), d));
		SendMessage(GetDlgItem(dlg, 1016), WM_SETTEXT, 0, (LPARAM)MapASCIIToANSI(SCRIPT_TEXT(TXT_Bump_Mapping), d));
		SendMessage(GetDlgItem(dlg, 1010), WM_SETTEXT, 0, (LPARAM)MapASCIIToANSI(SCRIPT_TEXT(TXT_Hardware_Acceleration), d));
		SendMessage(GetDlgItem(dlg, 1011), WM_SETTEXT, 0, (LPARAM)MapASCIIToANSI(SCRIPT_TEXT(TXT_Software_Mode), d));
		SendMessage(GetDlgItem(dlg, 1017), WM_SETTEXT, 0, (LPARAM)MapASCIIToANSI(SCRIPT_TEXT(TXT_Sound_Device), d));
		SendMessage(GetDlgItem(dlg, 1018), WM_SETTEXT, 0, (LPARAM)MapASCIIToANSI(SCRIPT_TEXT(TXT_Disable), d));
		SendMessage(GetDlgItem(dlg, 1014), WM_SETTEXT, 0, (LPARAM)MapASCIIToANSI(SCRIPT_TEXT(TXT_Low_Resolution_Textures), d));
		SendMessage(GetDlgItem(dlg, 1015), WM_SETTEXT, 0, (LPARAM)MapASCIIToANSI(SCRIPT_TEXT(TXT_Low_Resolution_Bump_Maps), d));
		SendMessage(GetDlgItem(dlg, 1013), WM_SETTEXT, 0, (LPARAM)MapASCIIToANSI(SCRIPT_TEXT(TXT_Texture_Bit_Depth), d));
		SendMessage(GetDlgItem(dlg, 1025), WM_SETTEXT, 0, (LPARAM)MapASCIIToANSI(SCRIPT_TEXT(TXT_Windowed), d));
		SendMessage(GetDlgItem(dlg, 1023), WM_SETTEXT, 0, (LPARAM)MapASCIIToANSI(SCRIPT_TEXT(TXT_Render_Options), d));
		SendMessage(GetDlgItem(dlg, 1029), WM_SETTEXT, 0, (LPARAM)MapASCIIToANSI(SCRIPT_TEXT(TXT_Volumetric_FX), d));
		SendMessage(GetDlgItem(dlg, 1030), WM_SETTEXT, 0, (LPARAM)MapASCIIToANSI(SCRIPT_TEXT(TXT_No_FMV), d));
		InitDDDevice(dlg, GetDlgItem(dlg, 1000));
		InitDSDevice(dlg, GetDlgItem(dlg, 1005));
		return 1;
	}

	if (message == WM_COMMAND)
	{
		switch ((ushort)wParam)
		{
		case IDOK:

			if (hfont)
				DeleteObject(hfont);

			SaveSetup(dlg);
			EndDialog(dlg, 1);
			return 1;

		case IDCANCEL:

			if (hfont)
				DeleteObject(hfont);

			EndDialog(dlg, 0);
			return 1;

		case 1000:

			if (((wParam >> 16) & 0xFFFF) == CBN_SELCHANGE)
			{
				nDDDevice = SendMessage(GetDlgItem(dlg, 1000), CB_GETCURSEL, 0, 0);
				InitD3DDevice(dlg, GetDlgItem(dlg, 1003));
			}

			break;

		case 1003:

			if (((wParam >> 16) & 0xFFFF) == CBN_SELCHANGE)
			{
				nD3DDevice = SendMessage(GetDlgItem(dlg, 1003), CB_GETCURSEL, 0, 0);
				InitResolution(dlg, GetDlgItem(dlg, 1004), 1);
			}

			break;

		case 1010:

			if (((wParam >> 16) & 0xFFFF) == BN_CLICKED)
			{
				nD3DDevice = 1;
				SendMessage(GetDlgItem(dlg, 1003), CB_SETCURSEL, 1, 0);
				InitResolution(dlg, GetDlgItem(dlg, 1004), 1);
			}

			break;

		case 1011:

			if (((wParam >> 16) & 0xFFFF) == BN_CLICKED)
			{
				nD3DDevice = 0;
				SendMessage(GetDlgItem(dlg, 1003), CB_SETCURSEL, 0, 0);
				InitResolution(dlg, GetDlgItem(dlg, 1004), 1);
			}

			break;

		case 1012:

			if (((wParam >> 16) & 0xFFFF) == BN_CLICKED)
			{
				if (SendMessage(GetDlgItem(dlg, 1012), BM_GETCHECK, 0, 0))
					Filter = 1;
				else
					Filter = 0;

				InitResolution(dlg, GetDlgItem(dlg, 1004), 0);
			}

			break;

		case 1014:

			if (((wParam >> 16) & 0xFFFF) == BN_CLICKED)
			{
				if (SendMessage(GetDlgItem(dlg, 1014), BM_GETCHECK, 0, 0))
					TextLow = 1;
				else
					TextLow = 0;

				InitResolution(dlg, GetDlgItem(dlg, 1004), 0);
			}

			break;

		case 1016:

			if (((wParam >> 16) & 0xFFFF) == BN_CLICKED)
			{
				if (SendMessage(GetDlgItem(dlg, 1016), BM_GETCHECK, 0, 0))
					BumpMap = 1;
				else
					BumpMap = 0;

				InitResolution(dlg, GetDlgItem(dlg, 1004), 0);
			}

			break;

		case 1018:

			if (((wParam >> 16) & 0xFFFF) == BN_CLICKED)
			{
				if (SendMessage(GetDlgItem(dlg, (ushort)wParam), BM_GETCHECK, 0, 0))
					EnableWindow(GetDlgItem(dlg, 1005), 0);
				else
					EnableWindow(GetDlgItem(dlg, 1005), 1);
			}

			break;

		case 1029:

			if (((wParam >> 16) & 0xFFFF) == BN_CLICKED)
			{
				if (SendMessage(GetDlgItem(dlg, 1029), BM_GETCHECK, 0, 0))
					VolumetricFx = 1;
				else
					VolumetricFx = 0;

				InitResolution(dlg, GetDlgItem(dlg, 1004), 0);
			}

			break;
		}
	}

	return 0;
}

bool DXSetupDialog()
{
	long ret;

	ShowCursor(1);
	ret = DialogBox(App.hInstance, MAKEINTRESOURCE(109), 0, DXSetupDlgProc);
	ShowCursor(0);

	if (ret == -1)
	{
		MessageBox(0, "Unable To Initialise Dialog", "", 0);
		return 0;
	}

	if (!ret)
		return 0;

	return 1;
}
