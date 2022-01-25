#include "../tomb4/pch.h"
#include "rope.h"
#include "../specific/specificfx.h"

void DrawRopeList()
{
	for (int i = 0; i < nRope; i++)
	{
		if (RopeList[i].Active)
			DrawRope(&RopeList[i]);
	}
}

void inject_rope(bool replace)
{
	INJECT(0x00459410, DrawRopeList, replace);
}
