#include "../tomb4/pch.h"
#include "../tomb4/libs/zlib/zlib.h"
#include "file.h"
#include "function_stubs.h"
#include "texture.h"
#include "lighting.h"
#include "dxsound.h"
#include "drawbars.h"
#include "dxshell.h"
#include "drawroom.h"
#include "../game/setup.h"
#include "../game/objects.h"
#include "../game/laraskin.h"
#include "../game/items.h"
#include "specificfx.h"
#include "../game/tomb4fx.h"
#include "audio.h"
#include "../game/spotcam.h"
#include "../game/control.h"
#include "../game/camera.h"
#include "polyinsert.h"
#include "../game/box.h"
#include "../game/draw.h"
#include "../game/effect2.h"
#include "../game/effects.h"
#include "../game/sound.h"
#include "winmain.h"
#include "../game/lara.h"
#include "output.h"
#include "../game/gameflow.h"

TEXTURESTRUCT* textinfo;
SPRITESTRUCT* spriteinfo;
THREAD LevelLoadingThread;

TEXTURESTRUCT* AnimatingWaterfalls[3];
long AnimatingWaterfallsV[3];

CHANGE_STRUCT* changes;
RANGE_STRUCT* ranges;
AIOBJECT* AIObjects;
short* aranges;
short* frames;
short* commands;
short* floor_data;
short* mesh_base;
long nAnimUVRanges;
long number_cameras;
short nAIObjects;

static FILE* level_fp;
static char* FileData;
static char* CompressedData;
static long num_items;

unsigned int __stdcall LoadLevel(void* name)
{
	OBJECT_INFO* obj;
	TEXTURESTRUCT* tex;
	char* pData;
	long version, size, compressedSize;
	short RTPages, OTPages, BTPages;

	Log(2, "LoadLevel");
	FreeLevel();
	memset(malloc_ptr, 0, MALLOC_SIZE);
	memset(&lara, 0, sizeof(LARA_INFO));

	Textures = (TEXTURE*)AddStruct(Textures, nTextures, sizeof(TEXTURE));
	nTextures = 1;
	Textures[0].tex = 0;
	Textures[0].surface = 0;
	Textures[0].width = 0;
	Textures[0].height = 0;
	Textures[0].bump = 0;

	S_InitLoadBar(20);
	S_LoadBar();

	CompressedData = 0;
	FileData = 0;
	level_fp = 0;
	level_fp = FileOpen((const char*)name);

	if (level_fp)
	{
		fread(&version, 1, 4, level_fp);
		fread(&RTPages, 1, 2, level_fp);
		fread(&OTPages, 1, 2, level_fp);
		fread(&BTPages, 1, 2, level_fp);

		Log(7, "Process Level Data");
		LoadTextures(RTPages, OTPages, BTPages);
		fread(&size, 1, 4, level_fp);
		fread(&compressedSize, 1, 4, level_fp);
		CompressedData = (char*)malloc(compressedSize);
		FileData = (char*)malloc(size);
		fread(CompressedData, compressedSize, 1u, level_fp);
		Decompress(FileData, CompressedData, compressedSize, size);
		free(CompressedData);

		pData = FileData;
		S_LoadBar();

		LoadRooms();
		S_LoadBar();

		LoadObjects();
		S_LoadBar();

		LoadSprites();
		S_LoadBar();

		LoadCameras();
		S_LoadBar();

		LoadSoundEffects();
		S_LoadBar();

		LoadBoxes();
		S_LoadBar();

		LoadAnimatedTextures();
		S_LoadBar();

		LoadTextureInfos();
		S_LoadBar();

		LoadItems();
		S_LoadBar();

		LoadAIInfo();
		S_LoadBar();

		LoadCinematic();
		S_LoadBar();

		if (acm_ready && !App.SoundDisabled)
			LoadSamples();

		free(pData);
		S_LoadBar();

		for (int i = 0; i < 3; i++)
		{
			obj = &objects[WATERFALL1 + i];

			if (obj->loaded)
			{
				tex = &textinfo[mesh_vtxbuf[obj->mesh_index]->gt4[4] & 0x7FFF];
				AnimatingWaterfalls[i] = tex;
				AnimatingWaterfallsV[i] = (long)tex->v1;
			}
		}

		S_LoadBar();
		S_GetUVRotateTextures();

		InitTarget_2();
		S_LoadBar();

		SetupGame();
		S_LoadBar();

		SetFadeClip(0, 1);
		reset_cutseq_vars();
		FileClose(level_fp);
	}

	LevelLoadingThread.active = 0;
	_endthreadex(1);
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
	static char cd_drive;

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
				file = CreateFile(path, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

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
	strcat(path_name, name);
	Log(5, "FileOpen - %s", path_name);
	file = fopen(path_name, "rb");

	if (!file)
		Log(1, "Unable To Open %s", path_name);

	return file;
}

void FileClose(FILE* file)
{
	Log(2, "FileClose");
	fclose(file);
}

long FileSize(FILE* file)
{
	long size;

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);
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

	count = fread(*dest, 1, size, file);
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
	LPDIRECTDRAWSURFACEX tSurf;
	LPDIRECT3DTEXTUREX pTex;
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
		fread(&size, 1, 4, level_fp);
		fread(&compressedSize, 1, 4, level_fp);

		CompressedData = (char*)malloc(compressedSize);
		FileData = (char*)malloc(size);

		fread(CompressedData, compressedSize, 1, level_fp);
		Decompress(FileData, CompressedData, compressedSize, size);

		fread(&size, 1, 4, level_fp);
		fread(&compressedSize, 1, 4, level_fp);
		fseek(level_fp, compressedSize, SEEK_CUR);
		free(CompressedData);
	}
	else
	{
		fread(&size, 1, 4, level_fp);
		fread(&compressedSize, 1, 4, level_fp);
		fseek(level_fp, compressedSize, SEEK_CUR);

		fread(&size, 1, 4, level_fp);
		fread(&compressedSize, 1, 4, level_fp);

		CompressedData = (char*)malloc(compressedSize);
		FileData = (char*)malloc(size);
		fread(CompressedData, compressedSize, 1, level_fp);
		Decompress(FileData, CompressedData, compressedSize, size);
		free(CompressedData);
	}

	pData = FileData;

	Log(5, "RTPages %d", RTPages);
	size = RTPages * skip * 0x10000;
	TextureData = (uchar*)malloc(size);
	memcpy(TextureData, FileData, size);
	FileData += size;
	S_LoadBar();

	for (int i = 0; i < RTPages; i++)
	{
		Textures = (TEXTURE*)AddStruct(Textures, nTextures, sizeof(TEXTURE));
		nTex = nTextures;
		nTextures++;
		tSurf = CreateTexturePage(App.TextureSize, App.TextureSize, 0, (long*)(TextureData + (i * skip * 0x10000)), 0, format);
		DXAttempt(tSurf->QueryInterface(TEXGUID, (LPVOID*)&pTex));
		Textures[nTex].tex = pTex;
		Textures[nTex].surface = tSurf;
		Textures[nTex].width = App.TextureSize;
		Textures[nTex].height = App.TextureSize;
		Textures[nTex].bump = 0;
		App.dx.lpD3DDevice->SetTexture(0, pTex);
	}

	free(TextureData);

	Log(5, "OTPages %d", OTPages);
	size = OTPages * skip * 0x10000;
	TextureData = (uchar*)malloc(size);
	memcpy(TextureData, FileData, size);
	FileData += size;
	S_LoadBar();

	for (int i = 0; i < OTPages; i++)
	{
		Textures = (TEXTURE*)AddStruct(Textures, nTextures, sizeof(TEXTURE));
		nTex = nTextures;
		nTextures++;
		tSurf = CreateTexturePage(App.TextureSize, App.TextureSize, 0, (long*)(TextureData + (i * skip * 0x10000)), 0, format);
		DXAttempt(tSurf->QueryInterface(TEXGUID, (LPVOID*)&pTex));
		Textures[nTex].tex = pTex;
		Textures[nTex].surface = tSurf;
		Textures[nTex].width = App.TextureSize;
		Textures[nTex].height = App.TextureSize;
		Textures[nTex].bump = 0;
		App.dx.lpD3DDevice->SetTexture(0, pTex);
	}

	free(TextureData);
	S_LoadBar();

	Log(5, "BTPages %d", BTPages);

	if (BTPages)
	{
		size = BTPages * skip * 0x10000;
		TextureData = (uchar*)malloc(size);
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
			DXAttempt(tSurf->QueryInterface(TEXGUID, (LPVOID*)&pTex));
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

		free(TextureData);
	}

	free(pData);

	fread(&size, 1, 4, level_fp);
	fread(&compressedSize, 1, 4, level_fp);
	CompressedData = (char*)malloc(compressedSize);
	FileData = (char*)malloc(size);
	fread(CompressedData, compressedSize, 1, level_fp);
	Decompress(FileData, CompressedData, compressedSize, size);
	free(CompressedData);

	pData = FileData;
	TextureData = (uchar*)malloc(0x40000);

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

		pComp = (char*)malloc(*(long*)CompressedData);
		Decompress(pComp, CompressedData + 4, size - 4, *(long*)CompressedData);
		free(CompressedData);

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
			DXAttempt(tSurf->QueryInterface(TEXGUID, (LPVOID*)&pTex));
			Textures[nTex].tex = pTex;
			Textures[nTex].surface = tSurf;
			Textures[nTex].width = 256;
			Textures[nTex].height = 256;
			Textures[nTex].bump = 0;
		}

		free(pComp);
	}

	//font
	memcpy(TextureData, FileData, 0x40000);
	FileData += 0x40000;

	Textures = (TEXTURE*)AddStruct(Textures, nTextures, sizeof(TEXTURE));
	nTex = nTextures;
	nTextures++;
	tSurf = CreateTexturePage(256, 256, 0, (long*)TextureData, 0, 0);
	DXAttempt(tSurf->QueryInterface(TEXGUID, (LPVOID*)&pTex));
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
	DXAttempt(tSurf->QueryInterface(TEXGUID, (LPVOID*)&pTex));
	Textures[nTex].tex = pTex;
	Textures[nTex].surface = tSurf;
	Textures[nTex].width = 256;
	Textures[nTex].height = 256;
	Textures[nTex].bump = 0;

	free(TextureData);
	free(pData);
	return 1;
}

bool LoadRooms()
{
	ROOM_INFO* r;
	long size, nDoors;

	Log(2, "LoadRooms");
	wibble = 0;
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
	memset(static_objects, 0, sizeof(STATIC_INFO) * NUMBER_STATIC_OBJECTS);

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
			stat->mesh_number = *(short*)FileData;
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

bool LoadSoundEffects()
{
	Log(2, "LoadSoundEffects");
	number_sound_effects = *(long*)FileData;
	FileData += sizeof(long);
	Log(8, "Number of SFX %d", number_sound_effects);

	if (number_sound_effects)
	{
		sound_effects = (OBJECT_VECTOR*)game_malloc(number_sound_effects * sizeof(OBJECT_VECTOR));
		memcpy(sound_effects, FileData, number_sound_effects * sizeof(OBJECT_VECTOR));
		FileData += number_sound_effects * sizeof(OBJECT_VECTOR);
	}

	return 1;
}

bool LoadBoxes()
{
	BOX_INFO* box;
	long size;

	Log(2, "LoadBoxes");
	num_boxes = *(long*)FileData;
	FileData += sizeof(long);

	boxes = (BOX_INFO*)game_malloc(sizeof(BOX_INFO) * num_boxes);
	memcpy(boxes, FileData, sizeof(BOX_INFO) * num_boxes);
	FileData += sizeof(BOX_INFO) * num_boxes;

	size = *(long*)FileData;
	FileData += sizeof(long);
	overlap = (ushort*)game_malloc(sizeof(ushort) * size);
	memcpy(overlap, FileData, sizeof(ushort) * size);
	FileData += sizeof(ushort) * size;

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			ground_zone[j][i] = (short*)game_malloc(sizeof(short) * num_boxes);
			memcpy(ground_zone[j][i], FileData, sizeof(short) * num_boxes);
			FileData += sizeof(short) * num_boxes;
		}

		ground_zone[4][i] = (short*)game_malloc(sizeof(short) * num_boxes);
		memcpy(ground_zone[4][i], FileData, sizeof(short) * num_boxes);
		FileData += sizeof(short) * num_boxes;
	}

	for (int i = 0; i < num_boxes; i++)
	{
		box = &boxes[i];

		if (box->overlap_index & 0x8000)
			box->overlap_index |= 0x4000;
		else if (gfLevelFlags & GF_TRAIN && box->height > -256)
			box->overlap_index |= 0xC000;
	}

	return 1;
}

bool LoadAnimatedTextures()
{
	long num_anim_ranges;

	num_anim_ranges = *(long*)FileData;
	FileData += sizeof(long);
	aranges = (short*)game_malloc(num_anim_ranges * 2);
	memcpy(aranges, FileData, num_anim_ranges * 2);
	FileData += num_anim_ranges * sizeof(short);
	nAnimUVRanges = *(char*)FileData;
	FileData += sizeof(char);
	return 1;
}

bool LoadTextureInfos()
{
	TEXTURESTRUCT* t;
	PHDTEXTURESTRUCT tex;
	long val;

	Log(2, "LoadTextureInfos");
	FileData += 3;

	val = *(long*)FileData;
	FileData += sizeof(long);
	Log(5, "Texture Infos : %d", val);
	textinfo = (TEXTURESTRUCT*)game_malloc(val * sizeof(TEXTURESTRUCT));

	for (int i = 0; i < val; i++)
	{
		t = &textinfo[i];
		memcpy(&tex, FileData, sizeof(PHDTEXTURESTRUCT));
		FileData += sizeof(PHDTEXTURESTRUCT);
		t->drawtype = tex.drawtype;
		t->tpage = tex.tpage & 0x7FFF;
		t->flag = tex.tpage ^ (tex.tpage ^ tex.flag) & 0x7FFF;
		t->u1 = float(tex.u1) * (1.0F / 65535.0F);
		t->v1 = float(tex.v1) * (1.0f / 65535.0F);
		t->u2 = float(tex.u2) * (1.0f / 65535.0F);
		t->v2 = float(tex.v2) * (1.0f / 65535.0F);
		t->u3 = float(tex.u3) * (1.0f / 65535.0F);
		t->v3 = float(tex.v3) * (1.0f / 65535.0F);
		t->u4 = float(tex.u4) * (1.0f / 65535.0F);
		t->v4 = float(tex.v4) * (1.0f / 65535.0F);
	}

	AdjustUV(val);
	Log(5, "Created %d Texture Pages", nTextures - 1);
	return 1;
}

bool LoadItems()
{
	ITEM_INFO* item;
	ROOM_INFO* r;
	FLOOR_INFO* floor;
	STATIC_INFO* stat;
	long x, y, z;

	Log(2, "LoadItems");
	num_items = *(long*)FileData;
	FileData += 4;

	if (!num_items)
		return 1;

	items = (ITEM_INFO*)game_malloc(256 * sizeof(ITEM_INFO));
	level_items = num_items;
	InitialiseItemArray(256);

	for (int i = 0; i < num_items; i++)
	{
		item = &items[i];

		item->object_number = *(short*)FileData;
		FileData += sizeof(short);

		item->room_number = *(short*)FileData;
		FileData += sizeof(short);

		item->pos.x_pos = *(long*)FileData;
		FileData += sizeof(long);

		item->pos.y_pos = *(long*)FileData;
		FileData += sizeof(long);

		item->pos.z_pos = *(long*)FileData;
		FileData += sizeof(long);

		item->pos.y_rot = *(short*)FileData;
		FileData += sizeof(short);

		item->shade = *(short*)FileData;
		FileData += sizeof(short);

		item->trigger_flags = *(short*)FileData;
		FileData += sizeof(short);

		item->flags = *(short*)FileData;
		FileData += sizeof(short);
	}

	for (int i = 0; i < num_items; i++)
		InitialiseItem(i);

	for (int i = 0; i < number_rooms; i++)
	{
		r = &room[i];

		for (int j = 0; j < r->num_meshes; j++)
		{
			x = (r->mesh[j].x - r->x) >> 10;
			z = (r->mesh[j].z - r->z) >> 10;

			floor = &(r->floor[x * r->x_size + z]);

			if (!(boxes[floor->box].overlap_index & 0x4000) && (gfCurrentLevel != 4 || i != 19 && i != 23 && i != 16))
			{
				stat = &static_objects[r->mesh[j].static_number];
				y = floor->floor << 8;

				if (y <= (r->mesh[j].y - stat->y_maxc + 512) && y < r->mesh[j].y - stat->y_minc)
				{
					if (!stat->x_maxc || !stat->x_minc || !stat->z_maxc || !stat->z_minc ||
						(stat->x_maxc ^ stat->x_minc) & 0x8000 && (stat->z_maxc ^ stat->z_minc) & 0x8000)
					{
						x = (r->mesh[j].x - r->x) >> 10;
						z = (r->mesh[j].z - r->z) >> 10;
						r->floor[x * r->x_size + z].stopper = 1;
					}
				}
			}
		}
	}

	return 1;
}

bool LoadCinematic()
{
	FileData += sizeof(short);
	return 1;
}

bool LoadAIInfo()
{
	long num_ai;

	num_ai = *(long*)FileData;
	FileData += sizeof(long);

	if (num_ai)
	{
		nAIObjects = (short)num_ai;
		AIObjects = (AIOBJECT*)game_malloc(sizeof(AIOBJECT) * num_ai);
		memcpy(AIObjects, FileData, sizeof(AIOBJECT) * num_ai);
		FileData += sizeof(AIOBJECT) * num_ai;
	}

	return 1;
}

bool LoadSamples()
{
	long num_samples, uncomp_size, comp_size;
	static long num_sample_infos;

	Log(2, "LoadSamples");
	sample_lut = (short*)game_malloc(MAX_SAMPLES * sizeof(short));
	memcpy(sample_lut, FileData, MAX_SAMPLES * sizeof(short));
	FileData += MAX_SAMPLES * sizeof(short);
	num_sample_infos = *(long*)FileData;
	FileData += sizeof(long);
	Log(8, "Number Of Sample Infos %d", num_sample_infos);

	if (!num_sample_infos)
	{
		Log(1, "No Sample Infos");
		return 0;
	}

	sample_infos = (SAMPLE_INFO*)game_malloc(sizeof(SAMPLE_INFO) * num_sample_infos);
	memcpy(sample_infos, FileData, sizeof(SAMPLE_INFO) * num_sample_infos);
	FileData += sizeof(SAMPLE_INFO) * num_sample_infos;
	num_samples = *(long*)FileData;
	FileData += sizeof(long);

	if (!num_samples)
	{
		Log(1, "No Samples");
		return 0;
	}

	Log(8, "Number Of Samples %d", num_samples);
	fread(&num_samples, 1, 4, level_fp);
	InitSampleDecompress();

	if (num_samples <= 0)
	{
		FreeSampleDecompress();
		return 1;
	}

	for (int i = 0; i < num_samples; i++)
	{
		fread(&uncomp_size, 1, 4, level_fp);
		fread(&comp_size, 1, 4, level_fp);
		fread(samples_buffer, comp_size, 1, level_fp);

		if (!DXCreateSampleADPCM(samples_buffer, comp_size, uncomp_size, i))
		{
			FreeSampleDecompress();
			return 0;
		}
	}

	FreeSampleDecompress();
	return 1;
}

void S_GetUVRotateTextures()
{
	TEXTURESTRUCT* tex;
	short* pRange;

	pRange = aranges + 1;

	for (int i = 0; i < nAnimUVRanges; i++, pRange++)
	{
		for (int j = (int)*(pRange++); j >= 0; j--, pRange++)
		{
			tex = &textinfo[*pRange];
			AnimatingTexturesV[i][j][0] = tex->v1;
		}

		pRange--;
	}
}

void AdjustUV(long num)
{
	TEXTURESTRUCT* tex;
	float u, v;
	ushort type;

	Log(2, "AdjustUV");

	for (int i = 0; i < num; i++)
	{
		tex = &textinfo[i];
		Textures[tex->tpage].tpage++;
		tex->tpage++;
		u = 1.0F / float(Textures[tex->tpage].width << 1);
		v = 1.0F / float(Textures[tex->tpage].height << 1);
		type = tex->flag & 7;

		if (tex->flag & 0x8000)
		{
			switch (type)
			{
			case 0:
				tex->u1 += u;
				tex->v1 += v;
				tex->u2 -= u;
				tex->v2 += v;
				tex->u3 += u;
				tex->v3 -= v;
				break;

			case 1:
				tex->u1 -= u;
				tex->v1 += v;
				tex->u2 -= u;
				tex->v2 -= v;
				tex->u3 += u;
				tex->v3 += v;
				break;

			case 2:
				tex->u1 -= u;
				tex->v1 -= v;
				tex->u2 += u;
				tex->v2 -= v;
				tex->u3 -= u;
				tex->v3 += v;
				break;

			case 3:
				tex->u1 += u;
				tex->v1 -= v;
				tex->u2 += u;
				tex->v2 += v;
				tex->u3 -= u;
				tex->v3 -= v;
				break;

			case 4:
				tex->u1 -= u;
				tex->v1 += v;
				tex->u2 += u;
				tex->v2 += v;
				tex->u3 -= u;
				tex->v3 -= v;
				break;

			case 5:
				tex->u1 += u;
				tex->v1 += v;
				tex->u2 += u;
				tex->v2 -= v;
				tex->u3 -= u;
				tex->v3 += v;
				break;

			case 6:
				tex->u1 += u;
				tex->v1 -= v;
				tex->u2 -= u;
				tex->v2 -= v;
				tex->u3 += u;
				tex->v3 += v;
				break;

			case 7:
				tex->u1 -= u;
				tex->v1 -= v;
				tex->u2 -= u;
				tex->v2 += v;
				tex->u3 += u;
				tex->v3 -= v;
				break;

			default:
				Log(1, "TextureInfo Type %d Not Found", type);
				break;
			}
		}
		else
		{
			switch (type)
			{
			case 0:
				tex->u1 += u;
				tex->v1 += v;
				tex->u2 -= u;
				tex->v2 += v;
				tex->u3 -= u;
				tex->v3 -= v;
				tex->u4 += u;
				tex->v4 -= v;
				break;

			case 1:
				tex->u1 -= u;
				tex->v1 += v;
				tex->u2 += u;
				tex->v2 += v;
				tex->u3 += u;
				tex->v3 -= v;
				tex->u4 -= u;
				tex->v4 -= v;
				break;

			default:
				Log(1, "TextureInfo Type %d Not Found", type);
				break;
			}
		}
	}
}

bool Decompress(char* pDest, char* pCompressed, long compressedSize, long size)
{
	z_stream stream;

	Log(2, "Decompress");
	memset(&stream, 0, sizeof(z_stream));
	stream.avail_in = compressedSize;
	stream.avail_out = size;
	stream.next_out = (Bytef*)pDest;
	stream.next_in = (Bytef*)pCompressed;
	inflateInit(&stream);
	inflate(&stream, Z_FINISH);

	if (stream.total_out != size)
	{
		Log(1, "Error Decompressing Data");
		return 0;
	}

	inflateEnd(&stream);
	Log(5, "Decompression OK");
	return 1;
}
