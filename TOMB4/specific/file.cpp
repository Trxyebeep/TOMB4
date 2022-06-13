#include "../tomb4/pch.h"
#include "file.h"
#include "function_stubs.h"
#include "texture.h"
#include "lighting.h"
#include "dxsound.h"
#include "LoadSave.h"
#include "dxshell.h"
#include "drawroom.h"
#include "../game/setup.h"
#include "../game/objects.h"
#include "../game/laraskin.h"

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

FILE* FileOpen(const char* name)
{
	FILE* file;
	char path_name[80];

	memset(path_name, 0, 80);
#ifndef NO_CD
	path_name[0] = cd_drive;
	path_name[1] = ':';		//original code
	path_name[2] = '\\';
#endif

	strcat(path_name, name);
	Log(5, "FileOpen - %s", path_name);
	file = OPEN(path_name, "rb");//file = fopen(path_name, "rb");

	if (!file)
		Log(1, "Unable To Open %s", path_name);

	return file;
}

void FileClose(FILE* file)
{
	Log(2, "FileClose");
	CLOSE(file);//fclose(file);
}

long FileSize(FILE* file)
{
	long size;

	SEEK(file, 0, SEEK_END);//fseek(file, 0, SEEK_END);
	size = TELL(file);//ftell(file);
	SEEK(file, 0, SEEK_SET);//fseek(file, 0, SEEK_SET);
	return size;
}

long LoadFile(const char* name, char** dest)
{
	FILE* file;
	long size, count;

	Log(2, "LoadFile");
	Log(5, "File - %s", name);
	file = FileOpen(name);

	if (!file)
		return 0;

	size = FileSize(file);

	if (!*dest)
		*dest = (char*)malloc(size);

	count = READ(*dest, 1, size, file); //fread(*dest, 1, size, file);
	Log(5, "Read - %d FileSize - %d", count, size);

	if (count != size)
	{
		Log(1, "Error Reading File");
		FileClose(file);
		free(*dest);
		return 0;
	}

	FileClose(file);
	return size;
}

bool LoadTextures(long RTPages, long OTPages, long BTPages)
{
	DXTEXTUREINFO* dxtex;
	LPDIRECTDRAWSURFACE4 tSurf;
	LPDIRECT3DTEXTURE2 pTex;
	uchar* TextureData;
	long* d;
	char* pData;
	char* pComp;
	char* s;
	long format, skip, size, compressedSize, nTex, c;
	uchar r, g, b, a;

	Log(2, "LoadTextures");
	nTextures = 1;
	format = 0;
	skip = 4;
	dxtex = &G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].TextureInfos[G_dxinfo->nTexture];

	if (dxtex->rbpp == 8 && dxtex->gbpp == 8 && dxtex->bbpp == 8 && dxtex->abpp == 8)
		format = 1;
	else if (dxtex->rbpp == 5 && dxtex->gbpp == 5 && dxtex->bbpp == 5 && dxtex->abpp == 1)
	{
		format = 2;
		skip = 2;
	}

	if (format <= 1)
	{
		READ(&size, 1, 4, level_fp);
		READ(&compressedSize, 1, 4, level_fp);

		CompressedData = (char*)MALLOC(compressedSize);
		FileData = (char*)MALLOC(size);

		READ(CompressedData, compressedSize, 1, level_fp);
		Decompress(FileData, CompressedData, compressedSize, size);

		READ(&size, 1, 4, level_fp);
		READ(&compressedSize, 1, 4, level_fp);
		SEEK(level_fp, compressedSize, SEEK_CUR);
		FREE(CompressedData);
	}
	else
	{
		READ(&size, 1, 4, level_fp);
		READ(&compressedSize, 1, 4, level_fp);
		SEEK(level_fp, compressedSize, SEEK_CUR);

		READ(&size, 1, 4, level_fp);
		READ(&compressedSize, 1, 4, level_fp);

		CompressedData = (char*)MALLOC(compressedSize);
		FileData = (char*)MALLOC(size);
		READ(CompressedData, compressedSize, 1, level_fp);
		Decompress(FileData, CompressedData, compressedSize, size);
		FREE(CompressedData);
	}

	pData = FileData;

	Log(5, "RTPages %d", RTPages);
	size = RTPages * skip * 0x10000;
	TextureData = (uchar*)MALLOC(size);
	memcpy(TextureData, FileData, size);
	FileData += size;
	S_LoadBar();

	for (int i = 0; i < RTPages; i++)
	{
		Textures = (TEXTURE*)AddStruct(Textures, nTextures, sizeof(TEXTURE));
		nTex = nTextures;
		nTextures++;
		tSurf = CreateTexturePage(App.TextureSize, App.TextureSize, 0, (long*)(TextureData + (i * skip * 0x10000)), 0, format);
		DXAttempt(tSurf->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&pTex));
		Textures[nTex].tex = pTex;
		Textures[nTex].surface = tSurf;
		Textures[nTex].width = App.TextureSize;
		Textures[nTex].height = App.TextureSize;
		Textures[nTex].bump = 0;
		App.dx.lpD3DDevice->SetTexture(0, pTex);
	}

	FREE(TextureData);

	Log(5, "OTPages %d", OTPages);
	size = OTPages * skip * 0x10000;
	TextureData = (uchar*)MALLOC(size);
	memcpy(TextureData, FileData, size);
	FileData += size;
	S_LoadBar();

	for (int i = 0; i < OTPages; i++)
	{
		Textures = (TEXTURE*)AddStruct(Textures, nTextures, sizeof(TEXTURE));
		nTex = nTextures;
		nTextures++;
		tSurf = CreateTexturePage(App.TextureSize, App.TextureSize, 0, (long*)(TextureData + (i * skip * 0x10000)), 0, format);
		DXAttempt(tSurf->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&pTex));
		Textures[nTex].tex = pTex;
		Textures[nTex].surface = tSurf;
		Textures[nTex].width = App.TextureSize;
		Textures[nTex].height = App.TextureSize;
		Textures[nTex].bump = 0;
		App.dx.lpD3DDevice->SetTexture(0, pTex);
	}

	FREE(TextureData);
	S_LoadBar();

	Log(5, "BTPages %d", BTPages);

	if (BTPages)
	{
		size = BTPages * skip * 0x10000;
		TextureData = (uchar*)MALLOC(size);
		memcpy(TextureData, FileData, size);
		FileData += size;

		for (int i = 0; i < BTPages; i++)
		{
			if (i < (BTPages >> 1))
				tSurf = CreateTexturePage(App.TextureSize, App.TextureSize, 0, (long*)(TextureData + (i * skip * 0x10000)), 0, format);
			else
			{
				if (!App.BumpMapping)
					break;

				tSurf = CreateTexturePage(App.BumpMapSize, App.BumpMapSize, 0, (long*)(TextureData + (i * skip * 0x10000)), 0, format);
			}

			Textures = (TEXTURE*)AddStruct(Textures, nTextures, sizeof(TEXTURE));
			nTex = nTextures;
			nTextures++;
			DXAttempt(tSurf->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&pTex));
			Textures[nTex].tex = pTex;
			Textures[nTex].surface = tSurf;

			if (i < (BTPages >> 1))
			{
				Textures[nTex].width = App.TextureSize;
				Textures[nTex].height = App.TextureSize;
			}
			else
			{
				Textures[nTex].width = App.BumpMapSize;
				Textures[nTex].height = App.BumpMapSize;
			}

			Textures[nTex].bump = 1;
			Textures[nTex].bumptpage = nTex + (BTPages >> 1);
		}

		FREE(TextureData);
	}

	FREE(pData);

	READ(&size, 1, 4, level_fp);
	READ(&compressedSize, 1, 4, level_fp);
	CompressedData = (char*)MALLOC(compressedSize);
	FileData = (char*)MALLOC(size);
	READ(CompressedData, compressedSize, 1, level_fp);
	Decompress(FileData, CompressedData, compressedSize, size);
	FREE(CompressedData);

	pData = FileData;
	TextureData = (uchar*)MALLOC(0x40000);

	if (!gfCurrentLevel)	//main menu logo
	{
		pComp = 0;
		CompressedData = 0;

		if (Gameflow->Language == US)
			size = LoadFile("data\\uslogo.pak", &CompressedData);
		else if (Gameflow->Language == GERMAN)
			size = LoadFile("data\\grlogo.pak", &CompressedData);
		else if (Gameflow->Language == FRENCH)
			size = LoadFile("data\\frlogo.pak", &CompressedData);
		else
			size = LoadFile("data\\uklogo.pak", &CompressedData);

		pComp = (char*)MALLOC(*(long*)CompressedData);
		Decompress(pComp, CompressedData + 4, size - 4, *(long*)CompressedData);
		FREE(CompressedData);

		for (int i = 0; i < 2; i++)
		{
			s = pComp + (i * 768);
			d = (long*)TextureData;

			for (int y = 0; y < 256; y++)
			{
				for (int x = 0; x < 256; x++)
				{
					r = *(s + (x * 3) + (y * 1536));
					g = *(s + (x * 3) + (y * 1536) + 1);
					b = *(s + (x * 3) + (y * 1536) + 2);
					a = -1;

					if (!r && !b && !g)
						a = 0;

					c = RGBA(r, g, b, a);
					*d++ = c;
				}
			}

			Textures = (TEXTURE*)AddStruct(Textures, nTextures, sizeof(TEXTURE));
			nTex = nTextures;
			nTextures++;
			tSurf = CreateTexturePage(256, 256, 0, (long*)TextureData, 0, 0);
			DXAttempt(tSurf->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&pTex));
			Textures[nTex].tex = pTex;
			Textures[nTex].surface = tSurf;
			Textures[nTex].width = 256;
			Textures[nTex].height = 256;
			Textures[nTex].bump = 0;
		}

		FREE(pComp);
	}

	//font
	memcpy(TextureData, FileData, 0x40000);
	FileData += 0x40000;

	Textures = (TEXTURE*)AddStruct(Textures, nTextures, sizeof(TEXTURE));
	nTex = nTextures;
	nTextures++;
	tSurf = CreateTexturePage(256, 256, 0, (long*)TextureData, 0, 0);
	DXAttempt(tSurf->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&pTex));
	Textures[nTex].tex = pTex;
	Textures[nTex].surface = tSurf;
	Textures[nTex].width = 256;
	Textures[nTex].height = 256;
	Textures[nTex].bump = 0;

	//sky
	memcpy(TextureData, FileData, 0x40000);
	FileData += 0x40000;

	Textures = (TEXTURE*)AddStruct(Textures, nTextures, sizeof(TEXTURE));
	nTex = nTextures;
	nTextures++;
	tSurf = CreateTexturePage(256, 256, 0, (long*)TextureData, 0, 0);
	DXAttempt(tSurf->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&pTex));
	Textures[nTex].tex = pTex;
	Textures[nTex].surface = tSurf;
	Textures[nTex].width = 256;
	Textures[nTex].height = 256;
	Textures[nTex].bump = 0;

	FREE(TextureData);
	FREE(pData);
	return 1;
}

bool LoadRooms()
{
	ROOM_INFO* r;
	long size, nDoors;

	Log(2, "LoadRooms");
	wibble = 0;
	MaxRoomLights = 0;
	NumLevelFogBulbs = 0;
	FileData += sizeof(long);
	number_rooms = *(short*)FileData;
	FileData += sizeof(short);
	Log(7, "Number Of Rooms %d", number_rooms);

	if (number_rooms < 0 || number_rooms > 1024)
	{
		Log(1, "Incorrect Number Of Rooms");
		return 0;
	}

	room = (ROOM_INFO*)game_malloc(number_rooms * sizeof(ROOM_INFO));

	if (!room)
		return 0;

	for (int i = 0; i < number_rooms; i++)
	{
		r = &room[i];

		r->x = *(long*)FileData;
		FileData += sizeof(long);

		r->y = 0;

		r->z = *(long*)FileData;
		FileData += sizeof(long);

		r->minfloor = *(long*)FileData;
		FileData += sizeof(long);

		r->maxceiling = *(long*)FileData;
		FileData += sizeof(long);

		size = *(long*)FileData;
		FileData += sizeof(long);
		r->data = (short*)game_malloc(size * sizeof(short));
		memcpy(r->data, FileData, size * sizeof(short));
		FileData += size * sizeof(short);

		nDoors = *(short*)FileData;
		FileData += sizeof(short);

		if (nDoors)
		{
			r->door = (short*)game_malloc((16 * nDoors + 1) * sizeof(short));
			r->door[0] = (short)nDoors;
			memcpy(r->door + 1, FileData, 16 * nDoors * sizeof(short));
			FileData += 16 * nDoors * sizeof(short);
		}
		else
			r->door = 0;

		r->x_size = *(short*)FileData;
		FileData += sizeof(short);

		r->y_size = *(short*)FileData;
		FileData += sizeof(short);

		size = r->x_size * r->y_size * sizeof(FLOOR_INFO);
		r->floor = (FLOOR_INFO*)game_malloc(size);
		memcpy(r->floor, FileData, size);
		FileData += size;

		r->ambient = *(long*)FileData;
		FileData += sizeof(long);

		r->num_lights = *(short*)FileData;
		FileData += sizeof(short);

		if (r->num_lights)
		{
			size = sizeof(LIGHTINFO) * r->num_lights;
			r->light = (LIGHTINFO*)game_malloc(size);
			memcpy(r->light, FileData, size);
			FileData += size;
		}
		else
			r->light = 0;

		r->num_meshes = *(short*)FileData;
		FileData += sizeof(short);

		if (r->num_meshes)
		{
			size = sizeof(MESH_INFO) * r->num_meshes;
			r->mesh = (MESH_INFO*)game_malloc(size);
			memcpy(r->mesh, FileData, size);
			FileData += size;

			for (int j = 0; j < r->num_meshes; j++)
				r->mesh[j].Flags = 1;
		}
		else
			r->mesh = 0;

		r->flipped_room = *(short*)FileData;
		FileData += sizeof(short);

		r->flags = *(short*)FileData;
		FileData += sizeof(short);

		r->MeshEffect = *(char*)FileData;
		FileData += sizeof(char);

		r->ReverbType = *(char*)FileData;
		FileData += sizeof(char);

		r->FlipNumber = *(char*)FileData;
		FileData += sizeof(char);

		r->left = 0x7FFF;
		r->top = 0x7FFF;
		r->bound_active = 0;
		r->right = 0;
		r->bottom = 0;
		r->item_number = NO_ITEM;
		r->fx_number = NO_ITEM;
		ProcessRoomData(r);
	}

	BuildOutsideTable();
	size = *(long*)FileData;
	FileData += sizeof(long);
	floor_data = (short*)game_malloc(2 * size);
	memcpy(floor_data, FileData, 2 * size);
	FileData += sizeof(short) * size;
	Log(0, "Floor Data Size %d @ %x", size, floor_data);
	return 1;
}

bool LoadObjects()
{
	OBJECT_INFO* obj;
	STATIC_INFO* stat;
	short** mesh;
	short** mesh_size;
	long size, num, slot;
	static long num_meshes, num_anims;

	Log(2, "LoadObjects");
	memset(objects, 0, sizeof(objects));
	memset(static_objects, 0, sizeof(static_objects));

	size = *(long*)FileData;
	FileData += sizeof(long);
	mesh_base = (short*)game_malloc(size * sizeof(short));
	memcpy(mesh_base, FileData, size * sizeof(short));
	FileData += size * sizeof(short);

	size = *(long*)FileData;
	FileData += sizeof(long);
	meshes = (short**)game_malloc(2 * size * sizeof(short*));
	memcpy(meshes, FileData, size * sizeof(short*));
	FileData += size * sizeof(short*);

	for (int i=0;i<size;i++)
		meshes[i] = mesh_base + (long)meshes[i] / 2;

	num_meshes = size;

	num_anims = *(long*)FileData;
	FileData += sizeof(long);
	anims = (ANIM_STRUCT*)game_malloc(sizeof(ANIM_STRUCT) * num_anims);
	memcpy(anims, FileData, sizeof(ANIM_STRUCT) * num_anims);
	FileData += sizeof(ANIM_STRUCT) * num_anims;

	size = *(long*)FileData;
	FileData += sizeof(long);
	changes = (CHANGE_STRUCT*)game_malloc(sizeof(CHANGE_STRUCT) * size);
	memcpy(changes, FileData, sizeof(CHANGE_STRUCT) * size);
	FileData += sizeof(CHANGE_STRUCT) * size;

	size = *(long*)FileData;
	FileData += sizeof(long);
	ranges = (RANGE_STRUCT*)game_malloc(sizeof(RANGE_STRUCT) * size);
	memcpy(ranges, FileData, sizeof(RANGE_STRUCT) * size);
	FileData += sizeof(RANGE_STRUCT) * size;

	size = *(long*)FileData;
	FileData += sizeof(long);
	commands = (short*)game_malloc(sizeof(short) * size);
	memcpy(commands, FileData, sizeof(short) * size);
	FileData += sizeof(short) * size;

	size = *(long*)FileData;
	FileData += sizeof(long);
	bones = (long*)game_malloc(sizeof(long) * size);
	memcpy(bones, FileData, sizeof(long) * size);
	FileData += sizeof(long) * size;

	size = *(long*)FileData;
	FileData += sizeof(long);
	frames = (short*)game_malloc(sizeof(short) * size);
	memcpy(frames, FileData, sizeof(short) * size);
	FileData += sizeof(short) * size;

	for (int i = 0; i < num_anims; i++)
		anims[i].frame_ptr = (short*)((long)anims[i].frame_ptr + (long)frames);

	num = *(long*)FileData;
	FileData += sizeof(long);

	for (int i = 0; i < num; i++)
	{
		slot = *(long*)FileData;
		FileData += sizeof(long);
		obj = &objects[slot];

		obj->nmeshes = *(short*)FileData;
		FileData += sizeof(short);

		obj->mesh_index = *(short*)FileData;
		FileData += sizeof(short);

		obj->bone_index = *(long*)FileData;
		FileData += sizeof(long);

		obj->frame_base = (short*)(*(short**)FileData);
		FileData += sizeof(short*);

		obj->anim_index = *(short*)FileData;
		FileData += sizeof(short);

		obj->loaded = 1;
	}

	CreateSkinningData();

	for (int i = 0; i < NUMBER_OBJECTS; i++)
	{
		obj = &objects[i];
		obj->mesh_index *= 2;
	}

	mesh = meshes;
	mesh_size = &meshes[num_meshes];
	memcpy(mesh_size, mesh, num_meshes * 4);

	for (int i = 0; i < num_meshes; i++)
	{
		*mesh++ = *mesh_size;
		*mesh++ = *mesh_size;
		mesh_size++;
	}

	InitialiseObjects();

	num = *(long*)FileData;	//statics
	FileData += sizeof(long);

	for (int i = 0; i < num; i++)
	{
		slot = *(long*)FileData;
		FileData += sizeof(long);
		stat = &static_objects[slot];

		stat->mesh_number = *(short*)FileData;
		FileData += sizeof(short);

		memcpy(&stat->x_minp, FileData, 6 * sizeof(short));
		FileData += 6 * sizeof(short);

		memcpy(&stat->x_minc, FileData, 6 * sizeof(short));
		FileData += 6 * sizeof(short);

		stat->flags = *(short*)FileData;
		FileData += sizeof(short);
	}

	for (int i = 0; i < NUMBER_STATIC_OBJECTS; i++)
	{
		stat = &static_objects[i];
		stat->mesh_number *= 2;
	}

	ProcessMeshData(num_meshes * 2);
	return 1;
}

bool LoadSprites()
{
	STATIC_INFO* stat;
	OBJECT_INFO* obj;
	SPRITESTRUCT* sptr;
	PHDSPRITESTRUCT sprite;
	long num_sprites, num_slots, slot;

	Log(2, "LoadSprites");
	FileData += 3;
	num_sprites = *(long*)FileData;
	FileData += sizeof(long);
	spriteinfo = (SPRITESTRUCT*)game_malloc(sizeof(SPRITESTRUCT) * num_sprites);

	for (int i = 0; i < num_sprites; i++)
	{
		sptr = &spriteinfo[i];
		memcpy(&sprite, FileData, sizeof(PHDSPRITESTRUCT));
		FileData += sizeof(PHDSPRITESTRUCT);
		sptr->height = sprite.height;
		sptr->offset = sprite.offset;
		sptr->tpage = sprite.tpage;
		sptr->width = sprite.width;
		sptr->x1 = float((sprite.x1) * (1.0F / 256.0F));
		sptr->y1 = float((sprite.y1) * (1.0F / 256.0F));
		sptr->x2 = float((sprite.x2) * (1.0F / 256.0F));
		sptr->y2 = float((sprite.y2) * (1.0F / 256.0F));
		sptr->x1 += (1.0F / 256.0F);
		sptr->y1 += (1.0F / 256.0F);
		sptr->x2 -= (1.0F / 256.0F);
		sptr->y2 -= (1.0F / 256.0F);
		sptr->tpage++;
	}

	num_slots = *(long*)FileData;
	FileData += sizeof(long);

	if (num_slots <= 0)
		return 1;

	for (int i = 0; i < num_slots; i++)
	{
		slot = *(long*)FileData;
		FileData += sizeof(long);

		if (slot >= NUMBER_OBJECTS)
		{
			slot -= NUMBER_OBJECTS;
			stat = &static_objects[slot];
			stat->mesh_number = *(short*)FileData;
			FileData += sizeof(short);
			stat->mesh_number = *(short*)FileData;	//what the hell
			FileData += sizeof(short);
		}
		else
		{
			obj = &objects[slot];
			obj->nmeshes = *(short*)FileData;
			FileData += sizeof(short);
			obj->mesh_index = *(short*)FileData;
			FileData += sizeof(short);
			obj->loaded = 1;
		}
	}

	return 1;
}

bool LoadCameras()
{
	Log(2, "LoadCameras");
	number_cameras = *(long*)FileData;
	FileData += sizeof(long);

	if (number_cameras)
	{
		camera.fixed = (OBJECT_VECTOR*)game_malloc(number_cameras * sizeof(OBJECT_VECTOR));
		memcpy(camera.fixed, FileData, number_cameras * sizeof(OBJECT_VECTOR));
		FileData += number_cameras * sizeof(OBJECT_VECTOR);
	}

	number_spotcams = *(short*)FileData;
	FileData += sizeof(long);				//<<---- look at me

	if (number_spotcams)
	{
		memcpy(SpotCam, FileData, number_spotcams * sizeof(SPOTCAM));
		FileData += number_spotcams * sizeof(SPOTCAM);
	}

	return 1;
}

void inject_file(bool replace)
{
	INJECT(0x00476470, LoadLevel, 0);

	INJECT(0x004768C0, S_LoadLevelFile, replace);
	INJECT(0x00476790, FreeLevel, 0);
	INJECT(0x00473C10, FindCDDrive, replace);
	INJECT(0x00473CE0, FileOpen, replace);
	INJECT(0x00473D80, FileClose, replace);
	INJECT(0x00473DA0, FileSize, replace);
	INJECT(0x00473DD0, LoadFile, 0);
	INJECT(0x00473F20, LoadTextures, replace);
	INJECT(0x004749C0, LoadRooms, replace);
	INJECT(0x00474E10, LoadObjects, replace);
	INJECT(0x00475730, LoadSprites, replace);
	INJECT(0x00475970, LoadCameras, replace);
}
