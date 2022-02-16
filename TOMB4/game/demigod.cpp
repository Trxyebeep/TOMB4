#include "../tomb4/pch.h"
#include "demigod.h"
#include "items.h"
#include "../specific/function_stubs.h"
#include "objects.h"

void TriggerDemigodMissile(PHD_3DPOS* pos, short room_number, short type)
{
	FX_INFO* fx;
	short fx_number;

	fx_number = CreateEffect(room_number);

	if (fx_number != NO_ITEM)
	{
		fx = &effects[fx_number];
		fx->pos.x_pos = pos->x_pos;
        fx->pos.y_pos = pos->y_pos - (GetRandomControl() & 0x3F) - 32;
        fx->pos.z_pos = pos->z_pos;
        fx->pos.x_rot = pos->x_rot;

        if (type < 4)
            fx->pos.y_rot = pos->y_rot;
        else
            fx->pos.y_rot = pos->y_rot + (GetRandomControl() & 0x7FF) - 1024;

        fx->pos.z_rot = 0;

		fx->room_number = room_number;
		fx->counter = 2 * GetRandomControl() + 0x8000;
		fx->flag1 = type;
		fx->speed = (GetRandomControl() & 0x1F) + 96;
		fx->object_number = BUBBLES;

		if (type >= 4)
			type--;

		fx->frame_number = objects[BUBBLES].mesh_index + type * 2;
	}
}

void inject_demigod(bool replace)
{
	INJECT(0x00404770, TriggerDemigodMissile, replace);
}
