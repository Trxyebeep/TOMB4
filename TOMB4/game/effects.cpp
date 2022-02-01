#include "../tomb4/pch.h"
#include "effects.h"
#include "../specific/polyinsert.h"
#include "../specific/function_table.h"

void SetFog(ITEM_INFO* item)
{
	GlobalFogOff = 0;

	if (IsVolumetric())
	{
		if (TriggerTimer == 100)
			GlobalFogOff = 1;
		else
		{
			gfFog.r = CLRR(FogTableColor[TriggerTimer]);
			gfFog.g = CLRG(FogTableColor[TriggerTimer]);
			gfFog.b = CLRB(FogTableColor[TriggerTimer]);
			savegame.fog_colour.r = gfFog.r;
			savegame.fog_colour.g = gfFog.g;
			savegame.fog_colour.b = gfFog.b;
			SetFogColor(gfFog.r, gfFog.g, gfFog.b);
		}
	}

	flipeffect = -1;
}

void inject_effects(bool replace)
{
	INJECT(0x00437AB0, SetFog, replace);
}
