#include "../tomb4/pch.h"
#include "gamemain.h"
#include "audio.h"
#include "file.h"
#include "function_stubs.h"
#include "function_table.h"
#include "3dmath.h"
#include "../game/text.h"
#include "time.h"
#include "winmain.h"
#include "../game/sound.h"
#include "../game/gameflow.h"
#ifdef DISCORD_RPC
#include "../tomb4/tomb4.h"
#endif

void GameClose()
{
	Log(2, "GameClose");
	ACMClose();
	FreeLevel();

	if (DestVB)
	{
		Log(4, "Released %s @ %x - RefCnt = %d", "Dest VB", DestVB, DestVB->Release());
		DestVB = 0;
	}
	else
		Log(1, "%s Attempt To Release NULL Ptr", "Dest VB");

	free(clipflags);

	if (wav_file_buffer)
		free(wav_file_buffer);

	if (ADPCMBuffer)
		free(ADPCMBuffer);

	free(malloc_buffer);
	free(gfScriptFile);
	free(gfLanguageFile);
}

unsigned int __stdcall GameMain(void* ptr)
{
	long fpcw;

	Log(2, "GameMain");

	if (GameInitialise())
	{
		InitialiseFunctionTable();
		HWInitialise();
		InitWindow(0, 0, App.dx.dwRenderWidth, App.dx.dwRenderHeight, 20, 20480, 80, App.dx.dwRenderWidth, App.dx.dwRenderHeight);
		InitFont();
		TIME_Init();
		App.SetupComplete = 1;
		fpcw = MungeFPCW(&FPCW);
		S_CDStop();
		ClearSurfaces();

		if (!App.SoundDisabled)
			SOUND_Init();

#ifdef DISCORD_RPC
		RPC_Init();
#endif

		DoGameflow();
		GameClose();
		S_CDStop();

#ifdef DISCORD_RPC
		RPC_close();
#endif

		if (fpcw)
			RestoreFPCW(FPCW);

		PostMessage(App.hWnd, WM_CLOSE, 0, 0);
		MainThread.active = 0;
		_endthreadex(1);
	}

	return 1;
}

void inject_gamemain(bool replace)
{
	INJECT(0x004770C0, GameClose, 0);
	INJECT(0x00476EC0, GameMain, replace);
}
