#include "../tomb4/pch.h"
#include "dxsound.h"
#include "dxshell.h"
#include "function_stubs.h"
#include "audio.h"

static LPDIRECTSOUNDBUFFER DSPrimary;
static DS_SAMPLE DS_Samples[32];
static DS_SAMPLE DS_Buffers[256];
static MMRESULT mmresult;
static WAVEFORMATEX pcm_format;
static HACMSTREAM hACMStream;
static ACMSTREAMHEADER ACMStreamHeader;

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

	if (DSPrimary && DXAttempt(DSPrimary->SetFormat(&pcfxFormat)) != DS_OK)
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

	if (DXAttempt(App.dx.lpDS->CreateSoundBuffer(&desc, &DSPrimary, 0)) == DS_OK)
	{
		DXChangeOutputFormat(sfx_frequencies[SoundQuality], 0);
		DSPrimary->Play(0, 0, DSBPLAY_LOOPING);
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
	mmresult = acmStreamOpen(&hACMStream, hACMDriver, (LPWAVEFORMATEX)source_pcm_format, &pcm_format, 0, 0, 0, 0);

	if (mmresult != DS_OK)
		Log(1, "Stream Open %d", mmresult);

	decompressed_samples_buffer = (char*)MALLOC(0x40000);
	samples_buffer = (char*)MALLOC(0x4005A);
	memset(&ACMStreamHeader, 0, sizeof(ACMStreamHeader));
	ACMStreamHeader.pbSrc = (uchar*)(samples_buffer + 90);
	ACMStreamHeader.cbStruct = 84;
	ACMStreamHeader.cbSrcLength = 0x40000;
	ACMStreamHeader.cbDstLength = 0x40000;
	ACMStreamHeader.pbDst = (uchar*)decompressed_samples_buffer;
	mmresult = acmStreamPrepareHeader(hACMStream, &ACMStreamHeader, 0);

	if (mmresult != DS_OK)
		Log(1, "Prepare Stream %d", mmresult);

	return 1;
}

bool FreeSampleDecompress()
{
	ACMStreamHeader.cbSrcLength = 0x40000;
	mmresult = acmStreamUnprepareHeader(hACMStream, &ACMStreamHeader, 0);

	if (mmresult != DS_OK)
		Log(1, "UnPrepare Stream %d", mmresult);

	mmresult = acmStreamClose(hACMStream, 0);

	if (mmresult != DS_OK)
		Log(1, "Stream Close %d", mmresult);

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
	mmresult = acmStreamConvert(hACMStream, &ACMStreamHeader, ACM_STREAMCONVERTF_BLOCKALIGN | ACM_STREAMCONVERTF_START);

	if (mmresult != DS_OK)
		Log(1, "Stream Convert %d", mmresult);

	desc.dwSize = 20;
	desc.dwFlags = DSBCAPS_STATIC | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
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

long DSGetFreeChannel()
{
	for (int i = 0; i < 32; i++)
	{
		if (!DSIsChannelPlaying(i))
			return i;
	}

	return -1;
}

long DXStartSample(long num, long volume, long pitch, long pan, ulong flags)
{
	LPDIRECTSOUNDBUFFER buffer;
	long channel;

	channel = DSGetFreeChannel();

	if (channel < 0 || DXAttempt(App.dx.lpDS->DuplicateSoundBuffer(DS_Buffers[num].buffer, &buffer)) != DS_OK)
		return -1;

	if (DXAttempt(buffer->SetVolume(volume)) != DS_OK || DXAttempt(buffer->SetCurrentPosition(0)) != DS_OK)
		return -1;

	DS_Samples[channel].buffer = buffer;
	DS_Samples[channel].playing = num;
	DSAdjustPitch(channel, pitch);
	DSAdjustPan(channel, pan);
	buffer->Stop();
	DXAttempt(buffer->Play(0, 0, flags));
	return channel;
}

long CalcVolume(long volume)
{
	long result;

	result = 8000 - long(float(0x7FFF - volume) * 0.30518511F);

	if (result > 0)
		result = 0;
	else if (result < -10000)
		result = -10000;

	result -= (100 - SFXVolume) * 50;

	if (result > 0)
		result = 0;

	if (result < -10000)
		result = -10000;

	return result;
}

void S_SoundStopAllSamples()
{
	for (int i = 0; i < 32; i++)
		DXStopSample(i);
}

void S_SoundStopSample(long num)
{
	DXStopSample(num);
}

long S_SoundPlaySample(long num, ushort volume, long pitch, short pan)
{
	return DXStartSample(num, CalcVolume(volume), pitch, pan, 0);
}

long S_SoundPlaySampleLooped(long num, ushort volume, long pitch, short pan)
{
	return DXStartSample(num, CalcVolume(volume), pitch, pan, DSBPLAY_LOOPING);
}

void DXFreeSounds()
{
	S_SoundStopAllSamples();

	for (int i = 0; i < 256; i++)
	{
		if (DS_Buffers[i].buffer)
		{
			Log(4, "Released %s @ %x - RefCnt = %d", "SoundBuffer", DS_Buffers[i].buffer, DS_Buffers[i].buffer->Release());
			DS_Buffers[i].buffer = 0;
		}
	}
}

long S_SoundSampleIsPlaying(long num)
{
	if (sound_active && DSIsChannelPlaying(num))
		return 1;

	return 0;
}

void S_SoundSetPanAndVolume(long num, short pan, ushort volume)
{
	if (sound_active)
	{
		DSChangeVolume(num, CalcVolume(volume));
		DSAdjustPan(num, pan);
	}
}

void S_SoundSetPitch(long num, long pitch)
{
	if (sound_active)
		DSAdjustPitch(num, pitch);
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
	INJECT(0x00473950, DSGetFreeChannel, replace);
	INJECT(0x00473970, DXStartSample, replace);
	INJECT(0x00473A50, CalcVolume, replace);
	INJECT(0x00473AB0, S_SoundStopAllSamples, replace);
	INJECT(0x00473AD0, S_SoundStopSample, replace);
	INJECT(0x00473AE0, S_SoundPlaySample, replace);
	INJECT(0x00473B10, S_SoundPlaySampleLooped, replace);
	INJECT(0x00473B40, DXFreeSounds, replace);
	INJECT(0x00473B90, S_SoundSampleIsPlaying, replace);
	INJECT(0x00473BB0, S_SoundSetPanAndVolume, replace);
	INJECT(0x00473BF0, S_SoundSetPitch, replace);
}
