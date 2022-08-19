#pragma once
#include "../global/types.h"

void SetSplineData(long num, long cam);
void InitialiseSpotCam(short Sequence);
void InitSpotCamSequences();
long Spline(long x, long* knots, long nk);
void CalculateSpotCams();

extern SPOTCAM SpotCam[];
extern long bTrackCamInit;
extern short LastSequence;
extern short CurrentFov;
extern short number_spotcams;
extern long bUseSpotCam;
extern long bDisableLaraControl;

enum spotcam_flags
{
	SP_SNAPCAMERA = (1 << 0),	//snaps the camera to the first spline
	SP_VIGNETTE = (1 << 1),	//?
	SP_LOOPCAMERA = (1 << 2),	//loops the sequence (if past last camera -> go back to first)
	SP_TRACKCAMERA = (1 << 3),	//?
	SP_NODRAWLARA = (1 << 4),	//disable drawing lara
	SP_TARGETLARA = (1 << 5),	//overrides the camera target to lara's position
	SP_SNAPBACK = (1 << 6),	//?
	SP_JUMPTO = (1 << 7),	//cuts to another camera in the same sequence
	SP_HOLD = (1 << 8),	//stops moving, or, "holds" the camera for a given time, the "timer" field
	SP_NOBREAK = (1 << 9),	//don't allow breaking the sequence with the look button
	SP_NOLARACONTROL = (1 << 10),	//disable lara control
	SP_ENABLELARACONTROL = (1 << 11),	//enable lara control
	SP_FADEINSCREEN = (1 << 12),	//do screen fadein
	SP_FADEOUTSCREEN = (1 << 13),	//do screen fadeout
	SP_TESTTRIGGER = (1 << 14),	//test heavy triggers
	SP_FLYBYONESHOT = (1 << 15)	//used in TestTriggers to force the flyby trigger to be one shot
};
