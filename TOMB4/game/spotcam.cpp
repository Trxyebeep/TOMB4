#include "../tomb4/pch.h"
#include "spotcam.h"
#include "tomb4fx.h"
#include "../specific/3dmath.h"
#include "control.h"
#include "camera.h"
#include "switch.h"
#include "../specific/dxshell.h"
#include "../specific/input.h"
#include "lara.h"
#include "gameflow.h"
#include "../tomb4/tomb4.h"

SPOTCAM SpotCam[128];
long bTrackCamInit = 0;
long bUseSpotCam = 0;
long bDisableLaraControl = 0;
short LastSequence;
short CurrentFov;
short number_spotcams;

static PHD_VECTOR LaraFixedPosition;
static PHD_VECTOR InitialCameraPosition;
static PHD_VECTOR InitialCameraTarget;

static long camera_xposition[18];
static long camera_yposition[18];
static long camera_zposition[18];
static long camera_xtarget[18];
static long camera_ytarget[18];
static long camera_ztarget[18];
static long camera_roll[18];
static long camera_fov[18];
static long camera_speed[18];
static long CameraFade;
static long LaraHealth;
static long LaraAir;
static long spline_to_camera;
static long spline_from_camera;
static long bCheckTrigger = 0;
static long current_spline_position;
static short current_sequence;
static short current_spline_camera;
static short current_camera_cnt;
static short first_camera;
static short last_camera;
static short InitialCameraRoom;
static short LastFov;
static short spotcam_timer;
static short spotcam_loopcnt;
static uchar SpotRemap[8];
static uchar CameraCnt[8];

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
	else
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

void CalculateSpotCams()
{
	SPOTCAM* FirstCam;
	SPOTCAM* CurrentCam;
	ITEM_INFO* item;
	CAMERA_INFO backup;
	camera_type ctype;
	long cpx, cpy, cpz, ctx, cty, ctz, cspeed, cfov, croll, next_spline_camera, cunt;
	long cs, cp, clen, tlen, cx, cy, cz, lx, ly, lz, sp;
	static long bFirstLook = 0;
	short spline_cnt;

	if (bDisableLaraControl)
	{
		lara_item->hit_points = (short)LaraHealth;
		lara.air = (short)LaraAir;
	}

	FirstCam = &SpotCam[first_camera];
	sp = 0;

	if (FirstCam->flags & 8)
		spline_cnt = current_camera_cnt + 2;
	else
		spline_cnt = 4;

	cpx = Spline(current_spline_position, camera_xposition, spline_cnt);
	cpy = Spline(current_spline_position, camera_yposition, spline_cnt);
	cpz = Spline(current_spline_position, camera_zposition, spline_cnt);
	ctx = Spline(current_spline_position, camera_xtarget, spline_cnt);
	cty = Spline(current_spline_position, camera_ytarget, spline_cnt);
	ctz = Spline(current_spline_position, camera_ztarget, spline_cnt);
	cspeed = Spline(current_spline_position, camera_speed, spline_cnt);
	croll = Spline(current_spline_position, camera_roll, spline_cnt);
	cfov = Spline(current_spline_position, camera_fov, spline_cnt);
	CurrentCam = &SpotCam[current_spline_camera];

	if (CurrentCam->flags & 0x1000 && CameraFade != current_spline_camera)
		CameraFade = current_spline_camera;

	if (CurrentCam->flags & 0x2000 && CameraFade != current_spline_camera)
		CameraFade = current_spline_camera;

	if (FirstCam->flags & 8)
	{
		cp = 0;
		cs = 0x2000;
		lx = lara_item->pos.x_pos;
		ly = lara_item->pos.y_pos;
		lz = lara_item->pos.z_pos;

		for (int i = 0; i < 8; i++)
		{
			tlen = 0x10000;

			for (int j = 0; j < 8; j++)
			{
				cx = Spline(sp, camera_xposition, spline_cnt) - lx;
				cy = Spline(sp, camera_yposition, spline_cnt) - ly;
				cz = Spline(sp, camera_zposition, spline_cnt) - lz;
				clen = phd_sqrt(SQUARE(cx) + SQUARE(cy) + SQUARE(cz));

				if (clen <= tlen)
				{
					cp = sp;
					tlen = clen;
				}

				sp += cs;

				if (sp > 0x10000)
					break;
			}

			cs >>= 1;
			sp = cp - (cs << 1);

			if (sp < 0)
				sp = 0;
		}

		current_spline_position += (cp - current_spline_position) >> 5;

		if (FirstCam->flags & 1)
		{
			if (abs(cp - current_spline_position) > 0x8000)
				current_spline_position = cp;
		}

		if (cp < 0)
			current_spline_position = 0;
		else if (cp > 0x10000)
			current_spline_position = 0x10000;
	}
	else if (!spotcam_timer)
		current_spline_position += cspeed;

	if (tomb4.cutseq_skipper && keymap[DIK_ESCAPE] && gfCurrentLevel)
		current_spline_position = 0x10000;

	if (!(input & IN_LOOK))
		bFirstLook = 0;

	if (FirstCam->flags & 0x200 || !(input & IN_LOOK) || gfGameMode == 1)
	{
		camera.pos.x = cpx;
		camera.pos.y = cpy;
		camera.pos.z = cpz;

		if (FirstCam->flags & 0x28)
		{
			camera.target.x = lara_item->pos.x_pos;
			camera.target.y = lara_item->pos.y_pos;
			camera.target.z = lara_item->pos.z_pos;
		}
		else
		{
			camera.target.x = ctx;
			camera.target.y = cty;
			camera.target.z = ctz;
		}

		if (CurrentCam->flags & 2)
		{
			item = &items[SpotCam[current_spline_camera].timer];

			if (item)
			{
				camera.target.x = item->pos.x_pos;
				camera.target.y = item->pos.y_pos;
				camera.target.z = item->pos.z_pos;
			}
		}

		if (IsRoomOutside(cpx, cpy, cpz) == -2)
		{
			camera.pos.room_number = SpotCam[current_spline_camera].room_number;
			GetFloor(camera.pos.x, camera.pos.y, camera.pos.z, &camera.pos.room_number);
		}
		else
			camera.pos.room_number = IsRoomOutsideNo;

		AlterFOV((short)cfov);
		phd_LookAt(camera.pos.x, camera.pos.y, camera.pos.z, camera.target.x, camera.target.y, camera.target.z, (short)croll);

		if (bCheckTrigger)
		{
			ctype = camera.type;
			camera.type = HEAVY_CAMERA;

			if (gfCurrentLevel)
				TestTriggersAtXYZ(camera.pos.x, camera.pos.y, camera.pos.z, camera.pos.room_number, 1, 0);
			else
			{
				TestTriggersAtXYZ(camera.pos.x, camera.pos.y, camera.pos.z, camera.pos.room_number, 0, 0);
				TestTriggersAtXYZ(camera.pos.x, camera.pos.y, camera.pos.z, camera.pos.room_number, 1, 0);
			}

			camera.type = ctype;
			bCheckTrigger = 0;
		}

		if (FirstCam->flags & 8)
			bTrackCamInit = 1;
		else if (current_spline_position > 0x10000 - cspeed)
		{
			if (CurrentCam->flags & 0x4000)
				bCheckTrigger = 1;

			if (CurrentCam->flags & 0x100)
			{
				if (spotcam_timer)
					spotcam_timer--;
				else
					spotcam_timer = SpotCam[current_spline_camera].timer >> 4;
			}

			if (!spotcam_timer)
			{
				current_spline_position = 0;

				if (current_spline_camera == first_camera)
					next_spline_camera = last_camera;
				else
					next_spline_camera = current_spline_camera - 1;

				cunt = 1;

				if (spline_from_camera)
				{
					spline_from_camera = 0;
					next_spline_camera = first_camera - 1;
				}
				else
				{
					if (CurrentCam->flags & 0x800)
						bDisableLaraControl = 0;

					if (CurrentCam->flags & 0x400)
					{
						SetFadeClip(16, 1);
						bDisableLaraControl = 1;
					}

					cunt = 0;

					if (CurrentCam->flags & 0x80)
					{
						next_spline_camera = first_camera + (SpotCam[current_spline_camera].timer & 0xF);
						current_spline_camera = (short)next_spline_camera;
						SetSplineData(cunt, next_spline_camera);	//INLINED
						cunt = 1;
					}

					SetSplineData(cunt, next_spline_camera);	//INLINED
					cunt++;
				}

				next_spline_camera++;

				while (cunt < 4)
				{
					if (FirstCam->flags & 4)
					{
						if (next_spline_camera > last_camera)
							next_spline_camera = first_camera;
					}
					else if (next_spline_camera > last_camera)
						next_spline_camera = last_camera;

					SetSplineData(cunt, next_spline_camera);	//INLINED
					next_spline_camera++;
					cunt++;
				}

				current_spline_camera++;

				if (current_spline_camera > last_camera)
				{
					if (FirstCam->flags & 4)
					{
						spotcam_loopcnt++;
						current_spline_camera = first_camera;
					}
					else if (FirstCam->flags & 0x40 || spline_to_camera)
					{
						if (bCheckTrigger)
						{
							ctype = camera.type;
							camera.type = HEAVY_CAMERA;

							if (gfCurrentLevel)
								TestTriggersAtXYZ(camera.pos.x, camera.pos.y, camera.pos.z, camera.pos.room_number, 1, 0);
							else
							{
								TestTriggersAtXYZ(camera.pos.x, camera.pos.y, camera.pos.z, camera.pos.room_number, 0, 0);
								TestTriggersAtXYZ(camera.pos.x, camera.pos.y, camera.pos.z, camera.pos.room_number, 1, 0);
							}

							camera.type = ctype;
							bCheckTrigger = 0;
						}

						SetFadeClip(0, 1);
						bUseSpotCam = 0;
						bDisableLaraControl = 0;
						bCheckTrigger = 0;
						camera.old_type = FIXED_CAMERA;
						camera.type = CHASE_CAMERA;
						camera.speed = 1;

						if (FirstCam->flags & 0x40)
						{
							camera.pos.x = InitialCameraPosition.x;
							camera.pos.y = InitialCameraPosition.y;
							camera.pos.z = InitialCameraPosition.z;
							camera.target.x = InitialCameraTarget.x;
							camera.target.y = InitialCameraTarget.y;
							camera.target.z = InitialCameraTarget.z;
							camera.pos.room_number = InitialCameraRoom;
						}

						AlterFOV(LastFov);
					}
					else
					{
						current_spline_camera--;
						SetSplineData(0, current_spline_camera - 1);	//both inlined
						SetSplineData(1, current_spline_camera);
						memcpy(&backup, &camera, sizeof(CAMERA_INFO));
						camera.old_type = FIXED_CAMERA;
						camera.type = CHASE_CAMERA;
						camera.speed = 1;
						CalculateCamera();
						camera_fov[2] = CurrentFov;
						InitialCameraPosition.x = camera.pos.x;
						InitialCameraPosition.y = camera.pos.y;
						InitialCameraPosition.z = camera.pos.z;
						InitialCameraTarget.x = camera.target.x;
						InitialCameraTarget.y = camera.target.y;
						InitialCameraTarget.z = camera.target.z;
						camera_xposition[2] = camera.pos.x;
						camera_yposition[2] = camera.pos.y;
						camera_zposition[2] = camera.pos.z;
						camera_xtarget[2] = camera.target.x;
						camera_ytarget[2] = camera.target.y;
						camera_ztarget[2] = camera.target.z;
						camera_roll[2] = 0;
						camera_speed[2] = camera_speed[1];
						camera_xposition[3] = camera.pos.x;
						camera_yposition[3] = camera.pos.y;
						camera_zposition[3] = camera.pos.z;
						camera_xtarget[3] = camera.target.x;
						camera_ytarget[3] = camera.target.y;
						camera_ztarget[3] = camera.target.z;
						camera_fov[3] = CurrentFov;
						camera_speed[3] = camera_speed[1] >> 1;
						memcpy(&camera, &backup, sizeof(CAMERA_INFO));
						phd_LookAt(backup.pos.x, backup.pos.y, backup.pos.z, backup.target.x, backup.target.y, backup.target.z, (short)croll);
						spline_to_camera = 1;
					}
				}
			}
		}
	}
	else if (FirstCam->flags & 8)
	{
		if (!bFirstLook)
		{
			camera.old_type = FIXED_CAMERA;
			bFirstLook = 1;
		}

		CalculateCamera();
	}
	else
	{
		SetFadeClip(0, 1);
		bUseSpotCam = 0;
		bDisableLaraControl = 0;
		camera.speed = 1;
		AlterFOV(LastFov);
		CalculateCamera();
		bCheckTrigger = 0;
	}
}
