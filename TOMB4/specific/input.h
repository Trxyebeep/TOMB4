#pragma once
#include "../global/vars.h"

void inject_input(bool replace);

long Key(long number);
long S_UpdateInput();
long ReadJoystick(long& x, long& y);

#ifdef GENERAL_FIXES
extern short ammo_change_timer;
extern char ammo_change_buf[12];
#endif
