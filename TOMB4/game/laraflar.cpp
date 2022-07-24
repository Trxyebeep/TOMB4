#include "../tomb4/pch.h"
#include "laraflar.h"
#include "../specific/3dmath.h"
#include "objects.h"
#include "../specific/output.h"
#ifdef GENERAL_FIXES
#include "draw.h"
#endif

void DrawFlareInAir(ITEM_INFO* item)
{
#ifdef GENERAL_FIXES
	short* bounds;
#endif

	phd_PushMatrix();
#ifdef GENERAL_FIXES
	bounds = GetBoundsAccurate(item);
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos - bounds[3], item->pos.z_pos);
#else
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
#endif

	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	phd_PutPolygons_train(meshes[objects[FLARE_ITEM].mesh_index], 0);
	phd_PopMatrix();

	if (gfLevelFlags & GF_MIRROR)
	{
		if (item->room_number == gfMirrorRoom)
		{
			phd_PushMatrix();
			phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, 2 * gfMirrorZPlane - item->pos.z_pos);
			phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
			phd_PutPolygons_train(meshes[objects[FLARE_ITEM].mesh_index], 0);
			phd_PopMatrix();
		}
	}
}

void draw_flare_meshes()
{
	lara.mesh_ptrs[LM_LHAND] = meshes[objects[FLARE_ANIM].mesh_index + LM_LHAND * 2];
}

void undraw_flare_meshes()
{
	lara.mesh_ptrs[LM_LHAND] = meshes[objects[LARA].mesh_index + LM_LHAND * 2];
}

void inject_laraflar(bool replace)
{
	INJECT(0x0042F7B0, DrawFlareInAir, replace);
	INJECT(0x0042FF10, draw_flare_meshes, replace);
	INJECT(0x0042FF30, undraw_flare_meshes, replace);
}
