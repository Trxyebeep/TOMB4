#include "../tomb4/pch.h"
#include "sound.h"
#include "../specific/3dmath.h"
#include "../specific/dxsound.h"

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

void inject_sound(bool replace)
{
	INJECT(0x0045F7F0, GetPanVolume, replace);
	INJECT(0x0045FA10, StopSoundEffect, replace);
}
