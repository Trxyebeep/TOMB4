#pragma once
#include "../global/vars.h"

void inject_spotcam(bool replace);

void SetSplineData(long num, long cam);
void InitialiseSpotCam(short Sequence);
void InitSpotCamSequences();
long Spline(long x, long* knots, long nk);
void CalculateSpotCams();
