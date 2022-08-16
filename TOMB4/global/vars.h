#pragma once
#include "types.h"
#include "math_tbls.h"

#define VAR_U_(address, type)			(*(type*)(address))
#define ARRAY_(address, type, length)	(*(type(*)length)(address))

#define meshes	VAR_U_(0x00533A80, short**)
#define bones	VAR_U_(0x00533A88, long*)
#define anims	VAR_U_(0x00533A68, ANIM_STRUCT*)
#define room	VAR_U_(0x00533A64, ROOM_INFO*)
#define level_items	VAR_U_(0x004BF68C, long)
#define aranges	VAR_U_(0x00753BDC, short*)
#define mesh_base	VAR_U_(0x00533A7C, short*)
#define AIObjects	VAR_U_(0x007FD204, AIOBJECT*)
#define number_rooms	VAR_U_(0x00533A5C, short)

#define nAIObjects	VAR_U_(0x007FD200, short)
#define level_fp	VAR_U_(0x00533A74, FILE*)
#define CompressedData	VAR_U_(0x00533A58, char*)
#define FileData	VAR_U_(0x005333A4, char*)
#define floor_data	VAR_U_(0x00533A78, short*)
#define changes	VAR_U_(0x00533A6C, CHANGE_STRUCT*)
#define ranges	VAR_U_(0x00533A70, RANGE_STRUCT*)
#define commands	VAR_U_(0x00533A8C, short*)
#define frames	VAR_U_(0x00533A84, short*)
#define number_cameras	VAR_U_(0x007FE940, long)
#define aranges	VAR_U_(0x00753BDC, short*)
#define nAnimUVRanges	VAR_U_(0x0053339C, long)
#define num_items	VAR_U_(0x00532C90, long)

#define objects	ARRAY_(0x0052B850, OBJECT_INFO, [465])

/*Bink bs*/
#define BinkSurfaceType	VAR_U_(0x007E6E00, long)
#define BinkSurface	VAR_U_(0x007E6E04, LPDIRECTDRAWSURFACEX)
#define Bink	VAR_U_(0x007E6E08, BINK_STRUCT*)

#define NO_HEIGHT	-32512
#define NO_ITEM	-1
#define FVF (D3DFVF_TEX2 | D3DFVF_SPECULAR | D3DFVF_DIFFUSE | D3DFVF_XYZRHW)
#define WINDOW_STYLE	(WS_OVERLAPPED | WS_BORDER | WS_CAPTION)
#define W2V_SHIFT	14
#define MAX_SAMPLES	370
#ifdef GENERAL_FIXES
#define MAX_DYNAMICS	64
#else
#define MAX_DYNAMICS	32
#endif

#ifdef GENERAL_FIXES
#define MALLOC_SIZE	15000000	//15MB
#else
#define MALLOC_SIZE	5000000		//5MB
#endif