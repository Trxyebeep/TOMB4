#pragma once
#include "../global/vars.h"

#define WeaponObject	( (long(__cdecl*)(long)) 0x0042EA70 )
#define LaraGun	( (void(__cdecl*)()) 0x0042D840 )
#define HitTarget	( (void(__cdecl*)(ITEM_INFO*, GAME_VECTOR*, long, long)) 0x0042E920 )
#define get_current_ammo_pointer	( (short*(__cdecl*)(long)) 0x0042F480 )
