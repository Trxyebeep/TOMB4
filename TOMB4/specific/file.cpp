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

bool FindCDDrive()
{
	HANDLE file;
	ulong drives, type;
	char path[14];
	char root[5];

	strcpy(path, "c:\\script.dat");
	drives = GetLogicalDrives();
	cd_drive = 'A';
	lstrcpy(root, "A:\\");

	while (drives)
	{
		if (drives & 1)
		{
			root[0] = cd_drive;
			type = GetDriveType(root);

			if (type == DRIVE_CDROM)
			{
				path[0] = cd_drive;
				file = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

				if (file != INVALID_HANDLE_VALUE)
				{
					CloseHandle(file);
					return 1;
				}
			}
		}

		cd_drive++;
		drives >>= 1;
	}

	return 0;
}

void inject_file(bool replace)
{
	INJECT(0x00476470, LoadLevel, 0);

	INJECT(0x004768C0, S_LoadLevelFile, replace);
	INJECT(0x00476790, FreeLevel, 0);
	INJECT(0x00473C10, FindCDDrive, replace);
}
