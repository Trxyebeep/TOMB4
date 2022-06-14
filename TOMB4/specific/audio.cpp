#include "../tomb4/pch.h"
#include "audio.h"
#include "file.h"
#include "function_stubs.h"

void OpenStreamFile(char* name)
{
	__try
	{
		EnterCriticalSection(&audio_cs);
	}
	__finally
	{
		LeaveCriticalSection(&audio_cs);
	}

	audio_stream_fp = FileOpen(name);

	if (!audio_stream_fp)
	{
		Log(1, "%s - Not Found", name);
		return;
	}

	SEEK(audio_stream_fp, 90, SEEK_SET);
	audio_fp_write_ptr = wav_file_buffer;
	memset(wav_file_buffer, 0, 0x37000);

	if (READ(wav_file_buffer, 1, 0x37000, audio_stream_fp) < 0x37000 && auido_play_mode == 1)
	{
		SEEK(audio_stream_fp, 90, SEEK_SET);
		Log(0, "FileReset In OpenStreamFile");
	}
}

void GetADPCMData()
{
	if (!audio_stream_fp)
		return;

	memset(audio_fp_write_ptr, 0, 0x5800);

	if (READ(audio_fp_write_ptr, 1, 0x5800, audio_stream_fp) < 0x5800 && auido_play_mode == 1)
	{
		Log(0, "FileReset In GetADPCMData");
		SEEK(audio_stream_fp, 90, 0);
	}

	audio_fp_write_ptr += 0x5800;

	if ((long)audio_fp_write_ptr >= long(wav_file_buffer + 0x37000))
		audio_fp_write_ptr = wav_file_buffer;
}

void ACMSetVolume()
{
	long volume;

	if (!MusicVolume)
		volume = -10000;
	else
		volume = -4000 * (100 - MusicVolume) / 100;

	if (G_DSBuffer)
		G_DSBuffer->SetVolume(volume);
}

void inject_audio(bool replace)
{
	INJECT(0x0046DE50, OpenStreamFile, replace);
	INJECT(0x0046E0F0, GetADPCMData, replace);
	INJECT(0x0046D7B0, ACMSetVolume, replace);
}
