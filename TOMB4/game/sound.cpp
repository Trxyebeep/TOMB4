#include "../tomb4/pch.h"
#include "sound.h"
#include "../specific/3dmath.h"
#include "../specific/dxsound.h"
#include "../specific/function_stubs.h"
#include "camera.h"
#include "gameflow.h"
#include "control.h"

SAMPLE_INFO* sample_infos;
SoundSlot LaSlot[32];
short* sample_lut;
long sound_active = 0;

void GetPanVolume(SoundSlot* slot)
{
	long dx, dy, dz, radius, distance, nPan, nVolume;

	if (slot->distance || slot->pos.x || slot->pos.y || slot->pos.z)
	{
		dx = slot->pos.x - camera.pos.x;
		dy = slot->pos.y - camera.pos.y;
		dz = slot->pos.z - camera.pos.z;
		radius = sample_infos[slot->nSampleInfo].radius << 10;

		if (dx < -radius || dx > radius || dy < -radius || dy > radius || dz < -radius || dz > radius)
		{
			slot->distance = 0;
			slot->nPan = 0;
			slot->nVolume = 0;
		}
		else
		{
			distance = SQUARE(dx) + SQUARE(dy) + SQUARE(dz);

			if (distance <= SQUARE(radius))
			{
				if (distance >= 0x100000)
					distance = phd_sqrt(distance) - 1024;
				else
					distance = 0;

				nPan = (CamRot.y << 4) + phd_atan(dz, dx);
				nVolume = slot->OrigVolume;

				if (distance)
					nVolume = (nVolume * (4096 - (phd_sin((distance << 14) / radius) >> 2))) >> 12;

				if (nVolume > 0)
				{
					if (nVolume > 0x7FFF)
						nVolume = 0x7FFF;

					slot->nVolume = nVolume;
					slot->nPan = nPan;
					slot->distance = distance;
				}
				else
				{
					slot->distance = 0;
					slot->nPan = 0;
					slot->nVolume = 0;
				}
			}
			else
			{
				slot->distance = 0;
				slot->nPan = 0;
				slot->nVolume = 0;
			}
		}
	}
}

void StopSoundEffect(long sfx)
{
	long lut;

	if (sound_active)
	{
		lut = sample_lut[sfx];

		for (int i = 0; i < 32; i++)
		{
			if (LaSlot[i].nSampleInfo >= lut && LaSlot[i].nSampleInfo < (lut + ((sample_infos[lut].flags >> 2) & 0xF)))
			{
				S_SoundStopSample(i);
				LaSlot[i].nSampleInfo = -1;
			}
		}
	}
}

void SOUND_Init()
{
	//empty func call here

	for (int i = 0; i < 32; i++)
		LaSlot[i].nSampleInfo = -1;

	sound_active = 1;
}

void SOUND_Stop()
{
	if (sound_active)
	{
		S_SoundStopAllSamples();

		for (int i = 0; i < 32; i++)
			LaSlot[i].nSampleInfo = -1;
	}
}

long SoundEffect(long sfx, PHD_3DPOS* pos, long flags)
{
	SAMPLE_INFO* info;
	PHD_3DPOS pos2;
	long lut, radius, pan, dx, dy, dz, distance, volume, OrigVolume, pitch, rnd, sample, flag, vol, slot;

	if (sfx == SFX_LARA_NO)
	{
		switch (Gameflow->Language)
		{
		case 1:
			sfx = SFX_LARA_NO_FRENCH;
			break;

		case 2:
		case 3:
		case 4:
			sfx = SFX_LARA_NO;
			break;

		case 6:
			sfx = SFX_LARA_NO_JAPAN;
			break;
		}
	}

	if (!sound_active || !(flags & SFX_ALWAYS) && (flags & SFX_WATER) != (room[camera.pos.room_number].flags & ROOM_UNDERWATER))
		return 0;

	lut = sample_lut[sfx];

	if (lut == -1)
	{
		//empty func call here
		sample_lut[sfx] = -2;
		return 0;
	}

	if (lut == -2)
		return 0;

	info = &sample_infos[lut];

	if (info->randomness)
	{
		if ((GetRandomDraw() & 0xFF) > info->randomness)
			return 0;
	}

	radius = (info->radius + 1) << 10;
	pan = 0;

	if (pos)
	{
		dx = pos->x_pos - camera.pos.x;
		dy = pos->y_pos - camera.pos.y;
		dz = pos->z_pos - camera.pos.z;

		if (dx < -radius || dx > radius || dy < -radius || dy > radius || dz < -radius || dz > radius)
			return 0;

		distance = SQUARE(dx) + SQUARE(dy) + SQUARE(dz);

		if (distance > SQUARE(radius))
			return 0;

		if (distance >= 0x100000)
			distance = phd_sqrt(distance) - 1024;
		else
			distance = 0;

		if (!(info->flags & 0x1000))
			pan = (CamRot.y << 4) + phd_atan(dz, dx);
	}
	else
	{
		distance = 0;
		pos2.x_pos = 0;
		pos2.y_pos = 0;
		pos2.z_pos = 0;
		pos = &pos2;
	}

	volume = info->volume << 6;

	if (info->flags & 0x4000)
		volume -= GetRandomDraw() << 12 >> 15;

	OrigVolume = volume;

	if (distance)
		volume = (volume * (4096 - (phd_sin((distance << 14) / radius) >> 2))) >> 12;

	if (volume <= 0)
		return 0;

	if (volume > 0x7FFF)
		volume = 0x7FFF;

	if (flags & SFX_SETPITCH)
		pitch = (flags >> 8) & 0xFFFFFF;
	else
		pitch = 0x10000;

	pitch += info->pitch << 9;

	if (info->flags & 0x2000)
		pitch += ((6000 * GetRandomDraw()) >> 14) - 6000;

	if (info->number < 0)
		return 0;

	rnd = (info->flags >> 2) & 0xF;

	if (rnd == 1)
		sample = info->number;
	else
		sample = info->number + ((rnd * GetRandomDraw()) >> 15);

	flag = info->flags & 3;

	switch (flag)
	{
	case 1:

		for (int i = 0; i < 32; i++)
		{
			if (LaSlot[i].nSampleInfo == lut)
			{
				if (S_SoundSampleIsPlaying(i))
					return 0;

				LaSlot[i].nSampleInfo = -1;
			}
		}

		break;

	case 2:

		for (int i = 0; i < 32; i++)
		{
			if (LaSlot[i].nSampleInfo == lut)
			{
				S_SoundStopSample(i);
				LaSlot[i].nSampleInfo = -1;
				break;
			}
		}

		break;

	case 3:

		for (int i = 0; i < 32; i++)
		{
			if (LaSlot[i].nSampleInfo == lut)
			{
				if (volume > LaSlot[i].nVolume)
				{
					LaSlot[i].OrigVolume = OrigVolume;
					LaSlot[i].nVolume = volume;
					LaSlot[i].nPan = pan;
					LaSlot[i].nPitch = pitch;
					LaSlot[i].distance = distance;
					LaSlot[i].pos.x = pos->x_pos;
					LaSlot[i].pos.y = pos->y_pos;
					LaSlot[i].pos.z = pos->z_pos;
					return 1;
				}

				return 0;
			}
		}

		break;
	}

	if (flag == 3)
		dx = S_SoundPlaySampleLooped(sample, (ushort)volume, pitch, (short)pan);
	else
		dx = S_SoundPlaySample(sample, (ushort)volume, pitch, (short)pan);

	if (dx >= 0)
	{
		LaSlot[dx].OrigVolume = OrigVolume;
		LaSlot[dx].nVolume = volume;
		LaSlot[dx].nPan = pan;
		LaSlot[dx].nPitch = pitch;
		LaSlot[dx].nSampleInfo = lut;
		LaSlot[dx].distance = distance;
		LaSlot[dx].pos.x = pos->x_pos;
		LaSlot[dx].pos.y = pos->y_pos;
		LaSlot[dx].pos.z = pos->z_pos;
		return 1;
	}

	if (dx == -1)
	{
		vol = 0x8000000;
		slot = -1;

		for (int i = 1; i < 32; i++)
		{
			if ((LaSlot[i].nSampleInfo >= 0) && (LaSlot[i].nVolume <= vol))
			{
				vol = LaSlot[i].nVolume;
				slot = i;
			}
		}

		if (volume > vol)
		{
			S_SoundStopSample(slot);
			LaSlot[slot].nSampleInfo = -1;

			if (flag == 3)
				dx = S_SoundPlaySampleLooped(sample, (ushort)volume, pitch, (short)pan);
			else
				dx = S_SoundPlaySample(sample, (ushort)volume, pitch, (short)pan);

			if (dx >= 0)
			{
				LaSlot[dx].OrigVolume = OrigVolume;
				LaSlot[dx].nVolume = volume;
				LaSlot[dx].nPan = pan;
				LaSlot[dx].nPitch = pitch;
				LaSlot[dx].nSampleInfo = lut;
				LaSlot[dx].distance = distance;
				LaSlot[dx].pos.x = pos->x_pos;
				LaSlot[dx].pos.y = pos->y_pos;
				LaSlot[dx].pos.z = pos->z_pos;
				return 1;
			}
		}

		return 0;
	}

//	if (sample >= 0)
		//empty func call here

	info->number = -1;
	return 0;
}

void SayNo()
{
	long fx;

	fx = SFX_LARA_NO;

	if (Gameflow->Language == FRENCH)
		fx = SFX_LARA_NO_FRENCH;
	else if (Gameflow->Language == JAPAN)
		fx = SFX_LARA_NO_JAPAN;

	SoundEffect(fx, 0, SFX_ALWAYS);
}
