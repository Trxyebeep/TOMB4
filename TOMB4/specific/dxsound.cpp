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

void DSChangeVolume(long num, long volume)
{
	if (DS_Samples[num].buffer)
		DS_Samples[num].buffer->SetVolume(volume);
}

void DSAdjustPitch(long num, long pitch)
{
	ulong frequency;

	if (DS_Samples[num].buffer)
	{
		frequency = ulong((float)pitch / 65536.0F * 22050.0F);

		if (frequency < 100)
			frequency = 100;
		else if (frequency > 100000)
			frequency = 100000;

		DS_Samples[num].buffer->SetFrequency(frequency);
	}
}

void DSAdjustPan(long num, long pan)
{
	if (DS_Samples[num].buffer)
	{
		if (pan < 0)
		{
			if (pan < -0x4000)
				pan = -0x4000 - pan;
		}
		else if (pan > 0 && pan > 0x4000)
			pan = 0x8000 - pan;

		pan >>= 4;
		DS_Samples[num].buffer->SetPan(pan);
	}
}

bool DXSetOutputFormat()
{
	DSBUFFERDESC desc;

	Log(2, "DXSetOutputFormat");
	memset(&desc, 0, sizeof(desc));
	desc.dwSize = sizeof(desc);
	desc.dwFlags = DSBCAPS_PRIMARYBUFFER;

	if (DXAttempt(App.dx.lpDS->CreateSoundBuffer(&desc, &G_DSPrimary, 0)) == DS_OK)
	{
		DXChangeOutputFormat(sfx_frequencies[SoundQuality], 0);
		G_DSPrimary->Play(0, 0, DSBPLAY_LOOPING);
		return 1;
	}

	Log(1, "Can't Get Primary Sound Buffer");
	return 0;
}

void inject_dxsound(bool replace)
{
	INJECT(0x004732E0, DXChangeOutputFormat, replace);
	INJECT(0x00473390, DSChangeVolume, replace);
	INJECT(0x004733B0, DSAdjustPitch, replace);
	INJECT(0x00473400, DSAdjustPan, replace);
	INJECT(0x00473460, DXSetOutputFormat, replace);
}
