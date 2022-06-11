#include "../tomb4/pch.h"
#include "cmdline.h"
#include "function_stubs.h"

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

void inject_cmdline(bool replace)
{
	INJECT(0x0046FE40, CLSetup, replace);
	INJECT(0x0046FE60, CLNoFMV, replace);
}
