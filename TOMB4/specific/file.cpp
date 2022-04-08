#include "../tomb4/pch.h"
#include "file.h"
#include "function_stubs.h"
#include "texture.h"
#include "lighting.h"
#include "dxsound.h"

unsigned int __stdcall LoadLevel(void* name)
{
	return 1;
}

long S_LoadLevelFile(long num)
{
	char name[80];

	Log(2, "S_LoadLevelFile");
	strcpy(name, &gfFilenameWad[gfFilenameOffset[num]]);
	strcat(name, ".TR4");
	LevelLoadingThread.active = 1;
	LevelLoadingThread.ended = 0;
	LevelLoadingThread.handle = _beginthreadex(0, 0, &LoadLevel, name, 0, (unsigned int*)&LevelLoadingThread.address);
	while (LevelLoadingThread.active);
	return 1;
}

void FreeLevel()
{
	MESH_DATA** vbuf;
	MESH_DATA* mesh;
	ROOM_INFO* r;

	Log(2, "FreeLevel");

	for (int i = 0; i < num_level_meshes; i++)
	{
		vbuf = &mesh_vtxbuf[i];
		mesh = *vbuf;

		if (mesh->SourceVB)
		{
			Log(4, "Released %s @ %x - RefCnt = %d", "Mesh VB", mesh->SourceVB, mesh->SourceVB->Release());
			mesh->SourceVB = 0;
		}
	}
	
	if (room)
	{
		for (int i = 0; i < number_rooms; i++)
		{
			r = &room[i];

			if (r->SourceVB)
			{
				Log(4, "Released %s @ %x - RefCnt = %d", "Source VB", r->SourceVB, r->SourceVB->Release());
				r->SourceVB = 0;
			}
			else
				Log(1, "%s Attempt To Release NULL Ptr", "Source VB");
		}
	}

	Log(5, "Free Textures");
	FreeTextures();
	Log(5, "Free Lights");
	FreeD3DLights();
	DXFreeSounds();
	free(OutsideRoomTable);
	free(OutsideRoomOffsets);
	malloc_ptr = malloc_buffer;
	malloc_free = malloc_size;
}

void inject_file(bool replace)
{
	INJECT(0x00476470, LoadLevel, 0);

	INJECT(0x004768C0, S_LoadLevelFile, replace);
	INJECT(0x00476790, FreeLevel, 0);
}
