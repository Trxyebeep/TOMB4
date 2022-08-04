#include "../tomb4/pch.h"
#include "../game/control.h"
#include "../game/lara2gun.h"
#include "../game/laramisc.h"
#include "../game/laraswim.h"
#include "../game/collide.h"
#include "../game/lara.h"
#include "../game/lara1gun.h"
#include "../game/bike.h"
#include "../specific/LoadSave.h"
#include "../specific/output.h"
#include "../game/init.h"
#include "../game/objects.h"
#include "../game/tomb4fx.h"
#include "../specific/specificfx.h"
#include "../specific/lighting.h"
#include "../game/train.h"
#include "../game/laraflar.h"
#include "../game/camera.h"
#include "../specific/3dmath.h"
#include "../game/gameflow.h"
#include "../game/delstuff.h"
#include "../game/deltapak.h"
#include "../game/hair.h"
#include "../game/draw.h"
#include "../specific/drawroom.h"
#include "../game/box.h"
#include "../game/newinv.h"
#include "../game/footprnt.h"
#include "../game/larasurf.h"
#include "../game/laraclmb.h"
#include "../game/health.h"
#include "../game/rope.h"
#include "../specific/registry.h"
#include "../specific/polyinsert.h"
#include "../game/effect2.h"
#include "../specific/file.h"
#include "../game/ahmet.h"
#include "../game/savegame.h"
#include "../game/effects.h"
#include "../game/croc.h"
#include "../specific/function_stubs.h"
#include "../game/bat.h"
#include "../game/clockworkbeetle.h"
#include "../game/laradouble.h"
#include "../game/senet.h"
#include "../game/sphinx.h"
#include "../game/spotcam.h"
#include "../game/traps.h"
#include "../game/wraith.h"
#include "../game/wildboar.h"
#include "../game/demigod.h"
#include "../specific/function_table.h"
#include "../specific/gamemain.h"
#include "../game/elements.h"
#include "../specific/dxshell.h"
#include "../game/voncroy.h"
#include "../game/debris.h"
#include "../game/people.h"
#include "../game/jeep.h"
#include "../game/pickup.h"
#include "../game/switch.h"
#include "../game/baboon.h"
#include "../specific/input.h"
#include "../game/deathsld.h"
#include "../specific/fmv.h"
#include "../game/setup.h"
#include "../specific/d3dmatrix.h"
#include "../game/door.h"
#include "../game/jean.h"
#include "../specific/time.h"
#include "../specific/cmdline.h"
#include "../specific/winmain.h"
#include "../specific/texture.h"
#include "../specific/clipping.h"
#include "../specific/audio.h"
#include "../specific/dxsound.h"
#include "../game/text.h"
#include "../game/harpy.h"
#include "../game/seth.h"
#include "../game/moveblok.h"
#include "../game/objlight.h"
#include "../game/mirror.h"
#include "../game/troops.h"
#include "../game/mummy.h"
#include "../game/laraskin.h"
#include "../game/sas.h"
#include "../game/hammerhead.h"
#include "../game/sphere.h"
#include "../game/sound.h"
#include "../game/missile.h"
#include "../game/items.h"
#include "../game/flmtorch.h"
#include "../game/scarab.h"
#include "../game/larafire.h"
#include "../game/lot.h"
#include "../game/sentrygun.h"
#include "../game/dog.h"

HINSTANCE hinstance = NULL;

#ifndef BUILD_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

DLL_EXPORT int dummyz();

int dummyz()
{
	return 0;
}

void inject_all(bool replace)
{
	inject_control(replace);
	inject_lara2gun(replace);
	inject_laramisc(replace);
	inject_laraswim(replace);
	inject_collide(replace);
	inject_lara(replace);
	inject_lara1gun(replace);
	inject_bike(replace);
	inject_loadsave(replace);
	inject_output(replace);
	inject_init(replace);
	inject_objects(replace);
	inject_tomb4fx(replace);
	inject_specificfx(replace);
	inject_lighting(replace);
	inject_train(replace);
	inject_laraflar(replace);
	inject_camera(replace);
	inject_3dmath(replace);
	inject_gameflow(replace);
	inject_delstuff(replace);
	inject_deltapack(replace);
	inject_hair(replace);
	inject_draw(replace);
	inject_drawroom(replace);
	inject_box(replace);
	inject_newinv(replace);
	inject_footprint(replace);
	inject_larasurf(replace);
	inject_laraclmb(replace);
	inject_health(replace);
	inject_rope(replace);
	inject_registry(replace);
	inject_polyinsert(replace);
	inject_effect2(replace);
	inject_file(replace);
	inject_ahmet(replace);
	inject_savegame(replace);
	inject_effects(replace);
	inject_croc(replace);
	inject_funcstubs(replace);
	inject_bat(replace);
	inject_clockwork(replace);
	inject_laradouble(replace);
	inject_senet(replace);
	inject_sphinx(replace);
	inject_spotcam(replace);
	inject_traps(replace);
	inject_wraith(replace);
	inject_wildboar(replace);
	inject_demigod(replace);
	inject_functbl(replace);
	inject_gamemain(replace);
	inject_elements(replace);
	inject_dxshell(replace);
	inject_voncroy(replace);
	inject_debris(replace);
	inject_people(replace);
	inject_jeep(replace);
	inject_pickup(replace);
	inject_switch(replace);
	inject_baboon(replace);
	inject_input(replace);
	inject_deathslide(replace);
	inject_fmv(replace);
	inject_setup(replace);
	inject_d3dmatrix(replace);
	inject_door(replace);
	inject_jean(replace);
	inject_time(replace);
	inject_cmdline(replace);
	inject_winmain(replace);
	inject_texture(replace);
	inject_clipping(replace);
	inject_audio(replace);
	inject_dxsound(replace);
	inject_text(replace);
	inject_harpy(replace);
	inject_seth(replace);
	inject_moveblok(replace);
	inject_objlight(replace);
	inject_mirror(replace);
	inject_troops(replace);
	inject_mummy(replace);
	inject_laraskin(replace);
	inject_sas(replace);
	inject_hammerhead(replace);
	inject_sphere(replace);
	inject_sound(replace);
	inject_missile(replace);
	inject_items(replace);
	inject_flmtorch(replace);
	inject_scarab(replace);
	inject_larafire(replace);
	inject_lot(replace);
	inject_sentry(replace);
	inject_dog(replace);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hinstance = hModule;
		inject_all(true);

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	return 1;
}

