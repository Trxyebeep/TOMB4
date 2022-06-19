#include "../tomb4/pch.h"
#include "dxsound.h"
#include "dxshell.h"
#include "function_stubs.h"

bool DXChangeOutputFormat(long nSamplesPerSec, bool force)
{
	WAVEFORMATEX pcfxFormat;
	static long lastSPC;

	if (!force && lastSPC == nSamplesPerSec)
		return 1;

	lastSPC = nSamplesPerSec;
	pcfxFormat.wFormatTag = 1;
	pcfxFormat.nChannels = 2;
	pcfxFormat.nSamplesPerSec = nSamplesPerSec;
	pcfxFormat.nAvgBytesPerSec = 4 * nSamplesPerSec;
	pcfxFormat.nBlockAlign = 4;
	pcfxFormat.wBitsPerSample = 16;
	pcfxFormat.cbSize = 0;
	S_SoundStopAllSamples();

	if (G_DSPrimary && DXAttempt(G_DSPrimary->SetFormat(&pcfxFormat)) != DS_OK)
	{
		Log(1, "Can't set sound output format to %d", pcfxFormat.nSamplesPerSec);
		return 0;
	}

	return 1;
}

void inject_dxsound(bool replace)
{
	INJECT(0x004732E0, DXChangeOutputFormat, replace);
}
