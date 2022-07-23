#include "../tomb4/pch.h"
#include "larafire.h"
#include "lara_states.h"

static short HoldStates[] =
{
	AS_WALK,
#ifndef GENERAL_FIXES
	AS_RUN,
#endif
	AS_STOP,
	AS_POSE,
	AS_TURN_R,
	AS_TURN_L,
	AS_BACK,
	AS_FASTTURN,
	AS_STEPLEFT,
	AS_STEPRIGHT,
	AS_WADE,
#ifndef GENERAL_FIXES
	AS_PICKUP,
	AS_SWITCHON,
	AS_SWITCHOFF,
#endif
	AS_DUCK,
	AS_DUCKROTL,
	AS_DUCKROTR,
	-1
};

static long CheckForHoldingState(long state)
{
	short* holds;

	holds = HoldStates;

	while (*holds >= 0)
	{
		if (state == *holds)
			return 1;

		holds++;
	}

	return 0;
}

void inject_larafire(bool replace)
{
	INJECT(0x0042DDC0, CheckForHoldingState, replace);
}
