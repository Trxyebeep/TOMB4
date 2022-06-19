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

bool DXDSCreate()
{
	sizeof(WAVEFORMATEX);
	Log(2, "DXDSCreate");
	DXAttempt(DirectSoundCreate(G_dxinfo->DSInfo[G_dxinfo->nDS].lpGuid, &App.dx.lpDS, 0));
	DXAttempt(App.dx.lpDS->SetCooperativeLevel(App.hWnd, DSSCL_EXCLUSIVE));
	DXSetOutputFormat();
	sound_active = 1;
	return 1;
}

bool InitSampleDecompress()
{
	pcm_format.wFormatTag = WAVE_FORMAT_PCM;
	pcm_format.cbSize = 0;
	pcm_format.nChannels = 1;
	pcm_format.nAvgBytesPerSec = 44100;
	pcm_format.nSamplesPerSec = 22050;
	pcm_format.nBlockAlign = 2;
	pcm_format.wBitsPerSample = 16;
	DS_mmresult = acmStreamOpen(&DS_hACMStream, hACMDriver, (LPWAVEFORMATEX)source_pcm_format, &pcm_format, 0, 0, 0, 0);

	if (DS_mmresult != DS_OK)
		Log(1, "Stream Open %d", DS_mmresult);

	decompressed_samples_buffer = (char*)MALLOC(0x40000);
	samples_buffer = (char*)MALLOC(0x4005A);
	memset(&ACMStreamHeader, 0, sizeof(ACMStreamHeader));
	ACMStreamHeader.pbSrc = (uchar*)(samples_buffer + 90);
	ACMStreamHeader.cbStruct = 84;
	ACMStreamHeader.cbSrcLength = 0x40000;
	ACMStreamHeader.cbDstLength = 0x40000;
	ACMStreamHeader.pbDst = (uchar*)decompressed_samples_buffer;
	DS_mmresult = acmStreamPrepareHeader(DS_hACMStream, &ACMStreamHeader, 0);

	if (DS_mmresult != DS_OK)
		Log(1, "Prepare Stream %d", DS_mmresult);

	return 1;
}

bool FreeSampleDecompress()
{
	ACMStreamHeader.cbSrcLength = 0x40000;
	DS_mmresult = acmStreamUnprepareHeader(DS_hACMStream, &ACMStreamHeader, 0);

	if (DS_mmresult != DS_OK)
		Log(1, "UnPrepare Stream %d", DS_mmresult);

	DS_mmresult = acmStreamClose(DS_hACMStream, 0);

	if (DS_mmresult != DS_OK)
		Log(1, "Stream Close %d", DS_mmresult);

	FREE(decompressed_samples_buffer);
	FREE(samples_buffer);
	return 1;
}

bool DXCreateSampleADPCM(char* data, long comp_size, long uncomp_size, long num)
{
	LPWAVEFORMATEX format;
	LPDIRECTSOUNDBUFFER buffer;
	LPVOID dest;
	DSBUFFERDESC desc;
	ulong bytes;

	Log(8, "DXCreateSampleADPCM");

	if (!App.dx.lpDS)
		return 0;

	format = (LPWAVEFORMATEX)(data + 20);

	if (format->nSamplesPerSec != 22050)
		Log(1, "Incorrect SamplesPerSec");

	ACMStreamHeader.cbSrcLength = comp_size - (sizeof(WAVEFORMATEX) + format->cbSize + 40);
	DS_mmresult = acmStreamConvert(DS_hACMStream, &ACMStreamHeader, ACM_STREAMCONVERTF_BLOCKALIGN | ACM_STREAMCONVERTF_START);

	if (DS_mmresult != DS_OK)
		Log(1, "Stream Convert %d", DS_mmresult);

	desc.dwSize = 20;
	desc.dwFlags = 226;
	desc.dwReserved = 0;
	desc.dwBufferBytes = uncomp_size - 32;
	desc.lpwfxFormat = &pcm_format;
	
	if (DXAttempt(App.dx.lpDS->CreateSoundBuffer(&desc, &buffer, 0)) != DS_OK)
	{
		Log(1, "Unable To Create Sound Buffer");
		return 0;
	}

	if (DXAttempt(buffer->Lock(0, uncomp_size - 32, &dest, &bytes, 0, 0, 0)) != DS_OK)
	{
		Log(1, "Unable To Lock Sound Buffer");
		return 0;
	}

	memcpy(dest, decompressed_samples_buffer, uncomp_size - 32);
	DXAttempt(buffer->Unlock(dest, bytes, 0, 0));
	DS_Buffers[num].frequency = pcm_format.nSamplesPerSec;
	DS_Buffers[num].buffer = buffer;
	return 1;
}

void DXStopSample(long num)
{
	if (num >= 0 && DS_Samples[num].buffer)
	{
		DXAttempt(DS_Samples[num].buffer->Stop());
		DXAttempt(DS_Samples[num].buffer->Release());
		DS_Samples[num].playing = 0;
		DS_Samples[num].buffer = 0;
	}
}

bool DSIsChannelPlaying(long num)
{
	ulong status;

	if (DS_Samples[num].buffer)
	{
		if (DXAttempt(DS_Samples[num].buffer->GetStatus(&status)) == DS_OK)
		{
			if (status & DSBSTATUS_PLAYING)
				return 1;

			DXStopSample(num);
		}
	}

	return 0;
}

void inject_dxsound(bool replace)
{
	INJECT(0x004732E0, DXChangeOutputFormat, replace);
	INJECT(0x00473390, DSChangeVolume, replace);
	INJECT(0x004733B0, DSAdjustPitch, replace);
	INJECT(0x00473400, DSAdjustPan, replace);
	INJECT(0x00473460, DXSetOutputFormat, replace);
	INJECT(0x00473500, DXDSCreate, replace);
	INJECT(0x00473570, InitSampleDecompress, replace);
	INJECT(0x00473690, FreeSampleDecompress, replace);
	INJECT(0x00473710, DXCreateSampleADPCM, replace);
	INJECT(0x004738B0, DXStopSample, replace);
	INJECT(0x00473900, DSIsChannelPlaying, replace);
}
