#include "../tomb4/pch.h"
#include "spotcam.h"
#include "tomb4fx.h"

void SetSplineData(long num, long cam)
{
	SPOTCAM* spotcam;
	ITEM_INFO* item;

	spotcam = &SpotCam[cam];

	camera_xposition[num] = spotcam->x;
	camera_yposition[num] = spotcam->y;
	camera_zposition[num] = spotcam->z;
	camera_xtarget[num] = spotcam->tx;
	camera_ytarget[num] = spotcam->ty;
	camera_ztarget[num] = spotcam->tz;

	if (spotcam->flags & 0x10)
	{
		camera_xtarget[num] = LaraFixedPosition.x;
		camera_ytarget[num] = LaraFixedPosition.y;
		camera_ztarget[num] = LaraFixedPosition.z;
	}
	else//useless
	{
		camera_xtarget[num] = spotcam->tx;
		camera_ytarget[num] = spotcam->ty;
		camera_ztarget[num] = spotcam->tz;
	}

	if (spotcam->flags & 2)
	{
		item = &items[spotcam->timer];

		if (item)
		{
			camera_xtarget[num] = item->pos.x_pos;
			camera_ytarget[num] = item->pos.y_pos;
			camera_ztarget[num] = item->pos.z_pos;
		}
	}

	camera_roll[num] = spotcam->roll;
	camera_speed[num] = spotcam->speed;
	camera_fov[num] = spotcam->fov;
}

void InitialiseSpotCam(short Sequence)
{
	SPOTCAM* s;
	long next_spline_camera, cunt;

	if (bTrackCamInit && Sequence == LastSequence)
	{
		bTrackCamInit = 0;
		return;
	}

	LastFov = CurrentFov;
	CameraFade = -1;
	LastSequence = Sequence;
	bTrackCamInit = 0;
	spotcam_timer = 0;
	spotcam_loopcnt = 0;
	bDisableLaraControl = 0;
	LaraHealth = lara_item->hit_points;
	LaraAir = lara.air;
	InitialCameraTarget.x = camera.target.x;
	InitialCameraTarget.y = camera.target.y;
	InitialCameraTarget.z = camera.target.z;
	InitialCameraPosition.x = camera.pos.x;
	InitialCameraPosition.y = camera.pos.y;
	InitialCameraPosition.z = camera.pos.z;
	InitialCameraRoom = camera.pos.room_number;
	LaraFixedPosition.x = lara_item->pos.x_pos;
	LaraFixedPosition.y = lara_item->pos.y_pos;
	LaraFixedPosition.z = lara_item->pos.z_pos;
	current_sequence = Sequence;
	current_spline_camera = 0;

	for (int i = 0; i < SpotRemap[Sequence]; i++)
		current_spline_camera += CameraCnt[i];

	current_camera_cnt = CameraCnt[SpotRemap[Sequence]];
	first_camera = current_spline_camera;
	last_camera = first_camera + current_camera_cnt - 1;
	current_spline_position = 0;
	spline_to_camera = 0;
	s = &SpotCam[current_spline_camera];

	if (s->flags & 0x400 || gfGameMode == 1)
	{
		bDisableLaraControl = 1;

		if (gfGameMode != 1)
			SetFadeClip(16, 1);
	}

	if (s->flags & 8)
	{
		spline_from_camera = 0;
		cunt = 0;
		SetSplineData(cunt, first_camera);	//INLINED
		cunt++;
		next_spline_camera = first_camera;

		for (int i = 0; i < current_camera_cnt; i++)
		{
			SetSplineData(cunt, next_spline_camera);	//INLINED
			cunt++;
			next_spline_camera++;
		}

		SetSplineData(cunt, last_camera);	//INLINED
	}
	else if (s->flags & 1)
	{
		spline_from_camera = 0;
		cunt = 0;
		next_spline_camera = current_spline_camera;
		SetSplineData(cunt, next_spline_camera);	//INLINED
		cunt++;

		while (cunt < 4)
		{
			if (next_spline_camera > last_camera)
				next_spline_camera = first_camera;

			SetSplineData(cunt, next_spline_camera);
			cunt++;
			next_spline_camera++;
		}

		current_spline_camera++;

		if (current_spline_camera > last_camera)
			current_spline_camera = first_camera;

		if (s->flags & 0x4000)
			bCheckTrigger = 1;
	}
	else
	{
		spline_from_camera = 1;
		camera_xtarget[0] = InitialCameraTarget.x;
		camera_ytarget[0] = InitialCameraTarget.y;
		camera_ztarget[0] = InitialCameraTarget.z;
		camera_xposition[0] = InitialCameraPosition.x;
		camera_yposition[0] = InitialCameraPosition.y;
		camera_zposition[0] = InitialCameraPosition.z;
		camera_roll[0] = 0;
		camera_fov[0] = CurrentFov;
		camera_xposition[1] = camera_xposition[0];
		camera_yposition[1] = camera_yposition[0];
		camera_zposition[1] = camera_zposition[0];
		camera_xtarget[1] = camera_xtarget[0];
		camera_ytarget[1] = camera_ytarget[0];
		camera_ztarget[1] = camera_ztarget[0];
		camera_roll[1] = camera_roll[0];
		camera_fov[1] = camera_fov[0];
		camera_speed[1] = camera_speed[0];
		cunt = current_spline_camera;
		SetSplineData(2, cunt);
		cunt++;

		if (cunt > last_camera)
			cunt = first_camera;

		SetSplineData(3, cunt);
	}
}

void InitSpotCamSequences()
{
	uchar s, cc, ce;

	bTrackCamInit = 0;

	if (number_spotcams)
	{
		ce = 0;
		s = SpotCam[0].sequence;
		cc = 1;

		for (int i = 1; i < number_spotcams; i++)
		{
			if (SpotCam[i].sequence == s)
				cc++;
			else
			{
				CameraCnt[ce] = cc;
				SpotRemap[s] = ce;
				s = SpotCam[i].sequence;
				cc = 1;
				ce++;
			}
		}

		CameraCnt[ce] = cc;
		SpotRemap[s] = ce;
	}
}

long Spline(long x, long* knots, long nk)
{
	long* k;
	long span, c1, c2;

	span = x * (nk - 3) >> 16;

	if (span >= nk - 3)
		span = nk - 4;

	k = &knots[span];
	x = x * (nk - 3) - span * 65536;
	c1 = (k[1] >> 1) - (k[2] >> 1) - k[2] + k[1] + (k[3] >> 1) + ((-k[0] - 1) >> 1);
	c2 = 2 * k[2] - 2 * k[1] - (k[1] >> 1) - (k[3] >> 1) + k[0];
	return ((__int64)x * (((__int64)x * (((__int64)x * c1 >> 16) + c2) >> 16) + (k[2] >> 1) + ((-k[0] - 1) >> 1)) >> 16) + k[1];
}

void inject_spotcam(bool replace)
{
	INJECT(0x00460B50, SetSplineData, replace);
	INJECT(0x00460420, InitialiseSpotCam, replace);
	INJECT(0x004602B0, InitSpotCamSequences, replace);
	INJECT(0x00460320, Spline, replace);
}
