#pragma once
#include "types.h"
#include "math_tbls.h"

#define VAR_U_(address, type)			(*(type*)(address))
#define ARRAY_(address, type, length)	(*(type(*)length)(address))

#define gfCurrentLevel	VAR_U_(0x007FD290, char)
#define gfLevelComplete	VAR_U_(0x007FD2B0, char)
#define Gameflow	VAR_U_(0x007FD278, GAMEFLOW*)
#define gfGameMode	VAR_U_(0x004AF451, char)
#define gfLevelFlags	VAR_U_(0x007FD260, short)
#define GameTimer	VAR_U_(0x007FD258, ulong)
#define meshes	VAR_U_(0x00533A80, short**)
#define bones	VAR_U_(0x00533A88, long*)
#define anims	VAR_U_(0x00533A68, ANIM_STRUCT*)
#define room	VAR_U_(0x00533A64, ROOM_INFO*)
#define level_items	VAR_U_(0x004BF68C, long)
#define aranges	VAR_U_(0x00753BDC, short*)
#define gfUVRotate	VAR_U_(0x007FD2E9, char)
#define gfStringOffset	VAR_U_(0x007FD274, ushort*)
#define gfStringWad	VAR_U_(0x007FD2B8, char*)
#define gfMirrorRoom	VAR_U_(0x007FD250, uchar)
#define gfMirrorZPlane	VAR_U_(0x007FDF64, long)
#define bDoCredits	VAR_U_(0x004BF669, char)
#define num_fmvs	VAR_U_(0x004BF65D, char)
#define gfScriptOffset	VAR_U_(0x004BF64C, ushort*)
#define gfScriptWad	VAR_U_(0x004BF658, uchar*)
#define gfStatus	VAR_U_(0x004BF650, long)
#define gfLegendTime	VAR_U_(0x004BF668, uchar)
#define gfNumMips	VAR_U_(0x004BF66B, uchar)
#define gfFog	VAR_U_(0x004BF664, CVECTOR)
#define gfInitialiseGame	VAR_U_(0x004AF452, uchar)
#define gfResetHubDest	VAR_U_(0x007FD288, uchar)
#define skipped_level	VAR_U_(0x004BF3CE, char)
#define gfCutNumber	VAR_U_(0x004BF66A, uchar)
#define	gfLayer1Vel	VAR_U_(0x007FD252, char)
#define	gfLayer2Vel	VAR_U_(0x007FD251, char)
#define gfLayer1Col	VAR_U_(0x007FD264, CVECTOR)
#define gfLayer2Col	VAR_U_(0x007FD25C, CVECTOR)
#define gfLegend	VAR_U_(0x007FD2E8, uchar)
#define gfLensFlare	VAR_U_(0x007FD2A0, PHD_VECTOR)
#define gfLensFlareColour	VAR_U_(0x007FD2B4, CVECTOR)
#define load_cam	VAR_U_(0x00533AA8, PHD_VECTOR)
#define load_target	VAR_U_(0x00533AB4, PHD_VECTOR)
#define load_roomnum	VAR_U_(0x004B27A5, uchar)
#define gfRequiredStartPos	VAR_U_(0x007FD270, uchar)
#define nFrames	VAR_U_(0x004AF454, long)
#define DEL_playingamefmv	VAR_U_(0x004BF65C, char)
#define Chris_Menu	VAR_U_(0x004BF66C, char)
#define title_controls_locked_out	VAR_U_(0x007FD268, char)
#define gfFilenameWad	VAR_U_(0x007FD28C, char*)
#define gfFilenameOffset	VAR_U_(0x007FD2EC, ushort*)
#define mesh_base	VAR_U_(0x00533A7C, short*)
#define AIObjects	VAR_U_(0x007FD204, AIOBJECT*)
#define number_rooms	VAR_U_(0x00533A5C, short)
#define gfScriptFile	VAR_U_(0x007FD2AC, uchar*)
#define gfExtensions	VAR_U_(0x004BF654, char*)
#define gfLanguageFile	VAR_U_(0x007FD26C, uchar*)
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

#define fmv_to_play	ARRAY_(0x004BF660, char, [2])
#define gfResidentCut	ARRAY_(0x007FD254, char, [4])
#define gfMips	ARRAY_(0x007FD280, char, [8])
#define	gfLevelNames	ARRAY_(0x007FD2C0, uchar, [40])
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